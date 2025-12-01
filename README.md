# ESP32 IoT Client

Send real sensor data from ESP32 to your IoT dashboard via HTTP/REST API.

## 📁 Files

- **`esp32_iot_client.ino`** - Main Arduino sketch
- **`config.h`** - Configuration (WiFi, API, credentials)
- **`sensors.h`** - Sensor reading functions
- **`SETUP_INSTRUCTIONS.md`** - Complete setup guide

## 🎯 Quick Start

1. **Wire the LDR sensor** (see SETUP_INSTRUCTIONS.md)
2. **Install Arduino IDE** and ESP32 board support
3. **Install ArduinoJson library**
4. **Edit `config.h`** with your WiFi and API details
5. **Upload to ESP32**
6. **Open Serial Monitor** (115200 baud) to see output
7. **Check dashboard** for live data!

## 🔌 Wiring

```
ESP32 Pin    →    Component
─────────────────────────────
3.3V         →    LDR (one side)
GPIO34       →    LDR (other side) + 10kΩ resistor
GND          →    10kΩ resistor (other side)
```

## ⚙️ Configuration

Edit `config.h`:

```cpp
#define WIFI_SSID "YourWiFiName"
#define WIFI_PASSWORD "YourPassword"
#define API_BASE_URL "https://synciot-backend.vercel.app/api"
#define ROBOT_ID 1
#define BOT_EMAIL "esp32bot@synciot.com"
#define BOT_PASSWORD "esp32secure123"
```

## 📊 Sensors

The code sends 3 sensors by default:

1. **Light Sensor** - LDR reading in lux
2. **Battery Monitor** - ESP32 power supply
3. **WiFi Signal** - Connection strength

## 🚀 Features

- ✅ Auto WiFi reconnection
- ✅ JWT authentication with token refresh
- ✅ Bulk sensor updates (efficient)
- ✅ Auto-create sensors on first run
- ✅ Update existing sensors on subsequent runs
- ✅ Detailed serial debug output
- ✅ Error handling and retry logic
- ✅ Updates robot's "lastSeen" timestamp

## 📡 Data Flow

```
ESP32 → Read LDR → Create JSON → POST to API → Backend saves → Dashboard shows
```

## 🐛 Troubleshooting

See `SETUP_INSTRUCTIONS.md` for detailed troubleshooting.

**Common issues:**
- **WiFi timeout** → Check SSID/password, use 2.4GHz
- **Auth failed** → Create bot user in database
- **Robot not found** → Check ROBOT_ID matches dashboard

## 📖 Full Documentation

See `SETUP_INSTRUCTIONS.md` for:
- Complete wiring guide
- Software installation
- Backend setup
- Testing procedures
- Advanced configuration

## 🔋 Power

- **Development:** USB cable
- **Deployment:** 3.7V LiPo battery
- **Outdoor:** Solar panel + battery

## 📈 Future Enhancements

- Add deep sleep for battery saving
- Real-time WebSocket connection
- Receive commands from dashboard
- OTA (Over-The-Air) firmware updates
- Add more sensor types

## 🎓 Learning Resources

- ESP32 Docs: https://docs.espressif.com/
- Arduino JSON: https://arduinojson.org/
- ESP32 Pinout: https://randomnerdtutorials.com/esp32-pinout-reference-gpios/

---

Made with ❤️ for IoT projects
# esp32-http-firmware
