/*!
 * SPMS Library
 * Copyright (c) 2024-2026 ApexCoderz
 * Licensed under Custom Non-Commercial License.
 * See LICENSE file for details.
 *
 * @file spms.system_manager.cpp
 */

#include "ESP_SPMS_V3.h"
#include "spms.system_manager.h"
#include "spms.globals.h"
#include "spms.sensor_module.h"
#include <ArduinoJson.h>

// PREFERENCES - Load & Save
bool loadWiFiCredentials()
{
  systemState.ssid = preferences.getString("ssid", "");
  return (systemState.ssid.length() > 0);
}

void saveWiFiCredentials(String ssid, String password)
{
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  systemState.ssid = ssid;
  Serial.println("$SYS> WiFi credentials saved");
}

bool loadStaticIPConfig()
{
  systemState.useStaticIP = preferences.getBool("useStaticIP", false);

  if (systemState.useStaticIP)
  {
    uint32_t ip = preferences.getUInt("staticIP", 0);
    uint32_t gw = preferences.getUInt("gateway", 0);
    uint32_t sn = preferences.getUInt("subnet", 0);
    uint32_t dns = preferences.getUInt("dns", 0);

    if (ip != 0)
    {
      systemState.staticIP = IPAddress(ip);
      systemState.gateway = IPAddress(gw);
      systemState.subnet = IPAddress(sn);
      systemState.dns = IPAddress(dns);
      return true;
    }
  }
  return false;
}

void saveStaticIPConfig()
{
  preferences.putBool("useStaticIP", systemState.useStaticIP);
  preferences.putUInt("staticIP", (uint32_t)systemState.staticIP);
  preferences.putUInt("gateway", (uint32_t)systemState.gateway);
  preferences.putUInt("subnet", (uint32_t)systemState.subnet);
  preferences.putUInt("dns", (uint32_t)systemState.dns);
  Serial.println("$SYS> Static IP config saved");
}

void loadMLConfig()
{
  mlModel.threshold = preferences.getFloat("mlThreshold", 0.45);
  mlModel.soilMoistureWeight = preferences.getFloat("mlWeight1", 0.50);
  mlModel.soilTempWeight = preferences.getFloat("mlWeight2", 0.20);
  mlModel.airHumidityWeight = preferences.getFloat("mlWeight3", 0.20);
  mlModel.airTempWeight = preferences.getFloat("mlWeight4", 0.10);
  mlModel.plantType = preferences.getString("plantType", "Umum");
  mlModel.optimalSoilMoisture = preferences.getFloat("optimalMoist", 60.0);
}

void saveMLConfig()
{
  preferences.putFloat("mlThreshold", mlModel.threshold);
  preferences.putFloat("mlWeight1", mlModel.soilMoistureWeight);
  preferences.putFloat("mlWeight2", mlModel.soilTempWeight);
  preferences.putFloat("mlWeight3", mlModel.airHumidityWeight);
  preferences.putFloat("mlWeight4", mlModel.airTempWeight);
  preferences.putString("plantType", mlModel.plantType);
  preferences.putFloat("optimalMoist", mlModel.optimalSoilMoisture);
  Serial.println("$SYS> ML config saved");
}

void loadCalibrationConfig()
{
  systemState.calAirTemp = preferences.getFloat("cal_at", 0.0);
  systemState.calAirHum = preferences.getFloat("cal_ah", 0.0);
  systemState.calSoilMoisture = preferences.getFloat("cal_sm", 0.0);
  systemState.calSoilTemp = preferences.getFloat("cal_st", 0.0);
}

void saveCalibrationConfig()
{
  preferences.putFloat("cal_at", systemState.calAirTemp);
  preferences.putFloat("cal_ah", systemState.calAirHum);
  preferences.putFloat("cal_sm", systemState.calSoilMoisture);
  preferences.putFloat("cal_st", systemState.calSoilTemp);
  Serial.println("$SYS> Calibration Saved!");
}

// CHART DATA MANAGEMENT
void addChartDataPoint()
{
  int idx = chartData.currentIndex;

  chartData.temps[idx] = sensorData.airTemp;
  chartData.humidity[idx] = sensorData.airHumidity;
  chartData.soilMoisture[idx] = sensorData.soilMoisturePercent;
  chartData.timestamps[idx] = millis() / 1000; // seconds

  chartData.currentIndex = (chartData.currentIndex + 1) % CHART_DATA_POINTS;

  if (chartData.currentIndex == 0)
  {
    chartData.bufferFilled = true;
  }
}

String getChartDataJSON()
{
  DynamicJsonDocument doc(4096);
  JsonArray tempArr = doc.createNestedArray("temps");
  JsonArray humArr = doc.createNestedArray("humidity");
  JsonArray moistArr = doc.createNestedArray("soilMoisture");
  JsonArray timeArr = doc.createNestedArray("timestamps");

  int points = chartData.bufferFilled ? CHART_DATA_POINTS : chartData.currentIndex;
  int startIdx = chartData.bufferFilled ? chartData.currentIndex : 0;

  for (int i = 0; i < points; i++)
  {
    int idx = (startIdx + i) % CHART_DATA_POINTS;
    tempArr.add(chartData.temps[idx]);
    humArr.add(chartData.humidity[idx]);
    moistArr.add(chartData.soilMoisture[idx]);
    timeArr.add(chartData.timestamps[idx]);
  }

  String output;
  serializeJson(doc, output);
  return output;
}

//  TinyML Algorithm
void runMLModel()
{

  // NORMALIZATION
  // Soil moisture (0-100% → 0-1, inverted, lower = need water)
  float normSoilMoisture = 1.0 - (sensorData.soilMoisturePercent / 100.0);

  // Soil temperature (optimal range scoring)
  float soilTempDiff = abs(sensorData.soilTemp - mlModel.optimalSoilTemp);
  float normSoilTemp = constrain(soilTempDiff / 15.0, 0, 1);

  // Air humidity (lower = more evaporation = need more water)
  float normAirHumidity = 1.0 - (sensorData.airHumidity / 100.0);

  // Air temperature (higher = more evaporation)
  float normAirTemp = constrain((sensorData.airTemp - mlModel.airTempMin) /
                                    (mlModel.airTempMax - mlModel.airTempMin),
                                0, 1);

  // WEIGHTED SCORING
  systemState.mlScore = (normSoilMoisture * mlModel.soilMoistureWeight) +
                        (normSoilTemp * mlModel.soilTempWeight) +
                        (normAirHumidity * mlModel.airHumidityWeight) +
                        (normAirTemp * mlModel.airTempWeight);

  // Clamp score
  systemState.mlScore = constrain(systemState.mlScore, 0, 1);

  // AUTO MODE CONTROL with Hysteresis
  if (systemState.autoMode)
  {
    if (systemState.mlScore > mlModel.threshold && !systemState.relayState)
    {
      // Turn ON
      controlRelay(true);
      Serial.println("$SYS> Auto watering: ON");
      Serial.print("   ML Score: ");
      Serial.println(systemState.mlScore, 3);
      /// i'm also heree, adit was anywhere and everywhere >_<
    }
    else if (systemState.mlScore < (mlModel.threshold - mlModel.hysteresis) && systemState.relayState)
    {
      // Turn OFF
      controlRelay(false);
      Serial.println("$SYS> Auto watering: OFF");
      Serial.print("   ML Score: ");
      Serial.println(systemState.mlScore, 3);
    }
  }
}
