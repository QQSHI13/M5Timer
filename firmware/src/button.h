#pragma once

#include "config.h"

class PomodoroButton {
public:
    void begin();
    ButtonEvent update();
    bool isPressed() const;
    
private:
    static constexpr unsigned long CLICK_TIMEOUT = 300;      // ms between clicks
    static constexpr unsigned long LONG_PRESS_DURATION = 3000; // ms for long press
    static constexpr unsigned long DEBOUNCE_DELAY = 50;      // ms debounce
    
    bool lastButtonState = false;
    bool buttonState = false;
    unsigned long buttonDownTime = 0;
    unsigned long lastDebounceTime = 0;
    unsigned long lastClickTime = 0;
    int clickCount = 0;
    bool longPressTriggered = false;
    bool singleClickPending = false;   // Single click waiting for double-click window
    bool doubleClickDetected = false;  // Tracks if we already handled a double-click
};
