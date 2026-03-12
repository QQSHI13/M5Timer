# PCB Design Specification

## Overview

Extension board for M5Stack AtomS3 Lite Pomodoro Timer. 24x24mm 2-layer PCB.

## Board Specifications

| Parameter | Value |
|-----------|-------|
| **Size** | **24.0 x 24.0 mm** |
| **Height (stack)** | **~9.5mm total** (PCB 1.6mm + components ~4mm + AtomS3 9.5mm) |
| **Layers** | 2 (top signal, bottom ground plane) |
| **Thickness** | 1.6mm |
| **Mounting** | **Single M2 hole at center (12mm, 12mm)** |
| **Connector** | GROVE 4P (HY2.0-4P) |

## Components (Top View - Components on BOTTOM Side)

Components face DOWN toward battery:

```
Top Edge (Y=24)
    │
    │  [USB-C]     [CHG LED]  [PWR LED]
    │     │            │          │
    │  [TP4056]  [DW01A] [FS8205A]
    │     │            │          │
    │     └────────────┴──────────┘
    │
    │        ◎ (12, 12) M2 hole
    │        Center mounting
    │
    │  [XC6206]    [Buzzer]
    │     │            │
    │     └────────────┘
    │
    │  [JST-PH]  [Switch]  [GROVE]
    │     │          │         │
Bottom Edge (Y=0)
```

## Component Placement (Coordinates from bottom-left)

| Component | X (mm) | Y (mm) | Rotation | Side |
|-----------|--------|--------|----------|------|
| USB-C | 6 | 20 | 180° | Bottom |
| TP4056 (U1) | 8 | 16 | 0° | Bottom |
| XC6206 (U2) | 5 | 8 | 90° | Bottom |
| DW01A (U3) | 14 | 16 | 0° | Bottom |
| FS8205A (U4) | 18 | 16 | 0° | Bottom |
| Red LED (CHG) | 16 | 21 | 0° | Bottom |
| Green LED (PWR) | 19 | 21 | 0° | Bottom |
| Buzzer | 16 | 8 | 0° | Bottom |
| JST-PH-2P | 4 | 2 | 0° | Bottom |
| Slide Switch | 12 | 2 | 0° | Bottom |
| GROVE 4P | 20 | 2 | 0° | Bottom |
| R1 (1k LED) | 14 | 19 | 0° | Bottom |
| R2 (1k LED) | 17 | 19 | 0° | Bottom |
| R3 (1.2k PROG) | 10 | 14 | 0° | Bottom |
| R4 (100 Buzzer) | 14 | 10 | 0° | Bottom |
| C1 (10uF) | 4 | 18 | 0° | Bottom |
| C2 (10uF) | 4 | 14 | 0° | Bottom |

## Schematic Netlist

### Power Section

```
USB-C VBUS ──┬── TP4056 VIN
             │
             ├── C1 (10uF) ── GND
             │
             └── (to 5V net)

TP4056 BAT ──┬── Battery+ (via switch)
             │
             ├── DW01A VCC
             │
             ├── FS8205A VCC
             │
             └── XC6206 VIN

TP4056 PROG ── R3 (1.2k) ── GND

TP4056 CHRG ── R1 (1k) ── Red LED ── VCC(3.3V)
TP4056 STDBY ── R2 (1k) ── Green LED ── VCC(3.3V)

Battery- ──┬── DW01A GND
           ├── FS8205A GND
           └── XC6206 GND

XC6206 VOUT ──┬── VCC (3.3V rail)
              │
              ├── C2 (10uF) ── GND
              │
              ├── GROVE Pin 2 (Red)
              ├── LEDs anodes
              └── (to 3.3V net)
```

### Control Section

```
GROVE Pin 1 (Black) ── GND

GROVE Pin 2 (Red) ── VCC (3.3V from LDO)

GROVE Pin 3 (Yellow/G2) ── (spare, NC or future use)

GROVE Pin 4 (White/G1) ── R4 (100Ω) ── Buzzer+ ── VCC
                         └── Buzzer- ── GND (or self-oscillating)
```

## PCB Layers

### Top Layer (Signal)

- 3.3V power rail (thickened trace)
- 5V power rail (from USB)
- Battery+ trace
- Signal traces (G1 to buzzer)
- LED cathode traces

### Bottom Layer (Ground Plane)

- Solid ground plane
- Thermal relief for GND pins
- Keep-away around M2 mounting hole

## Critical Clearances

| Feature | Clearance |
|---------|-----------|
| M2 hole edge to copper | 4mm radius |
| USB-C opening | 9 x 3.5mm cutout |
| Switch opening | 4 x 8mm cutout |
| Trace width (power) | 0.6mm |
| Trace width (signal) | 0.25mm |
| Via size | 0.6mm drill, 1.0mm pad |

## Pinouts

### GROVE 4P Connector (Bottom edge)

| Pin | Color | Signal | PCB Pad |
|-----|-------|--------|---------|
| 1 | Black | GND | Bottom layer pour |
| 2 | Red | 3.3V | Top layer, 0.6mm trace |
| 3 | Yellow | G2 (spare) | NC or small pad |
| 4 | White | G1 (buzzer) | Top layer to R4 |

### JST-PH-2P (Battery, left edge)

| Pin | Signal | Notes |
|-----|--------|-------|
| 1 | Battery+ | Through switch |
| 2 | Battery- | Direct to ground plane |

### USB-C (Top edge, charging only)

| Pin | Signal | Notes |
|-----|--------|-------|
| A1/B12 | GND | Bottom layer |
| A4/B9 | VBUS | 5V input to TP4056 |
| A5 | CC | 5.1k to GND (for power source detection) |
| Others | NC or GND | |

## Design Rules

- Minimum trace width: 0.25mm
- Minimum trace spacing: 0.2mm  
- Minimum via: 0.6mm drill
- Preferred via: 0.8mm drill, 1.5mm pad
- Ground via stitching: every 10mm
- Copper pour clearance: 0.3mm from board edge

## Silkscreen Labels

Top side (for assembly reference):
- "+" near JST-PH pin 1
- "BAT+" / "BAT-" near battery connector
- "USB" near USB-C
- Version/revision mark

## EasyEDA Import Instructions

1. Create new PCB, set size to 24 x 24 mm
2. Place M2 mounting hole at (12, 12) with 4mm keepout
3. Add components per placement table
4. Route per netlist
5. Add ground pour on bottom layer
6. Add silkscreen labels
7. Run DRC

## Gerber Output

Required layers:
- GTL (Top Copper)
- GBL (Bottom Copper)  
- GTO (Top Silkscreen)
- GBO (Bottom Silkscreen - optional)
- GTS (Top Soldermask)
- GBS (Bottom Soldermask)
- GKO (Board Outline)
- GML (Mill/Route)
- TXT (Drill file)

## Notes

- All SMD components on bottom side (facing battery)
- Through-hole connectors on bottom side
- No components on top side (AtomS3 Lite sits directly on PCB)
- Center M2 hole is the ONLY mounting point
- USB-C is for charging ONLY (no data lines needed)