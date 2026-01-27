/*!
 * SPMS Core Library
 * Copyright (c) 2024-2026 ApexCoderz
 * Licensed under Custom Non-Commercial License.
 * See LICENSE file for details.
 *
 * @file spms.config.h
 */

#ifndef SPMS_CONFIG_H
#define SPMS_CONFIG_H

//  CREDITS & MACROS
#define ApexCoderz // Do Not Remove
#define Codecztron // Do Not Remove

//  PIN DEFINITIONS
#define OLED_SDA 21
#define OLED_SCL 22
#define RELAY_PIN 26
#define DS18B20_PIN 13
#define OLED_RESET -1
#define OLED_HEIGHT 32
#define OLED_WIDTH 128
#define SCREEN_ADDRESS 0x3C
#define SOIL_MOISTURE_PIN 34

//  TIMING CONFIGURATION
#define SENSOR_READ_INTERVAL 2000    // 2 Seconds
#define DISPLAY_UPDATE_INTERVAL 3000 // 3 Seconds
#define ML_UPDATE_INTERVAL 10000     // 10 Seconds
#define CHART_DATA_POINTS 72         // 72 points = 24 Hour
#define CHART_SAVE_INTERVAL 1200000  // 20 Minute

#define MQTT_BROKER     "2c0f5cbc458c41f2b7d3ead26515e9c3.s1.eu.hivemq.cloud"
#define MQTT_PORT       8883                        
#define MQTT_USER       "AgroDurenIOT_1"                
#define MQTT_PASSWORD   "0gtG4uXx8Jq6TkJf"         
#define MQTT_TOPIC_PUB  "spms/data"
#define MQTT_TOPIC_SUB "spms/command"

#endif
