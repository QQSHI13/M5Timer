# Stack Assembly Diagram

## Overview

Single M2 screw through center hole connects all layers:

```
Side View (cross-section):

                    USB-C
                      │
    ╔═════════════════╧═════════════════╗  ← Top
    ║      M5Stack AtomS3 Lite          ║
    ║         24 x 24 x 9.5mm           ║
    ║                                   ║
    ║    [M2 hole @ center 12,12]       ║
    ║              │                    ║
    ║         ┌────┴────┐               ║
    ╚═════════│  ════   │═══════════════╝
              │  ║██║   │                   ← M2x20mm screw
    ╔═════════│  ║██║   │═══════════════╗
    ║      Extension Board              ║
    ║         24 x 24 x 1.6mm           ║
    ║         (components down)         ║
    ║    [M2 hole @ center 12,12]       ║
    ║              │                    ║
    ╚══════════════╪════════════════════╝
                   │
    ╔══════════════╪════════════════════╗
    ║        HAT Backpack               ║
    ║         28 x 28 x 2mm             ║
    ║    [Center post with M2 thread]   ║
    ║              │                    ║
    ╚══════════════╪════════════════════╝
                   │
              ╔════┴════╗
              ║ Battery ║                   ← 503040 LiPo
              ║30x40x5mm║
              ╚═════════╝
```

## Layer Stackup

| Layer | Component | Size | Mounting |
|-------|-----------|------|----------|
| **Top** | AtomS3 Lite | 24×24×9.5mm | Center M2 hole |
| **Middle** | Extension PCB | 24×24×1.6mm | Center M2 hole |
| **Spacer** | HAT Backpack | 28×28×8mm | Center post |
| **Bottom** | LiPo Battery | 30×40×5mm | Held by clips |

## Mounting Detail

```
Top-down view of mounting hole:

    24mm
    ←──→
    ┌────────────┐  ↑
    │            │  │
    │   ┌────┐   │  │
    │   │ ◎  │   │  │ 24mm
    │   │M2x2.2│  │  │
    │   └────┘   │  │
    │  (12,12)   │  │
    │   CENTER   │  ↓
    └────────────┘
    
Hole: 2.2mm diameter (M2 clearance)
Keepout: 8mm diameter (no copper/components)
```

## Assembly Order

1. **Battery** sits in bottom of backpack
2. **Backpack plate** sits on top of battery
3. **Extension PCB** mounts to 4 corner posts on backpack
   - Components face DOWN toward battery
   - M2 hole aligns with center post
4. **AtomS3 Lite** sits on top of extension PCB
   - M2 hole aligns
5. **M2x20mm screw** goes through all layers
   - Threads into backpack center post (or use nut)

## Clearances

```
Height breakdown:

AtomS3 Lite:     9.5mm  ┐
Gap (GROVE):     2.0mm  │  ← Cable space
Extension PCB:   1.6mm  │
Components:      4.0mm  ├ Total: ~17mm
Backpack plate:  2.0mm  │
Battery:         5.0mm  ┘

Total stack height: ~24mm
```

## Wiring

```
GROVE 4P cable connects AtomS3 to Extension board:

    AtomS3 (top)          Extension (bottom)
    ┌─────────┐           ┌─────────┐
    │ ○ ○ ○ ○ │◄─────────►│ ○ ○ ○ ○ │
    │G V G2 G1│           │G V G2 G1│
    └─────────┘           └─────────┘
    
    G=GND (Black)    V=3.3V (Red)
    G2=GPIO2 (Yellow, spare)
    G1=GPIO1 (White, Buzzer)
```

## 3D View

```
Isometric:

              ┌────────────┐
             ╱   AtomS3    ╲
            ╱    (top)      ╲
           ├──────────────────┤
           │  [USB-C] [Btn]   │
           │                  │
          ╱│◎                ◎│╲
         ╱ │    (center)      │ ╲
        │  ├──────────────────┤  │
        │  │  Extension Board │  │
        │  │   (components    │  │
        │  │     facing down) │  │
        │  │                  │  │
        │  │ [USB-C]          │  │
        │  │ [Switch] [GROVE] │  │
        │  ├──────────────────┤  │
        │  │   HAT Backpack   │  │
        │  │   (plate only)   │  │
        │  └──────────────────┘  │
        │      [Battery]         │
        └────────────────────────┘
              ↑
           M2 screw through center
```

## Key Points

- **Single M2 screw** is the only mechanical connection
- **All boards align** at center hole (12mm, 12mm)
- **GROVE cable** is the only electrical connection
- **No other mounting holes** needed
- **Components face down** toward battery (for protection)
- **USB-C accessible** from top (AtomS3) and side (extension)
