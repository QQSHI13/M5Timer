# Bill of Materials (BOM)

## Extension Board Components

### Power Management

| Part | Value | Package | Qty | Unit Price (CNY) | Source | Notes |
|------|-------|---------|-----|------------------|--------|-------|
| TP4056 | Charging IC | SOP-8 | 1 | ¥1.00 | LCSC/JLCPCB | Lithium charger, 1A max |
| XC6206P332MR | 3.3V LDO | SOT-23 | 1 | ¥0.35 | LCSC/JLCPCB | 200mA, ultra-low Iq |
| DW01A | Protection IC | SOT-23-6 | 1 | ¥0.55 | LCSC/JLCPCB | Battery protection (optional but recommended) |
| FS8205A | Dual MOSFET | SOT-23-6 | 1 | ¥0.55 | LCSC/JLCPCB | Protection switch (pair with DW01A) |
| LED Red | 0603 SMD | 0603 | 1 | ¥0.07 | LCSC/JLCPCB | Charging indicator |
| LED Green | 0603 SMD | 0603 | 1 | ¥0.07 | LCSC/JLCPCB | Full indicator |
| R1 | 1kΩ | 0603 | 1 | ¥0.07 | LCSC/JLCPCB | LED current limit |
| R2 | 1kΩ | 0603 | 1 | ¥0.07 | LCSC/JLCPCB | LED current limit |
| R3 | 1.2kΩ | 0603 | 1 | ¥0.07 | LCSC/JLCPCB | TP4056 prog resistor (1A charge) |
| C1 | 10µF | 0603 | 1 | ¥0.14 | LCSC/JLCPCB | Input bypass |
| C2 | 10µF | 0603 | 1 | ¥0.14 | LCSC/JLCPCB | Output bypass |

### Connectors & Switches

| Part | Value | Package | Qty | Unit Price (CNY) | Source | Notes |
|------|-------|---------|-----|------------------|--------|-------|
| JST-PH-2P | Battery connector | Through-hole | 1 | ¥0.35 | LCSC/JLCPCB | 2.0mm pitch, for LiPo |
| HY2.0-4P | GROVE connector | Through-hole | 1 | ¥0.45 | LCSC/JLCPCB | 4-pin, mates with AtomS3 Lite GROVE port |
| SS12D00 | Slide switch | Through-hole | 1 | ¥0.55 | LCSC/JLCPCB | SPDT, for power cutoff |
| USB-C | 16P receptacle | SMD | 1 | ¥1.75 | LCSC/JLCPCB | Charging input |

### Feedback

| Part | Value | Package | Qty | Unit Price (CNY) | Source | Notes |
|------|-------|---------|-----|------------------|--------|-------|
| Buzzer | Active 3V | 9x5.5mm | 1 | ¥1.40 | LCSC/JLCPCB | Self-oscillating, 3.3V rated |
| R4 | 100Ω | 0603 | 1 | ¥0.07 | LCSC/JLCPCB | Buzzer current limit (optional) |

### Mechanical

| Part | Value | Package | Qty | Unit Price (CNY) | Source | Notes |
|------|-------|---------|-----|------------------|--------|-------|
| Neodymium magnet | 6x3mm | - | 1 | ¥0.50 | Taobao/Hardware store | Press-fit into backpack, attracts M2 socket on AtomS3 Lite |

### Battery (External)

| Part | Value | Qty | Unit Price (CNY) | Source | Notes |
|------|-------|-----|------------------|--------|-------|
| LiPo 503040 | 3.7V 500mAh | 1 | ¥20.00 | Taobao/Tmall | With JST-PH connector pre-wired |

---

## Main Board

| Part | Value | Qty | Unit Price (CNY) | Source | Notes |
|------|-------|-----|------------------|--------|-------|
| M5Stack AtomS3 Lite | ESP32-S3 dev board | 1 | ¥49.90 | Taobao/M5Stack Official | **Same price as Atom Lite, better specs!** |

---

## Extension Board PCB

| Item | Qty | Unit Price (CNY) | Source | Notes |
|------|-----|------------------|--------|-------|
| PCB 24x24mm 2-layer | 5 | ¥7.00 | JLCPCB | Min order 5pcs |
| Shipping | 1 | ¥25-35 | JLCPCB | To China ~¥15-25 |

---

## 3D Printed Case

| Item | Qty | Unit Price (CNY) | Source | Notes |
|------|-----|------------------|--------|-------|
| Enclosure print | 1 | ~¥20-40 | JLCPCB 3D / 未来工场 | Simple backpack design |
| Shipping | 1 | varies | - | - |

---

## Cost Summary

| Category | Cost (CNY) |
|----------|------------|
| Components (LCSC) | ~¥14 |
| PCB (5pcs) | ~¥40-50 |
| **AtomS3 Lite** | **¥49.90** |
| Battery | ¥20 |
| 3D print | ~¥20-40 |
| **Total per unit** | **~¥144-184** |

**Savings**: ~¥20 compared to original Atom Lite design!

(Assuming you build one; extras cost ~¥75 each for components + battery)

---

## LCSC/JLCPCB Part Numbers (Quick Ref)

Search these on LCSC:
- TP4056: C165948
- XC6206P332MR: C5446
- DW01A: C276000
- FS8205A: C276001
- JST-PH-2P: C160331
- HY2.0-4P (GROVE): C2884835 or similar
- USB-C 16P: C165948 (check pinout!)
- SS12D00: C92584
- Buzzer 3V: C961112

*Verify footprints match before ordering!*
