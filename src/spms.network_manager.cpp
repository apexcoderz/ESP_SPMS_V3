/*!
 * SPMS Library
 * Copyright (c) 2024-2026 ApexCoderz
 * Licensed under Custom Non-Commercial License. 
 * See LICENSE file for details.
 * 
 * @file spms.network_manager.cpp
 */

#include "ESP_SPMS_V3.h"
#include "spms.network_manager.h"
#include "spms.globals.h"
#include "spms.system_manager.h"
#include "spms.sensor_module.h"
#include "spms.web_content.h"
#include <ArduinoJson.h>
#include <WiFi.h>


// WIFI SETUP - with Static IP
void setupWiFi() {
  Serial.println("\n$SYS> WiFi Setup...");
  
  // Try saved credentials
  if (systemState.ssid.length() > 0) {
    Serial.print("$SYS> Connecting to: ");
    Serial.println(systemState.ssid);
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    delay(100);
    
    // Configure Static IP jika diaktifkan
    if (systemState.useStaticIP) {
      Serial.println("$SYS> Using Static IP configuration");
      if (!WiFi.config(systemState.staticIP, systemState.gateway, 
                       systemState.subnet, systemState.dns)) {
        Serial.println("$SYS> ⚠ Static IP config failed, using DHCP");
      }
    }
    
    // hi adit is here!!
    WiFi.begin(systemState.ssid.c_str(), preferences.getString("password", "").c_str());
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
      systemState.wifiConnected = true;
      isAPMode = false;
      systemState.ipAddress = WiFi.localIP().toString();
      systemState.wifiRSSI = WiFi.RSSI();
      
      Serial.println("$SYS> WiFi Connected!");
      Serial.print("   IP: ");
      Serial.println(systemState.ipAddress);
      Serial.print("   RSSI: ");
      Serial.print(systemState.wifiRSSI);
      Serial.println(" dBm");
      return;
    } else {
      Serial.println("$SYS> Connection failed");
    }
  }
  
  // Start AP Mode
  Serial.println("$SYS> Starting Access Point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID.c_str(), apPassword.c_str());
  
  systemState.ipAddress = WiFi.softAPIP().toString();
  systemState.wifiConnected = false;
  isAPMode = true;
  
  Serial.print("$SYS> AP Started: ");
  Serial.println(apSSID);
  Serial.print("   AP IP: ");
  Serial.println(systemState.ipAddress);
}

void setupWebServer() {
  Serial.println("\n$SYS> Starting Web Server...");
  
  // API: MAIN PAGE
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", getHTML());
  });

  // API: Get Current Sensor Data
  server.on("/api/data", HTTP_GET, [](AsyncWebServerRequest *request){
    StaticJsonDocument<768> doc;
    
    // Sensor data
    doc["airTemp"] = sensorData.airTemp;
    doc["airHumidity"] = sensorData.airHumidity;
    doc["soilTemp"] = sensorData.soilTemp;
    doc["soilMoisture"] = sensorData.soilMoisturePercent;
    doc["soilMoistureRaw"] = sensorData.soilMoisture;
    
    // System state
    doc["relayState"] = systemState.relayState;
    doc["autoMode"] = systemState.autoMode;
    doc["mlScore"] = systemState.mlScore;
    doc["uptime"] = systemState.uptime / 1000;
    
    // WiFi info
    doc["wifiConnected"] = systemState.wifiConnected;
    doc["isAPMode"] = isAPMode;
    doc["ssid"] = systemState.ssid;
    doc["ipAddress"] = systemState.ipAddress;
    doc["rssi"] = systemState.wifiRSSI;
    
    // ML Config
    doc["mlThreshold"] = mlModel.threshold;
    doc["plantType"] = mlModel.plantType;
    
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });
  
  // API: Get Chart Data  
  server.on("/api/chart", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", getChartDataJSON());
    // you'l find me anywhere! aditya 
  });
  
  // API: Control Relay (Manual)
  server.on("/api/relay", HTTP_POST, [](AsyncWebServerRequest *request){
    if(request->hasParam("state", true)) {
      String state = request->getParam("state", true)->value();
      controlRelay(state == "true" || state == "1");
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Missing parameter");
    }
  });
  
  // API: Toggle Auto Mode
  server.on("/api/automode", HTTP_POST, [](AsyncWebServerRequest *request){
    if(request->hasParam("state", true)) {
      String state = request->getParam("state", true)->value();
      bool newState = (state == "true" || state == "1");
      systemState.autoMode = newState;
      // Persist the selection so restarts keep the user preference
      preferences.putBool("autoMode", newState);

      // Log for debugging
      Serial.print("$SYS> /api/automode set to ");
      Serial.println(newState ? "ON" : "OFF");

      // Immediately notify connected clients via SSE (as part of sensor-data channel)
      StaticJsonDocument<128> doc;
      doc["autoMode"] = systemState.autoMode;
      doc["relayState"] = systemState.relayState;
      String payload;
      serializeJson(doc, payload);
      events.send(payload.c_str(), "sensor-data", millis());

      // Return the JSON payload as confirmation
      request->send(200, "application/json", payload);
    } else {
      request->send(400, "text/plain", "Missing parameter");
    }
  });

  // API: Save WiFi Credentials & Auto Connect Check
  server.on("/api/wifi", HTTP_POST, [](AsyncWebServerRequest *request){
    if(request->hasParam("ssid", true) && request->hasParam("password", true)) {
      String ssid = request->getParam("ssid", true)->value();
      String password = request->getParam("password", true)->value();
      

      // 0. STOP main loop display update
      systemState.isWifiConfiguring = true; 

      // 1. saving credential data to memory
      saveWiFiCredentials(ssid, password);

      layarOled.clearDisplay();
      layarOled.setTextSize(1);
      layarOled.setCursor(0, 0);
      layarOled.println("Konfigurasi Disimpan");
      layarOled.setCursor(0, 15);
      layarOled.println("Mencoba Koneksi...");
      layarOled.display();

      // 3. activating Dual mode (AP+STA) 
      WiFi.mode(WIFI_AP_STA);
      WiFi.begin(ssid.c_str(), password.c_str());
      
      // 4. waiting for connection syatus (Timeout 15 sec)
      int attempt = 0;
      bool connected = false;
      while(attempt < 30) { // 30 x 500ms = 15 sec
        if(WiFi.status() == WL_CONNECTED) {
          connected = true;
          break;
        }
       
        layarOled.print(".");
        layarOled.display();
        delay(500);
        attempt++;
      }

      if(connected) {
        // A. display connectivity on oled disp
        layarOled.clearDisplay();
        layarOled.setTextSize(1);
        layarOled.setCursor(0, 0);
        layarOled.println("WiFi CONNECTED! [OK]");
        layarOled.setCursor(0, 15);
        layarOled.println("Restarting sytem...");
        layarOled.display();

        // B. send notification to webserver
        request->send(200, "text/plain", "connnection sucess, restarting system in 3 seconds...");
        
        // C. delay 3 sec
        delay(3000);
        
        // D. Shutdown AP Mode
        WiFi.softAPdisconnect(true);
        WiFi.mode(WIFI_STA);
        
        // E. Restart systems
        ESP.restart();
        
      } else {
        // if connection was failed ( tobat )
        
        //disp notification on oled display
        layarOled.clearDisplay();
        layarOled.setCursor(0, 0);
        layarOled.println("Can't connect!");
        layarOled.setCursor(0, 15);
        layarOled.println("check your password.");
        layarOled.display();
        
        //send notification on webserver
        request->send(400, "text/plain", "Cannot coonnected to WiFi... check your password");
  
        //0. Resume main loop
        delay(3000); 
        systemState.isWifiConfiguring = false; 
      }

    } else {
      request->send(400, "text/plain", "Missing parameters");
    }
  });
  
  
  // API: Configure Static IP
  server.on("/api/staticip", HTTP_POST, [](AsyncWebServerRequest *request){
    bool useStatic = request->hasParam("useStatic", true) && 
                     request->getParam("useStatic", true)->value() == "true";
    
    systemState.useStaticIP = useStatic;
    
    if (useStatic && request->hasParam("ip", true) && 
        request->hasParam("gateway", true) && 
        request->hasParam("subnet", true)) {
      
      String ipStr = request->getParam("ip", true)->value();
      String gwStr = request->getParam("gateway", true)->value();
      String snStr = request->getParam("subnet", true)->value();
      String dnsStr = request->hasParam("dns", true) ? 
                      request->getParam("dns", true)->value() : gwStr;
      
      systemState.staticIP.fromString(ipStr);
      systemState.gateway.fromString(gwStr);
      systemState.subnet.fromString(snStr);
      systemState.dns.fromString(dnsStr);
      
      saveStaticIPConfig();
      request->send(200, "text/plain", "Static IP saved. Restart to apply.");
    } else if (!useStatic) {
      saveStaticIPConfig();
      request->send(200, "text/plain", "DHCP enabled. Restart to apply.");
    } else {
      request->send(400, "text/plain", "Missing IP parameters");
    }
  });
  
  // API: Update ML Threshold
  server.on("/api/mlconfig", HTTP_POST, [](AsyncWebServerRequest *request){
    bool changed = false;
    
    if(request->hasParam("threshold", true)) {
      float newThreshold = request->getParam("threshold", true)->value().toFloat();
      if (newThreshold >= 0 && newThreshold <= 1.0) {
        mlModel.threshold = newThreshold;
        changed = true;
      }
    }
    
    if(request->hasParam("plantType", true)) {
      mlModel.plantType = request->getParam("plantType", true)->value();
      changed = true;
    }
    
    if(request->hasParam("optimalMoisture", true)) {
      mlModel.optimalSoilMoisture = request->getParam("optimalMoisture", true)->value().toFloat();
      changed = true;
    }
    
    if (changed) {
      saveMLConfig();
      request->send(200, "text/plain", "ML config updated");
    } else {
      request->send(400, "text/plain", "No valid parameters");
    }
  });
  
  // API: Get Plant Presets
  server.on("/api/presets", HTTP_GET, [](AsyncWebServerRequest *request){
    DynamicJsonDocument doc(1024);
    JsonArray array = doc.to<JsonArray>();
    
    for(int i = 0; i < NUM_PRESETS; i++) {
      JsonObject preset = array.createNestedObject();
      preset["name"] = PLANT_PRESETS[i].name;
      preset["moistureTarget"] = PLANT_PRESETS[i].moistureTarget;
      preset["threshold"] = PLANT_PRESETS[i].threshold;
      preset["description"] = PLANT_PRESETS[i].description;
    }
    
    String response;
    //Anytime Anywhere Adit is EVERYWHERE!
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });
  
  // API: Apply Plant Preset
  server.on("/api/applypreset", HTTP_POST, [](AsyncWebServerRequest *request){
    if(request->hasParam("index", true)) {
      int idx = request->getParam("index", true)->value().toInt();
      if (idx >= 0 && idx < NUM_PRESETS) {
        mlModel.plantType = PLANT_PRESETS[idx].name;
        mlModel.threshold = PLANT_PRESETS[idx].threshold;
        mlModel.optimalSoilMoisture = PLANT_PRESETS[idx].moistureTarget;
        saveMLConfig();
        request->send(200, "text/plain", "yeyy... Preset diterapkan");
      } else {
        request->send(400, "text/plain", "yahh.. Index preset tidak valid");
      }
    } else {
      request->send(400, "text/plain", "LOH!?. Parameternya ilang");
    }
  });
  
  // API: System Reset
  server.on("/api/reset", HTTP_POST, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Mereset data...");
    delay(500);
    ESP.restart();
  });
  
  // API: Clear WiFi Config (Reset to AP mode)
  server.on("/api/clearwifi", HTTP_POST, [](AsyncWebServerRequest *request){
    preferences.remove("ssid");
    preferences.remove("password");
    request->send(200, "text/plain", "Wifi berhasil di hapus, merestart...");
    delay(1000);
    ESP.restart();
  });
  
  // Server-Sent Events (SSE) 
  events.onConnect([](AsyncEventSourceClient *client){
    Serial.println("$SYS>  SSE Client Connected");
    client->send("connected", NULL, millis(), 1000);
  });
  server.addHandler(&events);
  
  // 404 Handler
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "Not Found");
  });
  
  // Start server
  server.begin();
  Serial.println("   $SYS>  Web Server Started");
  Serial.print("   Access at: http://");
  Serial.println(systemState.ipAddress);
}
