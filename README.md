# ESP32-SPMS: Smart Plant Monitoring System v3.0

![Platform](https://img.shields.io/badge/Platform-ESP32/8266-blue)
![Language](https://img.shields.io/badge/Language-C%2B%2B-orange)
![Language](https://img.shields.io/badge/Language-html-blue)
![License](https://img.shields.io/badge/License-Non--Commercial-red)
![Version](https://img.shields.io/badge/Version-3.0-green)

**ESP32-SPMS v3.0** is an advanced IoT solution designed for precision agriculture and smart gardening research. It integrates TinyML (Machine Learning) algorithms to make intelligent watering decisions based on environmental data, coupled with a responsive, modern Web UI and real-time OLED visualization.

Developed for **Research Purposes** at the **University of Jember**, Faculty of Engineering.

--- 

## FunFact
ApexCoders is a subsidiary of **Aditya Projects.id**, which also oversees other established companies including Aditya Developer, Aditya Electronics, and Astropulse Audio.

## Key Features

*   **TinyML Integration:** Calculates a "Need Water" score (0.0 - 1.0) based on weighted parameters (Soil Moisture, Soil Temp, Air Temp, Humidity) with hysteresis control.
*   **Interactive Web Dashboard:** 
    *   Real-time sensor monitoring using Server-Sent Events (SSE).
    *   Historical data charting (24-hour buffer) using Chart.js.
    *   Plant preset management.
    *   Machine Learning threshold configuration.
*   **Advanced Connectivity:** 
    *   Seamless WiFi configuration via Web Portal.
    *   **AP+STA Mode:** Ensures the device remains accessible while connecting to a local network.
    *   Static IP support.
*   **OLED Display & Animations:** 
    *   Real-time status updates.
    *   Fluid startup animations (Particle Burst, Glitch Reveal, Digital Dissolve).
    *   Live connection feedback.
*   **Persistent Storage:** Saves WiFi credentials, ML settings, and IP configurations using ESP32 Preferences.

---

## Hardware Requirements

*   **Microcontroller:** ESP32 or ESP8266 Development Board (DOIT DevKit V1 or similar).
*   **Sensors:**
    *   **AHT10:** Air Temperature & Humidity (I2C).
    *   **DS18B20:** Soil Temperature (OneWire).
    *   **Capacitive Soil Moisture Sensor:** Analog (v1.2 recommended).
*   **Actuator:** 5V Relay Module (Active LOW).
*   **Display:** SSD1306 OLED 0.96" (I2C).

### Pin Configuration (`spms.config.h`)

| Component | ESP32 Pin |
| :--- | :--- |
| **OLED SDA** | GPIO 21 |
| **OLED SCL** | GPIO 22 |
| **Relay** | GPIO 26 |
| **DS18B20** | GPIO 4 |
| **Soil Moisture** | GPIO 34 |

---

## Project Structure

The project follows a modular architecture for better maintainability and scalability:

ESP-SPMS-V3/
├── examples/
│   └── ESP-SPMS-V3
│       └──ESP-SPMS-V3.ino
├── src/            
│   ├── ESP-SPMS-V3.h
│   ├── config.h             
│   ├── types.h             
│   ├── globals.h            
│   ├── globals.cpp          
│   ├── web_content.h       
│   ├── web_content.cpp     
│   ├── display_module.h     
│   ├── display_module.cpp   
│   ├── sensor_module.h      
│   ├── sensor_module.cpp    
│   ├── network_manager.h    
│   ├── network_manager.cpp  
│   ├── system_manager.h     
│   └── system_manager.cpp  
├── library.properties
├── keywords.txt
├── ABOUT
├── DEPENDENCIES
├── LICENCE
└── README.md
