#include "sync.h"
#include "storage.h"
#include "buzzer.h"
#include "led.h"

static String buffer = "";

void logCommand(const String& cmd) {
    Serial.println("USB CMD: " + cmd);
}

void sendPong() {
    Serial.println("PONG");
}

void sendSettings(const Settings& settings) {
    Serial.println("SETTINGS:" + settings.toString());
}

bool processSerialCommands(Settings& settings, TimerState& timerState, bool& pingReceived) {
    while (Serial.available()) {
        char c = Serial.read();
        
        // Prevent buffer overflow - limit to 255 chars (leaving room for null terminator)
        if (c != '\n' && c != '\r' && buffer.length() >= 255) {
            buffer = "";  // Reset buffer if it gets too long
            continue;
        }
        
        if (c == '\n') {
            buffer.trim();
            if (buffer.length() > 0) {
                logCommand(buffer);
                
                if (buffer == "PING") {
                    pingReceived = true;
                    sendPong();
                    Serial.println("PING received, sent PONG");
                }
                else if (buffer == "PONG") {
                    Serial.println("Settings synced, exiting");
                    buffer = "";
                    return true;  // Exit sync mode
                }
                else if (buffer == "GET") {
                    sendSettings(settings);
                }
                else if (buffer.startsWith("SYNC:")) {
                    String data = buffer.substring(5);
                    settings.fromString(data);
                    saveSettings(settings);
                    
                    // Apply new LED brightness and buzzer volume immediately
                    extern void setLEDBrightness(uint8_t brightness);
                    extern void setBuzzerVolume(uint8_t volume);
                    setLEDBrightness(settings.ledBrightness);
                    setBuzzerVolume(settings.buzzerVolume);
                    
                    // Reset timer with new settings if not running
                    if (!timerState.isRunning) {
                        timerState.reset(settings);
                        saveTimerState(timerState);
                    }
                    
                    Serial.println("SYNC received");
                }
                else if (buffer == "RESET") {
                    settings.reset();
                    saveSettings(settings);
                    timerState.reset(settings);
                    saveTimerState(timerState);
                    playResetSound();
                    // Apply default brightness and volume
                    setLEDBrightness(settings.ledBrightness);
                    setBuzzerVolume(settings.buzzerVolume);
                    Serial.println("Settings reset to defaults");
                }
            }
            buffer = "";
        } else if (c != '\r') {
            buffer += c;
        }
    }
    
    return false;  // Stay in sync mode
}
