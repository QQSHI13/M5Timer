#pragma once

#include "config.h"
#include <FastLED.h>

class LED {
public:
    void begin();
    void setColor(const Color& color);
    void update();
    void setPulsing(bool enabled);
    void setSpinning(bool enabled);
    void setFlashing(bool enabled, int count = 3);
    void enable(bool enabled);
    bool isEnabled() const { return ledEnabled; }
    
private:
    Color currentColor = Color::off();
    bool pulsing = false;
    bool spinning = false;
    bool flashing = false;
    bool ledEnabled = true;
    int flashCount = 0;
    unsigned long animationStart = 0;
};
