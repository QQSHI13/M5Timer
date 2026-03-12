# Quick Start Guide

## Building the Firmware

```bash
cd /home/qq/.openclaw/workspace/esp32-button/firmware

# Install dependencies
pio pkg install

# Build
pio run

# Upload to AtomS3 Lite
pio run --target upload

# Monitor serial output
pio device monitor
```

## First Time Setup

1. **Connect AtomS3 Lite** via USB-C to your computer
2. **Upload firmware** (see above)
3. **Open serial monitor** to verify boot:
   ```
   === Pomodoro Button v1.0 ===
   M5AtomS3 Lite initialized
   Battery: 3.85V (65%)
   Ready!
   ```
4. **Press button** to test single click
5. **Long press (3s)** to test sync mode

## Synchronizing Settings

### Method 1: Serial Monitor

```bash
# Long press button for 3 seconds (LED spins blue)
# Then in serial monitor:

SET:work=25,break=5,longBreak=15,sessions=4,sound=1
```

### Method 2: Web Interface

1. Long press button for 3 seconds
2. Open `web-sync/index.html` in Chrome/Edge
3. Click "Connect Button"
4. Adjust settings
5. Click "Sync Settings"

## LED Reference

| Color | Pattern | Meaning |
|-------|---------|---------|
| 🔴 Red | Pulsing | Work mode running |
| 🔴 Red | Dim | Work mode paused |
| 🟢 Green | Pulsing | Short break running |
| 🟢 Green | Dim | Short break paused |
| 🔵 Blue | Pulsing | Long break running |
| 🔵 Blue | Dim | Long break paused |
| ⚪ White | Flashing | Timer completed |
| 🔵 Blue | Spinning | Sync mode |
| 🟡 Yellow | Brief flash | Low battery warning |
| ⚪ White | Brief flash | Entering sleep mode |
| 🔴 Red | Flashing | Critical battery → sleep |

## Button Controls

| Action | Result |
|--------|--------|
| Single click (running) | Reset timer |
| Single click (paused) | Switch mode (Work ↔ Break) |
| **Double click** | **Enter sleep mode** (press button to wake) |
| Long press 3s | Enter sync mode |
| Click in sync mode | Exit sync mode |

## Battery Life

- **Deep sleep:** ~10µA (years of standby)
- **Active:** ~50mA (LED on)
- **Typical use:** 4-8 months

## Ordering Parts

See `hardware/BOM.md` for complete list with LCSC part numbers.

Quick list:
- 1x M5Stack AtomS3 Lite (¥49.9)
- 1x PCB 24x24mm (JLCPCB, ~¥8/pc)
- 1x LiPo 503040 500mAh (~¥20)
- Components: ~¥14
- 3D print: ~¥20-40

**Total: ~¥144-184**

## Troubleshooting

### Won't compile
- Make sure using `m5stack-atoms3` board, not `m5stack-atom`
- Check PlatformIO is up to date: `pio upgrade`

### Battery reading wrong
- First reading after wake may be off by 0.1V (stabilizes in 30s)
- Check voltage divider on your PCB matches 2:1 ratio

### Won't enter deep sleep
- Timer must not be in COMPLETED state
- Not in SYNC mode
- Battery not critical

### Serial sync not working
- Use Chrome or Edge (Web Serial API)
- Ensure using data-capable USB cable
- Try direct USB connection (no hub)

## Project Files

```
esp32-button/
├── firmware/        # PlatformIO project
├── hardware/        # BOM, schematic
├── 3d-models/       # OpenSCAD + STL
├── docs/            # Assembly guides
└── web-sync/        # Settings web app
```

## Next Steps

1. Order components from LCSC/BOM
2. Order PCB from JLCPCB
3. Print backpack case
4. Assemble hardware
5. Upload firmware
6. Enjoy your Pomodoro timer!
