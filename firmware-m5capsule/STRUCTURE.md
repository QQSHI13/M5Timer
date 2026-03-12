# M5Capsule Pomodoro Timer - Final Version

## Quick Start

```bash
cd ~/m5capsule
pio run --target upload
pio device monitor
```

## Usage

| Action | Result |
|--------|--------|
| **Single Click** | Reset timer (if running) / Change mode (if completed) |
| **Long Press (3s)** | Enter deep sleep |
| **RST Button** | Reset device |

## LED Colors

- 🔴 **Red pulsing** = Work timer running
- 🟢 **Green pulsing** = Break timer running
- 🔵 **Blue pulsing** = Long break running
- ⚪ **White flashing** = Timer completed
- ⚫ **Off** = Sleeping

## Sleep Modes

### Light Sleep (During Timer)
- Wakes every 1 second to update timer
- LED shows current state
- Low power but keeps counting

### Deep Sleep (Manual)
- Long press (3s) to enter
- RTC alarm wakes when timer completes
- Button press also wakes
- Very low power (µA range)

## Architecture

```
┌─────────────┐
│   Setup     │
│  Start timer│
└──────┬──────┘
       ▼
┌─────────────┐     Single Click    ┌─────────────┐
│   Running   │ ───────────────────►│   Reset     │
│  (Light     │ ◄───────────────────│   & Restart │
│   Sleep)    │                     └─────────────┘
└──────┬──────┘
       │ Timer = 0
       ▼
┌─────────────┐     Single Click    ┌─────────────┐
│  Completed  │ ───────────────────►│ Change Mode │
│  (Alarm)    │                     │  & Start    │
└──────┬──────┘                     └─────────────┘
       │
       │ Long Press (3s)
       ▼
┌─────────────┐
│  Deep Sleep │◄──── RTC Alarm / Button
│             │
└─────────────┘
```

## File Structure

```
firmware-m5capsule/
├── src/
│   ├── main.cpp      # Main loop, sleep handling
│   ├── config.h      # Pin definitions
│   ├── button.cpp/h  # Simple click/long press detection
│   ├── timer.cpp/h   # Countdown timer logic
│   ├── led.cpp/h     # RGB LED (GPIO21)
│   ├── buzzer.cpp/h  # Buzzer (GPIO2)
│   ├── power.cpp/h   # RTC + deep sleep
│   ├── storage.cpp/h # NVS settings
│   └── usb_hid.cpp/h # USB sync (optional)
└── platformio.ini
```

## Button Logic

```
Button Pressed ──┬──► Held < 3s ──► Released ──► SINGLE_CLICK
                 │
                 └──► Held >= 3s ─────────────────► LONG_PRESS (deep sleep)
```

## Power Consumption

| State | Current |
|-------|---------|
| Active | ~50mA |
| Light sleep | ~5mA |
| Deep sleep | ~10µA |

With typical use: **4-8 months battery life**

## Notes

- RST button resets ESP32 (hardware reset)
- WAKE button (GPIO42) for all user input
- Power LED on module cannot be disabled (hardware)
- RGB LED on GPIO21 can be disabled via `DISABLE_RGB_LED`
