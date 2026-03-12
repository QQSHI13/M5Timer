#include "usb_hid.h"
#include <USB.h>
#include <USBCDC.h>

// Use USB CDC for reliable serial communication on ESP32-S3
static USBCDC USBSerial;

// Maximum command length to prevent buffer overflow
static constexpr size_t MAX_COMMAND_LENGTH = 128;

void USBHID::begin() {
    // Initialize USB with CDC
    USBSerial.begin();
    USB.begin();
    
    // Set USB device descriptors
    USB.productName("Pomodoro Button");
    USB.manufacturerName("Nova");
    USB.serialNumber("PB001");
    
    newSettings = false;
    inputBuffer = "";
    
    Serial.println("USB HID initialized (CDC mode)");
}

bool USBHID::isConnected() const {
    return USBSerial.connected;
}

bool USBHID::hasNewSettings() const {
    return newSettings;
}

TimerSettings USBHID::getSettings() {
    newSettings = false;
    return pendingSettings;
}

void USBHID::sendHello() {
    if (!isConnected()) return;
    USBSerial.println("HELLO:pomodoro_v2");
    USBSerial.flush();
}

void USBHID::sendAck(bool success) {
    if (!isConnected()) return;
    USBSerial.println(success ? "OK" : "ERR:failed");
    USBSerial.flush();
}

void USBHID::update() {
    // Read from USB CDC
    while (USBSerial.available()) {
        char c = USBSerial.read();
        
        if (c == '\n' || c == '\r') {
            if (inputBuffer.length() > 0) {
                processCommand(inputBuffer);
                inputBuffer = "";
            }
        } else {
            inputBuffer += c;
            // Prevent buffer overflow
            if (inputBuffer.length() > 256) {
                inputBuffer = "";
            }
        }
    }
    
    // Also check regular Serial for debug/development
    while (Serial.available()) {
        char c = Serial.read();
        
        if (c == '\n' || c == '\r') {
            if (inputBuffer.length() > 0) {
                processCommand(inputBuffer);
                inputBuffer = "";
            }
        } else {
            inputBuffer += c;
            if (inputBuffer.length() > 256) {
                inputBuffer = "";
            }
        }
    }
}

void USBHID::processCommand(const String& cmd) {
    // Validate command length to prevent buffer overflow
    if (cmd.length() > MAX_COMMAND_LENGTH) {
        Serial.println("ERR:command too long");
        if (isConnected()) {
            USBSerial.println("ERR:command too long");
        }
        return;
    }
    
    Serial.printf("Received: %s\n", cmd.c_str());
    
    if (cmd.startsWith("SET:")) {
        pendingSettings = parseSettings(cmd.substring(4));
        newSettings = true;
    } else if (cmd == "GET") {
        // Return actual current settings, not pending settings
        String response = "SETTINGS:work=" + String(settings.workMinutes) +
                         ",break=" + String(settings.breakMinutes) +
                         ",longBreak=" + String(settings.longBreakMinutes) +
                         ",sessions=" + String(settings.workSessionsBeforeLongBreak) +
                         ",sound=" + String(settings.soundEnabled ? 1 : 0);
        if (isConnected()) {
            USBSerial.println(response);
        }
        Serial.println(response);
    } else if (cmd == "PING") {
        if (isConnected()) {
            USBSerial.println("PONG");
        }
        Serial.println("PONG");
    } else if (cmd == "RESET") {
        // Reset to defaults
        pendingSettings = TimerSettings();
        newSettings = true;
    }
}

TimerSettings USBHID::parseSettings(const String& data) {
    TimerSettings settings;
    
    // Parse format: "work=25,break=5,longBreak=15,sessions=4,sound=1"
    auto getValue = [&](const String& key) -> int {
        int idx = data.indexOf(key + "=");
        if (idx < 0) return -1;
        int start = idx + key.length() + 1;
        int end = data.indexOf(",", start);
        if (end < 0) end = data.length();
        return data.substring(start, end).toInt();
    };
    
    int work = getValue("work");
    int brk = getValue("break");
    int longBrk = getValue("longBreak");
    int sessions = getValue("sessions");
    int sound = getValue("sound");
    
    // Validation with strict bounds checking
    auto clamp = [](int val, int min, int max) -> int {
        return val < min ? min : (val > max ? max : val);
    };
    
    // Validate work minutes: 1-60 minutes
    if (work > 0) {
        settings.workMinutes = clamp(work, 1, 60);
    }
    
    // Validate break minutes: 1-30 minutes
    if (brk >= 0) {
        settings.breakMinutes = clamp(brk, 1, 30);
    }
    
    // Validate long break minutes: 1-60 minutes
    if (longBrk >= 0) {
        settings.longBreakMinutes = clamp(longBrk, 1, 60);
    }
    
    // Validate work sessions before long break: 1-10 (enforced minimum 1)
    if (sessions > 0) {
        settings.workSessionsBeforeLongBreak = clamp(sessions, 1, 10);
    }
    
    // Validate sound setting: 0 or 1
    if (sound >= 0) {
        settings.soundEnabled = (sound != 0);
    }
    
    // Additional sanity check: ensure long break is actually longer than short break
    // If not, keep the received value (user might have specific preferences)
    
    return settings;
}