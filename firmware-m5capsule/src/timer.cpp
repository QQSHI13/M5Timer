#include "timer.h"

void Timer::begin(const TimerSettings& s) {
    settings = s;
    mode = TimerMode::WORK;
    state = TimerState::PAUSED;
    remainingSeconds = getDurationForMode(mode) * 60;
    lastTick = millis();
}

void Timer::update() {
    if (state != TimerState::RUNNING) return;
    
    unsigned long now = millis();
    if (now - lastTick >= 1000) {
        lastTick = now;
        
        if (remainingSeconds > 0) {
            remainingSeconds--;
        } else {
            state = TimerState::COMPLETED;
        }
    }
}

void Timer::start() {
    state = TimerState::RUNNING;
    lastTick = millis();
}

void Timer::pause() {
    state = TimerState::PAUSED;
}

void Timer::reset() {
    remainingSeconds = getDurationForMode(mode) * 60;
    if (state == TimerState::COMPLETED) {
        state = TimerState::PAUSED;
    }
    lastTick = millis();
}

void Timer::switchMode() {
    // Simple mode switch: Work -> Break -> Work
    // Long break every 4 work sessions would require session counter
    if (mode == TimerMode::WORK) {
        mode = TimerMode::BREAK;
    } else {
        mode = TimerMode::WORK;
    }
    remainingSeconds = getDurationForMode(mode) * 60;
    state = TimerState::PAUSED;
}

unsigned long Timer::getDurationForMode(TimerMode m) const {
    switch (m) {
        case TimerMode::WORK: return settings.workMinutes;
        case TimerMode::BREAK: return settings.breakMinutes;
        case TimerMode::LONG_BREAK: return settings.longBreakMinutes;
        default: return 25;
    }
}

void Timer::setMode(TimerMode m) {
    mode = m;
}

void Timer::setState(TimerState s) {
    state = s;
}

void Timer::setRemainingSeconds(unsigned long seconds) {
    remainingSeconds = seconds;
}
