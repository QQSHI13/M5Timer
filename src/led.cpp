#include "led.h"
#include <FastLED.h>

static CRGB leds[NUM_LEDS];

void setupLED() {
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(LED_BRIGHTNESS);
    leds[0] = CRGB::Black;
    FastLED.show();
}

void setLEDBrightness(uint8_t brightness) {
    FastLED.setBrightness(brightness);
    FastLED.show();
}

void updateLED(SystemMode systemMode, TimerMode timerMode) {
    switch (systemMode) {
        case SystemMode::INITIAL:
            leds[0] = CRGB::White;
            break;
            
        case SystemMode::SWITCH:
            leds[0] = CRGB::White;  // White LED for switch mode
            break;
            
        case SystemMode::SYNC:
            leds[0] = CRGB(255, 80, 0);  // Red-orange
            break;
            
        case SystemMode::TIMER:
            switch (timerMode) {
                case TimerMode::WORK:
                    leds[0] = CRGB::Red;
                    break;
                case TimerMode::BREAK:
                    leds[0] = CRGB::Green;
                    break;
                case TimerMode::LONG_BREAK:
                    leds[0] = CRGB::Blue;
                    break;
            }
            break;
    }
    
    FastLED.show();
}
