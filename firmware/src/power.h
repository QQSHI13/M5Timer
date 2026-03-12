#pragma once

#include "config.h"
#include "storage.h"

// Power management for extended battery life
// Uses ESP32-S3 deep sleep with timer and button wake sources

class PowerManager {
public:
    void begin();
    
    // Enter light sleep for short periods (ms)
    void lightSleep(uint32_t ms);
    
    // Enter deep sleep - wakes on button press or timer
    // Returns true if waking from deep sleep
    bool enterDeepSleep(uint32_t sleepMs);
    
    // Check if we woke from deep sleep
    static bool wokeFromDeepSleep();
    
    // Get wake cause
    static esp_sleep_wakeup_cause_t getWakeCause();
    
    // Configure wake sources before sleep
    void configureWakeSources(uint32_t timerMs);
    
    // Pre-sleep cleanup
    void prepareForSleep();
    
    // Post-wake initialization
    void wakeInit();
    
    // Calculate sleep duration based on timer state
    // Returns ms to sleep, or 0 if should not sleep
    uint32_t calculateSleepDuration(unsigned long timerRemainingMs, TimerState timerState);
    
private:
    static constexpr uint32_t MAX_SLEEP_MS = 3600000; // 1 hour max sleep
    static constexpr uint32_t MIN_SLEEP_MS = 100;     // 100ms minimum
};