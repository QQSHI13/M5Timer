#include "usb_protocol.h"

// Maximum command length to prevent buffer overflow
static constexpr size_t MAX_COMMAND_LENGTH = 256;

void USBProtocol::begin() {
    lastCommand = SyncCommand::NONE;
    inputBuffer = "";
    Serial.println("USB protocol ready (using built-in Serial)");
}

bool USBProtocol::isConnected() const {
    return Serial;
}

void USBProtocol::sendPong() {
    Serial.println("PONG");
    Serial.flush();
}

void USBProtocol::sendAck(bool success) {
    Serial.println(success ? "OK" : "ERR");
    Serial.flush();
}

void USBProtocol::sendSettings(const TimerSettings& s) {
    String response = "SETTINGS:work=" + String(s.workMinutes) +
                     ",break=" + String(s.breakMinutes) +
                     ",longBreak=" + String(s.longBreakMinutes) +
                     ",sessions=" + String(s.workSessionsBeforeLongBreak) +
                     ",sound=" + String(s.soundEnabled ? 1 : 0);
    Serial.println(response);
    Serial.flush();
}

void USBProtocol::update() {
    // Read from Serial (which is USB CDC on ESP32-S3)
    while (Serial.available()) {
        char c = Serial.read();
        
        if (c == '\n' || c == '\r') {
            if (inputBuffer.length() > 0) {
                processCommand(inputBuffer);
                inputBuffer = "";
            }
        } else {
            inputBuffer += c;
            // Prevent buffer overflow
            if (inputBuffer.length() > MAX_COMMAND_LENGTH) {
                inputBuffer = "";
            }
        }
    }
}

void USBProtocol::processCommand(const String& cmd) {
    // Validate command length
    if (cmd.length() > MAX_COMMAND_LENGTH) {
        Serial.println("ERR:command too long");
        return;
    }
    
    Serial.print("USB CMD: ");
    Serial.println(cmd);
    
    if (cmd == "PING") {
        // Web starts connection - respond with PONG
        lastCommand = SyncCommand::PING;
        sendPong();
        Serial.println("PING received, sent PONG");
        
    } else if (cmd == "PONG") {
        // Web signals it's done - time to exit sync mode
        lastCommand = SyncCommand::PONG;
        Serial.println("PONG received, will exit sync mode");
        
    } else if (cmd.startsWith("SYNC:")) {
        // Sync settings
        pendingSettings = parseSettings(cmd.substring(5));
        lastCommand = SyncCommand::SYNC;
        Serial.println("SYNC received");
        
    } else if (cmd == "GET") {
        // Return current settings
        lastCommand = SyncCommand::GET;
        sendSettings(settings);
        Serial.println("GET received, sent settings");
        
    } else if (cmd == "RESET") {
        // Reset to defaults
        pendingSettings = TimerSettings();
        lastCommand = SyncCommand::RESET;
        Serial.println("RESET received");
        
    } else {
        Serial.print("Unknown cmd: ");
        Serial.println(cmd);
    }
}

TimerSettings USBProtocol::parseSettings(const String& data) {
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
    
    if (work > 0) settings.workMinutes = clamp(work, 1, 60);
    if (brk >= 0) settings.breakMinutes = clamp(brk, 1, 30);
    if (longBrk >= 0) settings.longBreakMinutes = clamp(longBrk, 1, 60);
    if (sessions > 0) settings.workSessionsBeforeLongBreak = clamp(sessions, 1, 10);
    if (sound >= 0) settings.soundEnabled = (sound != 0);
    
    return settings;
}
