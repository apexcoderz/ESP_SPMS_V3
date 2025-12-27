/*!
 * SPMS Library
 * Copyright (c) 2024-2026 ApexCoderz
 * Licensed under Custom Non-Commercial License. 
 * See LICENSE file for details.
 * * @file spms.sensor_module.cpp
 */

#include "ESP_SPMS_V3.h"
#include "spms.sensor_module.h"
#include "spms.globals.h"
#include "spms.config.h" 
#include <ArduinoJson.h>

// SENSOR SETUP
void setupSensors() {
  Serial.println("\n$SYS> Initializing Sensors...");
  
  // Init AHT10/AHT20 (Suhu & Lembab Udara)
  if (!sensor_ahteh.begin()) {
    Serial.println("$SYS> ⚠ AHT10/AHT20 not found!");
  } else {
    Serial.println("$SYS> AHT10/AHT20 OK");
  }

  // Init DS18B20 (Suhu Tanah)
  ds18b20.begin();
  int deviceCount = ds18b20.getDeviceCount();
  Serial.print("$SYS> DS18B20 OK (");
  Serial.print(deviceCount);
  Serial.println(" devices)");

  // Init Soil Moisture Pin
  pinMode(SOIL_MOISTURE_PIN, INPUT);
  Serial.println("$SYS> Soil Moisture Sensor OK");
}

// READ SENSORS - Optimized dengan error handling & Kalibrasi
void readSensors() {
  // -----------------------------------------------------------
  // BACA SUHU & KELEMBABAN UDARA (AHT10/20)
  // -----------------------------------------------------------
  sensors_event_t humidity, temp;
  float rawAirTemp = 0.0;
  float rawAirHum = 0.0;

  if(sensor_ahteh.getEvent(&humidity, &temp)) {
    rawAirTemp = temp.temperature;
    rawAirHum = humidity.relative_humidity;
  } else {
    Serial.println("$SENS> Error reading AHT10");
  }

  // Terapkan Kalibrasi
  sensorData.airTemp = rawAirTemp + systemState.calAirTemp;
  sensorData.airHumidity = rawAirHum + systemState.calAirHum;

  // -----------------------------------------------------------
  // BACA KELEMBABAN TANAH (Capacitive/Resistive)
  // -----------------------------------------------------------
  long sum = 0;
  for (int i = 0; i < 10; i++) { // Rata-rata 10 sampel agar stabil
    sum += analogRead(SOIL_MOISTURE_PIN);
    delay(5);
  }
  int rawSoil = sum / 10;
  sensorData.soilMoisture = rawSoil; // Simpan nilai raw untuk debug

  // Konversi ke Persen (Map nilai analog ke 0-100%)
  // Sesuaikan nilai 3000 (Kering) dan 1200 (Basah) dengan karakteristik sensor Anda
  // Anda bisa ganti angka ini dengan SOIL_DRY dan SOIL_WET jika ada di config.h
  int moisturePercent = map(rawSoil, 3000, 1200, 0, 100); 
  
  // Terapkan Kalibrasi (Offset dalam %)
  sensorData.soilMoisturePercent = moisturePercent + (int)systemState.calSoilMoisture;

  // Pastikan range tetap 0-100%
  sensorData.soilMoisturePercent = constrain(sensorData.soilMoisturePercent, 0, 100);

  // -----------------------------------------------------------
  // 3. BACA SUHU TANAH (DS18B20)
  // -----------------------------------------------------------
  ds18b20.requestTemperatures(); 
  float rawSoilTemp = ds18b20.getTempCByIndex(0);

  // Cek jika sensor error (biasanya -127)
  if(rawSoilTemp == DEVICE_DISCONNECTED_C) {
     Serial.println("$SENS> ⚠ DS18B20 Error");
     rawSoilTemp = 0.0; 
  }

  // Terapkan Kalibrasi
  sensorData.soilTemp = rawSoilTemp + systemState.calSoilTemp;

  // -----------------------------------------------------------
  // 4. KIRIM UPDATE VIA SSE (Realtime ke App)
  // -----------------------------------------------------------
  StaticJsonDocument<512> doc;
  doc["airTemp"] = sensorData.airTemp;
  doc["airHumidity"] = sensorData.airHumidity;
  doc["soilTemp"] = sensorData.soilTemp;
  doc["soilMoisture"] = sensorData.soilMoisturePercent;
  doc["relayState"] = systemState.relayState;
  doc["mlScore"] = systemState.mlScore;
  doc["uptime"] = systemState.uptime / 1000;
  
  // Kirim juga data kalibrasi saat ini
  doc["calTemp"] = systemState.calAirTemp;
  doc["calHum"] = systemState.calAirHum;
  doc["calSoil"] = systemState.calSoilMoisture;
  doc["calSoilTemp"] = systemState.calSoilTemp;

  String jsonString;
  serializeJson(doc, jsonString);
  events.send(jsonString.c_str(), "sensor-data", millis());
}

// RELAY CONTROL
void controlRelay(bool state) {
  systemState.relayState = state;

  digitalWrite(RELAY_PIN, state ? LOW : HIGH); 

  Serial.print("$SYS> Relay: ");
  Serial.println(state ? "ON" : "OFF");
}