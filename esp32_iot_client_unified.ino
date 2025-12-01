/*
 * ESP32 IoT Client - Unified Single File Version
 * Sends sensor data to backend API via HTTP
 * 
 * Hardware:
 * - ESP32 Development Board (ESP32-WROOM-32)
 * - LDR (Light Dependent Resistor) connected to GPIO34
 * - 10kΩ resistor for LDR voltage divider
 * 
 * Wiring:
 * 3.3V → LDR → GPIO34 → 10kΩ Resistor → GND
 * 
 * Author: SyncIoT
 * Version: 2.0
 * Date: December 2025
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ==================== CONFIGURATION ====================

// WiFi Configuration
#define WIFI_SSID "Ritesh's A15"          // Replace with your WiFi SSID
#define WIFI_PASSWORD "797628974k"  // Replace with your WiFi password

// API Configuration
// For local testing: "http://192.168.1.X:3001/api" (replace X with your IP)
// For production: Use your Vercel backend URL
#define API_BASE_URL "https://synciot-backend.vercel.app/api"

// Robot Configuration
#define ROBOT_ID 1  // Your robot's ID from the database

// Bot Authentication - Create this user in your database first
#define BOT_EMAIL "esp32bot@synciot.com"
#define BOT_PASSWORD "esp32secure123"

// Sensor Configuration
#define LDR_PIN 34  // GPIO34 (ADC1_CH6) - Analog input pin

// LDR Calibration
#define LDR_MIN_VALUE 0      // Darkest reading
#define LDR_MAX_VALUE 4095   // Brightest reading (12-bit ADC)

// Timing Configuration
#define UPDATE_INTERVAL 30000  // Send data every 30 seconds (milliseconds)
#define WIFI_TIMEOUT 20000     // WiFi connection timeout (20 seconds)
#define HTTP_TIMEOUT 10000     // HTTP request timeout (10 seconds)

// Serial Configuration
#define SERIAL_BAUD_RATE 115200  // Serial monitor baud rate

// Debug Mode
#define DEBUG_MODE true  // Set to false to reduce serial output

// ==================== GLOBAL VARIABLES ====================

String authToken = "";
unsigned long lastUpdateTime = 0;
bool isAuthenticated = false;

// ==================== SENSOR FUNCTIONS ====================

/**
 * Initialize LDR sensor
 */
void initLDR() {
  pinMode(LDR_PIN, INPUT);
  
  if (DEBUG_MODE) {
    Serial.println("📡 LDR sensor initialized on pin " + String(LDR_PIN));
  }
}

/**
 * Read raw LDR value (0-4095 for 12-bit ADC)
 */
int readLDRRaw() {
  return analogRead(LDR_PIN);
}

/**
 * Read LDR as percentage (0-100%)
 * Higher value = brighter light
 */
float readLDRPercentage() {
  int rawValue = readLDRRaw();
  
  // Convert to percentage (0-100)
  float percentage = map(rawValue, LDR_MIN_VALUE, LDR_MAX_VALUE, 0, 100);
  
  // Constrain to valid range
  if (percentage < 0) percentage = 0;
  if (percentage > 100) percentage = 100;
  
  return percentage;
}

/**
 * Read LDR as lux (approximate)
 * This is a simplified conversion - calibrate for your specific LDR
 */
float readLDRLux() {
  int rawValue = readLDRRaw();
  
  // Simple exponential conversion (approximate)
  // For accurate lux readings, calibrate with a real lux meter
  float lux = pow(10, (rawValue / 4095.0) * 4);  // 0-10,000 lux range
  
  return lux;
}

/**
 * Get light level description
 */
String getLightLevel() {
  float percentage = readLDRPercentage();
  
  if (percentage < 20) return "Dark";
  if (percentage < 40) return "Dim";
  if (percentage < 60) return "Moderate";
  if (percentage < 80) return "Bright";
  return "Very Bright";
}

/**
 * Read battery/supply voltage (placeholder)
 * For actual battery monitoring, connect battery through voltage divider to ADC pin
 */
float readBatteryVoltage() {
  // For now, return a placeholder value
  // To implement: Connect battery to GPIO35 with voltage divider
  // int rawValue = analogRead(35);
  // float voltage = (rawValue / 4095.0) * 3.3 * 2;  // *2 for voltage divider
  return 3.7;  // Assume 3.7V LiPo battery
}

/**
 * Calculate battery percentage (for 3.7V LiPo)
 */
int readBatteryPercentage() {
  float voltage = readBatteryVoltage();
  
  // LiPo voltage range: 3.0V (empty) to 4.2V (full)
  float percentage = ((voltage - 3.0) / (4.2 - 3.0)) * 100;
  
  // Constrain to 0-100%
  if (percentage < 0) percentage = 0;
  if (percentage > 100) percentage = 100;
  
  return (int)percentage;
}

/**
 * Get WiFi signal strength in dBm
 */
int getWiFiSignalStrength() {
  return WiFi.RSSI();
}

/**
 * Get WiFi signal quality percentage
 */
int getWiFiSignalQuality() {
  int rssi = WiFi.RSSI();
  
  // Convert RSSI to quality percentage
  // -30 dBm = 100% (excellent)
  // -90 dBm = 0% (very poor)
  int quality = 2 * (rssi + 100);
  
  if (quality < 0) quality = 0;
  if (quality > 100) quality = 100;
  
  return quality;
}

// ==================== WIFI FUNCTIONS ====================

/**
 * Connect to WiFi network
 */
void connectWiFi() {
  Serial.println("📡 Connecting to WiFi: " + String(WIFI_SSID));
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    
    // Timeout after specified duration
    if (millis() - startTime > WIFI_TIMEOUT) {
      Serial.println("\n❌ WiFi connection timeout!");
      Serial.println("⚠️  Retrying in 5 seconds...");
      delay(5000);
      startTime = millis();
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
  }
  
  Serial.println("\n✅ WiFi connected!");
  Serial.println("📍 IP Address: " + WiFi.localIP().toString());
  Serial.println("📶 Signal Strength: " + String(WiFi.RSSI()) + " dBm");
  Serial.println();
}

// ==================== AUTHENTICATION ====================

/**
 * Authenticate with backend API
 */
void authenticate() {
  Serial.println("🔐 Authenticating with backend...");
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ No WiFi connection!");
    return;
  }
  
  HTTPClient http;
  String url = String(API_BASE_URL) + "/auth/login";
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(HTTP_TIMEOUT);
  
  // Create JSON payload
  StaticJsonDocument<200> doc;
  doc["email"] = BOT_EMAIL;
  doc["password"] = BOT_PASSWORD;
  
  String jsonPayload;
  serializeJson(doc, jsonPayload);
  
  if (DEBUG_MODE) {
    Serial.println("POST " + url);
    Serial.println("Payload: " + jsonPayload);
  }
  
  // Send POST request
  int httpCode = http.POST(jsonPayload);
  
  if (httpCode > 0) {
    String response = http.getString();
    
    if (DEBUG_MODE) {
      Serial.println("Response Code: " + String(httpCode));
      Serial.println("Response: " + response);
    }
    
    if (httpCode == 200) {
      // Parse response
      StaticJsonDocument<512> responseDoc;
      DeserializationError error = deserializeJson(responseDoc, response);
      
      if (!error) {
        authToken = responseDoc["token"].as<String>();
        isAuthenticated = true;
        Serial.println("✅ Authentication successful!");
        Serial.println("🎫 Token received and stored");
      } else {
        Serial.println("❌ Failed to parse response: " + String(error.c_str()));
        isAuthenticated = false;
      }
    } else {
      Serial.println("❌ Authentication failed! HTTP " + String(httpCode));
      Serial.println("Response: " + response);
      isAuthenticated = false;
    }
  } else {
    Serial.println("❌ HTTP request failed: " + http.errorToString(httpCode));
    isAuthenticated = false;
  }
  
  http.end();
  Serial.println();
}

// ==================== DATA TRANSMISSION ====================

/**
 * Send sensor data to backend API
 */
void sendDataToBackend(String jsonPayload) {
  Serial.println("🚀 Sending data to backend...");
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ No WiFi connection!");
    return;
  }
  
  HTTPClient http;
  String url = String(API_BASE_URL) + "/robots/" + String(ROBOT_ID) + "/sensors/bulk";
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + authToken);
  http.setTimeout(HTTP_TIMEOUT);
  
  if (DEBUG_MODE) {
    Serial.println("POST " + url);
    Serial.println("Payload: " + jsonPayload);
  }
  
  // Send POST request
  int httpCode = http.POST(jsonPayload);
  
  if (httpCode > 0) {
    String response = http.getString();
    
    Serial.println("📥 Response Code: " + String(httpCode));
    
    if (httpCode == 200 || httpCode == 201) {
      Serial.println("✅ Data sent successfully!");
      
      if (DEBUG_MODE) {
        Serial.println("Response: " + response);
      }
      
      // Parse response to show count
      StaticJsonDocument<512> responseDoc;
      DeserializationError error = deserializeJson(responseDoc, response);
      
      if (!error && responseDoc.containsKey("count")) {
        int count = responseDoc["count"];
        Serial.println("📊 Updated " + String(count) + " sensors");
      }
    } else if (httpCode == 401) {
      Serial.println("❌ Unauthorized! Token may have expired.");
      Serial.println("🔄 Re-authenticating...");
      isAuthenticated = false;
      authToken = "";
      authenticate();
    } else if (httpCode == 404) {
      Serial.println("❌ Robot not found! Check ROBOT_ID in config");
      Serial.println("Current ROBOT_ID: " + String(ROBOT_ID));
    } else {
      Serial.println("⚠️  HTTP Error " + String(httpCode));
      Serial.println("Response: " + response);
    }
  } else {
    Serial.println("❌ Request failed: " + http.errorToString(httpCode));
  }
  
  http.end();
  Serial.println();
}

/**
 * Read all sensors and send data to backend
 */
void readAndSendData() {
  Serial.println("────────────────────────────────────");
  Serial.println("⏱️  Time: " + String(millis() / 1000) + "s");
  Serial.println();
  
  // Read all sensors
  Serial.println("📊 Reading sensors...");
  
  float lightLux = readLDRLux();
  float lightPercent = readLDRPercentage();
  int rawLDR = readLDRRaw();
  String lightLevel = getLightLevel();
  
  int batteryPercent = readBatteryPercentage();
  int wifiSignal = getWiFiSignalStrength();
  
  // Display readings
  Serial.println("  💡 Light Level: " + lightLevel);
  Serial.println("  🔆 Light (Lux): " + String(lightLux, 1) + " lux");
  Serial.println("  📈 Light (%): " + String(lightPercent, 1) + "%");
  Serial.println("  🔢 Raw ADC: " + String(rawLDR));
  Serial.println("  🔋 Battery: " + String(batteryPercent) + "%");
  Serial.println("  📶 WiFi Signal: " + String(wifiSignal) + " dBm");
  Serial.println();
  
  // Check authentication
  if (!isAuthenticated || authToken == "") {
    Serial.println("⚠️  Not authenticated! Attempting to authenticate...");
    authenticate();
    if (!isAuthenticated) {
      Serial.println("❌ Skipping data send - authentication required");
      return;
    }
  }
  
  // Create JSON payload with sensor data
  StaticJsonDocument<1024> doc;
  JsonArray sensors = doc.createNestedArray("sensors");
  
  // Light sensor (in lux)
  JsonObject lightSensor = sensors.createNestedObject();
  lightSensor["name"] = "Light Sensor";
  lightSensor["type"] = "light";
  lightSensor["value"] = lightLux;
  lightSensor["unit"] = "lux";
  
  // Battery monitor
  JsonObject batterySensor = sensors.createNestedObject();
  batterySensor["name"] = "Battery Monitor";
  batterySensor["type"] = "battery";
  batterySensor["value"] = batteryPercent;
  batterySensor["unit"] = "%";
  
  // WiFi signal strength
  JsonObject signalSensor = sensors.createNestedObject();
  signalSensor["name"] = "WiFi Signal";
  signalSensor["type"] = "signal";
  signalSensor["value"] = wifiSignal;
  signalSensor["unit"] = "dBm";
  
  String jsonPayload;
  serializeJson(doc, jsonPayload);
  
  // Send to backend
  sendDataToBackend(jsonPayload);
}

// ==================== SETUP ====================

void setup() {
  // Initialize Serial Monitor
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);
  
  Serial.println("\n\n");
  Serial.println("=====================================");
  Serial.println("  ESP32 IoT Client Starting");
  Serial.println("  Version: 2.0 (Unified)");
  Serial.println("=====================================");
  Serial.println();
  
  // Initialize sensors
  initLDR();
  
  // Connect to WiFi
  connectWiFi();
  
  // Authenticate with backend
  authenticate();
  
  Serial.println("\n===== Initialization Complete =====");
  Serial.println("Starting sensor data transmission...\n");
}

// ==================== MAIN LOOP ====================

void loop() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️  WiFi disconnected! Reconnecting...");
    connectWiFi();
  }
  
  // Check if it's time to send data
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
    lastUpdateTime = currentTime;
    
    // Read sensors and send data
    readAndSendData();
  }
  
  // Small delay to prevent watchdog timeout
  delay(100);
}
