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
void switchToNextModeFromCompleted();
void enterLightSleep(uint32_t sleepMs);

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
    
    // Ensure serial is disabled in INITIAL mode
    Serial.end();
    
    // Check for double-click to enter sync mode
    ButtonEvent event = getButtonEvent();
    if (event == ButtonEvent::DOUBLE_CLICK) {
        Serial.begin(115200);  // Enable serial for SYNC mode
        delay(100);  // Wait for serial to stabilize
        Serial.flush();
        g_state.systemMode = SystemMode::SYNC;
        g_state.modeStartTime = millis();
        g_state.syncPingReceived = false;
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
        g_state.completedFromMode = g_timerState.mode;  // Set for proper transition logic
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
    
    // Light sleep during timer - wake on button press
    if (!isBuzzerActive() && g_timerState.isRunning && g_timerState.remainingSeconds > 0) {
        uint32_t sleepMs = (uint32_t)g_timerState.remainingSeconds * 1000;
        enterLightSleep(sleepMs);
    }
}

void handleSwitchMode() {
    // Ensure serial is disabled in SWITCH mode
    Serial.end();
    
    // Detect re-entry into SWITCH mode by checking if modeStartTime changed
    if (g_state.switchEntryTime != g_state.modeStartTime) {
        // First call after entering SWITCH mode - reset state
        g_state.switchEntryTime = g_state.modeStartTime;
        g_state.switchActionTime = millis();
        g_state.switchPreviewActive = false;
    }
    
    // Check for button events in switch mode
    ButtonEvent event = getButtonEvent();
    if (event == ButtonEvent::SINGLE_CLICK) {
        if (!g_state.switchPreviewActive) {
            // First click - start preview with next mode (cycle through all modes)
            g_state.switchPreviewActive = true;
            g_state.previewMode = getNextTimerMode(g_timerState, g_settings);
        } else {
            // Subsequent click - cycle through ALL modes: WORK -> BREAK -> LONG_BREAK -> WORK
            switch (g_state.previewMode) {
                case TimerMode::WORK:
                    g_state.previewMode = TimerMode::BREAK;
                    break;
                case TimerMode::BREAK:
                    g_state.previewMode = TimerMode::LONG_BREAK;
                    break;
                case TimerMode::LONG_BREAK:
                    g_state.previewMode = TimerMode::WORK;
                    break;
            }
        }
        // Reset timer, show preview, and play sound
        g_state.switchActionTime = millis();
        updateLED(SystemMode::TIMER, g_state.previewMode);
        playModeSwitchSound();
        return;
    } else if (event == ButtonEvent::DOUBLE_CLICK) {
        // Enter sync mode - enable serial first
        Serial.begin(115200);
        delay(100);  // Wait for serial to stabilize
        Serial.flush();
        g_state.systemMode = SystemMode::SYNC;
        g_state.modeStartTime = millis();
        g_state.syncPingReceived = false;
        updateLED(SystemMode::SYNC, TimerMode::WORK);
        return;
    }
    
    // Check for 4 second timeout - auto-switch to selected/previewed mode
    if (millis() - g_state.switchActionTime >= 4000) {
        if (g_state.switchPreviewActive) {
            // Switch to previewed mode
            g_state.switchPreviewActive = false;
            g_timerState.mode = g_state.previewMode;
            g_timerState.reset(g_settings);
            if (g_state.completedFromMode == TimerMode::WORK && g_timerState.mode != TimerMode::WORK) {
                g_timerState.completedWorkSessions++;
            } else if (g_state.completedFromMode == TimerMode::LONG_BREAK && g_timerState.mode == TimerMode::WORK) {
                g_timerState.completedWorkSessions = 0;
            }
            saveTimerState(g_timerState);
            g_state.systemMode = SystemMode::TIMER;
            g_state.modeStartTime = millis();
            updateLED(SystemMode::TIMER, g_timerState.mode);
            playTimerStartSound(g_timerState.mode, g_settings);
        } else {
            // No preview - auto-switch to next mode
            g_state.switchPreviewActive = false;
            g_state.systemMode = SystemMode::TIMER;
            g_state.modeStartTime = millis();
            switchToNextModeFromCompleted();
        }
    }
}

void handleSyncMode() {
    static unsigned long pongWaitStartTime = 0;
    
    // Check for timeout
    if (!g_state.syncPingReceived) {
        // Phase 1: Waiting for PING
        unsigned long elapsed = (millis() - g_state.modeStartTime) / 1000;
        if (elapsed >= SYNC_TIMEOUT_SECONDS) {
            Serial.println("Sync timeout - switching to WORK mode");
            Serial.flush();
            delay(100);
            // Always switch to WORK mode (phase one) after sync
            g_timerState.mode = TimerMode::WORK;
            g_timerState.reset(g_settings);
            saveTimerState(g_timerState);
            g_state.systemMode = SystemMode::TIMER;
            updateLED(SystemMode::TIMER, g_timerState.mode);
            playTimerStartSound(g_timerState.mode, g_settings);
            return;
        }
    } else {
        // Phase 2: PING received, waiting for PONG - 30 second timeout
        if (pongWaitStartTime == 0) {
            pongWaitStartTime = millis();
        }
        unsigned long pongElapsed = (millis() - pongWaitStartTime) / 1000;
        if (pongElapsed >= 30) {
            Serial.println("PONG timeout - switching to WORK mode");
            Serial.flush();
            pongWaitStartTime = 0;
            g_timerState.mode = TimerMode::WORK;
            g_timerState.reset(g_settings);
            saveTimerState(g_timerState);
            g_state.systemMode = SystemMode::TIMER;
            updateLED(SystemMode::TIMER, g_timerState.mode);
            playTimerStartSound(g_timerState.mode, g_settings);
            return;
        }
    }
    
    // Process serial commands
    if (processSerialCommands(g_settings, g_timerState, g_state.syncPingReceived)) {
        // PONG received - restart device to hide COM port and start fresh
        pongWaitStartTime = 0;
        Serial.println("Restarting device...");
        Serial.flush();
        delay(100);
        ESP.restart();
    }
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

// ============== POWER MANAGEMENT ==============
void enterLightSleep(uint32_t sleepMs) {
    if (sleepMs == 0) return;
    
    // Use M5.Power.lightSleep - handles GPIO wakeup automatically
    M5.Power.lightSleep(sleepMs);
}
