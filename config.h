/*
 * ESP32 IoT Client Configuration
 * Configure your WiFi, API, and sensor settings here
 */

#ifndef CONFIG_H
#define CONFIG_H

// ==================== WiFi Configuration ====================
#define WIFI_SSID "YourWiFiName"          // Replace with your WiFi SSID
#define WIFI_PASSWORD "YourWiFiPassword"  // Replace with your WiFi password

// ==================== API Configuration ====================
// For local testing, use: "http://192.168.1.X:3001/api" (replace X with your computer's IP)
// For production, use your Vercel backend URL:
#define API_BASE_URL "https://synciot-backend.vercel.app/api"

// ==================== Robot Configuration ====================
#define ROBOT_ID 1  // Your robot's ID from the database (check dashboard)

// Bot Authentication - Create this user in your database first
#define BOT_EMAIL "esp32bot@synciot.com"
#define BOT_PASSWORD "esp32secure123"

// ==================== Sensor Configuration ====================
// LDR (Light Sensor) Pin - Use ADC pin
#define LDR_PIN 34  // GPIO34 (ADC1_CH6) - Analog input pin

// LDR Calibration (adjust based on your sensor and lighting)
#define LDR_MIN_VALUE 0      // Darkest reading
#define LDR_MAX_VALUE 4095   // Brightest reading (12-bit ADC)

// ==================== Timing Configuration ====================
#define UPDATE_INTERVAL 30000  // Send data every 30 seconds (milliseconds)
#define WIFI_TIMEOUT 20000     // WiFi connection timeout (20 seconds)
#define HTTP_TIMEOUT 10000     // HTTP request timeout (10 seconds)

// ==================== Serial Configuration ====================
#define SERIAL_BAUD_RATE 115200  // Serial monitor baud rate

// ==================== Debug Mode ====================
#define DEBUG_MODE true  // Set to false to reduce serial output

#endif
