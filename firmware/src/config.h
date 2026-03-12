#pragma once

#include <Arduino.h>
#include <M5AtomS3.h>
#include <FastLED.h>

// M5Stack AtomS3 Lite pin definitions
// GROVE/HY2.0-4P connector: Black(GND), Red(5V), Yellow(G2), White(G1)
#define BUTTON_PIN 41       // Built-in button (GPIO41)
#define LED_PIN 35          // Built-in WS2812C RGB LED (GPIO35)
#define BATTERY_PIN 6       // Battery voltage divider (GPIO6 - internal)
// Note: BUZZER removed in minimal hardware design - using LED feedback only

// Deep sleep configuration
#define SLEEP_INTERVAL_MS 1000     // Wake every second to check timer
#define BUTTON_WAKEUP_LEVEL 0      // LOW level wake (button pressed)
#define LONG_PRESS_WAKEUP_MS 3000  // Same as button long press threshold

// AtomS3 Lite has native USB HID support (ESP32-S3FN8)
#define HAS_NATIVE_USB_HID 1

// Timer modes
enum class TimerMode {
    WORK,
    BREAK,
    LONG_BREAK
};

// Timer states
enum class TimerState {
    RUNNING,
    PAUSED,
    COMPLETED
};

// Button events
enum class ButtonEvent {
    NONE,
    SINGLE_CLICK,
    DOUBLE_CLICK,
    LONG_PRESS
};

// System modes
enum class SystemMode {
    TIMER,
    SYNC
};

// LED colors
struct Color {
    uint8_t r, g, b;
    static Color red() { return {255, 0, 0}; }
    static Color green() { return {0, 255, 0}; }
    static Color blue() { return {0, 0, 255}; }
    static Color white() { return {255, 255, 255}; }
    static Color yellow() { return {255, 255, 0}; }
    static Color off() { return {0, 0, 0}; }
};
