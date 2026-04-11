#include "sync.h"
#include "storage.h"
#include "buzzer.h"
#include "led.h"

static String buffer = "";

void clearSerialBuffer() {
    buffer = "";
}

void logCommand(const String& cmd) {
    Serial.println("USB CMD: " + cmd);
}

void sendPong() {
    Serial.println("PONG");
}

void sendSettings(const Settings& settings) {
    Serial.println("SETTINGS:" + settings.toString());
}

// Process a single command string. Returns true if should exit sync mode.
static bool processCommand(const String& cmd, Settings& settings, TimerState& timerState, bool& pingReceived) {
    logCommand(cmd);

    if (cmd == "PING") {
        pingReceived = true;
        sendPong();
        Serial.println("PING received, sent PONG");
    }
    else if (cmd == "PONG") {
        Serial.println("Settings synced, exiting");
        return true;  // Exit sync mode
    }
    else if (cmd == "GET") {
        sendSettings(settings);
    }
    else if (cmd.startsWith("SYNC:")) {
        String data = cmd.substring(5);
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
    else if (cmd == "RESET") {
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

    return false;  // Stay in sync mode
}

bool processSerialCommands(Settings& settings, TimerState& timerState, bool& pingReceived) {
    while (Serial.available()) {
        char c = Serial.read();

        // Prevent buffer overflow - limit to 255 chars (leaving room for null terminator)
        if (c != '\n' && c != '\r' && buffer.length() >= 255) {
            // Before resetting, check if there's a complete command in the buffer
            int newlinePos = buffer.indexOf('\n');
            if (newlinePos >= 0) {
                // Process the first complete command
                String cmd = buffer.substring(0, newlinePos);
                cmd.trim();
                if (cmd.length() > 0) {
                    bool shouldExit = processCommand(cmd, settings, timerState, pingReceived);
                    if (shouldExit) {
                        buffer = buffer.substring(newlinePos + 1);
                        return true;
                    }
                }
                // Keep any remaining data after the newline
                buffer = buffer.substring(newlinePos + 1);
            } else {
                // No complete command found - reset buffer
                buffer = "";
            }
            continue;
        }

        if (c == '\n') {
            buffer.trim();
            if (buffer.length() > 0) {
                bool shouldExit = processCommand(buffer, settings, timerState, pingReceived);
                buffer = "";
                if (shouldExit) {
                    return true;  // Exit sync mode
                }
            } else {
                buffer = "";
            }
        } else if (c != '\r') {
            buffer += c;
        }
    }

    return false;  // Stay in sync mode
}
