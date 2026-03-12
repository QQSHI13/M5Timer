# Assembly Guide

## Tools Required

- [ ] Soldering iron (temperature controlled, 300-350°C)
- [ ] Solder wire (0.6-0.8mm, rosin core)
- [ ] Tweezers
- [ ] Wire cutters
- [ ] Multimeter
- [ ] M2 screwdriver
- [ ] Hot glue gun (optional, for battery)
- [ ] USB-C cable (for firmware upload and sync)

## Component Checklist

### Extension Board Components
- [ ] TP4056 (U1) x1
- [ ] XC6206P332MR (U2) x1
- [ ] DW01A (U3) x1 - optional but recommended
- [ ] FS8205A (U4) x1 - optional but recommended
- [ ] 0603 Red LED x1
- [ ] 0603 Green LED x1
- [ ] 0603 Resistors: 1kΩ x2, 1.2kΩ x1, 100Ω x1
- [ ] 0603 Capacitors: 10µF x2
- [ ] USB-C female connector x1
- [ ] JST-PH 2P connector x1
- [ ] HY2.0-4P (GROVE) connector x1
- [ ] Slide switch (SS12D00) x1
- [ ] M2x10mm screw and nut x1

### Other
- [ ] PCB x1
- [ ] M5Stack AtomS3 Lite x1
- [ ] LiPo battery 503040 500mAh x1
- [ ] 3D printed backpack x1
- [ ] GROVE 4P cable (10cm) x1

## Soldering Steps

### Step 1: SMD Components (Resistors & Capacitors)

**Tip**: Tin one pad first, hold component with tweezers, then solder the other pad.

1. **Resistors** (R1, R2 1kΩ; R3 1.2kΩ; R4 100Ω)
   - 0603 package is small, use tweezers
   - No polarity for resistors

2. **Capacitors** (C1, C2 10µF)
   - 0603 ceramic capacitors have no polarity

### Step 2: LEDs

**Polarity matters!**
- Red LED: CHRG (charging)
- Green LED: STDBY (charge complete)
- Marked end of 0603 LED is negative (usually green dot or notch)

### Step 3: IC Chips

**TP4056 (SOP-8)**
```
Pinout (top view, notch on left):
     ┌──────┐
  1  │      │  8
  2  │  TP  │  7
  3  │ 4056 │  6
  4  │      │  5
     └──────┘

1: TEMP (can be left unconnected)
2: PROG → 1.2kΩ → GND
3: GND → GND
4: VIN → USB 5V
5: BAT → Battery (through protection circuit)
6: STDBY → Green LED (1kΩ) → VCC
7: CHRG → Red LED (1kΩ) → VCC
8: CE → VCC (enable)
```

**XC6206 (SOT-23)**
```
Pinout (top view):
   ┌───┐
 1 │   │ 3
    │   │
     └─2─┘

1: VIN → Battery
2: VOUT → 3.3V output
3: GND → GND
```

### Step 4: Connectors

1. **USB-C female**
   - SMD soldering, watch orientation
   - VBUS → TP4056 VIN
   - GND → GND

2. **JST-PH 2P (battery)**
   - Through-hole
   - Watch polarity marks

3. **HY2.0-4P (GROVE)**
   - Notch faces inward
   - Pinout:
     - Pin 1 (Black): GND
     - Pin 2 (Red): 3.3V (from LDO)
     - Pin 3 (Yellow): G2 (spare)
     - Pin 4 (White): G1 (spare)

4. **Slide switch**
   - 3 pins: center is common
   - Wire: Battery+ → Switch → TP4056 BAT

### Step 5: Final Checks

- Verify all solder joints with multimeter
- Check for shorts between power rails
- Ensure no cold solder joints on connectors

## Testing (Before Connecting AtomS3 Lite)

### Multimeter Checks

1. **Short check**
   - Measure 3.3V to GND: should be open (infinite)
   - Measure 5V to GND: should be open

2. **LDO output test**
   - Power with battery or USB
   - Measure LDO VOUT: should be 3.3V

3. **Charging test**
   - Connect USB
   - Red LED should light (charging)

### AtomS3 Lite Connection Test

1. Ensure switch is OFF
2. Insert AtomS3 Lite into GROVE connector
3. Turn switch ON
4. AtomS3 Lite LED should light (blue or white)

## Firmware Upload (First Time)

1. **Connect USB-C** from AtomS3 Lite to computer
2. **Open PlatformIO** (or Arduino IDE)
3. **Compile and upload**
   ```bash
   pio run --target upload
   ```
   Note: ESP32-S3 uses native USB, no additional drivers needed on most systems.
4. **Open serial monitor**
   ```bash
   pio device monitor
   ```
5. Should see boot message:
   ```
   === Pomodoro Button v1.0 ===
   Settings: work=25min, break=5min...
   ```

## Final Assembly (Backpack Style)

### Step 1: Install Battery
1. Place 503040 battery in bottom of 3D backpack
2. Connect JST-PH to extension board

### Step 2: Install Extension Board
1. Place extension board on top shelf of backpack
   - **Note**: Components face DOWN (toward battery)
   - USB-C aligns with side opening
   - Switch aligns with side opening
   - Center M2 hole aligned with post

### Step 3: Connect GROVE Cable
1. Connect one end to extension board GROVE
2. Route cable up through top
3. Connect other end to AtomS3 Lite GROVE

### Step 4: Stack and Screw
1. Place AtomS3 Lite on top (button up)
2. Align center M2 holes
3. Insert M2x10mm screw through both boards
4. Tighten nut from bottom (inside backpack)

### Step 5: Done!
No lid needed - AtomS3 Lite sits exposed on top.

## Final Dimensions

- **Overall size**: 26 x 26 x 20 mm
- **Weight**: ~30g (with battery)
- **USB access**: Top (AtomS3 Lite USB-C)
- **Charging**: Side (extension board USB-C)

## Function Test

| Test | Action | Expected Result |
|------|--------|-----------------|
| Power on | Turn on switch | AtomS3 Lite LED lights |
| Single click | Press button | Start/pause timer |
| Long press | Hold 3 seconds | Enter sync mode (blue spin) |
| Charging | Plug in side USB-C | Red LED lights |
| Timer complete | Wait for timer end | LED rainbow spin |

## Troubleshooting

### AtomS3 Lite won't light
- Check battery voltage
- Check switch is ON
- Measure GROVE Pin 2 voltage (should be 3.3V)
- Check LDO output

### Won't charge
- Check USB-C soldering
- Check TP4056 wiring
- Measure TP4056 BAT pin voltage

### LED not working
- Check AtomS3 Lite seating
- Verify GROVE cable connection
- Check for power on LED pin (GPIO35)

### Sync fails
- Ensure long press enters sync mode (blue spin)
- Check USB connection
- Use Chrome/Edge browser (for web sync)

## Settings Sync

### Method 1: Serial Monitor (Recommended)

1. Long press button 3 seconds (blue spin)
2. Connect USB to computer
3. Open PlatformIO serial monitor:
   ```bash
   pio device monitor
   ```
4. Type command:
   ```
   SET:work=25,break=5,longBreak=15,sessions=4,sound=1
   ```
5. Device replies "OK" when saved

### Method 2: Web Interface

1. Long press to enter sync mode
2. Open `web-sync/index.html` in Chrome/Edge
3. Click "Connect Button"
4. Adjust and sync

### Settings Format

```
SET:work=25,break=5,longBreak=15,sessions=4,sound=1
```

- `work=25` - Work duration (min, 1-60)
- `break=5` - Short break (min, 1-30)
- `longBreak=15` - Long break (min, 5-60)
- `sessions=4` - Work sessions before long break (2-10)
- `sound=1` - Sound on/off (0=off, 1=on)

---

## Safety Notes

⚠️ **LiPo Battery Safety**
- Don't puncture or crush battery
- Don't short positive and negative terminals
- Use proper charger
- Stop using if battery swells

⚠️ **Soldering Safety**
- Don't overheat iron (keep ~320°C)
- Don't solder too long (< 3 seconds per joint)
- Work in ventilated area

## Done!

You now have a standalone Pomodoro timer button!

**Next**: Long press 3 seconds to enter sync mode and set your work preferences.
