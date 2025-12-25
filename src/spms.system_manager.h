/*!
 * SPMS Library
 * Copyright (c) 2024-2026 ApexCoderz
 * Licensed under Custom Non-Commercial License. 
 * See LICENSE file for details.
 * 
 * @file spms.system_manager.h
 */

#ifndef SPMS_SYSTEM_MANAGER_H
#define SPMS_SYSTEM_MANAGER_H

#include <Arduino.h>

extern void saveWiFiCredentials(String ssid, String password);
extern bool loadWiFiCredentials();
extern void saveStaticIPConfig();
extern bool loadStaticIPConfig();
extern void loadMLConfig();
extern void saveMLConfig();
extern void addChartDataPoint();
String getChartDataJSON();
extern void runMLModel();

#endif