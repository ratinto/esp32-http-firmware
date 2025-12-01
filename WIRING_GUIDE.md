# ESP32 + LDR Wiring Diagram

## 🔌 Components List

| Qty | Component | Notes |
|-----|-----------|-------|
| 1 | ESP32 Development Board | Any variant (DevKitC, WROOM, etc.) |
| 1 | LDR (Photoresistor) | Light Dependent Resistor |
| 1 | 10kΩ Resistor | Brown-Black-Orange or Brown-Black-Black-Red |
| 1 | Breadboard | Half-size or full-size |
| 3 | Jumper Wires | Male-to-Male |
| 1 | USB Cable | For programming and power |

---

## 📐 Schematic Diagram

```
                    ESP32
                 ┌─────────┐
                 │         │
       3.3V ─────┤ 3V3     │
                 │         │
                 │         │
                 │     GPIO34 ├──────┐
                 │         │         │
                 │         │         │
       GND ──────┤ GND     │         │
                 │         │         │
                 └─────────┘         │
                                     │
                                     │
                                   ┌─┴─┐
                 3.3V ──────────── │   │ LDR
                                   │   │ (Photoresistor)
                                   └─┬─┘
                                     │
                                     ├─────── GPIO34
                                     │
                                   ┌─┴─┐
                                   │   │ 10kΩ
                                   │   │ Resistor
                                   └─┬─┘
                                     │
                      GND ───────────┘
```

---

## 🔍 Breadboard Layout

```
     Breadboard
 ────────────────────
 
 + Rail (3.3V)
 ────────────────────
      │
      │
    [LDR]  ← One leg connected to + rail
      │
      └──────┐
             │
         [GPIO34]  ← Middle point
             │
             │
         [10kΩ]  ← Resistor
             │
             └──────┐
                    │
 ────────────────────
 - Rail (GND)
 ────────────────────
```

---

## 📝 Step-by-Step Wiring

### Step 1: Place Components on Breadboard

1. **Insert LDR:**
   - Place LDR legs in breadboard rows (e.g., Row 10 and Row 15)
   - LDR has no polarity - either orientation works

2. **Insert 10kΩ Resistor:**
   - Place one leg in Row 15 (same as LDR second leg)
   - Place other leg in Row 20
   - Resistor has no polarity

### Step 2: Connect Power Rails

3. **Connect 3.3V:**
   - Red jumper wire from ESP32 `3V3` pin to + rail on breadboard
   - Then from + rail to LDR first leg (Row 10)

4. **Connect GND:**
   - Black jumper wire from ESP32 `GND` pin to - rail on breadboard
   - Then from - rail to resistor second leg (Row 20)

### Step 3: Connect Signal Pin

5. **Connect GPIO34:**
   - Yellow/Green jumper wire from ESP32 `GPIO34` pin
   - To the junction point between LDR and resistor (Row 15)

---

## 🎨 Color-Coded Connections

| Wire Color | From | To | Purpose |
|------------|------|----|----|
| 🔴 Red | ESP32 3V3 | LDR leg 1 | Power (+) |
| ⚫ Black | ESP32 GND | Resistor leg 2 | Ground (-) |
| 🟡 Yellow | ESP32 GPIO34 | LDR leg 2 / Resistor leg 1 | Signal (ADC input) |

---

## 🔬 How It Works

### Voltage Divider Circuit

The LDR and resistor form a **voltage divider**:

```
3.3V ─── LDR (variable resistance) ─── GPIO34 ─── 10kΩ (fixed) ─── GND
```

**In Bright Light:**
- LDR resistance is LOW (~1kΩ)
- More voltage drops across the 10kΩ resistor
- GPIO34 reads HIGHER voltage (~2.5V)
- ADC value: HIGH (3000-4000)

**In Darkness:**
- LDR resistance is HIGH (~100kΩ)
- More voltage drops across the LDR
- GPIO34 reads LOWER voltage (~0.5V)
- ADC value: LOW (500-1500)

### ESP32 ADC

- GPIO34 is an **analog input pin** (ADC1_CH6)
- Reads voltage from 0V to 3.3V
- Converts to digital value 0-4095 (12-bit resolution)
- Code converts this to lux or percentage

---

## 📍 ESP32 Pinout Reference

```
                    ESP32-WROOM-32
         ┌─────────────────────────────┐
         │                             │
    EN   │ 1                        38 │ GND
   VP    │ 2                        37 │ D23
   VN    │ 3                        36 │ D22
   D34 ←─┤ 4   ← USE THIS            35 │ TXD0
   D35   │ 5       (GPIO34)          34 │ RXD0
   D32   │ 6                        33 │ D21
   D33   │ 7                        32 │ D19
   D25   │ 8                        31 │ D18
   D26   │ 9                        30 │ D5
   D27   │10                        29 │ D17
   D14   │11                        28 │ D16
   D12   │12                        27 │ D4
   D13   │13                        26 │ D0
   GND ←─┤14   ← GROUND             25 │ D2
   VIN   │15                        24 │ D15
   3V3 ←─┤16   ← POWER 3.3V         23 │ D8
         │                             │
         └─────────────────────────────┘
```

**Key Pins Used:**
- **GPIO34** (Pin 4) - Analog input for LDR
- **3V3** (Pin 16) - 3.3V power supply
- **GND** (Pin 14) - Ground

---

## ⚠️ Important Notes

### GPIO34 Characteristics:
- ✅ ADC1_CH6 - Can be used as analog input
- ✅ Input only (perfect for sensors)
- ✅ 12-bit resolution (0-4095)
- ⚠️ Cannot be used as output
- ⚠️ No pull-up resistor

### Power Considerations:
- ✅ Use 3.3V, NOT 5V (ESP32 is 3.3V logic)
- ✅ LDR can handle 3.3V safely
- ⚠️ Never connect 5V to GPIO pins

### Resistor Value:
- 📌 10kΩ recommended for most LDRs
- 📌 Can use 4.7kΩ to 47kΩ range
- 📌 Affects sensitivity and range
- 📌 Experiment for your lighting conditions

---

## 🧪 Testing the Circuit

### With Multimeter:
1. **Power Off:** Measure LDR resistance in dark (~100kΩ) and light (~1kΩ)
2. **Power On:** Measure voltage at GPIO34
3. **Bright Light:** Should read 2.0-3.0V
4. **Dark:** Should read 0.3-1.0V

### With Serial Monitor:
1. Upload code
2. Open Serial Monitor (115200 baud)
3. Check "Raw ADC" value
4. Cover LDR → value should decrease
5. Shine light → value should increase

---

## 🛠️ Alternative Configurations

### Configuration 1: Current Setup (Recommended)
```
3.3V → LDR → GPIO34 → 10kΩ → GND
```
- Bright = High reading
- Dark = Low reading

### Configuration 2: Inverted
```
3.3V → 10kΩ → GPIO34 → LDR → GND
```
- Bright = Low reading
- Dark = High reading
- Less common, but works

### Configuration 3: With Series Resistor (Advanced)
```
3.3V → 1kΩ → LDR → GPIO34 → 10kΩ → GND
```
- Limits current for sensitive LDRs
- More linear response
- Better long-term reliability

---

## 📸 Physical Build Tips

1. **Breadboard Rows:**
   - Use adjacent rows for easy troubleshooting
   - Keep wires short and organized
   - Leave space around ESP32

2. **Wire Management:**
   - Use different colors for power/ground/signal
   - Bend wires neatly
   - Avoid crossing wires

3. **Component Placement:**
   - Place ESP32 in center of breadboard
   - Sensors on one side
   - Power rails on edges

4. **LDR Orientation:**
   - Face LDR upward (light-sensitive side up)
   - Keep clear of obstructions
   - For outdoor use, add clear protective cover

---

## 🔍 Troubleshooting Physical Issues

| Problem | Check |
|---------|-------|
| No readings | Check all 3 connections are secure |
| Always max value | LDR might be reversed (try flipping) |
| Always zero | Check 3.3V is connected |
| Unstable readings | Add 0.1µF capacitor across GPIO34 and GND |
| No power | Check USB cable, try different port |

---

## 📐 Dimensions & Spacing

| Component | Size | Notes |
|-----------|------|-------|
| ESP32 Board | ~28mm x 52mm | Check your specific model |
| Breadboard | 170mm x 45mm | Half-size standard |
| LDR | ~5mm diameter | GL5528 or similar |
| 10kΩ Resistor | 1/4W | Color: Brown-Black-Orange |

---

## 🎓 Additional Sensor Ideas

Once LDR works, try adding:

| Sensor | Pin | Purpose |
|--------|-----|---------|
| DHT22 | GPIO4 | Temperature & Humidity |
| HC-SR04 (Trig) | GPIO5 | Distance |
| HC-SR04 (Echo) | GPIO18 | Distance |
| Button | GPIO23 | Manual trigger |
| LED | GPIO2 | Status indicator |

---

**Ready to wire?** Follow the color-coded connections above!

**Need help?** Double-check each connection matches the diagram.

🔌 Happy wiring! 🤖
