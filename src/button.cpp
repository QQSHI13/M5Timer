#include "button.h"
#include "config.h"
#include <driver/gpio.h>

static ButtonState buttonState;

void setupButton() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    // Configure GPIO42 as wakeup source for light sleep
    gpio_wakeup_enable((gpio_num_t)BUTTON_PIN, GPIO_INTR_LOW_LEVEL);
    esp_sleep_enable_gpio_wakeup();
}

void updateButton() {
    bool rawState = digitalRead(BUTTON_PIN);
    unsigned long now = millis();
    
    // Debounce
    if (rawState != buttonState.lastRawState) {
        buttonState.lastDebounceTime = now;
        buttonState.lastRawState = rawState;
    }
    
    if ((now - buttonState.lastDebounceTime) > DEBOUNCE_MS) {
        if (rawState != buttonState.debouncedState) {
            buttonState.debouncedState = rawState;
            
            // Button press (active LOW)
            if (buttonState.debouncedState == LOW) {
                buttonState.clickCount++;
                buttonState.lastClickTime = now;
                
                if (buttonState.clickCount == 1) {
                    buttonState.waitingForDouble = true;
                }
            }
        }
    }
    
    // Check for double-click timeout
    if (buttonState.waitingForDouble && 
        (now - buttonState.lastClickTime) > DOUBLE_CLICK_MS) {
        buttonState.waitingForDouble = false;
    }
}

ButtonEvent getButtonEvent() {
    if (buttonState.clickCount == 0) {
        return ButtonEvent::NONE;
    }
    
    // If we have 2 clicks, it's a double-click
    if (buttonState.clickCount >= 2) {
        buttonState.clickCount = 0;
        buttonState.waitingForDouble = false;
        return ButtonEvent::DOUBLE_CLICK;
    }
    
    // If waiting period expired with 1 click, it's a single-click
    if (buttonState.clickCount == 1 && !buttonState.waitingForDouble) {
        buttonState.clickCount = 0;
        return ButtonEvent::SINGLE_CLICK;
    }
    
    return ButtonEvent::NONE;
}

unsigned long getLastClickTime() {
    return buttonState.lastClickTime;
}
