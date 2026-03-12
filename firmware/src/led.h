#pragma once

#include "config.h"

class LED {
public:
    void begin();
    void setColor(const Color& color);
    void update();  // Call in loop for animations
    void setPulsing(bool enabled);
    void setSpinning(bool enabled);
    void setFlashing(bool enabled, int count = 3);
    
private:
    Color currentColor = Color::off();
    bool pulsing = false;
    bool spinning = false;
    bool flashing = false;
    int flashCount = 0;
    unsigned long animationStart = 0;
};
