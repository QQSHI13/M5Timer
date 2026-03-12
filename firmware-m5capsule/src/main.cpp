/*
 * M5Capsule Pomodoro Timer - Web Sync Compatible
 * 
 * State Machine:
 * 1. INITIAL MODE (cold boot/wake from deep sleep/hardware reset)
 *    - Wait 5 seconds (white LED)
 *    - 5s timeout → restore timer → TIMER MODE
 * 
 * 2. TIMER MODE (normal operation)
 *    - Light sleep, wake on RTC
 *    - Click → Reset/Change mode
 *    - Long press (3s) → SYNC MODE (blue LED)
 * 
 * 3. SYNC MODE
 *    - Blue spinning LED
 *    - 15s inactivity timeout
 *    - USB serial communication (Web Serial API compatible)
 *    - Exit → back to TIMER MODE
 * 
 * Protocol:
 *   - Handshake: "HELLO:pomodoro_v2"
 *   - Set: "SET:work=25,break=5,longBreak=15,sessions=4,sound=1"
 *   - Response: "OK" or "ERR:failed"
 */

#include "config.h"
#include "button.h"
#include "timer.h"
#include "storage.h"
#include "usb_protocol.h"
#include "buzzer.h"
#include "led.h"
#include <M5Capsule.h>
#include <M5Unified.h>
#include <Preferences.h>
#include <esp_task_wdt.h>
#include <esp_sleep.h>

// Global objects
PomodoroButton button;
Timer timer;
Storage storage;
USBProtocol usbProtocol;
Buzzer buzzer;
LED led;
Preferences prefs;

// Settings
TimerSettings settings;

// State
SystemMode systemMode = SystemMode::INITIAL;
bool completionSignaled = false;

// Timers
unsigned long initialModeStartTime = 0;
unsigned long syncModeStartTime = 0;
unsigned long lastActivityTime = 0;

// Constants
static constexpr uint32_t INITIAL_MODE_DURATION = 5000;     // 5 seconds
static constexpr uint32_t SYNC_TIMEOUT = 60000;              // 60 seconds max
static constexpr uint32_t SYNC_INACTIVITY = 15000;           // 15 seconds inactivity
static constexpr uint32_t LIGHT_SLEEP_SEC = 1;               // 1 second light sleep
static constexpr uint32_t WDT_TIMEOUT = 30;

// Forward declarations
void handleInitialMode(ButtonEvent event);
void handleTimerMode(ButtonEvent event);
void handleSyncMode(ButtonEvent event);
void handleTimerClick();
void handleLightSleep();
void enterSyncMode();
void exitSyncMode();
void enterDeepSleep();
void saveTimerState();
void restoreTimerState();
void updateTimerLED();
const char* getModeName();

void setup() {
    // Watchdog
    esp_task_wdt_init(WDT_TIMEOUT, true);
    esp_task_wdt_add(NULL);
    
    // Power hold
    pinMode(POWER_HOLD_PIN, OUTPUT);
    digitalWrite(POWER_HOLD_PIN, HIGH);
    
    // Init M5
    auto cfg = M5.config();
    M5.begin(cfg);
    delay(100);
    
    // Init hardware
    button.begin();
    led.begin();
    buzzer.begin();
    storage.begin();
    usbProtocol.begin();
    
    // Load settings
    settings = storage.load();
    usbProtocol.setCurrentSettings(settings);
    
    // Check wake cause
    esp_sleep_wakeup_cause_t wakeCause = esp_sleep_get_wakeup_cause();
    bool fromDeepSleep = (wakeCause != ESP_SLEEP_WAKEUP_UNDEFINED);
    
    Serial.begin(115200);
    Serial.println("\n=== M5Capsule Pomodoro ===");
    
    if (fromDeepSleep) {
        Serial.println("Wake from deep sleep");
        Serial.print("Wake cause: ");
        switch (wakeCause) {
            case ESP_SLEEP_WAKEUP_TIMER: Serial.println("RTC timer"); break;
            case ESP_SLEEP_WAKEUP_GPIO: Serial.println("Button"); break;
            default: Serial.println("Other"); break;
        }
    } else {
        Serial.println("Cold boot / Hardware reset");
    }
    
    // Always start in INITIAL mode (5 second window)
    systemMode = SystemMode::INITIAL;
    initialModeStartTime = millis();
    lastActivityTime = millis();
    
    // Initialize timer (will be restored after initial mode)
    timer.begin(settings);
    
    Serial.println("INITIAL MODE: 5 seconds");
    Serial.println("Timeout → Timer mode (restore state)");
    Serial.println("Long-press (3s) in timer mode → Sync mode");
    
    // Boot feedback
    led.setColor(Color::white());
    if (settings.soundEnabled) buzzer.playClick();
}

void loop() {
    esp_task_wdt_reset();
    M5.update();
    
    // Update subsystems (button once per loop!)
    ButtonEvent event = button.update();
    led.update();
    buzzer.update();
    
    switch (systemMode) {
        case SystemMode::INITIAL:
            handleInitialMode(event);
            break;
        case SystemMode::TIMER:
            handleTimerMode(event);
            break;
        case SystemMode::SYNC:
            handleSyncMode(event);
            break;
    }
}

// ============================================
// INITIAL MODE (5 second window on boot)
// ============================================
void handleInitialMode(ButtonEvent event) {
    // Check for double-click to enter sync mode
    if (event == ButtonEvent::DOUBLE_CLICK) {
        Serial.println("Double-click → SYNC MODE");
        enterSyncMode();
        return;
    }
    
    // Check for 5 second timeout
    unsigned long elapsed = millis() - initialModeStartTime;
    if (elapsed >= INITIAL_MODE_DURATION) {
        Serial.println("Initial mode timeout → TIMER MODE");
        
        // Restore timer state from memory
        restoreTimerState();
        
        // Enter timer mode
        systemMode = SystemMode::TIMER;
        lastActivityTime = millis();
        
        Serial.print("Restored: ");
        Serial.print(getModeName());
        Serial.print(" - ");
        Serial.print(timer.getRemainingSeconds() / 60);
        Serial.print(":");
        Serial.println(timer.getRemainingSeconds() % 60);
        
        // Brief feedback
        led.setColor(Color::green());
        if (settings.soundEnabled) buzzer.playClick();
        delay(200);
        return;
    }
    
    // Solid white LED during initial mode (always on for visibility)
    led.setColor(Color::white());
    
    // Countdown beep every second
    static unsigned long lastBeep = 0;
    if (millis() - lastBeep >= 1000) {
        lastBeep = millis();
        unsigned long remaining = (INITIAL_MODE_DURATION - elapsed) / 1000 + 1;
        Serial.print(remaining);
        Serial.println("...");
        if (settings.soundEnabled) buzzer.beep(50, 800);
    }
}

// ============================================
// TIMER MODE (normal operation)
// ============================================
void handleTimerMode(ButtonEvent event) {
    // Update timer
    timer.update();
    
    // Check for button events
    if (event == ButtonEvent::SINGLE_CLICK) {
        lastActivityTime = millis();
        handleTimerClick();
    }
    
    if (event == ButtonEvent::LONG_PRESS) {
        lastActivityTime = millis();
        Serial.println("Long press → SYNC MODE");
        enterSyncMode();
        return;
    }
    
    // Check timer completion
    if (timer.isCompleted() && !completionSignaled) {
        completionSignaled = true;
        led.setSpinning(true);
        if (settings.soundEnabled) buzzer.playAlarm();
        Serial.println("Timer COMPLETED!");
        saveTimerState();  // Save completion state
    }
    
    // Update LED display
    updateTimerLED();
    
    // Light sleep to save power
    handleLightSleep();
}

void handleTimerClick() {
    Serial.println("Click!");
    completionSignaled = false;
    
    if (timer.getState() == TimerState::RUNNING) {
        // Reset timer
        timer.reset();
        timer.start();
        led.setFlashing(true, 1);
        if (settings.soundEnabled) buzzer.playClick();
        Serial.println("Timer reset");
    } else {
        // Change mode
        timer.switchMode();
        timer.start();
        led.setFlashing(true, 2);
        if (settings.soundEnabled) buzzer.playDoubleClick();
        Serial.print("Mode: ");
        Serial.println(getModeName());
    }
    
    // Save new state
    saveTimerState();
}

void handleLightSleep() {
    // Don't sleep if buzzer playing
    if (buzzer.isPlaying()) return;
    
    // Don't sleep if timer just completed
    if (timer.isCompleted() && !completionSignaled) return;
    
    // Only sleep if timer is running
    if (timer.getState() == TimerState::RUNNING) {
        // LED off during sleep
        led.setColor(Color::off());
        
        // Light sleep using M5.Power (handles RTC/PMIC)
        M5.Power.lightSleep(LIGHT_SLEEP_SEC * 1000000ULL);
        
        // Woke up - restore LED
        updateTimerLED();
    }
}

// ============================================
// SYNC MODE (New Protocol: PING -> PONG -> SYNC -> PONG -> exit)
// ============================================
static bool syncPingReceived = false;

void handleSyncMode(ButtonEvent event) {
    // Update USB Protocol
    usbProtocol.update();
    
    // Check for button press to exit
    if (event != ButtonEvent::NONE) {
        if (event == ButtonEvent::SINGLE_CLICK || event == ButtonEvent::LONG_PRESS) {
            Serial.println("Button press → exit sync");
            exitSyncMode();
            return;
        }
    }
    
    // Process commands
    if (usbProtocol.hasCommand()) {
        lastActivityTime = millis();
        SyncCommand cmd = usbProtocol.getCommand();
        
        switch (cmd) {
            case SyncCommand::PING:
                // Web starts connection - respond with PONG
                syncPingReceived = true;
                Serial.println("PING received, connection active");
                break;
                
            case SyncCommand::PONG:
                // Web signals done - exit sync mode
                Serial.println("PONG received from web, exiting sync mode");
                exitSyncMode();
                return;
                
            case SyncCommand::SYNC: {
                // Apply settings but STAY in sync mode
                if (!syncPingReceived) {
                    Serial.println("SYNC without PING - ignoring");
                    break;
                }
                TimerSettings newSettings = usbProtocol.getPendingSettings();
                settings = newSettings;
                storage.save(newSettings);
                timer.updateSettings(settings);
                usbProtocol.setCurrentSettings(settings);
                
                // Visual feedback but stay in sync
                led.setColor(Color::green());
                led.setSpinning(false);
                if (settings.soundEnabled) buzzer.playSyncTone();
                delay(200);
                led.setSpinning(true);
                led.setColor(Color::blue());
                
                Serial.println("Settings applied, waiting for PONG to exit");
                break;
            }
            
            case SyncCommand::GET:
                // GET sends settings, stay in sync mode
                Serial.println("Settings sent");
                break;
                
            case SyncCommand::RESET: {
                // Reset to defaults but STAY in sync mode
                if (!syncPingReceived) {
                    Serial.println("RESET without PING - ignoring");
                    break;
                }
                settings = TimerSettings();
                storage.save(settings);
                timer.updateSettings(settings);
                usbProtocol.setCurrentSettings(settings);
                
                // Visual feedback but stay in sync
                led.setColor(Color::green());
                led.setSpinning(false);
                if (settings.soundEnabled) buzzer.playSyncTone();
                delay(200);
                led.setSpinning(true);
                led.setColor(Color::blue());
                
                Serial.println("Settings reset, waiting for PONG to exit");
                break;
            }
            
            default:
                break;
        }
    }
    
    // Check for 15s timeout waiting for PING (only before first PING)
    unsigned long elapsed = millis() - syncModeStartTime;
    if (!syncPingReceived && elapsed >= SYNC_INACTIVITY) {
        Serial.println("Sync timeout (no PING) → TIMER MODE");
        exitSyncMode();
        return;
    }
    
    // Check for 60s max timeout
    if (elapsed >= SYNC_TIMEOUT) {
        Serial.println("Sync max timeout → TIMER MODE");
        exitSyncMode();
        return;
    }
    
    // Update LED (spinning blue)
    led.setSpinning(true);
    led.setColor(Color::blue());
    led.update();
    
    // Print countdown every 5 seconds
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint >= 5000) {
        lastPrint = millis();
        if (!syncPingReceived) {
            unsigned long remaining = (SYNC_INACTIVITY - elapsed) / 1000;
            Serial.print("Waiting for PING, timeout in ");
            Serial.print(remaining);
            Serial.println("s");
        }
    }
}

void enterSyncMode() {
    systemMode = SystemMode::SYNC;
    syncModeStartTime = millis();
    lastActivityTime = millis();
    syncPingReceived = false;
    timer.pause();
    
    Serial.println("=== SYNC MODE ===");
    Serial.println("Waiting for PING (15s timeout)...");
    Serial.println("Protocol: PING -> PONG -> SYNC -> PONG -> exit");
    
    led.setSpinning(true);
    led.setColor(Color::blue());
    
    if (settings.soundEnabled) buzzer.playSyncTone();
    // Don't send HELLO - wait for PING from web
}

void exitSyncMode() {
    systemMode = SystemMode::TIMER;
    led.setSpinning(false);
    led.setFlashing(false);
    completionSignaled = false;
    lastActivityTime = millis();
    syncPingReceived = false;
    
    timer.start();
    updateTimerLED();
    
    Serial.println("Back to TIMER MODE");
    saveTimerState();
}

// ============================================
// DEEP SLEEP
// ============================================
void enterDeepSleep() {
    Serial.println("Entering deep sleep...");
    Serial.flush();
    
    // Save state before sleeping
    saveTimerState();
    
    // Turn off LED
    led.setColor(Color::off());
    buzzer.stop();
    
    // Configure button wake (GPIO 42)
    pinMode(WAKE_BUTTON_PIN, INPUT_PULLUP);
    gpio_wakeup_enable((gpio_num_t)WAKE_BUTTON_PIN, GPIO_INTR_LOW_LEVEL);
    esp_sleep_enable_gpio_wakeup();
    
    delay(50);
    esp_deep_sleep_start();
}

// ============================================
// STATE PERSISTENCE
// ============================================
void saveTimerState() {
    prefs.begin("pomodoro", false);
    prefs.putUChar("mode", (uint8_t)timer.getMode());
    prefs.putUChar("state", (uint8_t)timer.getState());
    prefs.putUInt("remaining", timer.getRemainingSeconds());
    prefs.putBool("completed", completionSignaled);
    prefs.end();
}

void restoreTimerState() {
    prefs.begin("pomodoro", true);  // Read-only
    
    // Check if we have saved state
    if (prefs.isKey("mode")) {
        uint8_t modeVal = prefs.getUChar("mode", 0);
        uint8_t stateVal = prefs.getUChar("state", 0);
        uint32_t remaining = prefs.getUInt("remaining", 0);
        
        // Restore timer state
        timer.setMode(static_cast<TimerMode>(modeVal));
        timer.setRemainingSeconds(remaining);
        
        // Only restore RUNNING state if we were running
        // PAUSED/COMPLETED start fresh
        if (stateVal == static_cast<uint8_t>(TimerState::RUNNING)) {
            timer.setState(TimerState::RUNNING);
        } else {
            timer.setState(TimerState::PAUSED);
        }
        
        completionSignaled = prefs.getBool("completed", false);
    }
    
    prefs.end();
}

// ============================================
// HELPERS
// ============================================
void updateTimerLED() {
    TimerMode mode = timer.getMode();
    TimerState state = timer.getState();
    
    Color color;
    switch (mode) {
        case TimerMode::WORK: color = Color::red(); break;
        case TimerMode::BREAK: color = Color::green(); break;
        case TimerMode::LONG_BREAK: color = Color::blue(); break;
        default: color = Color::white();
    }
    
    if (state == TimerState::RUNNING) {
        led.setPulsing(true);
        led.setColor(color);
    } else if (state == TimerState::COMPLETED) {
        led.setPulsing(false);
        led.setFlashing(true, 5);
        led.setColor(Color::white());
    } else {
        led.setPulsing(false);
        Color dim = {(uint8_t)(color.r/4), (uint8_t)(color.g/4), (uint8_t)(color.b/4)};
        led.setColor(dim);
    }
}

const char* getModeName() {
    switch (timer.getMode()) {
        case TimerMode::WORK: return "WORK";
        case TimerMode::BREAK: return "BREAK";
        case TimerMode::LONG_BREAK: return "LONG BREAK";
        default: return "?";
    }
}
