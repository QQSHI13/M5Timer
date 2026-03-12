# Component Placement Guide

## PCB: 24x24mm Extension Board for AtomS3 Lite

## Step-by-Step Assembly Order

### Step 1: SMD Resistors & Capacitors (0603)

| Ref | Value | LCSC Part | Footprint | Position | Notes |
|-----|-------|-----------|-----------|----------|-------|
| R1 | 1kΩ | C21190 | 0603 | Top-left of LEDs | LED current limit |
| R2 | 1kΩ | C21190 | 0603 | Top-right of LEDs | LED current limit |
| R3 | 1.2kΩ | C22790 | 0603 | Near TP4056 pin 2 | Charge current set |
| R4 | 100Ω | C22775 | 0603 | Between G1 and buzzer | Buzzer limit |
| C1 | 10µF | C440198 | 0603 | Near USB VBUS | Input bypass |
| C2 | 10µF | C440198 | 0603 | Near XC6206 VOUT | Output bypass |

**Tip:** Tin one pad, hold component with tweezers, solder other pad.

### Step 2: LEDs (0603)

| Ref | Color | LCSC Part | Position | Notes |
|-----|-------|-----------|----------|-------|
| D1 | Red | C2286 | Top edge, left | CHRG indicator |
| D2 | Green | C2297 | Top edge, right | PWR/STDBY indicator |

**Polarity:** Green dot or notch = cathode (negative). Check datasheet!

### Step 3: ICs

| Ref | Part | LCSC Part | Package | Position | Notes |
|-----|------|-----------|---------|----------|-------|
| U1 | TP4056 | C165948 | SOP-8 | Top-center, near USB | Charging IC |
| U2 | XC6206P332MR | C5446 | SOT-23 | Left-center | 3.3V LDO |
| U3 | DW01A | C276000 | SOT-23-6 | Top-right | Protection IC |
| U4 | FS8205A | C276001 | SOT-23-6 | Right of DW01A | Protection MOSFET |

**IC Pin 1 orientation:** Usually marked with dot or notch. Check footprints!

#### TP4056 Pinout (SOP-8, top view):
```
     ┌────┐
  1  │    │  8
TEMP │ TP │ CE  ← Tie to VCC
  2  │4056│  7
PROG │    │ CHRG ← Red LED
  3  │    │  6
 GND │    │ STDBY ← Green LED
  4  │    │  5
 VIN └────┘ BAT ← Battery+
```

#### XC6206 Pinout (SOT-23, top view):
```
    ┌───┐
VIN │   │ VOUT ← 3.3V out
  1 │   │  2
    └───┘
      3
     GND
```

### Step 4: Through-Hole Components

| Ref | Part | LCSC Part | Position | Notes |
|-----|------|-----------|----------|-------|
| J1 | USB-C 16P | C165948 | Top edge, centered | Charging input |
| J2 | JST-PH-2P | C160331 | Bottom-left | Battery connector |
| SW1 | Slide Switch | C92584 | Bottom-center | Power switch |
| J3 | GROVE 4P | C2884835 | Bottom-right | AtomS3 connection |
| LS1 | Buzzer 3V | C961112 | Right-center | Active buzzer |

**USB-C Pinout (front view, looking into connector):**
```
A1(GND) A4(VBUS) A5(CC)   B9(VBUS) B12(GND)
   │        │       │         │        │
   └────────┴───────┘         └────────┘
            │                      │
          GND plane             5V to TP4056
          (bottom layer)
```

**GROVE 4P Pinout (front view, notch facing left):**
```
Pin 1 (Black/GND)  ──────┐
Pin 2 (Red/3.3V)   ──────┤  GND  │  3.3V  │  G2  │  G1
Pin 3 (Yellow/G2)  ──────┤ Black │  Red   │Yellow│ White
Pin 4 (White/G1)   ──────┘
       ↑
    Notch/key
```

## PCB Layout Diagram (Top View)

```
Y=24mm (Top Edge)
  │
  │    [D1] ○  ○ [D2]          LEDs (Red=CHG, Green=PWR)
  │     │            │
  │    [R1]        [R2]        1kΩ current limit resistors
  │     │            │
  │  ┌───────────────────┐
  │  │      [U1]         │     TP4056 charger (SOP-8)
  │  │    TP4056         │
  │  └───────────────────┘
  │       │    │
  │     [C1]  [R3]             C1=10µF, R3=1.2kΩ
  │
  │         ◎                    
  │       (12,12)              ← M2 MOUNTING HOLE (CENTER)
  │      ┌─────┐                  2.2mm drill, 8mm keepout
  │      │     │
  │      │     │
  │      └─────┘
  │
  │  [U2]          [LS1]       U2=XC6206 LDO, LS1=Buzzer
  │ XC6206           │
  │   │             [R4]       R4=100Ω
  │  [C2]                      C2=10µF
  │
  │ [J2]    [SW1]    [J3]      J2=Battery, SW1=Power, J3=GROVE
  │ JST-PH  Switch   4P
  │
Y=0mm (Bottom Edge)
      │        │        │
     X=4mm    X=12mm   X=20mm

Dimensions: 24mm x 24mm
Origin: Bottom-left corner (0,0)
Mounting: Center hole at (12,12)
```

## Mounting Hole Detail

```
         24mm
    ┌────────────────┐
    │                │
    │    ┌────┐      │
    │    │ ◎  │      │  ← M2 hole (2.2mm drill)
    │    │12,12│     │    at exact center
    │    └────┘      │
    │                │
    │   Extension    │
    │    Board       │
    │   24 x 24mm    │
    └────────────────┘

Keepout: 8mm diameter around hole
         (no copper, no components)
```

## Trace Routing Guide

### Critical Traces (Top Layer)

1. **VBUS (5V from USB)**
   - Width: 0.6mm
   - Route: USB-C A4/B9 → TP4056 VIN (pin 4)
   - Decouple with C1 near TP4056

2. **BAT+ (Battery positive)**
   - Width: 0.6mm
   - Route: TP4056 BAT (pin 5) → Switch → JST-PH pin 1
   - Also to: DW01A VCC, FS8205A VIN, XC6206 VIN

3. **VCC (3.3V output)**
   - Width: 0.6mm
   - Route: XC6206 VOUT (pin 2) → C2 → GROVE pin 2 → LED anodes

4. **G1 to Buzzer**
   - Width: 0.25mm
   - Route: GROVE pin 4 → R4 → Buzzer+

### Bottom Layer

- **Solid ground plane** covering entire layer
- **Thermal relief** on all GND pads
- **Keepout** around M2 mounting hole (4mm radius)

## Via Placement

Place vias for GND connections:
- Near each IC GND pin
- Near connectors
- 4 corners of board
- Via size: 0.6mm drill, 1.0mm pad

## Silkscreen Labels

Top silkscreen (for assembly reference):
```
┌────────────────────────┐
│  + ○  CHG    PWR  ○ +  │  (+ = LED anodes)
│                        │
│      [TP4056]          │
│                        │
│         ◎              │  (M2 hole symbol)
│                        │
│  [LDO]      [Buzzer]   │
│                        │
│ BAT  [SW]      [GROVE] │
│                        │
│   Extension v1.0       │
└────────────────────────┘
```

## Assembly Checklist

- [ ] All 0603 resistors and capacitors soldered
- [ ] LEDs in correct orientation (test with multimeter diode mode)
- [ ] ICs oriented correctly (pin 1 markings)
- [ ] USB-C pins not bridged (check with magnifier)
- [ ] JST-PH polarity correct (+ on pin 1)
- [ ] Switch pins not bridged
- [ ] GROVE connector notch orientation correct
- [ ] Buzzer polarity correct (if polarized)
- [ ] No solder bridges on IC pins
- [ ] Continuity: GND plane complete
- [ ] Isolation: VCC not shorted to GND

## Testing After Assembly

### Before Connecting AtomS3

1. **Visual inspection** under magnifier
2. **Continuity test**:
   - GND to all GND pins: Beep
   - VCC to GND: No beep (open)
   - BAT+ to GND (switch off): No beep
3. **Power test**:
   - Connect battery (switch OFF)
   - Turn switch ON
   - Measure XC6206 VOUT: Should be 3.3V
4. **Charging test**:
   - Plug in USB-C
   - Red LED should light (charging)
   - Measure TP4056 BAT pin: Should be ~4.2V if charging

### First Power-Up

1. Ensure switch is OFF
2. Connect GROVE cable to AtomS3 Lite
3. Turn switch ON
4. AtomS3 LED should light (blue or white boot animation)

## Troubleshooting

| Problem | Check |
|---------|-------|
| No power | Battery voltage, switch position, LDO output |
| Not charging | USB-C soldering, TP4056 orientation, PROG resistor |
| Wrong LED | LED polarity, resistor values |
| Buzzer silent | Buzzer polarity, R4 value, G1 connection |
| AtomS3 won't boot | GROVE pinout, 3.3V on pin 2, GND on pin 1 |

## Files Reference

- `schematic.md` - Circuit diagram
- `BOM.md` - Complete parts list with LCSC numbers
- `PCB-Design.md` - This file
- `pcb-template.json` - EasyEDA import template