/*
 * Sensor Reading Functions
 * Functions to read LDR and other sensors
 */

#ifndef SENSORS_H
#define SENSORS_H

#include "config.h"

// ==================== LDR (Light Sensor) Functions ====================

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

// ==================== Battery Monitoring (Optional) ====================
// ESP32 can monitor its own supply voltage

/**
 * Read battery/supply voltage (if connected to ADC)
 * For accurate readings, use a voltage divider
 */
float readBatteryVoltage() {
  // Example for 3.7V LiPo battery with voltage divider
  // Adjust based on your setup
  // int rawValue = analogRead(35);  // GPIO35 (ADC1_CH7)
  // float voltage = (rawValue / 4095.0) * 3.3 * 2;  // *2 for voltage divider
  // return voltage;
  
  // For now, return a placeholder
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

// ==================== WiFi Signal Strength ====================

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

#endif
