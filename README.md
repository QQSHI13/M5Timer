# M5Timer

A standalone Pomodoro timer for M5Capsule (M5Stack StampS3).

## Features

- **Standalone operation** - No phone needed
- **RGB LED feedback** - Red (work), Green (break), Blue (long break), White (completed)
- **Passive buzzer** - Audio alerts when timer completes
- **USB sync** - Configure settings via web interface
- **Long battery life** - Optimized for deep sleep

## Hardware

- **Board**: M5Capsule (ESP32-S3, 8MB Flash, 320KB RAM)
- **LED**: WS2812 RGB LED (GPIO21)
- **Button**: WAKE button (GPIO42)
- **Buzzer**: Passive buzzer (GPIO2)
- **RTC**: BM8563 I2C RTC (SDA=GPIO8, SCL=GPIO10)

## Getting Started

### Web Sync

Use the web interface to configure your timer:

**Tools Suite**: https://qqshi13.github.io/tools-suite/web-sync.html

**Direct**: https://qqshi13.github.io/web-sync.html

### Firmware

```bash
# Build
pio run

# Upload
pio run --target upload

# Monitor
pio device monitor
```

## Usage

| Action | Result |
|--------|--------|
| Single click (running) | Reset timer |
| Single click (paused) | Switch mode (Work ↔ Break) |
| Double click (initial mode) | Enter sync mode |
| Long press (3s) | Enter sync mode |

## License

MIT

---

Built with ❤️ by QQ & Nova ☄️
