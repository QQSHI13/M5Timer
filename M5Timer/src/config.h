#pragma once

#include <Arduino.h>

// ============== PIN DEFINITIONS ==============
#define LED_PIN     21
#define BUTTON_PIN  42
#define BUZZER_PIN  2
#define POWER_PIN   46

// ============== LED CONFIG ==============
#define NUM_LEDS    1
#define LED_BRIGHTNESS 128

// ============== BUTTON CONFIG ==============
#define DEBOUNCE_MS     50
#define DOUBLE_CLICK_MS 400

// ============== TIMER CONFIG ==============
#define INITIAL_MODE_SECONDS  5
#define SYNC_TIMEOUT_SECONDS  15

// ============== ENUMS ==============
enum class SystemMode {
    INITIAL,
    TIMER,
    SYNC
};

enum class TimerMode {
    WORK,
    BREAK,
    LONG_BREAK,
    COMPLETED
};

enum class ButtonEvent {
    NONE,
    SINGLE_CLICK,
    DOUBLE_CLICK
};

enum class SoundType {
    COUNTDOWN,
    WORK_END,      // Ascending C5-E5-G5 (523→659→784 Hz)
    BREAK_END,     // Descending G5-E5-C5 (784→659→523 Hz)
    CHIME          // Default pleasant chime
};
