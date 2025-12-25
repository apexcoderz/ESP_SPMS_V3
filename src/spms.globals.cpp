/*!
 * SPMS Library
 * Copyright (c) 2024-2026 ApexCoders
 * Licensed under Custom Non-Commercial License. 
 * See LICENSE file for details.
 * 
 * @file spms.globals.cpp
 */

#include "ESP_SPMS_V3.h"
#include "spms.globals.h"

// Hardware Objects
Adafruit_AHTX0 sensor_ahteh;
Preferences preferences;
AsyncWebServer server(80);
OneWire oneWire(DS18B20_PIN);
AsyncEventSource events("/events");
DallasTemperature ds18b20(&oneWire);
Adafruit_SSD1306 layarOled(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

// Data Structures
SensorData sensorData;
ChartData chartData;
SystemState systemState;
MLModel mlModel;

// Presets
const PlantPreset PLANT_PRESETS[] = {
  {"Umum", 60.0, 0.45, "Tanaman hias & sayuran umum"},
  {"Cabai", 65.0, 0.42, "Cabai rawit, cabai merah"},
  {"Tomat", 70.0, 0.40, "Tomat sayur & tomat ceri"},
  {"Kangkung", 75.0, 0.38, "Kangkung & sayuran hijau"},
  {"Kaktus", 35.0, 0.65, "Kaktus & sukulen"},
  {"Anggrek", 55.0, 0.48, "Anggrek & tanaman tropis"},
  {"Padi", 80.0, 0.35, "Padi & tanaman air"},
  {"Jagung", 60.0, 0.45, "Jagung & serealia"}
};
const int NUM_PRESETS = 8;

// WiFi Config
String apSSID = "SPMSv3-Config";
String apPassword = "SPMSAP26A";
bool isAPMode = true;

// Animation
const int MAX_PARTICLES = 30;
Particle particles[MAX_PARTICLES];
AnimState currentState = PARTICLE_BURST;
unsigned long stateStartTime = 0;
int glitchFrame = 0;
int frameProgress = 0;
int pulseFrame = 0;
int dissolveFrame = 0;
bool pixelMap[128][32];

int displayMode = 0;

// Timing
unsigned long lastSensorRead = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastMLUpdate = 0;
unsigned long lastChartSave = 0;