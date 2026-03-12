#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include "config.h"

enum class ButtonEvent {
    NONE,
    SINGLE_CLICK,
    DOUBLE_CLICK,
    LONG_PRESS
};

class PomodoroButton {
public:
    void begin();
    ButtonEvent update();
    bool isPressed();
    
private:
    // State machine
    enum class State {
        IDLE,
        PRESSED,
        WAITING_DOUBLE,
        LONG_PRESS_SENT
    };
    
    State state = State::IDLE;
    unsigned long pressStartTime = 0;
    unsigned long pressEndTime = 0;
    
    // Timing constants
    static constexpr uint16_t DEBOUNCE_MS = 50;
    static constexpr uint16_t DOUBLE_CLICK_MS = 400;
    static constexpr uint16_t LONG_PRESS_MS = 3000;
    
    bool lastRawState = false;
    unsigned long lastDebounceTime = 0;
};

#endif
