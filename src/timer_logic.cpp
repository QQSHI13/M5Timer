#include "timer_logic.h"

TimerMode getNextTimerMode(const TimerState& state, const Settings& settings) {
    switch (state.mode) {
        case TimerMode::WORK:
            // After work, go to break or long break
            if (state.completedWorkSessions + 1 >= settings.workSessionsBeforeLongBreak) {
                return TimerMode::LONG_BREAK;
            } else {
                return TimerMode::BREAK;
            }
            
        case TimerMode::BREAK:
            // After break, go to work
            return TimerMode::WORK;
            
        case TimerMode::LONG_BREAK:
            // After long break, reset counter and go to work
            return TimerMode::WORK;
            
        default:
            return TimerMode::WORK;
    }
}

TimerMode getNextModeFromCompleted(TimerMode completedMode, const TimerState& state, const Settings& settings) {
    switch (completedMode) {
        case TimerMode::WORK:
            // After work, go to break or long break
            if (state.completedWorkSessions + 1 >= settings.workSessionsBeforeLongBreak) {
                return TimerMode::LONG_BREAK;
            } else {
                return TimerMode::BREAK;
            }
            
        case TimerMode::BREAK:
            // After break, go to work
            return TimerMode::WORK;
            
        case TimerMode::LONG_BREAK:
            // After long break, reset counter and go to work
            return TimerMode::WORK;
            
        default:
            return TimerMode::WORK;
    }
}

String timerModeToString(TimerMode mode) {
    switch (mode) {
        case TimerMode::WORK: return "WORK";
        case TimerMode::BREAK: return "BREAK";
        case TimerMode::LONG_BREAK: return "LONG_BREAK";
        default: return "UNKNOWN";
    }
}
