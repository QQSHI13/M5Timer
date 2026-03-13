#pragma once

#include <Arduino.h>

// ============== PIN DEFINITIONS ==============
#define LED_PIN     21
#define BUTTON_PIN  42
#define BUZZER_PIN  2
#define POWER_PIN   46

// ============== LED CONFIG ==============
#define NUM_LEDS    1
#define LED_BRIGHTNESS 16

// ============== BUZZER CONFIG ==============
#define BUZZER_VOLUME 24  // ~10% of 255 (duty cycle for PWM)

// ============== BUTTON CONFIG ==============
#define DEBOUNCE_MS     50
#define DOUBLE_CLICK_MS 400

// ============== TIMER CONFIG ==============
#define INITIAL_MODE_SECONDS  4
#define SYNC_TIMEOUT_SECONDS  10

// ============== ENUMS ==============
enum class SystemMode {
    INITIAL,
    TIMER,
    SWITCH,
    SYNC
};

enum class TimerMode {
    WORK,
    BREAK,
    LONG_BREAK
};

enum class ButtonEvent {
    NONE,
    SINGLE_CLICK,
    DOUBLE_CLICK
};

enum class SoundType {
    COUNTDOWN,
    WORK_END,      // Triumphant fanfare C5-E5-G5-C6
    BREAK_END,     // Gentle descending G5-E5-C5-G4
    CHIME,         // Nice bell chime E5-G5-B5
    MODE_SWITCH,   // Quick blip for mode cycling
    RESET_SOUND    // Reset confirmation sound
};
