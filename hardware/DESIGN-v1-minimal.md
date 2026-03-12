# ESP32 Pomodoro Button - Minimal Design (V1)

## Overview

Ultra-minimal extension board for M5Stack AtomS3 Lite. Just power regulation — no charging, no buzzer.

**Battery system:** Swappable battery packs (2x). Charge externally, swap when dead.

---

## Hardware Architecture

### Extension Board Components (2 total)

| Component | Function | Package |
|-----------|----------|---------|
| **XC6206P332MR** | 3.3V LDO regulator | SOT-23 |
| **JST-PH-2P** | Battery connector (female) | Through-hole or SMD |

### Power Flow

```
Battery Pack (3.7V) ──► JST-PH ──► XC6206 VIN
                                        │
                                        ▼
                                   VOUT (3.3V)
                                        │
                    ┌───────────────────┴───────────────────┐
                    │                                       │
                    ▼                                       ▼
              AtomS3 5V pin                            AtomS3 GND
              (power input)                            (ground)
```

---

## Battery Packs (×2)

Each pack contains:
- **503040 LiPo** 3.7V 500mAh (with built-in protection circuit)
- **JST-PH male plug** (mates with board)
- **3D printed case** (snaps onto extension board)

### Charging
- Charge externally using USB LiPo charger (TP4056 module)
- Charge time: ~1 hour at 500mA
- Swap packs when one dies

---

## PCB Specifications

| Parameter | Value |
|-----------|-------|
| Size | 24.0 × 24.0 mm |
| Layers | 2 |
| Thickness | 1.6mm |
| Mounting | Single M2 hole at center (12,12) |
| Keep-out | 8mm diameter around center hole |

### Component Placement

```
Top View (looking down at extension board):

        24mm
    ┌─────────────┐
    │             │
    │   ◎         │  ← M2 mounting hole (center)
    │  (8mm KO)   │
    │             │
    │  [XC6206]   │  ← Center
    │             │
    │             │
[JST-PH]      [HY-2.0-4P]  ← Side: battery | Top: to AtomS3 (GROVE)
    │             │
    └─────────────┘

Side View (stack):

     AtomS3 Lite (top)
          │
     ┌────┴────┐  ← HY-2.0-4P connector (plugs into AtomS3 GROVE)
     │ Extension│
     │  Board   │  ← 1.6mm PCB
     │  [XC6206]│
     └────┬────┘
          │ JST-PH (side-facing)
     ┌────┴────┐
     │ Battery │  ← Snaps on from side
     │  Pack   │
     └─────────┘
```

---

## Pin Connections

### Extension Board → AtomS3 Lite (via HY-2.0-4P)

| Extension | AtomS3 | Function |
|-----------|--------|----------|
| Pin 1 (Black) | GND | Ground |
| Pin 2 (Red) | 5V | 3.3V power input (from LDO) |
| Pin 3 (Yellow) | G2 | GPIO2 (spare/not used) |
| Pin 4 (White) | G1 | GPIO1 (spare/not used) |

### Battery → Extension Board

| Battery | JST-PH | Extension |
|---------|--------|-----------|
| 3.7V+ | Pin 1 | XC6206 VIN |
| GND | Pin 2 | GND |

---

## Bill of Materials (Per Unit)

### Extension Board PCB

| Item | Qty | Price (CNY) | Source |
|------|-----|-------------|--------|
| XC6206P332MR | 1 | ¥0.35 | LCSC C5446 |
| JST-PH-2P (female) | 1 | ¥0.35 | LCSC C160331 |
| HY-2.0-4P (male) | 1 | ¥0.45 | LCSC C2884835 |
| PCB 24×24mm 2-layer | 5 | ¥7.00/5pcs | JLCPCB |
| **Subtotal** | — | **~¥8.50** | — |

### Battery Packs (×2)

| Item | Qty | Price (CNY) | Source |
|------|-----|-------------|--------|
| 503040 LiPo (protected) | 2 | ¥20 each | Taobao |
| JST-PH connector (male) | 2 | ¥0.35 each | LCSC |
| 3D printed case | 2 | ~¥5 each | JLCPCB 3D |
| **Subtotal** | — | **~¥50** | — |

### Main Board

| Item | Qty | Price (CNY) | Source |
|------|-----|-------------|--------|
| M5Stack AtomS3 Lite | 1 | ¥49.90 | Taobao/M5Stack |

### External Charger (Shared)

| Item | Qty | Price (CNY) | Source |
|------|-----|-------------|--------|
| TP4056 USB charger module | 1 | ¥3-5 | Taobao |

### Total Cost

| Component | Cost |
|-----------|------|
| Extension board | ¥8.50 |
| Battery packs (2×) | ¥50 |
| AtomS3 Lite | ¥50 |
| External charger | ¥4 |
| **Total** | **~¥112.50** |

---

## Battery Life

| Usage Pattern | Duration |
|---------------|----------|
| Deep sleep only | ~4.7 years |
| Light use (1h/day) | ~10 days |
| Normal use (2h/day) | **~4-5 days** |
| Heavy use (4h/day) | ~2-3 days |

**Workflow:** Use Pack A → Dies → Swap to Pack B → Charge Pack A → Repeat

---

## Firmware Changes (V1)

### Removed Features
- Buzzer/alarm sounds
- USB charging detection
- Charge status LEDs

### Retained Features
- Pomodoro timer logic
- RGB LED visual feedback (replaces buzzer)
- Deep sleep power management
- Button controls
- Battery voltage monitoring

### LED Feedback Mapping

| Event | Old (with buzzer) | New (LED only) |
|-------|-------------------|----------------|
| Timer complete | Beep melody | Flashing green pattern |
| Mode switch | Short beep | Brief color flash |
| Low battery | — | Pulsing red |
| Critical battery | — | Fast red flashing |

---

## Future Versions

### V2 (Planned)
- Add TP4056 + USB-C for in-board charging
- Add buzzer back for audio feedback
- Charge while using, no swapping needed

### V3 (Planned)
- Dedicated external charger (dual-slot)
- Charge both packs simultaneously
- Battery level indicator on charger

---

## Assembly

1. Solder XC6206 to extension board
2. Solder JST-PH connector (side-facing)
3. Solder HY-2.0-4P connector (top-facing, mates with AtomS3 GROVE)
4. Print 2× battery pack cases
5. Install protected LiPo cells in cases
6. Wire JST-PH male to battery
7. Stack: AtomS3 → Extension → [snap on battery]

---

## Safety Notes

- **Always use protected LiPo batteries** (with built-in PCM)
- **Never short-circuit battery terminals**
- **Charge only with proper LiPo chargers**
- **Store batteries at 50% charge for long-term**

---

*Design date: 2026-03-07*  
*Version: 1.0 (Minimal/Swappable Battery)*
