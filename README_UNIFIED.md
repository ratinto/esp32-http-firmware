# ESP32 IoT Client - Unified Single File

**Simple, all-in-one ESP32 firmware for SyncIoT project**

This is a unified version with all code in a single `.ino` file - no external headers needed!

## 📁 File Structure

```
esp32/
├── esp32_iot_client_unified.ino  ← USE THIS FILE (single file, everything included)
├── config.h                      ← Legacy (kept for reference)
├── sensors.h                     ← Legacy (kept for reference)
├── esp32_iot_client.ino         ← Legacy multi-file version
└── README_UNIFIED.md            ← This file
```

## 🚀 Quick Start

### 1. Hardware Wiring

```
ESP32 Pin Layout:
┌─────────────────┐
│   ESP32 Board   │
│                 │
│  3.3V ─────────┼───── LDR ───── GPIO34
│                 │                  │
│   GND ─────────┼──── 10kΩ ────────┘
│                 │
└─────────────────┘
```

**Component List:**
- ESP32-WROOM-32 Development Board
- LDR (Light Dependent Resistor)
- 10kΩ Resistor
- Breadboard & Jumper Wires

### 2. Arduino IDE Setup

1. **Install ESP32 Board Support:**
   - File → Preferences
   - Add to "Additional Board Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Tools → Board → Boards Manager → Search "esp32" → Install

2. **Install Required Libraries:**
   - Sketch → Include Library → Manage Libraries
   - Install:
     - `ArduinoJson` (by Benoit Blanchon) - Version 6.x

3. **Select Board:**
   - Tools → Board → ESP32 Arduino → ESP32 Dev Module
   - Tools → Port → Select your ESP32 port (COM3, /dev/ttyUSB0, etc.)

### 3. Configure the Code

Open `esp32_iot_client_unified.ino` and edit these lines (around line 24-38):

```cpp
// WiFi Configuration
#define WIFI_SSID "YourWiFiName"          // ← Change this
#define WIFI_PASSWORD "YourWiFiPassword"  // ← Change this

// API Configuration
#define API_BASE_URL "https://synciot-backend.vercel.app/api"  // ← Your backend URL

// Robot Configuration
#define ROBOT_ID 1  // ← Your robot ID from dashboard

// Bot Authentication
#define BOT_EMAIL "esp32bot@synciot.com"       // ← Bot user email
#define BOT_PASSWORD "esp32secure123"          // ← Bot user password
```

### 4. Create Bot User (First Time Only)

You need to create a bot user account in your backend:

**Option A: Using cURL**
```bash
curl -X POST https://synciot-backend.vercel.app/api/auth/signup \
  -H "Content-Type: application/json" \
  -d '{
    "email": "esp32bot@synciot.com",
    "password": "esp32secure123",
    "name": "ESP32 Bot"
  }'
```

**Option B: Using your frontend**
- Go to signup page
- Create account with the bot credentials

### 5. Upload to ESP32

1. Connect ESP32 to USB
2. Click Upload button (→) in Arduino IDE
3. Wait for "Done uploading"
4. Open Serial Monitor (Tools → Serial Monitor)
5. Set baud rate to **115200**

### 6. Monitor Output

You should see:

```
=====================================
  ESP32 IoT Client Starting
  Version: 2.0 (Unified)
=====================================

📡 LDR sensor initialized on pin 34
📡 Connecting to WiFi: YourWiFiName
.....
✅ WiFi connected!
📍 IP Address: 192.168.1.100
📶 Signal Strength: -45 dBm

🔐 Authenticating with backend...
✅ Authentication successful!
🎫 Token received and stored

===== Initialization Complete =====
Starting sensor data transmission...

────────────────────────────────────
⏱️  Time: 2s

📊 Reading sensors...
  💡 Light Level: Bright
  🔆 Light (Lux): 850.5 lux
  📈 Light (%): 65.3%
  🔢 Raw ADC: 2673
  🔋 Battery: 61%
  📶 WiFi Signal: -45 dBm

🚀 Sending data to backend...
📥 Response Code: 200
✅ Data sent successfully!
📊 Updated 3 sensors
```

## ⚙️ Configuration Options

All configuration is at the top of the `.ino` file:

| Option | Description | Default |
|--------|-------------|---------|
| `WIFI_SSID` | Your WiFi network name | "YourWiFiName" |
| `WIFI_PASSWORD` | Your WiFi password | "YourWiFiPassword" |
| `API_BASE_URL` | Backend API URL | Production URL |
| `ROBOT_ID` | Your robot's database ID | 1 |
| `BOT_EMAIL` | Bot user email | "esp32bot@synciot.com" |
| `BOT_PASSWORD` | Bot user password | "esp32secure123" |
| `LDR_PIN` | GPIO pin for LDR | 34 |
| `UPDATE_INTERVAL` | Send interval (ms) | 30000 (30s) |
| `DEBUG_MODE` | Enable verbose logging | true |

## 📊 What Data Gets Sent?

Every 30 seconds, the ESP32 sends:

1. **Light Sensor**
   - Value: 0-10,000 lux
   - Type: "light"
   - Unit: "lux"

2. **Battery Monitor**
   - Value: 0-100%
   - Type: "battery"
   - Unit: "%"
   - *Note: Currently shows placeholder, connect battery for real readings*

3. **WiFi Signal**
   - Value: -90 to -30 dBm
   - Type: "signal"
   - Unit: "dBm"

## 🔧 Troubleshooting

### WiFi Won't Connect
- Check SSID and password (case-sensitive!)
- Make sure WiFi is 2.4GHz (ESP32 doesn't support 5GHz)
- Try moving ESP32 closer to router

### Authentication Failed
- Make sure bot user exists in database
- Check email/password match
- Verify backend is accessible

### No Data Appearing
- Check `ROBOT_ID` matches your dashboard
- Look at Serial Monitor for error messages
- Verify backend URL is correct

### "Robot not found" Error
- Login to dashboard
- Check the ID of your robot
- Update `ROBOT_ID` in code

## 🎯 Next Steps

1. **Add More Sensors:**
   - Duplicate the sensor object code
   - Add your sensor reading function
   - Update the JSON payload

2. **Adjust Update Interval:**
   - Change `UPDATE_INTERVAL` (in milliseconds)
   - Example: 10000 = 10 seconds

3. **Add Battery Monitoring:**
   - Connect battery to GPIO35 via voltage divider
   - Uncomment code in `readBatteryVoltage()` function

4. **Customize Light Calibration:**
   - Test your LDR in different lighting
   - Adjust `LDR_MIN_VALUE` and `LDR_MAX_VALUE`
   - Modify the lux conversion formula

## 📝 Code Structure

The unified file contains these sections:

```cpp
// CONFIGURATION (Lines 1-60)
// - WiFi, API, Robot, Sensor settings

// GLOBAL VARIABLES (Lines 62-66)
// - authToken, timing variables

// SENSOR FUNCTIONS (Lines 68-180)
// - initLDR(), readLDR(), battery, WiFi signal

// WIFI FUNCTIONS (Lines 182-220)
// - connectWiFi()

// AUTHENTICATION (Lines 222-280)
// - authenticate()

// DATA TRANSMISSION (Lines 282-420)
// - sendDataToBackend(), readAndSendData()

// SETUP (Lines 422-450)
// - Initialize everything

// MAIN LOOP (Lines 452-470)
// - Check WiFi, send data periodically
```

## ✨ Advantages of Unified Version

✅ **Single file** - Easy to share and manage  
✅ **No includes** - All code in one place  
✅ **Simple setup** - Just edit config at top  
✅ **Easy upload** - One file to Arduino IDE  
✅ **Portable** - Copy anywhere, works immediately  

## 📚 Additional Resources

- [ESP32 Pinout Reference](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
- [ArduinoJson Documentation](https://arduinojson.org/)
- [ESP32 WiFi Documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html)

---

**Need Help?** Check the Serial Monitor output - it shows detailed debugging information!
