# Firmware Specification

## Overview

Standalone Pomodoro timer with long break support, deep sleep power management, and USB serial settings sync for **M5Stack AtomS3 Lite**.

## Features

- **Standalone operation**: No WiFi, no PC required after initial setup
- **Three modes**: Work, Short Break, Long Break
- **Session tracking**: After N work sessions, auto-switch to long break
- **Deep sleep**: Ultra-low power between timer ticks (3-6 month battery life)
- **Battery monitoring**: Low battery warning and protective deep sleep
- **USB CDC sync**: Long press to connect to PC and sync settings
- **EEPROM storage**: Settings persist across power cycles
- **Click detection**: Single click (context-aware), Long press (sync mode)
- **Feedback**: RGB LED status + buzzer alarms

## Button Logic

| State | Single Click | Double Click | LED Color |
|-------|--------------|--------------|-----------|
| Work running | Reset timer | **Sleep** | 🔴 Red pulsing |
| Work paused | Switch to Break | **Sleep** | 🔴 Red dim |
| Short Break running | Reset timer | **Sleep** | 🟢 Green pulsing |
| Short Break paused | Switch to Work | **Sleep** | 🟢 Green dim |
| Long Break running | Reset timer | **Sleep** | 🔵 Blue pulsing |
| Long Break paused | Switch to Work | **Sleep** | 🔵 Blue dim |
| **Any** → Long press (3s) | **Enter sync mode** | - | **🔵 Blue spinning** |
| Sync mode | Exit sync mode | Exit sync mode | (previous state) |
| Low battery | - | - | 🟡 Yellow flash |
| Critical battery | - | - | 🔴 Red flash → Sleep |

## Timer Flow

```
Work (25 min) → Short Break (5 min) → Work → Short Break → ...
                      ↓ (every N sessions)
              Long Break (15 min)
                      ↓
                   Work
```

## Power Management

The firmware uses **deep sleep** to achieve 3-6 month battery life:

- **Running timer**: Wakes every second to update countdown
- **Paused timer**: Sleeps for up to 1 hour (button wakes instantly)
- **Critical battery**: Enters protective sleep until charged
- **Current draw**:
  - Active: ~50mA (LED on, buzzer)
  - Deep sleep: ~10µA
  - Average with typical use: <1mA

## Settings (EEPROM)

| Setting | Default | Range |
|---------|---------|-------|
| workMinutes | 25 | 1-60 |
| breakMinutes | 5 | 1-30 |
| longBreakMinutes | 15 | 5-60 |
| workSessionsBeforeLongBreak | 4 | 2-10 |
| soundEnabled | true | true/false |

## USB Serial Sync Protocol

### PC to Device

```
Command format: "SET:work=25,break=5,longBreak=15,sessions=4,sound=1\n"
Response: "OK\n" or "ERR:failed\n"
```

### Device to PC

```
On sync entry: "HELLO:pomodoro_v2\n"
On settings request: "GET\n" → "SETTINGS:work=25,break=5,longBreak=15,sessions=4,sound=1\n"
Ping: "PING\n" → "PONG\n"
```

### How to Sync

1. Long press button (3s) to enter sync mode
2. Connect USB cable
3. Open serial monitor (115200 baud):
   ```bash
   pio device monitor
   ```
4. Type command and press Enter:
   ```
   SET:work=25,break=5,longBreak=15,sessions=4,sound=1
   ```
5. Device responds with "OK"

## Pin Mapping (AtomS3 Lite)

| Function | Pin | Notes |
|----------|-----|-------|
| Onboard button | GPIO41 | Built into AtomS3 Lite (active low) |
| RGB LED | GPIO35 | Built-in WS2812C |
| Buzzer | GPIO1 | Extension board via GROVE White wire (G1) |
| Spare GPIO | GPIO2 | GROVE Yellow wire (G2) |
| Battery ADC | GPIO6 | Internal voltage divider |

## LED Colors

| Mode | Running | Paused |
|------|---------|--------|
| Work | Red pulsing | Red dim (25%) |
| Short Break | Green pulsing | Green dim (25%) |
| Long Break | Blue pulsing | Blue dim (25%) |
| Completed | White flashing | - |
| Sync mode | Blue spinning | - |
| Low battery | Yellow flash | (overlays current mode) |
| Critical | Red flashing | → Sleep |

## Battery Management

### Voltage Thresholds

| Level | Voltage | Action |
|-------|---------|--------|
| Full | 4.2V | 100% indicated |
| Normal | 3.7V - 4.1V | Normal operation |
| Low | 3.5V | Yellow warning flash |
| Critical | <3.3V | Red flash, then protective deep sleep |

### Protective Sleep

When battery is critical, device:
1. Flashes red LED 5 times
2. Beeps if sound enabled
3. Enters deep sleep for 1 hour
4. Will wake on button press (to check if charged)

### Manual Sleep Mode

**Double-click** the button at any time to enter manual sleep:
- LED flashes white briefly
- Two beeps (if sound enabled)
- Enters deep sleep immediately
- **Press button to wake** — device resumes from where it left off

Useful when:
- Putting device away for extended period
- Traveling
- Saving battery when not in use

Timer state is preserved during manual sleep.

## Dependencies

Add to `platformio.ini`:

```ini
lib_deps = 
    m5stack/M5AtomS3 @ ^1.0.0
    fastled/FastLED @ ^3.6.0
    m5stack/M5Unified @ ^0.1.14
```

## File Structure

```
firmware/
├── platformio.ini
└── src/
    ├── main.cpp          # Main state machine with sleep
    ├── config.h          # Pin definitions, constants
    ├── button.cpp/h      # Click detection (single, long press)
    ├── timer.cpp/h       # Pomodoro logic with session tracking
    ├── led.cpp/h         # RGB LED animations
    ├── buzzer.cpp/h      # Alarm sounds
    ├── storage.cpp/h     # EEPROM settings (v2 with long break)
    ├── usb_hid.cpp/h     # USB CDC sync protocol
    ├── battery.cpp/h     # Battery monitoring (NEW)
    └── power.cpp/h       # Deep sleep management (NEW)
```

## Build & Upload

```bash
# Build
pio run

# Upload (connect AtomS3 Lite via USB-C)
pio run --target upload

# Monitor serial output
pio device monitor
```

## Serial Debug Output

```
=== Pomodoro Button v1.0 ===
M5AtomS3 Lite initialized
Battery: 3.85V (65%)
Settings: work=25min, break=5min, longBreak=15min, sessions=4, sound=on
Ready!
[WORK] 24 min remaining (sessions: 0)
[WORK] 23 min remaining (sessions: 0)
...
[WORK] COMPLETED! (sessions: 1)
Auto-switch: SHORT BREAK
[BREAK] STARTED (sessions: 1)
Entering deep sleep for 1000 ms...
```

## First Boot

1. Upload firmware
2. Open serial monitor to verify boot
3. Check battery voltage is displayed
4. Press button to test click detection
5. Long press (3s) to enter sync mode
6. Connect to PC and sync settings via web interface

## Troubleshooting

### Won't enter deep sleep
- Check timer is not in COMPLETED state
- Ensure not in SYNC mode
- Battery must not be critical

### Battery reading incorrect
- First reading after wake may be slightly off (stabilizes after 30s)
- Check voltage divider is correct for your hardware

### USB sync not working
- Ensure using Chrome/Edge (Web Serial API)
- Check USB cable supports data (not charge-only)
- Try direct connection (no USB hub)

## Migration from Atom Lite

If upgrading from original Atom Lite firmware:
1. Update `platformio.ini` board to `m5stack-atoms3`
2. Change library from `M5Atom` to `M5AtomS3`
3. Update pin definitions (BUTTON_PIN 39→41, LED_PIN 27→35)
4. Re-upload firmware
5. Settings will be preserved (EEPROM compatible)
