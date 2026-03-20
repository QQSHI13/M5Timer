#pragma once

#include "config.h"
#include "types.h"

// Clear any partial data left in the serial receive buffer
void clearSerialBuffer();

// Process incoming serial commands during sync mode
// Returns true if should exit sync mode (PONG received)
bool processSerialCommands(Settings& settings, TimerState& timerState, bool& pingReceived);

// Send PONG response
void sendPong();

// Send settings string
void sendSettings(const Settings& settings);

// Log command to serial
void logCommand(const String& cmd);
