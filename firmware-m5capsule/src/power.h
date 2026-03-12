#pragma once

#include "config.h"
#include <Wire.h>
#include <I2C_BM8563.h>

// Power management for M5Capsule using BM8563 RTC
// Handles GPIO46 power hold and RTC IRQ-based deep sleep wakeups

class PowerManager {
public:
    void begin();
    
    // Enter light sleep for short periods (ms)
    void lightSleep(uint32_t ms);
    
    // Enter deep sleep - wakes on button press or RTC alarm
    // The sleepMs parameter is the timer remaining time
    bool enterDeepSleep(uint32_t sleepMs);
    
    // Check if we woke from deep sleep
    static bool wokeFromDeepSleep();
    
    // Get wake cause
    static esp_sleep_wakeup_cause_t getWakeCause();
    
    // Configure wake sources before sleep
    // sleepMs: time until timer completes (0 = no timer wake)
    void configureWakeSources(uint32_t sleepMs);
    
    // Pre-sleep cleanup
    void prepareForSleep();
    
    // Post-wake initialization
    void wakeInit();
    
    // Power off (set GPIO46 LOW)
    void powerOff();
    
    // Set power hold (GPIO46 HIGH)
    void setPowerHold(bool hold);
    
    // Calculate sleep duration based on timer state
    // Returns ms to sleep, or 0 if should not sleep
    uint32_t calculateSleepDuration(unsigned long timerRemainingMs, TimerState timerState);
    
    // RTC functions
    void setRTCAlarmAfterSeconds(uint32_t seconds);
    void clearRTCAlarm();
    
private:
    static constexpr uint32_t MAX_SLEEP_MS = 3600000;  // 1 hour max sleep
    static constexpr uint32_t MIN_SLEEP_MS = 100;      // 100ms minimum
    static constexpr gpio_num_t RTC_IRQ_PIN = GPIO_NUM_5;  // RTC interrupt pin (if connected)
    
    I2C_BM8563 rtc;
    bool rtcInitialized = false;
};
