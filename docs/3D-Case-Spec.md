# 3D Backpack Case Design

## Design Overview

Simple **backpack** style case - just the bottom part. AtomS3 Lite sits directly on top, no separate lid needed.

```
Exploded View:

    AtomS3 Lite (24x24mm)
    ┌────────────────────┐  ← Top (no case here)
    │  Button + RGB LED  │     USB-C accessible
    │  ◎ M2 center hole  │
    └────────┬───────────┘
             │ M2x10mm screw
    ┌────────┴───────────┐
    │ Extension Board    │  ← Sits in backpack top
    │ (components down)  │
    └────────┬───────────┘
             │ GROVE cable (inside)
    ┌────────┴───────────┐
    │ 3D Backpack        │  ← Simple box with openings
    │  [USB-C] [SW]      │     Holds battery + PCB
    └────────────────────┘
             │
        [Battery 503040]
```

## Backpack Specifications

### Dimensions
- **Outer**: 26 x 26 x 12 mm
- **Wall thickness**: 1.5mm
- **Internal cavity**: 23.5 x 23.5 x 10 mm

### Features

1. **Top shelf** - Holds extension PCB (24x24mm)
2. **Bottom compartment** - Houses 503040 battery
3. **Center M2 post** - Mounting point for screw
4. **Side openings** - USB-C and switch access

### Openings

**USB-C Opening:**
- Location: Side wall
- Size: 12 x 6 mm
- Position: Top half (aligned with PCB)

**Switch Opening:**
- Location: Side wall (adjacent to USB-C)
- Size: 10 x 4 mm
- Position: Top half

**GROVE Cable Slot:**
- Location: Top edge (optional - for cable management)
- Size: 8 x 3 mm slot

## Assembly

1. **Install battery** in bottom of backpack
2. **Connect JST-PH** to extension board
3. **Place extension board** on top shelf
4. **Connect GROVE cable** to extension board
5. **Route GROVE cable** up to top
6. **Connect AtomS3 Lite** to GROVE cable
7. **Align M2 holes** and insert screw
8. **Tighten nut** on underside

## Print Settings

**Material**: PETG (recommended) or ABS
- PETG: Tough, chemical resistant

**Settings:**
- Layer height: 0.2mm
- Walls: 3
- Infill: 20%
- Supports: Minimal (only for USB/switch openings)
- Orientation: Open side up

## File

- `backpack.scad` - OpenSCAD source
- `backpack.stl` - Exported model

## Cost

3D printing service: ¥20-40
