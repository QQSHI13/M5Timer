#include "timer.h"

void Timer::begin(const TimerSettings& s) {
    settings = s;
    mode = TimerMode::WORK;
    state = TimerState::PAUSED;
    remainingSeconds = getDurationForMode(mode) * 60;
    completionTriggered = false;
    completedWorkSessions = 0;
    lastTick = millis();
}

void Timer::update() {
    if (state != TimerState::RUNNING) return;
    
    unsigned long now = millis();
    if (now - lastTick >= 1000) {
        lastTick = now;
        
        if (remainingSeconds > 0) {
            remainingSeconds--;
            
            // Debug output every minute
            if (remainingSeconds % 60 == 0) {
                const char* modeStr = "UNKNOWN";
                switch (mode) {
                    case TimerMode::WORK: modeStr = "WORK"; break;
                    case TimerMode::BREAK: modeStr = "BREAK"; break;
                    case TimerMode::LONG_BREAK: modeStr = "LONG_BREAK"; break;
                }
                Serial.printf("[%s] %lu min remaining (sessions: %d)\n",
                              modeStr, remainingSeconds / 60, completedWorkSessions);
            }
        } else {
            // Timer completed
            state = TimerState::COMPLETED;
            completionTriggered = false;
            
            // Increment work session counter if work completed
            if (mode == TimerMode::WORK) {
                completedWorkSessions++;
            }
            
            const char* modeStr = "UNKNOWN";
            switch (mode) {
                case TimerMode::WORK: modeStr = "WORK"; break;
                case TimerMode::BREAK: modeStr = "BREAK"; break;
                case TimerMode::LONG_BREAK: modeStr = "LONG_BREAK"; break;
            }
            Serial.printf("[%s] COMPLETED! (sessions: %d)\n", modeStr, completedWorkSessions);
        }
    }
}

void Timer::start() {
    if (state == TimerState::COMPLETED) {
        // Auto-switch mode on completion
        autoSwitchMode();
    }
    state = TimerState::RUNNING;
    lastTick = millis();
    completionTriggered = false;
    
    const char* modeStr = "UNKNOWN";
    switch (mode) {
        case TimerMode::WORK: modeStr = "WORK"; break;
        case TimerMode::BREAK: modeStr = "BREAK"; break;
        case TimerMode::LONG_BREAK: modeStr = "LONG_BREAK"; break;
    }
    Serial.printf("[%s] STARTED (sessions: %d)\n", modeStr, completedWorkSessions);
}

void Timer::pause() {
    state = TimerState::PAUSED;
    
    const char* modeStr = "UNKNOWN";
    switch (mode) {
        case TimerMode::WORK: modeStr = "WORK"; break;
        case TimerMode::BREAK: modeStr = "BREAK"; break;
        case TimerMode::LONG_BREAK: modeStr = "LONG_BREAK"; break;
    }
    Serial.printf("[%s] PAUSED\n", modeStr);
}

void Timer::reset() {
    remainingSeconds = getDurationForMode(mode) * 60;
    completionTriggered = false;
    if (state == TimerState::COMPLETED) {
        state = TimerState::PAUSED;
    }
    
    const char* modeStr = "UNKNOWN";
    switch (mode) {
        case TimerMode::WORK: modeStr = "WORK"; break;
        case TimerMode::BREAK: modeStr = "BREAK"; break;
        case TimerMode::LONG_BREAK: modeStr = "LONG_BREAK"; break;
    }
    Serial.printf("[%s] RESET to %d min\n", modeStr, getDurationForMode(mode));
}

void Timer::switchMode() {
    // Manual mode switch - go to work if in any break mode
    if (mode == TimerMode::BREAK || mode == TimerMode::LONG_BREAK) {
        mode = TimerMode::WORK;
    } else {
        // From work, auto-determine break type
        if (isLongBreakNext()) {
            mode = TimerMode::LONG_BREAK;
        } else {
            mode = TimerMode::BREAK;
        }
    }
    
    remainingSeconds = getDurationForMode(mode) * 60;
    state = TimerState::PAUSED;
    completionTriggered = false;
    
    const char* modeStr = "UNKNOWN";
    switch (mode) {
        case TimerMode::WORK: modeStr = "WORK"; break;
        case TimerMode::BREAK: modeStr = "BREAK"; break;
        case TimerMode::LONG_BREAK: modeStr = "LONG_BREAK"; break;
    }
    Serial.printf("SWITCHED to %s mode (%d min)\n", modeStr, getDurationForMode(mode));
}

void Timer::autoSwitchMode() {
    // Called when timer completes - auto switch to next mode
    if (mode == TimerMode::WORK) {
        // Just completed work - determine break type
        if (isLongBreakNext()) {
            mode = TimerMode::LONG_BREAK;
            Serial.println("Auto-switch: LONG BREAK (4 sessions completed!)");
        } else {
            mode = TimerMode::BREAK;
            Serial.println("Auto-switch: SHORT BREAK");
        }
    } else {
        // Completed break - back to work
        mode = TimerMode::WORK;
        Serial.println("Auto-switch: WORK");
    }
    
    remainingSeconds = getDurationForMode(mode) * 60;
    state = TimerState::PAUSED;
    completionTriggered = true;
}

bool Timer::isLongBreakNext() const {
    return completedWorkSessions > 0 && 
           (completedWorkSessions % settings.workSessionsBeforeLongBreak == 0);
}

void Timer::updateSettings(const TimerSettings& s) {
    settings = s;
    // Reset timer with new settings
    remainingSeconds = getDurationForMode(mode) * 60;
    Serial.println("Settings updated, timer reset");
}

unsigned long Timer::getDurationForMode(TimerMode m) const {
    switch (m) {
        case TimerMode::WORK:
            return settings.workMinutes;
        case TimerMode::BREAK:
            return settings.breakMinutes;
        case TimerMode::LONG_BREAK:
            return settings.longBreakMinutes;
        default:
            return 25;
    }
}
