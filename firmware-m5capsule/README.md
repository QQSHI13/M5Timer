# M5Capsule Pomodoro Timer

M5Capsule-based Pomodoro timer with deep sleep power management using BM8563 RTC.

## Hardware

- **Board**: M5Capsule (SKU: K129)
- **SoC**: ESP32-S3FN8 (8MB Flash)
- **Built-in sensors**:
  - BMI270 6-axis IMU (I2C 0x69) - not used
  - SPM1423 MEMS microphone - not used
  - BM8563 RTC (I2C 0x51) - used for timed wakeups
  - Buzzer (GPIO2) - used for audio feedback
  - WAKE button (GPIO42) - user input
- **Power**: 250mAh built-in battery
- **Power control**: GPIO46 (HOLD) - must be HIGH to stay powered

## Pin Mapping

| Function | Pin | Description |
|----------|-----|-------------|
| POWER_HOLD | GPIO46 | Must be HIGH to maintain power |
| WAKE_BUTTON | GPIO42 | Built-in wake button (active LOW) |
| RGB_LED | GPIO21 | WS2812C RGB LED for visual feedback |
| BUZZER | GPIO2 | Built-in buzzer for audio feedback |
| I2C_SDA | GPIO8 | I2C data for RTC |
| I2C_SCL | GPIO10 | I2C clock for RTC |

## Features

### Timer Logic (same as original)
- **Single click**: Reset timer (running) / Switch mode (paused)
- **Double click**: Enter manual sleep mode
- **Auto-switch**: Work → Short Break → Work → Long Break
- **Session tracking**: Long break after N work sessions
- **Buzzer**: Audio feedback for all actions and alarm when timer completes

### Power Management
- **Power on**: Set GPIO46 HIGH during initialization
- **Deep sleep**: Use BM8563 RTC alarm for timer-based wakeups
- **Wake sources**: 
  - WAKE button (GPIO42) - always active
  - RTC alarm - wakes when timer completes
- **Sleep logic**: 
  - Timer running → Sleep for exact remaining time (RTC wakes when done)
  - Timer paused → Sleep for 1 hour or until button pressed
- **Battery life**: Several months with typical Pomodoro usage

### Settings Sync
- Long press (3s) on WAKE button to enter sync mode
- Connect USB - device appears as USB CDC serial
- Settings sync via serial (same protocol as original)

### Storage
- Uses ESP32 NVS (Non-Volatile Storage) for settings
- Persists across deep sleep cycles

## Build & Flash

```bash
cd firmware-m5capsule
pio run --target upload
pio device monitor
```

## Serial Commands (Sync Mode)

- `GET` - Get current settings
- `SET:work=25,break=5,longBreak=15,sessions=4,sound=1` - Update settings
- `PING` - Test connection
- `RESET` - Reset to defaults

## Migration Notes from AtomS3 Lite

| Feature | AtomS3 Lite | M5Capsule |
|---------|-------------|-----------|
| Power control | Always on (external battery) | GPIO46 HOLD |
| Button | GPIO41 | GPIO42 (WAKE) |
| LED | WS2812 RGB LED (GPIO35) | None - use buzzer |
| Buzzer | External (GPIO1) | Built-in (GPIO2) |
| RTC | None (software timer) | BM8563 hardware RTC |
| Storage | EEPROM | NVS |

## Key Differences from Original

1. **No RGB LED**: M5Capsule has no LED, using buzzer for all feedback
2. **Power management**: Must hold GPIO46 HIGH to stay powered
3. **RTC wakeups**: Uses BM8563 for accurate timed wakeups from deep sleep
4. **NVS storage**: Uses ESP32 NVS instead of EEPROM
5. **Built-in buzzer**: Uses GPIO2 buzzer instead of external buzzer
