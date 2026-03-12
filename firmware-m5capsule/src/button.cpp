#include "button.h"

void PomodoroButton::begin() {
    pinMode(WAKE_BUTTON_PIN, INPUT_PULLUP);
    state = State::IDLE;
    lastRawState = digitalRead(WAKE_BUTTON_PIN) == LOW;
    lastDebounceTime = millis();
}

bool PomodoroButton::isPressed() {
    bool raw = digitalRead(WAKE_BUTTON_PIN) == LOW;
    
    if (raw != lastRawState) {
        lastDebounceTime = millis();
    }
    lastRawState = raw;
    
    if ((millis() - lastDebounceTime) > DEBOUNCE_MS) {
        return raw;
    }
    return false;
}

ButtonEvent PomodoroButton::update() {
    bool pressed = isPressed();
    unsigned long now = millis();
    ButtonEvent event = ButtonEvent::NONE;
    
    switch (state) {
        case State::IDLE:
            if (pressed) {
                pressStartTime = now;
                state = State::PRESSED;
            }
            break;
            
        case State::PRESSED:
            if (!pressed) {
                // Button released - check for click
                if (now - pressStartTime < LONG_PRESS_MS) {
                    pressEndTime = now;
                    state = State::WAITING_DOUBLE;
                }
            } else if (now - pressStartTime >= LONG_PRESS_MS) {
                // Long press detected
                event = ButtonEvent::LONG_PRESS;
                state = State::LONG_PRESS_SENT;
            }
            break;
            
        case State::WAITING_DOUBLE:
            if (pressed && (now - pressEndTime < DOUBLE_CLICK_MS)) {
                // Double click detected
                event = ButtonEvent::DOUBLE_CLICK;
                state = State::IDLE;
            } else if (now - pressEndTime >= DOUBLE_CLICK_MS) {
                // Single click confirmed
                event = ButtonEvent::SINGLE_CLICK;
                state = State::IDLE;
            }
            break;
            
        case State::LONG_PRESS_SENT:
            if (!pressed) {
                state = State::IDLE;
            }
            break;
    }
    
    return event;
}
