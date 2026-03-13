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
void handleSwitchMode();
void handleSyncMode();
void switchToNextMode();
void switchToNextModeFromCompleted();

// ============== SETUP ==============
void setup() {
    // Setup hardware (serial not started - only used in SYNC mode)
    setupPower();
    setupLED();
    setupBuzzer();
    setupButton();
    setupRTC();
    
    // Load settings and state
    loadSettings(g_settings);
    loadTimerState(g_timerState);
    setBuzzerSettings(g_settings);
    
    // Enter initial mode
    g_state.systemMode = SystemMode::INITIAL;
    g_state.modeStartTime = millis();
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
        case SystemMode::SWITCH:
            handleSwitchMode();
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
        g_state.systemMode = SystemMode::SYNC;
        g_state.modeStartTime = millis();
        g_state.syncPingReceived = false;
        Serial.begin(115200);
        delay(100);
        Serial.println("=== SYNC MODE ===");
        Serial.println("Waiting for PING (15s timeout)...");
        Serial.flush();
        updateLED(SystemMode::SYNC, TimerMode::WORK);
        return;
    }
    
    // Handle countdown and timeout (no serial output in initial mode)
    unsigned long remaining = INITIAL_MODE_SECONDS - elapsed;
    static int lastBeepSecond = -1;
    
    if ((int)remaining != lastBeepSecond && remaining <= 5) {
        playCountdownBeep(remaining);
        lastBeepSecond = remaining;
    }
    
    if (elapsed >= INITIAL_MODE_SECONDS) {
        // Start fresh with WORK mode (deep work, part 1)
        g_timerState.mode = TimerMode::WORK;
        g_timerState.completedWorkSessions = 0;
        g_timerState.reset(g_settings);
        saveTimerState(g_timerState);
        
        g_state.systemMode = SystemMode::TIMER;
        updateLED(SystemMode::TIMER, g_timerState.mode);
        playTimerStartSound(g_timerState.mode, g_settings);
    }
}

void handleTimerMode() {
    static I2C_BM8563_TimeTypeDef lastRTC;
    static uint8_t lastSecond = 255;
    
    I2C_BM8563_TimeTypeDef rtcTime;
    rtc.getTime(&rtcTime);
    
    if (lastSecond == 255) {
        lastRTC = rtcTime;
        lastSecond = rtcTime.seconds;
    }
    
    // Check for button press - enter SWITCH mode to select next timer
    ButtonEvent event = getButtonEvent();
    if (event == ButtonEvent::SINGLE_CLICK) {
        // Enter switch mode to allow mode selection
        g_state.systemMode = SystemMode::SWITCH;
        g_state.modeStartTime = millis();
        updateLED(SystemMode::SWITCH, g_timerState.mode);
        playChime();
        return;
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
        }
        
        if (g_timerState.remainingSeconds == 0) {
            // Timer finished - enter SWITCH mode
            g_state.completedFromMode = g_timerState.mode;
            g_timerState.isRunning = false;
            saveTimerState(g_timerState);
            g_state.systemMode = SystemMode::SWITCH;
            g_state.modeStartTime = millis();
            updateLED(SystemMode::SWITCH, g_timerState.mode);
            break;
        }
    }
    
    if (elapsedSeconds > 0 || g_timerState.remainingSeconds == 0) {
        saveTimerState(g_timerState);
    }
    
    // Light sleep
    if (!isBuzzerActive() && g_timerState.isRunning) {
        M5.Power.lightSleep(1000000);
        delay(10);
    }
}

void handleSwitchMode() {
    static unsigned long switchModeStartTime = 0;
    static bool previewActive = false;
    
    // Initialize switch mode timer and preview on first entry
    if (switchModeStartTime == 0) {
        switchModeStartTime = millis();
        previewActive = false;
    }
    
    // Check for button events in switch mode
    ButtonEvent event = getButtonEvent();
    if (event == ButtonEvent::SINGLE_CLICK) {
        if (!previewActive) {
            // First click - start preview with next mode
            previewActive = true;
            g_state.previewMode = getNextTimerMode(g_timerState, g_settings);
        } else {
            // Subsequent click - cycle to next mode
            TimerState tempState = g_timerState;
            tempState.mode = g_state.previewMode;
            g_state.previewMode = getNextTimerMode(tempState, g_settings);
        }
        // Reset timer and show preview
        switchModeStartTime = millis();
        updateLED(SystemMode::TIMER, g_state.previewMode);
        return;
    } else if (event == ButtonEvent::DOUBLE_CLICK) {
        // Enter sync mode - reopen serial
        switchModeStartTime = 0;
        previewActive = false;
        g_state.systemMode = SystemMode::SYNC;
        g_state.modeStartTime = millis();
        g_state.syncPingReceived = false;
        Serial.begin(115200);
        delay(100);
        Serial.println("Double-click → SYNC MODE");
        Serial.println("=== SYNC MODE ===");
        Serial.println("Waiting for PING (15s timeout)...");
        updateLED(SystemMode::SYNC, TimerMode::WORK);
        return;
    }
    
    // Check for 5 second timeout - auto-switch to selected/previewed mode
    if (millis() - switchModeStartTime >= 5000) {
        switchModeStartTime = 0;
        if (previewActive) {
            // Switch to previewed mode
            previewActive = false;
            g_timerState.mode = g_state.previewMode;
            g_timerState.reset(g_settings);
            if (g_state.completedFromMode == TimerMode::WORK && g_timerState.mode != TimerMode::WORK) {
                g_timerState.completedWorkSessions++;
            } else if (g_state.completedFromMode == TimerMode::LONG_BREAK && g_timerState.mode == TimerMode::WORK) {
                g_timerState.completedWorkSessions = 0;
            }
            saveTimerState(g_timerState);
            g_state.systemMode = SystemMode::TIMER;
            updateLED(SystemMode::TIMER, g_timerState.mode);
            playTimerStartSound(g_timerState.mode, g_settings);
        } else {
            // No preview - auto-switch to next mode
            previewActive = false;
            switchToNextModeFromCompleted();
        }
    }
}

void handleSyncMode() {
    // Check for timeout
    if (!g_state.syncPingReceived) {
        unsigned long elapsed = (millis() - g_state.modeStartTime) / 1000;
        if (elapsed >= SYNC_TIMEOUT_SECONDS) {
            Serial.println("Sync timeout - returning to TIMER MODE");
            Serial.flush();
            delay(100);
            Serial.end();
            g_state.systemMode = SystemMode::TIMER;
            if (!g_timerState.isRunning) {
                g_timerState.reset(g_settings);
            }
            updateLED(SystemMode::TIMER, g_timerState.mode);
            playChime();  // Sound on sync exit
            return;
        }
    }
    
    // Process serial commands
    if (processSerialCommands(g_settings, g_timerState, g_state.syncPingReceived)) {
        // PONG received - transition to timer mode
        g_state.systemMode = SystemMode::TIMER;
        Serial.flush();
        delay(100);
        Serial.end();
        if (!g_timerState.isRunning) {
            g_timerState.reset(g_settings);
        }
        updateLED(SystemMode::TIMER, g_timerState.mode);
        playChime();  // Sound on sync exit
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
}
