# 🚀 ESP32 Quick Start Card

## ⚡ 5-Minute Setup Checklist

### ☑️ Prerequisites
- [ ] ESP32 board
- [ ] LDR sensor + 10kΩ resistor
- [ ] USB cable
- [ ] Arduino IDE installed
- [ ] WiFi credentials ready

### ☑️ Backend Setup (One-time)
```bash
# Create bot user
curl -X POST https://synciot-backend.vercel.app/api/auth/signup \
  -H "Content-Type: application/json" \
  -d '{"email":"esp32bot@synciot.com","password":"esp32secure123","name":"ESP32 Bot"}'
```

### ☑️ Wiring
```
3.3V ───── LDR ───── GPIO34 ───── 10kΩ ───── GND
```

### ☑️ Code Configuration
Edit `config.h`:
```cpp
#define WIFI_SSID "YourWiFi"
#define WIFI_PASSWORD "password"
#define ROBOT_ID 1  // from dashboard URL
```

### ☑️ Upload
1. Open `esp32_iot_client.ino`
2. Tools → Board → ESP32 Dev Module
3. Tools → Port → (select ESP32)
4. Upload (→ button)
5. Serial Monitor (115200 baud)

### ☑️ Verify
- [ ] Serial shows "WiFi connected"
- [ ] Serial shows "Authentication successful"
- [ ] Serial shows "Data sent successfully"
- [ ] Dashboard shows 3 new sensors
- [ ] Values update every 30s

---

## 🔧 Pin Reference

| Function | ESP32 Pin | Notes |
|----------|-----------|-------|
| LDR Input | GPIO34 | ADC1_CH6 (analog) |
| 3.3V Power | 3V3 | For LDR |
| Ground | GND | For resistor |

---

## 📡 API Endpoints Used

| Endpoint | Purpose | Method |
|----------|---------|--------|
| `/auth/login` | Get JWT token | POST |
| `/robots/:id/sensors/bulk` | Send sensor data | POST |

---

## 🐛 Quick Troubleshooting

| Problem | Solution |
|---------|----------|
| WiFi timeout | Check SSID/password, use 2.4GHz |
| Auth failed | Create bot user (curl command above) |
| Robot not found | Check ROBOT_ID in config.h |
| LDR not changing | Check wiring, GPIO34 is analog pin |
| No dashboard data | Wait 30s, refresh page |

---

## 📊 Expected Serial Output

```
✅ WiFi connected!
✅ Authentication successful!
📊 Reading sensors...
  💡 Light Level: Bright
  🔆 Light (Lux): 752.3 lux
✅ Data sent successfully!
```

---

## 🎯 Test Checklist

- [ ] Cover LDR → light decreases
- [ ] Flashlight on LDR → light increases
- [ ] Dashboard updates after 30s
- [ ] All 3 sensors visible (Light, Battery, Signal)

---

## 📁 Important Files

```
esp32/
├── esp32_iot_client.ino   ← Main code
├── config.h                ← EDIT THIS
├── sensors.h               ← Sensor functions
├── SETUP_INSTRUCTIONS.md   ← Full guide
└── README.md               ← Overview
```

---

## ⚙️ Default Settings

- **Update Interval:** 30 seconds
- **WiFi Timeout:** 20 seconds
- **HTTP Timeout:** 10 seconds
- **Serial Baud:** 115200
- **LDR Pin:** GPIO34

---

## 🔗 Quick Links

- Dashboard: https://synciot-frontend.vercel.app/dashboard
- Backend API: https://synciot-backend.vercel.app/api
- ESP32 Pinout: https://randomnerdtutorials.com/esp32-pinout-reference-gpios/

---

## 📝 Bot Credentials (Default)

```
Email: esp32bot@synciot.com
Password: esp32secure123
```

*(Create this user first with curl command)*

---

## 🎓 Next Steps After Basic Setup

1. Add more sensors (DHT22, ultrasonic, etc.)
2. Enable deep sleep for battery
3. Add WebSocket for real-time updates
4. Add commands from dashboard to ESP32
5. Build robot chassis

---

**Need detailed help?** See `SETUP_INSTRUCTIONS.md`

**Ready to code?** Open `esp32_iot_client.ino` in Arduino IDE!

🤖 Happy IoT building!
