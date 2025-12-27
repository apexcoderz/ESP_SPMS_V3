/*!
 * SPMS Library
 * Copyright (c) 2024-2026 ApexCoderz
 * Licensed under Custom Non-Commercial License. 
 * See LICENSE file for details.
 * 
 * @file spms.network_manager.h
 */

#ifndef SPMS_NETWORK_MANAGER_H
#define SPMS_NETWORK_MANAGER_H
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

void setupWiFi();
void setupWebServer();
void controlRelay(bool state);

void setupMQTT();
void checkMQTTConnection();
void publishSensorData();

extern WiFiClientSecure espClient;
extern PubSubClient mqttClient;
#endif
