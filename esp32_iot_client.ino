/*
 * ESP32 IoT Client - Main Sketch
 * Sends sensor data to backend API via HTTP
 * 
 * Hardware:
 * - ESP32 Development Board
 * - LDR (Light Dependent Resistor) connected to GPIO34
 * - 10kΩ resistor for LDR voltage divider
 * 
 * Wiring:
 * LDR → 3.3V
 * LDR → GPIO34 (with 10kΩ resistor to GND)
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "sensors.h"

// Global variables
String authToken = "";
unsigned long lastUpdateTime = 0;
bool isAuthenticated = false;

// ==================== Setup ====================
void setup() {
  // Initialize Serial Monitor
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);
  
  Serial.println("\n\n");
  Serial.println("=====================================");
  Serial.println("  ESP32 IoT Client Starting");
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

// ==================== Main Loop ====================
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

// ==================== WiFi Connection ====================
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

// ==================== Authentication ====================
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

// ==================== Read Sensors and Send Data ====================
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

// ==================== Send Data to Backend ====================
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
      Serial.println("❌ Robot not found! Check ROBOT_ID in config.h");
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
