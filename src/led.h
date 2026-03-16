#pragma once

#include "config.h"
#include "types.h"

void setupLED();
void updateLED(SystemMode systemMode, TimerMode timerMode);
void setLEDBrightness(uint8_t brightness);
