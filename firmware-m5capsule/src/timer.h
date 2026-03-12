#pragma once

#include "config.h"

class Timer {
public:
    void begin(const TimerSettings& s);
    void update();  // Call every loop (handles light sleep continuation)
    
    void start();   // Start/Resume timer
    void pause();   // Pause timer
    void reset();   // Reset to current mode's duration
    void switchMode();  // Switch to next mode (Work->Break->Work...)
    
    // State getters
    TimerMode getMode() const { return mode; }
    TimerState getState() const { return state; }
    unsigned long getRemainingSeconds() const { return remainingSeconds; }
    bool isCompleted() const { return state == TimerState::COMPLETED; }
    
    // State setters (for restoration)
    void setMode(TimerMode m);
    void setState(TimerState s);
    void setRemainingSeconds(unsigned long seconds);
    
    void updateSettings(const TimerSettings& s) { settings = s; reset(); }
    
private:
    TimerSettings settings;
    TimerMode mode = TimerMode::WORK;
    TimerState state = TimerState::PAUSED;
    unsigned long remainingSeconds = 0;
    unsigned long lastTick = 0;
    
    unsigned long getDurationForMode(TimerMode m) const;
};
