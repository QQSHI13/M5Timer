# ESP32 Smart Button Project

A standalone Pomodoro timer based on **M5Stack AtomS3 Lite** with USB serial settings sync.

## Overview

| Spec | Value |
|------|-------|
| MCU | M5Stack AtomS3 Lite (ESP32-S3FN8) |
| Power | 3.7V LiPo (500mAh) |
| Runtime | **4-8 months** (deep sleep optimized) |
| Charging | USB-C via TP4056 |
| Feedback | RGB LED + Piezo buzzer |
| Mounting | Backpack style (single M2 screw) |
| Sync | USB CDC Serial |

## Why AtomS3 Lite?

| Feature | Atom Lite | **AtomS3 Lite** |
|---------|-----------|-----------------|
| SoC | ESP32-PICO-D4 | **ESP32-S3FN8** |
| Flash | 4MB | **8MB** |
| USB HID | ❌ No | ✅ **Native CDC** |
| Deep Sleep | ~10-20 µA | **~7-10 µA** |
| Battery Life | 3-6 months | **4-8 months** |
| Price | ¥49.9 | **¥49.9** |

**Winner**: AtomS3 Lite has native USB, more flash, better battery life — **same price!**

## Features

### Standalone Timer
- **Single click**: Reset timer (running) / Switch mode (paused)
- **Double click**: Enter manual sleep mode (press button to wake)
- **Auto-switch**: Work → Short Break → Work → Long Break
- **Session tracking**: Long break after N work sessions
- **LED colors**: 🔴 Work, 🟢 Short break, 🔵 Long break
- **Buzzer**: Alarm when timer completes

### Power Management
- **Deep sleep**: 1-second wake intervals during timer, 1-hour when paused
- **Manual sleep**: Double-click to sleep indefinitely (wake on button press)
- **Battery monitoring**: Low battery warning + protective sleep
- **Wake on button**: Instant wake from any sleep state

### Settings Sync
- Long press (3s): Enter sync mode
- Connect USB — device appears as USB CDC serial
- Settings sync via serial or web interface

## Hardware

Simple **backpack** design with snap-fit + magnet:

```
    AtomS3 Lite (top)   ← Button + USB-C + RGB LED
         │ Snap-fit on GROVE side + Magnet in center
    Extension Board     ← Components face down, metal socket for magnet
         │ GROVE cable
    3D Backpack         ← Battery holder, snap-fit lip, magnet cavity
         │
    LiPo Battery
```

**Mounting:**
- Snap-fit lip on the GROVE connector side (top edge) to hold PCB in place
- 6x3mm neodymium magnet in center cavity, attracting the M2 socket on the AtomS3 Lite
- No screws needed!

**Connection:** GROVE 4P cable

### GROVE 4P Pinout (AtomS3 Lite)
| Pin | Color | Function |
|-----|-------|----------|
| 1 | Black | GND |
| 2 | Red | 3.3V (from LDO) |
| 3 | Yellow | G2 (spare) |
| 4 | **White** | **G1 → Buzzer** |

## Build

1. Order **AtomS3 Lite** (¥49.9)
2. Design PCB (24x24mm, EasyEDA) - see `hardware/schematic.md`
3. Order PCB from JLCPCB (~¥40-50/5pcs)
4. Order components (~¥14) - see `hardware/BOM.md`
5. Order battery 503040 (~¥20)
6. 3D print backpack (~¥20-40) - see `3d-models/backpack.scad`

**Total: ~¥144-184**

## Firmware

### Quick Start

```bash
cd firmware
pio run --target upload
pio device monitor
```

### Key Files

| File | Purpose |
|------|---------|
| `src/main.cpp` | Main state machine with deep sleep |
| `src/power.cpp/h` | Power management (NEW) |
| `src/battery.cpp/h` | Battery monitoring (NEW) |
| `src/timer.cpp/h` | Pomodoro logic |
| `src/button.cpp/h` | Click detection |
| `src/led.cpp/h` | RGB LED control |
| `src/buzzer.cpp/h` | Alarm sounds |
| `src/storage.cpp/h` | EEPROM settings |
| `src/usb_hid.cpp/h` | USB CDC sync |

## Files

| Directory | Contents |
|-----------|----------|
| `hardware/` | Schematic, BOM, component list |
| `3d-models/` | Backpack case (OpenSCAD + STL) |
| `firmware/` | PlatformIO code (M5AtomS3 + deep sleep) |
| `docs/` | Assembly guides |
| `web-sync/` | Web interface for settings |

## Quick Start

1. Solder components to PCB (see `docs/Assembly-Guide.md`)
2. Print `backpack.scad`
3. Assemble: Battery → PCB → AtomS3 Lite, screw together
4. Upload firmware: `pio run --target upload`
5. Long press 3s to enter sync mode, configure settings

## Power Consumption

| State | Current | Battery Life |
|-------|---------|--------------|
| Active (LED on) | ~50mA | ~10 hours |
| Deep sleep | ~10µA | **Years** |
| Typical use | ~0.5mA avg | **4-8 months** |

---
*Project started: 2026-03-06*  
*Updated: 2026-03-07 — Added deep sleep, battery monitoring, power management*
