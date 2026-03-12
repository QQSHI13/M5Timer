#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Pin definitions for M5Capsule
#define POWER_HOLD_PIN  46
#define WAKE_BUTTON_PIN 42
#define BUZZER_PIN      2
#define LED_PIN         21
#define I2C_SDA_PIN     8
#define I2C_SCL_PIN     10

// System modes
enum class SystemMode {
    INITIAL,    // 5 second window after boot
    TIMER,      // Normal pomodoro timer operation
    SYNC        // USB sync mode
};

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

// Timer settings structure
struct TimerSettings {
    uint8_t workMinutes = 25;
    uint8_t breakMinutes = 5;
    uint8_t longBreakMinutes = 15;
    uint8_t workSessionsBeforeLongBreak = 4;
    bool soundEnabled = true;
};

// LED Color struct
struct Color {
    uint8_t r, g, b;
    
    static Color off() { return {0, 0, 0}; }
    static Color red() { return {128, 0, 0}; }
    static Color green() { return {0, 128, 0}; }
    static Color blue() { return {0, 0, 128}; }
    static Color white() { return {128, 128, 128}; }
};

#endif
