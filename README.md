# M5Timer ⏲️

A hardware Pomodoro timer for the M5Capsule. Physical buttons, LED feedback, and web sync.

![M5Timer Photo](photo.jpg)

## ✨ Features

- **🔘 Physical Controls** — Real buttons for start/pause and mode switching
- **💡 LED Feedback** — Color-coded LEDs show current mode (Work/Break)
- **🔊 Audio Alerts** — Buzzer notifications when timer completes
- **🌐 Web Sync** — Configure settings via USB from your browser
- **💾 Persistent Storage** — Settings saved to flash memory
- **🔋 Battery Efficient** — Light sleep mode for long battery life
- **⏰ RTC Support** — Accurate timing even when device sleeps

## 🚀 Quick Start

### Hardware Requirements
- M5Capsule (ESP32-S3 based)
- USB-C cable

### Software Setup
```bash
# Clone the repository
git clone https://github.com/QQSHI13/M5Timer.git
cd M5Timer

# Build and flash
pio run --target upload

# Monitor serial output
pio device monitor
```

### Web Configuration
1. Connect M5Timer to your computer via USB
2. Open https://qqshi13.github.io/M5Timer/web-sync.html
3. Click "Connect" and select your device
4. Adjust settings and sync

## 🛠️ Tech Stack

- **Firmware:** C++ (Arduino/ESP32 framework)
- **Hardware:** M5Capsule (ESP32-S3, BM8563 RTC, WS2812 LED)
- **Web UI:** HTML, JavaScript (Web Serial API)
- **Build:** PlatformIO

## 📖 How to Use

### Physical Controls
| Action | Button Press |
|--------|-------------|
| Start/Pause | Single press |
| Switch Mode | Double press |
| Enter Sync | Hold 2 seconds |

### LED Colors
| Color | Mode |
|-------|------|
| 🔴 Red | Work (25 min) |
| 🟢 Green | Short Break (5 min) |
| 🔵 Blue | Long Break (15 min) |

## 📝 Why I Built This

I love the Pomodoro technique, but phone apps are too distracting. I wanted a dedicated device that:
- Has no notifications except the timer
- Works without my phone
- Feels satisfying to use
- Can be customized via web

## 🐛 Known Issues

- Web Serial API only works in Chrome/Edge
- Battery life depends on LED brightness settings

## 🔮 Future Plans

- [ ] Vibration motor support
- [ ] Multiple timer profiles
- [ ] WiFi sync for remote configuration
- [ ] E-ink display version

## 📄 License

This project is licensed under the [GPL-3.0 License](LICENSE).

## 🙏 Credits

Built with ❤️ by [QQ](https://github.com/QQSHI13) & [Nova ☄️](https://openclaw.ai)

Powered by [OpenClaw](https://openclaw.ai)

---

**⭐ Star this repo if you find it useful!**
