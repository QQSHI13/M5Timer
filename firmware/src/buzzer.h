#pragma once

#include "config.h"

// Buzzer class stub - minimal hardware has no buzzer
// All methods are no-ops for API compatibility
class Buzzer {
public:
    void begin() {}  // No-op
    void beep(int durationMs = 100) {}  // No-op: use LED feedback instead
    void playAlarm() {}  // No-op: use LED spinning pattern instead
    void playSyncTone() {}  // No-op: use LED pattern instead
    void stop() {}  // No-op
    void update() {}  // No-op
};
