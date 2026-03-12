#include "button.h"

void PomodoroButton::begin() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    // Give time for pullup to stabilize
    delay(10);
    lastButtonState = (digitalRead(BUTTON_PIN) == LOW);
}

bool PomodoroButton::isPressed() const {
    return digitalRead(BUTTON_PIN) == LOW;  // Active low
}

ButtonEvent PomodoroButton::update() {
    bool reading = isPressed();
    ButtonEvent event = ButtonEvent::NONE;
    unsigned long now = millis();
    
    // Debounce
    if (reading != lastButtonState) {
        lastDebounceTime = now;
    }
    
    if ((now - lastDebounceTime) > DEBOUNCE_DELAY) {
        // Button state has stabilized
        if (reading != buttonState) {
            buttonState = reading;
            
            if (buttonState) {
                // Button pressed (active low)
                buttonDownTime = now;
                longPressTriggered = false;
                
                // Check for double click timing
                if ((now - lastClickTime) < CLICK_TIMEOUT) {
                    clickCount++;
                } else {
                    clickCount = 1;
                }
                lastClickTime = now;
                singleClickPending = false;  // Reset pending flag on new press
                
                // If this is the second click in quick succession, it's a double click
                if (clickCount == 2) {
                    event = ButtonEvent::DOUBLE_CLICK;
                    clickCount = 0;  // Reset count
                    doubleClickDetected = true;  // Mark that we handled this sequence
                }
            } else {
                // Button released
                if (!longPressTriggered && (now - buttonDownTime) < LONG_PRESS_DURATION) {
                    // Short release - mark single click as pending
                    // It will be confirmed if no second click arrives within timeout
                    if (clickCount == 1 && !doubleClickDetected) {
                        singleClickPending = true;
                    }
                }
            }
        }
        
        // Check for long press while button is held
        if (buttonState && !longPressTriggered) {
            if ((now - buttonDownTime) >= LONG_PRESS_DURATION) {
                longPressTriggered = true;
                event = ButtonEvent::LONG_PRESS;
                clickCount = 0;  // Reset click count on long press
                singleClickPending = false;  // Cancel any pending single click
                doubleClickDetected = false;
            }
        }
        
        // Check for single click confirmation (double click window expired and button is released)
        if (!buttonState && singleClickPending && clickCount == 1) {
            if ((now - lastClickTime) >= CLICK_TIMEOUT) {
                if (!longPressTriggered) {
                    event = ButtonEvent::SINGLE_CLICK;
                }
                clickCount = 0;
                singleClickPending = false;
                doubleClickDetected = false;
            }
        }
        
        // Reset doubleClickDetected flag when click window expires
        if (doubleClickDetected && (now - lastClickTime) >= CLICK_TIMEOUT) {
            doubleClickDetected = false;
            clickCount = 0;
        }
        
        // Safety: reset click count if it gets too high (prevents overflow/bugs)
        if (clickCount > 2) {
            clickCount = 0;
            singleClickPending = false;
            doubleClickDetected = false;
        }
    }
    
    lastButtonState = reading;
    return event;
}
