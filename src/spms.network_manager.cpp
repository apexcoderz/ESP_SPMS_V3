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
#include <HTTPClient.h>

// Instansiasi Client
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

// HTTP Logging Control
unsigned long lastHttpLog = 0;
const long httpLogInterval = 60000; // Kirim data tiap 60 detik (Bisa diubah)

// Root CA HiveMQ (Let's Encrypt ISRG Root X1)
// Opsional: Jika setInsecure() tidak jalan, uncomment setCACert di setupMQTT
const char *root_ca =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
    "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
    "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
    "WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
    "ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
    "MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3ullqGPhhvxm9eOgye\n"
    "H4L85nQjy4mgKVByNrjyftY+MaZLdvhBWIgquCAsVMG6P56X72+5LMMj4VfdNYq9\n"
    "T72tFthlabpldcO2UjradD4ZYIeLN6R67Ysg621fbgfJ9CVxK8f58oH+3fPq7q7o\n"
    "996+G0AsxFP1vFpZ+Fp1l9qpsLLc491fb1z9/R98g1391zlh121K8P+9B0b1t2q6\n"
    "q2+5m+Gq/j0kFeJ9L5R1L6t6+7+5h+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+1+\n"
    "-----END CERTIFICATE-----\n";

// =============================================================
// MQTT CALLBACK (Menerima Perintah dari Backend Node.js)
// =============================================================
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("$MQTT> Pesan masuk [");
  Serial.print(topic);
  Serial.print("]: ");

  // 1. Convert Payload ke String
  String message;
  for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }
  Serial.println(message);

  // 2. Parse JSON
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error)
  {
    Serial.print("$MQTT> JSON Error: ");
    Serial.println(error.c_str());
    return;
  }

  // 3. Eksekusi Perintah Berdasarkan "type"
  const char *type = doc["type"];
  JsonObject data = doc["payload"];

  if (strcmp(type, "SET_RELAY") == 0)
  {
    bool state = data["state"];
    Serial.printf("$MQTT> CMD: Set Relay -> %s\n", state ? "ON" : "OFF");
    controlRelay(state); // Update Hardware
    // systemState.relayState diupdate di dalam controlRelay
  }
  else if (strcmp(type, "SET_MODE") == 0 || strcmp(type, "SET_AUTO") == 0)
  {
    bool mode = data["autoMode"];
    Serial.printf("$MQTT> CMD: Set Auto Mode -> %s\n", mode ? "ON" : "OFF");
    systemState.autoMode = mode;
  }
  else if (strcmp(type, "SET_ML_CONFIG") == 0)
  {
    float thresh = data["threshold"];
    const char *plant = data["plantType"];
    
    Serial.printf("$MQTT> CMD: Config ML -> Th: %.2f, Plant: %s\n", thresh, plant);
    
    mlModel.threshold = thresh;
    mlModel.plantType = String(plant);
    saveMLConfig(); // Simpan ke EEPROM/Preferences
  }
  else if (strcmp(type, "SET_CALIBRATION") == 0)
  {
    Serial.println("$MQTT> CMD: Set Calibration");
    systemState.calAirTemp = data["calTemp"];
    systemState.calAirHum = data["calHum"];
    systemState.calSoilMoisture = data["calSoil"];
    systemState.calSoilTemp = data["calSoilTemp"];
    saveCalibrationConfig();
  }
  else if (strcmp(type, "SYSTEM_ACTION") == 0)
  {
    const char *action = data["action"];
    if (strcmp(action, "RESTART") == 0 || strcmp(action, "reboot") == 0)
    {
      Serial.println("$MQTT> CMD: Restarting System...");
      delay(1000);
      ESP.restart();
    }
  }
}

// =============================================================
// MQTT SETUP & CONNECTION
// =============================================================

void setupMQTT()
{
  // Konfigurasi SSL HiveMQ
  // espClient.setCACert(root_ca); // Gunakan sertifikat jika perlu
  espClient.setInsecure(); // Skip verifikasi sertifikat (Lebih mudah & stabil)

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback); // Daftarkan fungsi callback di atas
  mqttClient.setBufferSize(2048);       // Perbesar buffer untuk JSON panjang
}

void checkMQTTConnection()
{
  if (!mqttClient.connected())
  {
    Serial.print("$NET> Connecting to HiveMQ...");

    // Create unique Client ID
    String clientId = "ESP32-SPMS-" + String(random(0xffff), HEX);

    if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD))
    {
      Serial.println("Connected!");
      systemState.mqttConnected = true;

      // Subscribe ke topik Command
      mqttClient.subscribe(MQTT_TOPIC_SUB);
      Serial.print("$NET> Subscribed to: ");
      Serial.println(MQTT_TOPIC_SUB);
    }
    else
    {
      Serial.print("Failed, rc=");
      Serial.println(mqttClient.state());
      systemState.mqttConnected = false;
      // Delay kecil agar tidak spamming koneksi jika gagal terus
      delay(2000); 
    }
  }
  mqttClient.loop();
}

void sendDataToServer() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  WiFiClientSecure client;
  client.setInsecure(); // Agar koneksi HTTPS ke Vercel tidak ribet sertifikat

  // Mulai koneksi
  if (!http.begin(client, SERVER_LOG_URL)) {
    Serial.println("$NET> Gagal connect ke Server Log");
    return;
  }

  http.addHeader("Content-Type", "application/json");

  // Susun JSON string manual (Hemat memori)
  // Pastikan nama key ("temp", "hum") SAMA PERSIS dengan di sensorController.js
  String payload = "{";
  payload += "\"device_id\":\"ESP32-MAIN\",";
  payload += "\"temp\":" + String(sensorData.airTemp) + ",";
  payload += "\"hum\":" + String(sensorData.airHumidity) + ",";
  payload += "\"soil_moist\":" + String(sensorData.soilMoisturePercent) + ",";
  payload += "\"soil_temp\":" + String(sensorData.soilTemp) + ",";
  payload += "\"ml_score\":" + String(systemState.mlScore) + ",";
  payload += "\"relay\":" + String(systemState.relayState ? "true" : "false");
  payload += "}";

  Serial.print("$NET> Kirim Log HTTP... ");
  int httpCode = http.POST(payload);

  if (httpCode > 0) {
    Serial.printf("OK (%d)\n", httpCode);
  } else {
    Serial.printf("Gagal (%s)\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void publishSensorData()
{
  // --- BAGIAN 1: MQTT (Kirim Data Realtime) ---
  // Kode ini hanya akan dicompile jika ENABLE_MQTT_PUBLISH diset ke 1
  #if ENABLE_MQTT_PUBLISH == 1 
    if (mqttClient.connected())
    {
      StaticJsonDocument<512> doc;
      doc["temp"] = sensorData.airTemp;
      doc["hum"] = sensorData.airHumidity;
      doc["soil_moist"] = sensorData.soilMoisturePercent;
      doc["soil_temp"] = sensorData.soilTemp;
      doc["ml_score"] = systemState.mlScore;
      doc["relay"] = systemState.relayState;
      doc["auto_mode"] = systemState.autoMode;
      doc["rssi"] = WiFi.RSSI();

      char jsonBuffer[512];
      serializeJson(doc, jsonBuffer);

      if (mqttClient.publish(MQTT_TOPIC_PUB, jsonBuffer)) {
        Serial.println("$NET> MQTT Published");
      }
    }
  #endif

  // --- BAGIAN 2: HTTP LOGGING (Wajib Aktif untuk Database) ---
  unsigned long now = millis();
  if (now - lastHttpLog > httpLogInterval) 
  {
    lastHttpLog = now;
    sendDataToServer(); 
  }
}

// =============================================================
// WIFI SETUP (Static IP & AP Mode)
// =============================================================
void setupWiFi()
{
  Serial.println("\n$SYS> WiFi Setup...");

  // Try saved credentials
  if (systemState.ssid.length() > 0)
  {
    Serial.print("$SYS> Connecting to: ");
    Serial.println(systemState.ssid);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    delay(100);

    // Configure Static IP jika diaktifkan
    if (systemState.useStaticIP)
    {
      Serial.println("$SYS> Using Static IP configuration");
      if (!WiFi.config(systemState.staticIP, systemState.gateway,
                       systemState.subnet, systemState.dns))
      {
        Serial.println("$SYS> ⚠ Static IP config failed, using DHCP");
      }
    }

    WiFi.begin(systemState.ssid.c_str(), preferences.getString("password", "").c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30)
    {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
      systemState.wifiConnected = true;
      isAPMode = false;
      systemState.ipAddress = WiFi.localIP().toString();
      systemState.wifiRSSI = WiFi.RSSI();

      Serial.println("$SYS> WiFi Connected!");
      Serial.print("   IP: ");
      Serial.println(systemState.ipAddress);
      return;
    }
    else
    {
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

// =============================================================
// WEB SERVER SETUP
// =============================================================
void setupWebServer()
{
  Serial.println("\n$SYS> Starting Web Server...");

  // CORS Headers
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");

  // API: MAIN PAGE
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", getHTML()); });

  // API: Calibration
  server.on("/api/calibration", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    bool changed = false;
    if(request->hasParam("calTemp", true)) {
      systemState.calAirTemp = request->getParam("calTemp", true)->value().toFloat();
      changed = true;
    }
    if(request->hasParam("calHum", true)) {
      systemState.calAirHum = request->getParam("calHum", true)->value().toFloat();
      changed = true;
    }
    if(request->hasParam("calSoil", true)) {
      systemState.calSoilMoisture = request->getParam("calSoil", true)->value().toFloat();
      changed = true;
    }
    if(request->hasParam("calSoilTemp", true)) {
      systemState.calSoilTemp = request->getParam("calSoilTemp", true)->value().toFloat();
      changed = true;
    }

    if(changed) {
      saveCalibrationConfig(); 
      request->send(200, "text/plain", "Calibration Saved");
    } else {
      request->send(400, "text/plain", "No params");
    } });

  // API: Get Current Data
  server.on("/api/data", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    StaticJsonDocument<1024> doc;
    
    // Sensor data
    doc["airTemp"] = sensorData.airTemp;
    doc["airHumidity"] = sensorData.airHumidity;
    doc["soilTemp"] = sensorData.soilTemp;
    doc["soilMoisture"] = sensorData.soilMoisturePercent;
    
    // System state
    doc["relayState"] = systemState.relayState;
    doc["autoMode"] = systemState.autoMode;
    doc["mlScore"] = systemState.mlScore;
    doc["uptime"] = systemState.uptime / 1000;
    
    // WiFi info
    doc["wifiConnected"] = systemState.wifiConnected;
    doc["ssid"] = systemState.ssid;
    doc["ipAddress"] = systemState.ipAddress;
    
    // ML Config
    doc["mlThreshold"] = mlModel.threshold;
    doc["plantType"] = mlModel.plantType;

    // Calibration
    doc["calTemp"] = systemState.calAirTemp;
    doc["calHum"] = systemState.calAirHum;
    doc["calSoil"] = systemState.calSoilMoisture;
    doc["calSoilTemp"] = systemState.calSoilTemp;
    
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response); });

  // API: Get Chart Data
  server.on("/api/chart", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              request->send(200, "application/json", getChartDataJSON());
            });

  // API: Control Relay
  server.on("/api/relay", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    if(request->hasParam("state", true)) {
      String state = request->getParam("state", true)->value();
      bool on = (state == "true" || state == "1");
      controlRelay(on);
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Missing parameter");
    } });

  // API: Toggle Auto Mode
  server.on("/api/automode", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    if(request->hasParam("state", true)) {
      String state = request->getParam("state", true)->value();
      systemState.autoMode = (state == "true" || state == "1");
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Missing parameter");
    } });

  // API: Configure Static IP
  server.on("/api/staticip", HTTP_POST, [](AsyncWebServerRequest *request)
            {
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
    } });

  // API: WiFi Config
  server.on("/api/wifi", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    if(request->hasParam("ssid", true) && request->hasParam("password", true)) {
      String ssid = request->getParam("ssid", true)->value();
      String password = request->getParam("password", true)->value();

      systemState.isWifiConfiguring = true; 
      saveWiFiCredentials(ssid, password);

      layarOled.clearDisplay();
      layarOled.setCursor(0, 0);
      layarOled.println("Konfigurasi Disimpan");
      layarOled.display();

      WiFi.mode(WIFI_AP_STA);
      WiFi.begin(ssid.c_str(), password.c_str());
      
      int attempt = 0;
      bool connected = false;
      while(attempt < 30) {
        if(WiFi.status() == WL_CONNECTED) {
          connected = true;
          break;
        }
        delay(500);
        attempt++;
      }

      if(connected) {
        request->send(200, "text/plain", "Connected! Restarting...");
        delay(3000);
        WiFi.softAPdisconnect(true);
        ESP.restart();
      } else {
        request->send(400, "text/plain", "Failed to connect");
        systemState.isWifiConfiguring = false; 
      }
    } else {
      request->send(400, "text/plain", "Missing parameters");
    } });

  // API: ML Config
  server.on("/api/mlconfig", HTTP_POST, [](AsyncWebServerRequest *request)
            {
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
      request->send(400, "text/plain", "No parameters");
    } });

  // API: System Reset
  server.on("/api/reset", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    request->send(200, "text/plain", "Resetting...");
    delay(500);
    ESP.restart(); });

  // 404 & CORS OPTIONS
  server.onNotFound([](AsyncWebServerRequest *request)
                    {
    if (request->method() == HTTP_OPTIONS) {
      request->send(200);
    } else {
      request->send(404, "text/plain", "Not Found");
    } });

  server.begin();
  Serial.println("   $SYS>  Web Server Started");
}