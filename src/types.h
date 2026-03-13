#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "config.h"

// ============== SETTINGS STRUCT ==============
struct Settings {
    uint8_t workMinutes = 25;
    uint8_t breakMinutes = 5;
    uint8_t longBreakMinutes = 15;
    uint8_t workSessionsBeforeLongBreak = 4;
    bool soundEnabled = true;
    
    void load(Preferences& prefs);
    void save(Preferences& prefs) const;
    void reset();
    String toString() const;
    bool fromString(const String& str);
};

// ============== TIMER STATE STRUCT ==============
struct TimerState {
    TimerMode mode = TimerMode::WORK;
    int remainingSeconds = 25 * 60;
    bool isRunning = false;
    uint8_t completedWorkSessions = 0;
    
    void load(Preferences& prefs);
    void save(Preferences& prefs) const;
    int getDurationMinutes(const Settings& settings) const;
    void reset(const Settings& settings);
};

// ============== BUZZER TONE STRUCT ==============
struct Tone {
    uint16_t frequency;
    uint16_t durationMs;
};

// ============== BUZZER STATE STRUCT ==============
struct BuzzerState {
    bool active = false;
    unsigned long toneStartTime = 0;
    Tone currentTone;
    Tone queue[4];
    uint8_t queueSize = 0;
    uint8_t queueIndex = 0;
};

// ============== BUTTON STATE STRUCT ==============
struct ButtonState {
    bool lastRawState = HIGH;
    bool debouncedState = HIGH;
    unsigned long lastDebounceTime = 0;
    unsigned long lastClickTime = 0;
    int clickCount = 0;
    bool waitingForDouble = false;
};

// ============== GLOBAL STATE ==============
struct GlobalState {
    SystemMode systemMode = SystemMode::INITIAL;
    unsigned long modeStartTime = 0;
    bool syncPingReceived = false;
    TimerMode completedFromMode = TimerMode::WORK;
    TimerMode previewMode = TimerMode::WORK;  // For SWITCH mode preview
};
