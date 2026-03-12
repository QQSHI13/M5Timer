# Schematic Specification

## Physical Design

**Backpack Style Mounting:**
- AtomS3 Lite sits on top (button facing up, USB-C accessible)
- Extension board mounts on bottom (components facing down into backpack)
- **Single M2 screw** through center hole connects both boards
- GROVE 4P cable connects the two PCBs
- Simple plastic backpack holds battery and extension board

```
Side View:

     AtomS3 Lite (24x24mm, 9.5mm height)
    ┌────────────────────┐  ← Top: Button + RGB LED + USB-C
    │  M5Stack AtomS3    │     (USB-C accessible from top)
    │                    │
    │  ◎ M2 hole (center)│  ← Single mounting point
    └────────┬───────────┘
             │ M2x10mm screw (passes through)
    ┌────────┴───────────┐
    │ Extension Board    │  ← Bottom: Components face down
    │ (24x24mm, 1.6mm)   │     into backpack
    │  ├─ TP4056         │
    │  ├─ XC6206         │
    │  └─ Buzzer         │
    └────────┬───────────┘
             │ GROVE 4P cable (connects to AtomS3 Lite)
    ┌────────┴───────────┐
    │ Backpack Case      │  ← Holds extension board + battery
    │ (3D printed)       │
    │  [USB-C] [SW]      │  ← Side access ports
    └────────────────────┘
             │
        [Battery 503040]
```

## MCU: AtomS3 Lite vs Atom Lite

| Feature | Atom Lite | AtomS3 Lite (This Project) |
|---------|-----------|---------------------------|
| SoC | ESP32-PICO-D4 | **ESP32-S3FN8** |
| Flash | 4MB | **8MB** |
| USB | External chip | **Native USB OTG** |
| USB HID | ❌ Not supported | **✅ Native support** |
| Deep Sleep | ~10-20 µA | **~7-10 µA** |
| Battery Life | 3-6 months | **4-8 months** |
| Price | ¥49.9 | **¥49.9** |

**Winner**: AtomS3 Lite has native USB HID, more flash, better battery life — **same price!**

## Pin Differences

| Function | Atom Lite | **AtomS3 Lite** |
|----------|-----------|-----------------|
| Button | GPIO39 | **GPIO41** |
| RGB LED | GPIO27 (SK6812) | **GPIO35 (WS2812C)** |
| GROVE Yellow | G26 | **G2** |
| **GROVE White (Buzzer)** | G32 | **G1** |

## Circuit Overview

The extension board provides battery power management, charging, and feedback. It mounts underneath the AtomS3 Lite using the single center M2 screw hole and connects via GROVE 4P.

## Schematic Diagram (Text)

```
                              USB-C 5V Input
                                   │
                                   ▼
                         ┌─────────────────┐
                         │    TP4056       │
                         │  (Charger IC)   │
                         │                 │
                    5V ──┤VIN         BAT ──┼──┬──► To Battery (JST-PH)
                    GND ──┤GND         GND ──┼──┘    (3.7V LiPo)
                         │        PROG(1.2k) │
                         │        ▲          │
                         │        │          │
                         └────────┴──────────┘
                              │
                              ▼
                         ┌─────────────────┐
                         │  XC6206 LDO     │
                         │  (3.3V Reg)     │
                         │                 │
              3.7V ──────┤VIN         VOUT ─┼──► 3.3V → GROVE Pin 2
              GND ───────┤GND          GND ─┼──► GND → GROVE Pin 1
                         └─────────────────┘
                              │
              ┌───────────────┼───────────────┐
              │               │               │
              ▼               ▼               ▼
         ┌─────────┐    ┌─────────┐    ┌─────────────┐
         │  AtomS3 │    │ Buzzer  │    │ Slide SW    │
         │  Lite   │    │  (3V)   │    │ (On/Off)    │
         │   5V    │◄───┤         │    │             │
         │   GND   │◄───┤   GND   │    │   Battery+ ─┼──◄┐
         │   G1    │───►│   +     │    │   Battery- ─┼──◄┘
         │   G2    │───►│  (spare)│    └─────────────┘
         └─────────┘    └─────────┘
           (4P GROVE)
```

## Mechanical Connection

### Single M2 Mounting (Center)
```
Assembly Order:

1. AtomS3 Lite (top)
   └─ M2 hole centered
          │
          ▼
2. M2x10mm screw (or M2x12mm)
   └─ Passes through AtomS3 Lite center hole
          │
          ▼
3. Extension Board (bottom)
   └─ M2 hole centered (aligned with AtomS3 Lite)
          │
          ▼
4. M2 Nut (or threaded insert in backpack)
   └─ Secures the stack
```

**Stack Height:**
- AtomS3 Lite: 9.5mm
- Gap (GROVE cable): ~2mm
- Extension PCB: 1.6mm
- Components: ~4mm
- **Total stack: ~17mm**

## Detailed Connections

### GROVE 4P Connector

The only connection between AtomS3 Lite and extension board:

```
AtomS3 Lite GROVE          Extension Board GROVE
    ┌─────┐                    ┌─────┐
    │ ● ● │                    │ ● ● │
    │ ● ● │◄──── 4P cable ────►│ ● ● │
    └─────┘                    └─────┘
```

**Pinout:**
| Pin | Color | Function | Direction |
|-----|-------|----------|-----------|
| 1 | Black | GND | Common |
| 2 | Red | 3.3V | Ext → Atom (from LDO) |
| 3 | Yellow | G2 | Ext ← Atom (Spare) |
| 4 | **White** | **G1** | Ext ← Atom (**Buzzer control**) |

**Note**: AtomS3 Lite uses G1/G2 instead of G26/G33 on Atom Lite!

### TP4056 Charger

```
Pin    Function        Connection
─────────────────────────────────────
VIN    5V input        USB-C VBUS (on extension board)
GND    Ground          USB-C GND, system GND
BAT    Battery +       JST-PH pin 1 (via switch)
PROG   Charge current  1.2kΩ resistor to GND
STDBY  Charge done     Green LED (via 1kΩ)
CHRG   Charging        Red LED (via 1kΩ)
```

### XC6206 LDO

```
Pin    Function        Connection
─────────────────────────────────────
VIN    Input           Battery + (3.7V)
VOUT   Output          3.3V rail → GROVE Pin 2 → Atom 5V pin
GND    Ground          System GND → GROVE Pin 1
```

### Buzzer Circuit

```
AtomS3 Lite G1 ──┬──► Buzzer (+)
                 │
                [100Ω] (optional)
                 │
                GND
```

## PCB Layout Notes

### Board Dimensions
- **Size**: **24.0 x 24.0 mm** (matches AtomS3 Lite exactly)
- **Mounting**: **Single M2 hole at center (12mm, 12mm)**
- **Thickness**: 1.6mm standard
- **Layers**: 2 (top signal, bottom ground plane)
- **Total stack height**: ~20mm (AtomS3 9.5mm + PCB 1.6mm + components 4mm + battery 5mm)

### Keep-Out Areas

**Center M2 Mounting Hole:**
- Hole diameter: 2.2mm (M2 screw clearance)
- Keep-out diameter: 8mm (no components, allow screw head/nut)
- This is the ONLY mounting point

**GROVE Connector:**
- Position: Any edge (cable will route to AtomS3 Lite below)
- Recommended: Bottom edge, facing outward
- Clearance: 5mm from board edge for cable bend

### Component Placement (Top View - Components on BOTTOM side)

Components are placed on the **bottom** of the PCB (facing down toward battery):

```
Top Side (facing down):
┌────────────────────────────┐ 24mm
│  [USB-C]     [Red LED]     │ ← Top edge
│        [Green LED]         │
│                            │
│     [TP4056] [DW01A]       │
│              [FS8205A]     │
│                            │
│        ◎                   │ ← Center: M2 hole
│      (8mm keep-out)        │
│                            │
│  [XC6206]    [Buzzer]      │
│                            │
│  [JST-PH]  [Switch]        │ ← Bottom edge
│                            │
│     [HY2.0-4P]             │ ← GROVE connector
└────────────────────────────┘
          24mm
```

### Trace Widths
- Power traces (5V, 3.3V, Battery): 0.6mm minimum
- Signal traces: 0.25mm
- Ground: Use polygon pour on bottom layer

## Assembly Steps

1. **Solder components** to extension board (on bottom side)
2. **Place battery** in backpack case
3. **Connect JST-PH** from battery to extension board
4. **Insert extension board** into backpack (M2 hole aligned)
5. **Connect GROVE cable** between AtomS3 Lite and extension board
6. **Stack AtomS3 Lite** on top (M2 holes aligned)
7. **Insert M2 screw** through center hole
8. **Tighten nut** on underside (inside backpack)

## Firmware Pin Definitions

Update `config.h` for AtomS3 Lite:

```cpp
// M5Stack AtomS3 Lite pin definitions
// GROVE/HY2.0-4P connector: Black(GND), Red(5V), Yellow(G2), White(G1)
#define BUTTON_PIN 41       // Built-in button (GPIO41, not 39!)
#define LED_PIN 35          // Built-in WS2812C RGB LED (GPIO35, not 27!)
#define BUZZER_PIN 1        // Extension board buzzer (G1 - White wire)
```

## 3D Printed Backpack

Simple case that holds:
- Extension board (top of backpack)
- Battery (bottom of backpack)
- Side openings for USB-C and switch
- Center M2 post for mounting

No lid needed - AtomS3 Lite sits directly on top of the stack.
