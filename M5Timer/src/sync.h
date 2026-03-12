#pragma once

#include "config.h"
#include "types.h"

// Process incoming serial commands during sync mode
// Returns true if should exit sync mode (PONG received)
bool processSerialCommands(Settings& settings, TimerState& timerState, bool& pingReceived);

// Send PONG response
void sendPong();

// Send settings string
void sendSettings(const Settings& settings);

// Log command to serial
void logCommand(const String& cmd);
