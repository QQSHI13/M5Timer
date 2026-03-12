#include "led.h"

static CRGB leds[1];
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
    if (!ledEnabled) return;
    currentColor = color;
    leds[0] = CRGB(color.r, color.g, color.b);
    FastLED.show();
}

void LED::setPulsing(bool p) {
    pulsing = p;
    spinning = false;
    flashing = false;
}

void LED::setSpinning(bool s) {
    spinning = s;
    pulsing = false;
    flashing = false;
    if (s) animationStart = millis();
}

void LED::setFlashing(bool f, int count) {
    flashing = f;
    flashCount = count * 2;
    pulsing = false;
    spinning = false;
    if (f) animationStart = millis();
}

void LED::enable(bool e) {
    ledEnabled = e;
    if (!e) {
        leds[0] = CRGB::Black;
        FastLED.show();
    }
}

void LED::update() {
    if (!ledEnabled) return;
    
    unsigned long now = millis();
    
    if (pulsing) {
        float phase = (now % 2000) / 2000.0 * 2 * PI;
        float b = (sin(phase) + 1) / 2 * 0.8 + 0.2;
        leds[0] = CRGB(currentColor.r * b, currentColor.g * b, currentColor.b * b);
        FastLED.show();
    }
    else if (spinning) {
        leds[0] = CHSV((now / 5) % 256, 255, 255);
        FastLED.show();
    }
    else if (flashing) {
        unsigned long elapsed = now - animationStart;
        int flash = elapsed / 150;
        
        if (flash >= flashCount) {
            flashing = false;
            setColor(currentColor);
        } else {
            leds[0] = (flash % 2 == 0) ? CRGB(currentColor.r, currentColor.g, currentColor.b) : CRGB::Black;
            FastLED.show();
        }
    }
}
