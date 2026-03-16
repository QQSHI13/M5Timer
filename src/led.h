#pragma once

#include "config.h"
#include "types.h"

void setupLED();
void updateLED(SystemMode systemMode, TimerMode timerMode);
void setLEDBrightness(uint8_t brightness);
void setLEDColor(uint8_t r, uint8_t g, uint8_t b);  // NEW: Direct color control
