#include "led.h"
#include <FastLED.h>

static CRGB leds[NUM_LEDS];

void setupLED() {
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(LED_BRIGHTNESS);
    leds[0] = CRGB::Black;
    FastLED.show();
}

void updateLED(SystemMode systemMode, TimerMode timerMode) {
    switch (systemMode) {
        case SystemMode::INITIAL:
            leds[0] = CRGB::White;
            break;
            
        case SystemMode::SYNC:
            leds[0] = CRGB::Blue;
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
                case TimerMode::COMPLETED:
                    leds[0] = CRGB::White;
                    break;
            }
            break;
    }
    
    FastLED.show();
}
