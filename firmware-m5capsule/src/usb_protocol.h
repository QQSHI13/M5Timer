#pragma once

#include "config.h"
#include <Arduino.h>

// USB Serial protocol for web sync
// Protocol: PING <- PONG -> [multiple commands] <- PONG (web signals done) -> exit

enum class SyncCommand {
    NONE,
    PING,      // Start connection
    PONG,      // Web signals done, exit sync mode
    SYNC,      // Sync settings
    GET,       // Get current settings
    RESET      // Reset to defaults
};

class USBProtocol {
public:
    void begin();
    void update();  // Call in loop when in sync mode
    bool isConnected() const;
    
    // Check for commands
    bool hasCommand() const { return lastCommand != SyncCommand::NONE; }
    SyncCommand getCommand() { 
        SyncCommand cmd = lastCommand; 
        lastCommand = SyncCommand::NONE; 
        return cmd; 
    }
    TimerSettings getPendingSettings() { return pendingSettings; }
    
    // Responses
    void sendPong();
    void sendAck(bool success);
    void sendSettings(const TimerSettings& s);
    
    void setCurrentSettings(const TimerSettings& s) { settings = s; }
    
private:
    SyncCommand lastCommand = SyncCommand::NONE;
    TimerSettings settings;        // Current active settings
    TimerSettings pendingSettings; // New settings received via SYNC
    String inputBuffer;
    
    void processCommand(const String& cmd);
    TimerSettings parseSettings(const String& data);
};
