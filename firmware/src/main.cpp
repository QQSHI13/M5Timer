#include "config.h"
#include "button.h"
#include "led.h"
#include "timer.h"
#include "storage.h"
#include "usb_hid.h"
#include "power.h"
#include "battery.h"
#include <esp_task_wdt.h>

// Global objects - Note: Buzzer removed in minimal hardware
PomodoroButton button;
LED led;
Timer timer;
Storage storage;
USBHID usbHid;
PowerManager powerManager;
BatteryMonitor battery;

// System state
SystemMode systemMode = SystemMode::TIMER;
TimerSettings settings;
unsigned long syncStartTime = 0;
static constexpr unsigned long SYNC_TIMEOUT = 60000; // 60 seconds

// Track if timer completion was already signaled (loaded from storage)
bool completionSignaled = false;

// Watchdog timeout in seconds
static constexpr uint32_t WDT_TIMEOUT_SECONDS = 30;

// Function declarations
void handleTimerMode();
void handleSyncMode();
void handleSingleClick();
void enterManualSleep();
void updateTimerLED();
void enterSyncMode();
void exitSyncMode();
void handleLowBattery();
void checkSleep();

void setup() {
    // Initialize watchdog early to catch startup issues
    esp_task_wdt_init(WDT_TIMEOUT_SECONDS, true);
    esp_task_wdt_add(NULL); // Add current task
    
    // Check wake cause first
    bool fromDeepSleep = PowerManager::wokeFromDeepSleep();
    
    // Initialize M5AtomS3
    auto cfg = M5.config();
    M5.begin(cfg);
    delay(100);
    
    // Initialize hardware
    button.begin();
    led.begin();
    storage.begin();
    usbHid.begin();
    powerManager.begin();
    battery.begin();
    
    // Feed watchdog after initialization
    esp_task_wdt_reset();
    
    if (fromDeepSleep) {
        // Quick wake from deep sleep - reinitialize
        powerManager.wakeInit();
        Serial.println("\n=== Pomodoro Button (Wake) ===");
    } else {
        // Cold boot
        Serial.println("\n=== Pomodoro Button v1.0 ===");
        Serial.println("M5AtomS3 Lite initialized");
    }
    
    // Load settings
    settings = storage.load();
    
    // Load completionSignaled flag from storage (persisted across deep sleep)
    completionSignaled = storage.loadCompletionFlag();
    
    // Update USB HID with current settings for GET command
    usbHid.setCurrentSettings(settings);
    
    Serial.printf("Settings: work=%dmin, break=%dmin, longBreak=%dmin, sessions=%d, sound=%s\n",
                  settings.workMinutes, settings.breakMinutes, 
                  settings.longBreakMinutes, settings.workSessionsBeforeLongBreak,
                  settings.soundEnabled ? "on" : "off");
    
    // Initialize timer
    timer.begin(settings);
    
    // On cold boot, start timer; on wake from sleep, restore state
    if (!fromDeepSleep) {
        timer.start();
        completionSignaled = false;
        
        // Boot animation
        led.setColor(Color::white());
        delay(200);
        led.setColor(Color::off());
        delay(100);
        led.setColor(Color::green());
        delay(100);
        led.setColor(Color::off());
    }
    
    // Check battery on startup
    battery.update();
    if (battery.isCritical()) {
        handleLowBattery();
        return; // Will deep sleep
    }
    
    Serial.println("Ready!");
}

void loop() {
    // Reset watchdog timer
    esp_task_wdt_reset();
    
    M5.update();  // Update M5AtomS3 state
    
    // Update hardware
    button.update();
    led.update();
    battery.update();
    
    // Check for critical battery
    if (battery.isCritical() && systemMode != SystemMode::SYNC) {
        handleLowBattery();
        return;
    }
    
    if (systemMode == SystemMode::TIMER) {
        handleTimerMode();
    } else {
        handleSyncMode();
    }
    
    // Consider entering sleep to save power
    checkSleep();
}

void handleTimerMode() {
    // Update timer
    timer.update();
    
    // Check for button events
    ButtonEvent event = button.update();
    
    if (event == ButtonEvent::LONG_PRESS) {
        enterSyncMode();
        return;
    }
    
    if (event == ButtonEvent::DOUBLE_CLICK) {
        // Double click enters manual sleep mode
        enterManualSleep();
        return;
    }
    
    if (event == ButtonEvent::SINGLE_CLICK) {
        handleSingleClick();
        completionSignaled = false;  // Reset completion flag on interaction
    }
    
    // Update LED based on timer state
    updateTimerLED();
    
    // Check for timer completion - LED-only feedback (no buzzer in minimal hardware)
    if (timer.isCompleted() && !completionSignaled) {
        completionSignaled = true;
        // Flash LED pattern instead of buzzer (minimal hardware has no buzzer)
        led.setSpinning(true);  // Rainbow spin for timer completion
    }
}

void handleSingleClick() {
    TimerState state = timer.getState();
    
    if (state == TimerState::RUNNING) {
        // Reset timer
        timer.reset();
        // LED flash feedback instead of buzzer (minimal hardware)
        led.setFlashing(true, 1);
        Serial.println("Timer reset");
    } else {
        // Switch mode
        timer.switchMode();
        // LED flash feedback instead of buzzer (minimal hardware)
        led.setFlashing(true, 2);
        
        TimerMode newMode = timer.getMode();
        const char* modeName = "UNKNOWN";
        switch (newMode) {
            case TimerMode::WORK: modeName = "WORK"; break;
            case TimerMode::BREAK: modeName = "BREAK"; break;
            case TimerMode::LONG_BREAK: modeName = "LONG BREAK"; break;
        }
        Serial.printf("Switched to %s\n", modeName);
    }
}

void enterManualSleep() {
    Serial.println("Manual sleep requested (double-click)");
    
    // Visual feedback - LED flash pattern (no buzzer in minimal hardware)
    led.setColor(Color::white());
    delay(100);
    led.setColor(Color::off());
    delay(100);
    led.setColor(Color::white());
    delay(100);
    led.setColor(Color::off());
    
    // Optional: save current timer state
    timer.pause();
    
    Serial.println("Entering deep sleep. Press button to wake.");
    Serial.flush();
    
    // Prepare for sleep
    powerManager.prepareForSleep();
    
    // Enter deep sleep - wake on button press (no timer wake)
    // Using a very long sleep duration since we only want button wake
    powerManager.enterDeepSleep(86400000); // 24 hours (max practical)
    
    // Device resets here after wake - code below is unreachable
    // powerManager.wakeInit(); // REMOVED: Unreachable code after deep sleep
}

void updateTimerLED() {
    TimerMode mode = timer.getMode();
    TimerState state = timer.getState();
    
    // Check battery status - flash if low
    if (battery.isLow() && (millis() / 500) % 2 == 0) {
        // Brief yellow flash for low battery
        led.setColor(Color::yellow());
        return;
    }
    
    // Color based on mode
    Color color;
    switch (mode) {
        case TimerMode::WORK:
            color = Color::red();
            break;
        case TimerMode::BREAK:
            color = Color::green();
            break;
        case TimerMode::LONG_BREAK:
            color = Color::blue();
            break;
        default:
            color = Color::white();
    }
    
    if (state == TimerState::RUNNING) {
        led.setPulsing(true);
        led.setFlashing(false);
        led.setColor(color);
    } else if (state == TimerState::COMPLETED) {
        // Flash white when completed
        led.setPulsing(false);
        led.setFlashing(true, 5);
        led.setColor(Color::white());
    } else {
        // Paused - dim solid
        led.setPulsing(false);
        led.setFlashing(false);
        Color dimColor = {
            static_cast<uint8_t>(color.r / 4),
            static_cast<uint8_t>(color.g / 4),
            static_cast<uint8_t>(color.b / 4)
        };
        led.setColor(dimColor);
    }
}

void enterSyncMode() {
    systemMode = SystemMode::SYNC;
    syncStartTime = millis();
    timer.pause();
    
    Serial.println("=== SYNC MODE ===");
    Serial.println("Connect USB and visit sync page");
    
    // LED feedback: spinning blue for sync mode (no buzzer in minimal hardware)
    led.setSpinning(true);
    led.setColor(Color::blue());
    
    usbHid.sendHello();
}

void handleSyncMode() {
    // Check for timeout
    if (millis() - syncStartTime > SYNC_TIMEOUT) {
        Serial.println("Sync timeout");
        exitSyncMode();
        return;
    }
    
    // Update USB HID
    usbHid.update();
    
    // Check for button press to exit
    ButtonEvent event = button.update();
    if (event == ButtonEvent::SINGLE_CLICK || event == ButtonEvent::LONG_PRESS) {
        Serial.println("Button pressed, exiting sync");
        exitSyncMode();
        return;
    }
    
    // Update spinning animation
    led.update();
    
    // Check for new settings
    if (usbHid.hasNewSettings()) {
        TimerSettings newSettings = usbHid.getSettings();
        
        Serial.printf("New settings: work=%d, break=%d, longBreak=%d, sessions=%d, sound=%s\n",
                      newSettings.workMinutes, newSettings.breakMinutes,
                      newSettings.longBreakMinutes, newSettings.workSessionsBeforeLongBreak,
                      newSettings.soundEnabled ? "on" : "off");
        
        // Save and apply
        storage.save(newSettings);
        settings = newSettings;
        timer.updateSettings(settings);
        usbHid.setCurrentSettings(settings);  // Update USB HID with new settings
        
        usbHid.sendAck(true);
        
        // Success feedback: LED flash (no buzzer in minimal hardware)
        led.setSpinning(false);
        led.setFlashing(true, 3);
        led.setColor(Color::green());
        
        delay(1000);
        exitSyncMode();
    }
}

void exitSyncMode() {
    systemMode = SystemMode::TIMER;
    led.setSpinning(false);
    led.setFlashing(false);
    completionSignaled = false;
    Serial.println("Back to timer mode");
}

void handleLowBattery() {
    Serial.println("CRITICAL: Battery too low!");
    
    // Persist completionSignaled flag before sleeping
    storage.saveCompletionFlag(completionSignaled);
    
    // LED warning pattern: rapid red flashing (no buzzer in minimal hardware)
    for (int i = 0; i < 10; i++) {
        led.setColor(Color::red());
        delay(100);
        led.setColor(Color::off());
        delay(100);
    }
    
    // Prepare for sleep
    powerManager.prepareForSleep();
    
    // Deep sleep for 1 hour - will wake on button press or timer
    // User must replace battery
    powerManager.enterDeepSleep(3600000);
}

void checkSleep() {
    // Don't sleep if sync mode or timer just completed
    if (systemMode == SystemMode::SYNC) return;
    if (timer.isCompleted() && !completionSignaled) return;
    
    // Calculate appropriate sleep duration
    unsigned long remainingMs = timer.getRemainingSeconds() * 1000UL;
    uint32_t sleepMs = powerManager.calculateSleepDuration(remainingMs, timer.getState());
    
    if (sleepMs >= 1000) {  // Only sleep if at least 1 second
        // Persist completionSignaled flag before sleeping
        storage.saveCompletionFlag(completionSignaled);
        
        // Prepare for sleep
        powerManager.prepareForSleep();
        
        // Enter deep sleep
        powerManager.enterDeepSleep(sleepMs);
        
        // Note: If we get here, deep sleep didn't happen (for testing)
        // In real operation, device resets after deep sleep
        // powerManager.wakeInit(); // Only called if deep sleep fails
    }
}