# Migration Guide: AtomS3 Lite → M5Capsule

This document describes the changes made when migrating the Pomodoro timer from M5Stack AtomS3 Lite to M5Capsule.

## Hardware Changes

### Pin Mapping

| Function | AtomS3 Lite | M5Capsule | Notes |
|----------|-------------|-----------|-------|
| Power Control | N/A | GPIO46 | Must be HIGH to stay powered |
| Button | GPIO41 | GPIO42 | WAKE button on M5Capsule |
| LED | GPIO35 (WS2812) | GPIO21 (WS2812) | RGB LED on M5Capsule |
| Buzzer | GPIO1 (external) | GPIO2 | Built-in on M5Capsule |
| I2C SDA | - | GPIO8 | For BM8563 RTC |
| I2C SCL | - | GPIO10 | For BM8563 RTC |

### Key Hardware Differences

1. **Power Management**: M5Capsule uses GPIO46 as a power hold pin. The device powers off when GPIO46 goes LOW (unless USB is connected).

2. **No RGB LED**: M5Capsule has no RGB LED. All visual feedback has been replaced with audio feedback using the built-in buzzer.

3. **BM8563 RTC**: M5Capsule has a hardware RTC for accurate timekeeping and timed wakeups from deep sleep.

4. **Built-in Battery**: 250mAh LiPo battery built-in, no external battery needed.

## Software Changes

### Libraries

```ini
; Original (AtomS3)
lib_deps = 
    m5stack/M5AtomS3
    fastled/FastLED
    m5stack/M5Unified

; New (M5Capsule)
lib_deps = 
    m5stack/M5Unified
    m5stack/M5Capsule
    tanakamasayuki/I2C BM8563 RTC
```

### Power Management

**Original (AtomS3)**:
- Simple deep sleep with timer and button wake
- No power hold pin

**New (M5Capsule)**:
```cpp
// Must set GPIO46 HIGH during initialization
pinMode(POWER_HOLD_PIN, OUTPUT);
digitalWrite(POWER_HOLD_PIN, HIGH);

// Power off by setting GPIO46 LOW
void PowerManager::powerOff() {
    digitalWrite(POWER_HOLD_PIN, LOW);
}
```

### Storage

**Original (AtomS3)**:
- Used EEPROM for settings storage
- Custom checksum validation

**New (M5Capsule)**:
- Uses ESP32 NVS (Non-Volatile Storage)
- Better endurance and wear leveling
- Simpler API with built-in error handling

### RTC Wakeups

**Original (AtomS3)**:
- Used ESP32's internal timer for wakeups
- Limited to ~1 hour max sleep duration

**New (M5Capsule)**:
- Uses ESP32's internal timer wakeups (same as original)
- BM8563 RTC is present on board but not used for wakeups in this implementation

### User Feedback

**Original (AtomS3)**:
- RGB LED for visual feedback (colors for modes, pulsing, flashing)
- Buzzer stub (no actual buzzer in minimal hardware)

**New (M5Capsule)**:
- No LED - all feedback via buzzer
- Different tones for different modes:
  - Work mode: High beep (1000Hz)
  - Break mode: Medium beep (800Hz)
  - Long break: Low beep (600Hz)
  - Timer complete: Alarm pattern
  - Sync mode: Two-tone beep

## File Structure

```
firmware-m5capsule/
├── platformio.ini      # PlatformIO config with M5Capsule libraries
├── README.md           # Project documentation
├── MIGRATION.md        # This file
└── src/
    ├── main.cpp        # Main application logic
    ├── config.h        # Pin definitions and enums
    ├── timer.cpp/h     # Pomodoro timer logic (unchanged)
    ├── button.cpp/h    # WAKE button handler (updated pin)
    ├── buzzer.cpp/h    # Buzzer with audio patterns (new)
    ├── power.cpp/h     # Power management with BM8563 RTC (new)
    ├── storage.cpp/h   # NVS-based settings storage (updated)
    └── usb_hid.cpp/h   # USB CDC sync (unchanged)
```

## Behavioral Differences

| Feature | AtomS3 Lite | M5Capsule |
|---------|-------------|-----------|
| Feedback | LED colors + patterns | Buzzer tones + patterns |
| Boot | LED animation | Two-tone beep |
| Timer complete | LED rainbow spin | Alarm beep pattern |
| Low battery | LED yellow flash | Lower priority - no specific indicator |
| Sleep indication | LED off | Brief beep before sleep |

## Building and Flashing

```bash
cd firmware-m5capsule

# Build
pio run

# Flash
pio run --target upload

# Monitor
pio device monitor
```

## Protocol Compatibility

The USB CDC serial protocol remains unchanged:
- `GET` - Retrieve current settings
- `SET:work=25,break=5,longBreak=15,sessions=4,sound=1` - Update settings
- `PING` - Test connectivity
- `RESET` - Reset to defaults

Response format is identical to the original implementation.
