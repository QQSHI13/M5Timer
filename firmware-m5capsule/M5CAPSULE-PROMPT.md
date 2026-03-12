# M5Capsule Pomodoro Timer - Complete Refactor Specification

## Project Goal
Create a clean, production-ready Pomodoro timer for M5Capsule (M5Stack StampS3) from scratch.

## Hardware
- **Board**: M5Capsule (ESP32-S3, 8MB Flash, 320KB RAM)
- **LED**: WS2812 RGB LED on GPIO21 (FastLED library)
- **Button**: WAKE button on GPIO42 (INPUT_PULLUP, active LOW)
- **Buzzer**: Passive buzzer on GPIO2 (ledc PWM)
- **Power**: GPIO46 must be HIGH to stay powered (M5PM1 PMIC)
- **RTC**: BM8563 I2C RTC (SDA=GPIO8, SCL=GPIO10)

## State Machine

### INITIAL MODE (5 seconds after cold boot/deep sleep wake)
- **Entry**: Power on, hardware reset, or wake from deep sleep
- **LED**: Solid white (no pulsing/blinking)
- **Action**: Wait 5 seconds with countdown beeps
- **Events**:
  - Double-click → SYNC MODE
  - 5s timeout → restore saved timer state → TIMER MODE

### TIMER MODE (normal operation)
- **LED colors**:
  - Work: Red pulsing
  - Break: Green pulsing  
  - Long Break: Blue pulsing
  - Completed: White flashing
- **Power saving**: Light sleep (M5.Power.lightSleep) wakes every second via RTC
- **Button events**:
  - Single-click: Reset timer if running, else change mode
  - Long-press 3s: Enter SYNC MODE (not deep sleep!)

### SYNC MODE (USB configuration)
- **Entry**: Long-press in timer mode OR double-click during initial mode
- **LED**: Blue spinning (rainbow effect)
- **Timeout**: 15 seconds waiting for PING (before connection), unlimited after PING received
- **Exit**: When web sends `PONG` to signal done
- **Protocol** (line-based, 115200 baud):
  ```
  Web → Device: PING\n
  Device → Web: PONG\n
  [Multiple commands allowed]
  Web → Device: GET\n
  Device → Web: SETTINGS:work=25,...\n
  Web → Device: SYNC:work=30,break=5,...\n
  [Apply settings but stay in sync mode]
  
  Web → Device: PONG\n
  [Device exits to TIMER MODE immediately]
  ```
- **Commands**:
  - `PING` - Start connection (device responds PONG)
  - `SYNC:key=value,...` - Apply settings (device stays in sync)
  - `GET` - Get current settings (device stays in sync)
  - `RESET` - Reset to defaults (device stays in sync)
  - `PONG` - Web signals done, device exits sync mode

### DEEP SLEEP MODE
- **Entry**: None from UI (for future use)
- **Wake sources**: Button press (GPIO42)
- **On wake**: Boot into INITIAL MODE (5s white LED)

## Key Implementation Requirements

### Button Handling
- Must support: single-click, double-click, long-press
- Debounce: 50ms
- Double-click window: 400ms
- Long-press threshold: 3000ms
- **CRITICAL**: Call update() ONLY ONCE per loop, pass event to handlers

### LED Control (FastLED)
- Default brightness: 128 (out of 255)
- Animations must be non-blocking (use millis())
- States: solid, pulsing (sine wave), spinning (rainbow), flashing (on/off)

### Buzzer (ledc PWM)
- All sounds non-blocking
- Use ledcAttachPin/ledcWriteTone/ledcDetachPin
- No delay() in sound functions

### Timer Logic
- Count down seconds
- Auto-switch modes: Work → Break → Work → Long Break (every 4 work sessions)
- Save state to flash on changes
- Restore state on boot (after 5s initial mode)

### Power Management
- GPIO46 must be HIGH: `pinMode(46, OUTPUT); digitalWrite(46, HIGH);`
- Light sleep: `M5.Power.lightSleep(1000000)` (1 second, RTC-based)
- USB CDC: Use built-in `Serial` only, do NOT create USBCDC instance

### Storage (Preferences or NVS)
- Settings: workMinutes, breakMinutes, longBreakMinutes, workSessionsBeforeLongBreak, soundEnabled
- Timer state: currentMode, remainingSeconds, isRunning

## File Structure
```
src/
├── main.cpp          # Setup, loop, state machine
├── config.h          # Pins, constants, enums
├── button.h/cpp      # Button class with event detection
├── led.h/cpp         # LED class with animations
├── buzzer.h/cpp      # Non-blocking buzzer
├── timer.h/cpp       # Pomodoro timer logic
├── storage.h/cpp     # Settings and state persistence
└── usb_protocol.h/cpp # Serial command handler
```

## Libraries (platformio.ini)
```ini
[env:m5capsule]
platform = espressif32
board = m5stack-stamps3
framework = arduino
lib_deps = 
    M5Unified
    M5Capsule
    FastLED
```

## Anti-Patterns to Avoid
1. ❌ Creating USBCDC instance - use Serial directly
2. ❌ Calling button.update() multiple times per loop
3. ❌ Using delay() for animations or sounds
4. ❌ Blocking serial reads
5. ❌ Deep sleep on long-press (use sync mode instead)

## Testing Checklist
- [ ] Cold boot shows 5s white LED with beeps
- [ ] Double-click during 5s window enters sync mode (blue spinning)
- [ ] 5s timeout restores timer and enters timer mode
- [ ] Single-click resets/changes timer mode
- [ ] Long-press enters sync mode from timer
- [ ] Sync protocol: PING → PONG → SYNC → PONG → exit
- [ ] Light sleep preserves timer accuracy
- [ ] Settings persist after power cycle

## Serial Output Examples
Boot:
```
=== M5Capsule Pomodoro ===
Cold boot / Hardware reset
INITIAL MODE: 5 seconds
5...
4...
3...
2...
1...
Initial mode timeout → TIMER MODE
Restored: WORK - 24:32
```

Sync:
```
=== SYNC MODE ===
Waiting for PING (15s timeout)...
USB CMD: PING
PING received, sent PONG
USB CMD: SYNC:work=25,break=5,longBreak=15,sessions=4,sound=1
SYNC received
Settings synced, exiting
```
