#pragma once

#include "types.h"

// ============== SETTINGS METHODS ==============
void loadSettings(Settings& settings);
void saveSettings(const Settings& settings);
void resetSettings(Settings& settings);

// ============== TIMER STATE METHODS ==============
void loadTimerState(TimerState& timerState);
void saveTimerState(const TimerState& timerState);
