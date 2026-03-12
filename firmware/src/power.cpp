#include "power.h"
#include "led.h"

extern LED led;
extern SystemMode systemMode;

void PowerManager::begin() {
    // Configure button pin for wake
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    
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
    if (sleepMs == 0 || sleepMs > MAX_SLEEP_MS) {
        sleepMs = MAX_SLEEP_MS;
    }
    
    Serial.printf("Entering deep sleep for %lu ms...\n", sleepMs);
    Serial.flush();
    
    // Configure wake sources
    configureWakeSources(sleepMs);
    
    // Enter deep sleep
    esp_deep_sleep_start();
    
    // Never returns (except in simulation)
    return false;
}

bool PowerManager::wokeFromDeepSleep() {
    return esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_UNDEFINED;
}

esp_sleep_wakeup_cause_t PowerManager::getWakeCause() {
    return esp_sleep_get_wakeup_cause();
}

void PowerManager::configureWakeSources(uint32_t timerMs) {
    // Timer wake (primary wake source)
    esp_sleep_enable_timer_wakeup(timerMs * 1000ULL);
    
    // Button wake using ext0 (single GPIO)
    // Button is active LOW, so wake on LOW level
    esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, BUTTON_WAKEUP_LEVEL);
}

void PowerManager::prepareForSleep() {
    // Turn off LED
    led.setColor(Color::off());
    
    // Note: Buzzer removed - no audio feedback in minimal hardware
    
    // Disable ADC
    adc_power_off();
    
    // Ensure serial is flushed
    Serial.flush();
    
    // Small delay to ensure everything settles
    delay(50);
}

void PowerManager::wakeInit() {
    // Re-enable ADC
    adc_power_on();
    
    // Small delay for stabilization
    delay(10);
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
    
    // If timer is running, sleep until next second tick or timer completion
    if (timerState == TimerState::RUNNING) {
        if (timerRemainingMs <= 1000) {
            // Timer about to complete - sleep until completion
            return timerRemainingMs;
        }
        // Sleep for 1 second to check timer
        return 1000;
    }
    
    // Timer is paused - can sleep longer
    // Wake periodically to check for button presses
    // In deep sleep, button will wake us anyway
    return MAX_SLEEP_MS; // Sleep up to 1 hour when paused
}