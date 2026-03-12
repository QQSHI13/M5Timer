#include "power.h"
#include "buzzer.h"
#include "led.h"

extern Buzzer buzzer;
extern LED led;
extern SystemMode systemMode;

void PowerManager::begin() {
    // Configure power hold pin - MUST be set HIGH to stay powered
    pinMode(POWER_HOLD_PIN, OUTPUT);
    digitalWrite(POWER_HOLD_PIN, HIGH);
    
    // Initialize I2C for RTC
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    
    // Initialize BM8563 RTC
    rtc.begin();
    rtcInitialized = true;
    
    // Clear any pending alarm
    clearRTCAlarm();
    
    // Configure WAKE button pin for wake
    pinMode(WAKE_BUTTON_PIN, INPUT_PULLUP);
    
    // Log wake cause on startup
    if (wokeFromDeepSleep()) {
        auto cause = getWakeCause();
        const char* causeStr = "unknown";
        switch (cause) {
            case ESP_SLEEP_WAKEUP_TIMER: causeStr = "timer"; break;
            case ESP_SLEEP_WAKEUP_EXT0: causeStr = "button (ext0)"; break;
            case ESP_SLEEP_WAKEUP_EXT1: causeStr = "button (ext1)"; break;
            case ESP_SLEEP_WAKEUP_GPIO: causeStr = "gpio"; break;
            case ESP_SLEEP_WAKEUP_UART: causeStr = "uart"; break;
            default: break;
        }
        Serial.printf("Woke from deep sleep: %s\n", causeStr);
    }
}

void PowerManager::lightSleep(uint32_t ms) {
    esp_sleep_enable_timer_wakeup(ms * 1000ULL);
    esp_light_sleep_start();
}

bool PowerManager::enterDeepSleep(uint32_t sleepMs) {
    // For RTC-based wakeup, we don't use ESP32 timer
    // The RTC alarm will trigger the wake via interrupt
    
    Serial.printf("Entering deep sleep, timer will wake in %lu ms...\n", sleepMs);
    Serial.flush();
    
    // Configure wake sources (RTC alarm + button)
    configureWakeSources(sleepMs);
    
    // Enter deep sleep
    esp_deep_sleep_start();
    
    // Never returns
    return false;
}

bool PowerManager::wokeFromDeepSleep() {
    return esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_UNDEFINED;
}

esp_sleep_wakeup_cause_t PowerManager::getWakeCause() {
    return esp_sleep_get_wakeup_cause();
}

void PowerManager::configureWakeSources(uint32_t timerMs) {
    // Set RTC alarm for timer wake (if we have a timer running)
    if (rtcInitialized && timerMs > 0) {
        // Convert milliseconds to seconds for RTC alarm
        uint32_t sleepSeconds = timerMs / 1000;
        if (sleepSeconds > 0) {
            // Cap at reasonable limits (1 second min, 24 hours max)
            if (sleepSeconds < 1) sleepSeconds = 1;
            if (sleepSeconds > 86400) sleepSeconds = 86400;
            
            setRTCAlarmAfterSeconds(sleepSeconds);
        }
    } else {
        // No timer - clear any pending alarm
        clearRTCAlarm();
    }
    
    // Button wake using ext0 (WAKE button is active LOW)
    esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKE_BUTTON_PIN, 0);
    
    // Note: The BM8563 RTC alarm will pull the IRQ pin low when triggered
    // If this pin is connected to the ESP32, we should also enable ext1 wake for it
    // On M5Capsule, the RTC IRQ may be connected to GPIO5 or another pin
    // For now, we also enable ESP32 timer as a backup wake source
    if (timerMs > 0 && timerMs <= 3600000) {
        // Use ESP32 timer as backup (max 1 hour)
        esp_sleep_enable_timer_wakeup(timerMs * 1000ULL);
    }
}

void PowerManager::prepareForSleep() {
    // Turn off LED
    led.setColor(Color::off());
    
    // Turn off buzzer
    buzzer.stop();
    
    // Ensure serial is flushed
    Serial.flush();
    
    // Small delay to ensure everything settles
    delay(50);
}

void PowerManager::wakeInit() {
    // Ensure power hold is set
    digitalWrite(POWER_HOLD_PIN, HIGH);
    
    // Clear RTC alarm on wake
    clearRTCAlarm();
    
    // Small delay for stabilization
    delay(10);
}

void PowerManager::powerOff() {
    Serial.println("Powering off...");
    Serial.flush();
    
    // Clear RTC alarm
    clearRTCAlarm();
    
    // Set GPIO46 LOW to power off (only works when no USB power)
    digitalWrite(POWER_HOLD_PIN, LOW);
    
    // If still powered (USB connected), enter deep sleep indefinitely
    delay(100);
    Serial.println("USB power detected, entering deep sleep instead");
    esp_sleep_enable_ext0_wakeup((gpio_num_t)WAKE_BUTTON_PIN, 0);
    esp_deep_sleep_start();
}

void PowerManager::setPowerHold(bool hold) {
    digitalWrite(POWER_HOLD_PIN, hold ? HIGH : LOW);
}

uint32_t PowerManager::calculateSleepDuration(unsigned long timerRemainingMs, TimerState timerState) {
    // Don't sleep if timer is completed (need to notify user)
    if (timerState == TimerState::COMPLETED) {
        return 0;
    }
    
    // Don't sleep if in sync mode
    if (systemMode == SystemMode::SYNC) {
        return 0;
    }
    
    // If timer is running, return the exact remaining time
    // The RTC will wake us when the timer completes
    if (timerState == TimerState::RUNNING) {
        return timerRemainingMs;
    }
    
    // Timer is paused - sleep for a long time (1 hour)
    // Button press will wake us
    return MAX_SLEEP_MS;
}

// ============================================
// RTC Functions
// ============================================

void PowerManager::setRTCAlarmAfterSeconds(uint32_t seconds) {
    if (!rtcInitialized) return;
    
    // Cap at reasonable limits
    if (seconds < 1) seconds = 1;
    if (seconds > 86400) seconds = 86400;
    
    // Set RTC alarm using the simple seconds API
    // The library handles calculating the alarm time internally
    rtc.SetAlarmIRQ((int)seconds);
    
    Serial.printf("RTC alarm set for %lu seconds\n", seconds);
}

void PowerManager::clearRTCAlarm() {
    if (!rtcInitialized) return;
    rtc.clearIRQ();
}
