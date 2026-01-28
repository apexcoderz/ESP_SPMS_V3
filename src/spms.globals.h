/*!
 * SPMS Library
 * Copyright (c) 2024-2026 ApexCoderz
 * Licensed under Custom Non-Commercial License. 
 * See LICENSE file for details.
 * 
 * @file spms.globals.h
 */

#ifndef SPMS_GLOBALS_H
#define SPMS_GLOBALS_H

#include <Arduino.h>
#include <Wire.h>
#include <SPIFFS.h>
#include <OneWire.h>
#include <Preferences.h>
#include <Adafruit_GFX.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_SSD1306.h>
#include <DallasTemperature.h>
#include <ESPAsyncWebServer.h>
#include "spms.types.h"
#include "spms.config.h"
// server log URL
#define SERVER_LOG_URL "https://server-node-agro-duren.vercel.app/api/sensor/log"
// --- KONFIGURASI PENGIRIMAN DATA ---
#define ENABLE_MQTT_PUBLISH  0  // Set ke 0 untuk MATIKAN kirim data MQTT (Hemat)
                                // Set ke 1 untuk NYALAKAN kirim data MQTT (Realtime)

// Hardware Objects
extern Adafruit_AHTX0 sensor_ahteh;
extern Preferences preferences;
extern AsyncWebServer server;
extern OneWire oneWire;
extern AsyncEventSource events;
extern DallasTemperature ds18b20;
extern Adafruit_SSD1306 layarOled;

// Global Data structs
extern SensorData sensorData;
extern ChartData chartData;
extern SystemState systemState;
extern MLModel mlModel;

// Constants
extern const PlantPreset PLANT_PRESETS[];
extern const int NUM_PRESETS;
extern String apSSID;
extern String apPassword;
extern bool isAPMode;

// Animation Globals
extern const int MAX_PARTICLES;
extern Particle particles[];
extern AnimState currentState;
extern unsigned long stateStartTime;
extern int glitchFrame;
extern int frameProgress;
extern int pulseFrame;
extern int dissolveFrame;
extern bool pixelMap[128][32];

// Display State
extern int displayMode;

// Timing
extern unsigned long lastSensorRead;
extern unsigned long lastDisplayUpdate;
extern unsigned long lastMLUpdate;
extern unsigned long lastChartSave;

#endif