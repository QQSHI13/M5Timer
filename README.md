# M5Timer ⏱️

A standalone Pomodoro timer for M5Capsule (M5Stack ESP32-S3). No phone needed—just press the button and focus.

![M5Timer](hardware-photo.jpg)

---

## ✨ Features

### Hardware
- **🔘 Single Button Operation** — Press to start/pause/reset
- **💡 RGB LED Feedback** — Visual status indication:
  - 🔴 Red — Work session
  - 🟢 Green — Short break
  - 🔵 Blue — Long break
  - ⚪ White — Timer completed
- **🔊 Passive Buzzer** — Audio alerts when timer completes
- **🔌 USB Configuration** — Configure settings via web interface
- **🔋 Long Battery Life** — Optimized for deep sleep between sessions

### Timer Modes
- **Work** — 25 minutes (configurable)
- **Short Break** — 5 minutes
- **Long Break** — 15 minutes (after 4 work sessions)
- **Auto-progression** — Automatically cycles through Pomodoro sequence

### Web Sync
Connect via USB to configure:
- Work/break durations
- LED brightness
- Buzzer volume
- Session tracking

---

## 🔧 Hardware Requirements

| Component | Specification |
|-----------|---------------|
| **Board** | M5Capsule (ESP32-S3, 8MB Flash, 320KB RAM) |
| **LED** | WS2812 RGB LED (GPIO21) |
| **Button** | WAKE button (GPIO42) |
| **Buzzer** | Passive buzzer (GPIO2) |
| **RTC** | BM8563 I2C RTC (SDA=GPIO8, SCL=GPIO10) |

---

## 🚀 Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) installed
- M5Capsule device
- USB-C cable

### Installation

```bash
# Clone the repository
git clone https://github.com/QQSHI13/M5Timer.git
cd M5Timer

# Build and upload
pio run --target upload

# Open serial monitor (optional)
pio device monitor
```

### Usage

1. **Single Press** — Start/pause timer
2. **Double Press** — Skip to next phase
3. **Long Press (3s)** — Reset timer
4. **Connect USB** — Open `web-sync.html` to configure settings

---

## 🛠️ Technologies

- **Hardware**: M5Capsule (ESP32-S3)
- **Framework**: Arduino / PlatformIO
- **Libraries**:
  - M5Unified — Hardware abstraction
  - Adafruit NeoPixel — LED control
  - BM8563 — RTC support
- **Web Config**: HTML5, JavaScript, Web Serial API

---

## 📂 Project Structure

```
M5Timer/
├── src/
│   └── main.cpp          # Main firmware
├── web-sync.html         # Web configuration interface
├── platformio.ini        # PlatformIO configuration
└── README.md             # This file
```

---

## 🔋 Power Management

The M5Timer is optimized for battery life:
- Deep sleep between button presses
- RTC maintains timer state
- LED brightness adjustable
- Estimated battery life: Several days of intermittent use

---

## 📝 License

This project is licensed under the **GNU General Public License v3.0 (GPL-3.0)**.

See [LICENSE](./LICENSE) for details.

---

## 🙏 Credits

Built with ❤️ by **QQ** and **Nova** ☄️

Powered by [OpenClaw](https://openclaw.ai)

Uses [M5Stack](https://m5stack.com/) hardware and libraries.
