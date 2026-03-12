#include <Arduino.h>
#include <M5Unified.h>
#include <I2C_BM8563.h>

#include "config.h"
#include "types.h"
#include "storage.h"
#include "hardware.h"
#include "led.h"
#include "buzzer.h"
#include "button.h"
#include "timer_logic.h"
#include "sync.h"

// ============== GLOBAL OBJECTS ==============
I2C_BM8563 rtc(I2C_BM8563_DEFAULT_ADDRESS, Wire1);

// ============== GLOBAL STATE ==============
Settings g_settings;
TimerState g_timerState;
GlobalState g_state;



// ============== FORWARD DECLARATIONS ==============
void handleInitialMode();
void handleTimerMode();
void handleSyncMode();
void switchToNextMode();
void switchToNextModeFromCompleted();

// ============== SETUP ==============
void setup() {
    Serial.begin(115200);
    delay(100);
    
    Serial.println("\n=== M5Capsule Pomodoro ===");
    
    // Setup hardware
    setupPower();
    setupLED();
    setupBuzzer();
    setupButton();
    setupRTC();
    
    // Load settings and state
    loadSettings(g_settings);
    loadTimerState(g_timerState);
    setBuzzerSettings(g_settings);
    
    // Check reset reason
    esp_reset_reason_t resetReason = esp_reset_reason();
    if (resetReason == ESP_RST_POWERON || resetReason == ESP_RST_EXT) {
        Serial.println("Cold boot / Hardware reset");
    } else if (resetReason == ESP_RST_DEEPSLEEP) {
        Serial.println("Wake from deep sleep");
    } else {
        Serial.println("Other reset reason");
    }
    
    // Enter initial mode
    g_state.systemMode = SystemMode::INITIAL;
    g_state.modeStartTime = millis();
    Serial.println("INITIAL MODE: 5 seconds");
    updateLED(SystemMode::INITIAL, TimerMode::WORK);
}

// ============== LOOP ==============
void loop() {
    // Update button once per loop - CRITICAL
    updateButton();
    
    // Update non-blocking buzzer
    updateBuzzer();
    
    // Handle current mode
    switch (g_state.systemMode) {
        case SystemMode::INITIAL:
            handleInitialMode();
            break;
        case SystemMode::TIMER:
            handleTimerMode();
            break;
        case SystemMode::SYNC:
            handleSyncMode();
            break;
    }
}

// ============== MODE TRANSITION HANDLERS ==============
void handleInitialMode() {
    unsigned long now = millis();
    unsigned long elapsed = (now - g_state.modeStartTime) / 1000;
    
    // Check for double-click to enter sync mode
    ButtonEvent event = getButtonEvent();
    if (event == ButtonEvent::DOUBLE_CLICK) {
        Serial.println("Double-click detected → SYNC MODE");
        g_state.systemMode = SystemMode::SYNC;
        g_state.modeStartTime = millis();
        g_state.syncPingReceived = false;
        Serial.println("=== SYNC MODE ===");
        Serial.println("Waiting for PING (15s timeout)...");
        updateLED(SystemMode::SYNC, TimerMode::WORK);
        return;
    }
    
    // Handle countdown and timeout
    unsigned long remaining = INITIAL_MODE_SECONDS - elapsed;
    static int lastBeepSecond = -1;
    
    if ((int)remaining != lastBeepSecond && remaining <= 5) {
        playCountdownBeep(remaining);
        if (remaining > 0) {
            Serial.println(String(remaining) + "...");
        }
        lastBeepSecond = remaining;
    }
    
    if (elapsed >= INITIAL_MODE_SECONDS) {
        Serial.println("Initial mode timeout → TIMER MODE");
        
        loadTimerState(g_timerState);
        
        Serial.println("Restored: " + timerModeToString(g_timerState.mode) + 
                      " - " + String(g_timerState.remainingSeconds / 60) + ":" + 
                      String(g_timerState.remainingSeconds % 60));
        
        g_state.systemMode = SystemMode::TIMER;
        Serial.println("TIMER MODE");
        if (!g_timerState.isRunning) {
            g_timerState.reset(g_settings);
        }
        updateLED(SystemMode::TIMER, g_timerState.mode);
        playTimerStartSound(g_timerState.mode, g_settings);
    }
}

void handleTimerMode() {
    static I2C_BM8563_TimeTypeDef lastRTC;
    static uint8_t lastSecond = 255;
    static uint8_t completionDisplaySeconds = 0;
    
    I2C_BM8563_TimeTypeDef rtcTime;
    rtc.getTime(&rtcTime);
    
    if (lastSecond == 255) {
        lastRTC = rtcTime;
        lastSecond = rtcTime.seconds;
    }
    
    // Check for button press
    ButtonEvent event = getButtonEvent();
    if (event == ButtonEvent::SINGLE_CLICK) {
        Serial.println("Single-click: " + String(g_timerState.isRunning ? "Reset timer" : "Change mode"));
        
        if (g_timerState.isRunning) {
            // Reset current timer
            g_timerState.reset(g_settings);
            saveTimerState(g_timerState);
            lastRTC = rtcTime;
            completionDisplaySeconds = 0;
            updateLED(SystemMode::TIMER, g_timerState.mode);
            playChime();
        } else {
            // Manual mode switch
            switchToNextMode();
            lastRTC = rtcTime;
            completionDisplaySeconds = 0;
        }
    }
    
    // Calculate elapsed seconds using RTC
    int elapsedSeconds = 0;
    if (rtcTime.seconds != lastSecond) {
        if (rtcTime.seconds > lastSecond) {
            elapsedSeconds = rtcTime.seconds - lastSecond;
        } else {
            elapsedSeconds = (60 - lastSecond) + rtcTime.seconds;
        }
        lastSecond = rtcTime.seconds;
        lastRTC = rtcTime;
    }
    
    // Update timer
    while (elapsedSeconds > 0 && g_timerState.isRunning) {
        elapsedSeconds--;
        
        if (g_timerState.remainingSeconds > 0) {
            g_timerState.remainingSeconds--;
            
            if (g_timerState.remainingSeconds % 60 == 0) {
                Serial.println(timerModeToString(g_timerState.mode) + 
                              " - " + String(g_timerState.remainingSeconds / 60) + 
                              ":" + String(g_timerState.remainingSeconds % 60));
            }
        }
        
        if (g_timerState.remainingSeconds == 0) {
            g_state.completedFromMode = g_timerState.mode;
            g_timerState.isRunning = false;
            g_timerState.mode = TimerMode::COMPLETED;
            completionDisplaySeconds = 0;
            updateLED(SystemMode::TIMER, TimerMode::COMPLETED);
            Serial.println("Timer completed!");
            break;
        }
    }
    
    if (elapsedSeconds > 0 || g_timerState.remainingSeconds == 0) {
        saveTimerState(g_timerState);
    }
    
    // Handle completion display
    if (g_timerState.mode == TimerMode::COMPLETED) {
        completionDisplaySeconds += elapsedSeconds;
        if (completionDisplaySeconds >= 5) {
            completionDisplaySeconds = 0;
            switchToNextModeFromCompleted();
            lastRTC = rtcTime;
            lastSecond = rtcTime.seconds;
        }
    }
    
    // Light sleep
    if (!isBuzzerActive() && g_timerState.isRunning) {
        M5.Power.lightSleep(1000000);
    }
}

void handleSyncMode() {
    // Check for timeout
    if (!g_state.syncPingReceived) {
        unsigned long elapsed = (millis() - g_state.modeStartTime) / 1000;
        if (elapsed >= SYNC_TIMEOUT_SECONDS) {
            Serial.println("Sync timeout - returning to TIMER MODE");
            g_state.systemMode = SystemMode::TIMER;
            Serial.println("TIMER MODE");
            if (!g_timerState.isRunning) {
                g_timerState.reset(g_settings);
            }
            updateLED(SystemMode::TIMER, g_timerState.mode);
            return;
        }
    }
    
    // Process serial commands
    if (processSerialCommands(g_settings, g_timerState, g_state.syncPingReceived)) {
        // PONG received - transition to timer mode
        g_state.systemMode = SystemMode::TIMER;
        Serial.println("TIMER MODE");
        if (!g_timerState.isRunning) {
            g_timerState.reset(g_settings);
        }
        updateLED(SystemMode::TIMER, g_timerState.mode);
    }
}

void switchToNextMode() {
    TimerMode oldMode = g_timerState.mode;
    g_timerState.mode = getNextTimerMode(g_timerState, g_settings);
    g_timerState.reset(g_settings);
    
    if (oldMode == TimerMode::WORK) {
        g_timerState.completedWorkSessions++;
    } else if (oldMode == TimerMode::LONG_BREAK) {
        g_timerState.completedWorkSessions = 0;
    }
    
    saveTimerState(g_timerState);
    updateLED(SystemMode::TIMER, g_timerState.mode);
    playChime();
    Serial.println("Switched to " + timerModeToString(g_timerState.mode));
}

void switchToNextModeFromCompleted() {
    TimerMode newMode = getNextModeFromCompleted(g_state.completedFromMode, g_timerState, g_settings);
    g_timerState.mode = newMode;
    g_timerState.reset(g_settings);
    
    if (g_state.completedFromMode == TimerMode::WORK) {
        g_timerState.completedWorkSessions++;
    } else if (g_state.completedFromMode == TimerMode::LONG_BREAK) {
        g_timerState.completedWorkSessions = 0;
    }
    
    saveTimerState(g_timerState);
    updateLED(SystemMode::TIMER, g_timerState.mode);
    playTimerStartSound(newMode, g_settings);
    Serial.println("Auto-switched to " + timerModeToString(g_timerState.mode));
}
