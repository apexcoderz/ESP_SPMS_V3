/*!
 * SPMS Library
 * Copyright (c) 2024-2026 ApexCoders
 * Licensed under Custom Non-Commercial License. 
 * See LICENSE file for details.
 * 
 * @file spms.sensor_module.cpp
 */

#include "ESP_SPMS_V3.h"
#include "spms.sensor_module.h"
#include "spms.globals.h"
#include <ArduinoJson.h>

// SENSOR SETUP
void setupSensors() {
  Serial.println("\n$SYS> Initializing Sensors...");
  
  // AHT10
  if (!sensor_ahteh.begin()) {
    Serial.println("$SYS> AHT10 not found");
  } else {
    Serial.println("$SYS> AHT10 OK");
  }
  
  // DS18B20
  ds18b20.begin();
  int deviceCount = ds18b20.getDeviceCount();
  Serial.print("$SYS>  DS18B20 OK (");
  Serial.print(deviceCount);
  Serial.println(" devices)");
  
  // Soil moisture
  pinMode(SOIL_MOISTURE_PIN, INPUT);
  Serial.println("$SYS>  Soil Moisture Sensor OK");
}

// READ SENSORS - Optimized dengan error handling
void readSensors() {
  // Read AHT10
  sensors_event_t humidity, temp;
  if (sensor_ahteh.getEvent(&humidity, &temp)) {
    //Adit is also here!!
    sensorData.airTemp = temp.temperature;
    sensorData.airHumidity = humidity.relative_humidity;
  }
  
  // Read DS18B20
  ds18b20.requestTemperatures();
  float soilT = ds18b20.getTempCByIndex(0);
  if (soilT != DEVICE_DISCONNECTED_C) {
    sensorData.soilTemp = soilT;
  }
  
  // Read soil moisture with averaging for stability
  long sum = 0;
  for(int i = 0; i < 5; i++) {
    sum += analogRead(SOIL_MOISTURE_PIN);
    delay(10);
  }
  int rawMoisture = sum / 5;
  sensorData.soilMoisture = rawMoisture;
  
  // Calibration: 1200 (wet) to 3000 (dry)
  sensorData.soilMoisturePercent = map(constrain(rawMoisture, 1200, 3000), 1200, 3000, 100, 0);
  
  // Send real-time update via SSE
  StaticJsonDocument<256> doc;
  doc["airTemp"] = sensorData.airTemp;
  doc["airHumidity"] = sensorData.airHumidity;
  doc["soilTemp"] = sensorData.soilTemp;
  doc["soilMoisture"] = sensorData.soilMoisturePercent;
  doc["relayState"] = systemState.relayState;
  doc["mlScore"] = systemState.mlScore;
  doc["uptime"] = systemState.uptime / 1000; // seconds
  
  String jsonString;
  serializeJson(doc, jsonString);
  events.send(jsonString.c_str(), "sensor-data", millis());
}

// RELAY CONTROL
void controlRelay(bool state) {
  systemState.relayState = state;
  digitalWrite(RELAY_PIN, state ? LOW : HIGH); // Relay Active LOW
  
  Serial.print("$SYS>  Relay: ");
  Serial.println(state ? "ON" : "OFF");
}