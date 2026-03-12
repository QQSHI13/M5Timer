#pragma once

#include "config.h"
#include "types.h"

// Get next timer mode based on current state
TimerMode getNextTimerMode(const TimerState& state, const Settings& settings);

// Get next mode after completion
TimerMode getNextModeFromCompleted(TimerMode completedMode, const TimerState& state, const Settings& settings);

// String conversion
String timerModeToString(TimerMode mode);
