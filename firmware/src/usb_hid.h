#pragma once

#include "config.h"
#include "storage.h"

class USBHID {
public:
    void begin();
    void update();  // Call in loop when in sync mode
    bool isConnected() const;
    bool hasNewSettings() const;
    TimerSettings getSettings();
    void sendAck(bool success);
    void sendHello();
    void setCurrentSettings(const TimerSettings& s) { settings = s; }
    
private:
    bool newSettings = false;
    TimerSettings settings;        // Current active settings for GET command
    TimerSettings pendingSettings; // New settings received via SET command
    String inputBuffer;
    
    void processCommand(const String& cmd);
    TimerSettings parseSettings(const String& data);
};
