/*!
 * SPMS Library
 * Copyright (c) 2024-2026 ApexCoderz
 * Licensed under Custom Non-Commercial License.
 * See LICENSE file for details.
 *
 * @file spms.types.h
 */

#ifndef SPMS_TYPES_H
#define SPMS_TYPES_H

#include <Arduino.h>
#include <IPAddress.h>

// Sensor data structure
struct SensorData
{
  float airTemp = 0;
  float airHumidity = 0;
  float soilTemp = 0;
  int soilMoisture = 0;
  int soilMoisturePercent = 0;
};

// Historical data
struct ChartData
{
  float temps[72]; // Using constant 72 manually to avoid circular dependency
  float humidity[72];
  float soilMoisture[72];
  unsigned long timestamps[72];
  int currentIndex = 0;
  bool bufferFilled = false;
};

// System states
struct SystemState
{
  bool relayState = false;
  bool autoMode = true;
  float mlScore = 0;
  bool wifiConnected = false;
  String ssid = "";
  String ipAddress = "";
  bool useStaticIP = false;
  IPAddress staticIP;
  IPAddress gateway;
  IPAddress subnet;
  IPAddress dns;
  unsigned long uptime = 0;
  int wifiRSSI = 0;
  bool isWifiConfiguring = false;
  float calAirTemp = 0.0;      // Kalibrasi Suhu Udara
  float calAirHum = 0.0;       // Kalibrasi Kelembaban Udara
  float calSoilMoisture = 0.0; // Kalibrasi Kelembaban Tanah
  float calSoilTemp = 0.0;     // Kalibrasi Suhu Tanah
};

struct MLModel
{
  float soilMoistureWeight = 0.50;
  float soilTempWeight = 0.20;
  float airHumidityWeight = 0.20;
  float airTempWeight = 0.10;
  float threshold = 0.45;
  float hysteresis = 0.10;
  float soilTempMin = 15.0;
  float soilTempMax = 35.0;
  float airTempMin = 15.0;
  float airTempMax = 40.0;
  float optimalSoilMoisture = 60.0;
  float optimalSoilTemp = 25.0;
  String plantType = "General";
};

struct PlantPreset
{
  String name;
  float moistureTarget;
  float threshold;
  String description;
};

// Animation States
enum AnimState
{
  PARTICLE_BURST,
  GLITCH_REVEAL,
  HOLOGRAM_FRAME,
  PULSE_GLOW,
  DIGITAL_DISSOLVE,
  INFO_DISPLAY,
  FINISHED
};

struct Particle
{
  float x, y, vx, vy;
  bool active;
};

#endif