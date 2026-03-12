#pragma once

#include "config.h"
#include "storage.h"

class Timer {
public:
    void begin(const TimerSettings& settings);
    void update();  // Call every loop
    
    void start();
    void pause();
    void reset();
    void switchMode();
    
    TimerMode getMode() const { return mode; }
    TimerState getState() const { return state; }
    unsigned long getRemainingSeconds() const { return remainingSeconds; }
    uint8_t getCompletedWorkSessions() const { return completedWorkSessions; }
    bool isCompleted() const { return state == TimerState::COMPLETED; }
    bool isLongBreakNext() const;
    
    void updateSettings(const TimerSettings& settings);
    
private:
    TimerSettings settings;
    TimerMode mode = TimerMode::WORK;
    TimerState state = TimerState::PAUSED;
    unsigned long remainingSeconds = 0;
    unsigned long lastTick = 0;
    bool completionTriggered = false;
    uint8_t completedWorkSessions = 0;  // Count of completed work sessions
    
    unsigned long getDurationForMode(TimerMode m) const;
    void autoSwitchMode();
};
