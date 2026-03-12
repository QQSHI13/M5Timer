#include "led.h"
#include <FastLED.h>

static CRGB leds[1];  // Only 1 LED on Atom Lite
static bool initialized = false;

void LED::begin() {
    if (!initialized) {
        FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, 1);
        FastLED.setBrightness(128);
        initialized = true;
    }
    setColor(Color::off());
}

void LED::setColor(const Color& color) {
    currentColor = color;
    leds[0] = CRGB(color.r, color.g, color.b);
    FastLED.show();
}

void LED::setPulsing(bool enabled) {
    pulsing = enabled;
    if (!enabled) {
        setColor(currentColor);
    }
}

void LED::setSpinning(bool enabled) {
    spinning = enabled;
    if (enabled) {
        animationStart = millis();
    }
}

void LED::setFlashing(bool enabled, int count) {
    flashing = enabled;
    flashCount = count * 2; // On/off cycles
    if (enabled) {
        animationStart = millis();
    }
}

void LED::update() {
    unsigned long now = millis();
    
    if (pulsing) {
        // Sine wave pulse over 2 seconds
        float phase = (now % 2000) / 2000.0 * 2 * PI;
        float brightness = (sin(phase) + 1) / 2 * 0.8 + 0.2; // 0.2 to 1.0
        uint8_t r = currentColor.r * brightness;
        uint8_t g = currentColor.g * brightness;
        uint8_t b = currentColor.b * brightness;
        leds[0] = CRGB(r, g, b);
        FastLED.show();
    }
    else if (spinning) {
        // HSV color wheel
        uint8_t hue = (now / 5) % 256;  // Full rotation every ~1.3s
        leds[0] = CHSV(hue, 255, 255);
        FastLED.show();
    }
    else if (flashing) {
        unsigned long flashInterval = 150; // ms per flash
        unsigned long elapsed = now - animationStart;
        int currentFlash = elapsed / flashInterval;
        
        if (currentFlash >= flashCount) {
            flashing = false;
            setColor(currentColor);
        } else {
            if (currentFlash % 2 == 0) {
                leds[0] = CRGB(currentColor.r, currentColor.g, currentColor.b);
            } else {
                leds[0] = CRGB::Black;
            }
            FastLED.show();
        }
    }
}
