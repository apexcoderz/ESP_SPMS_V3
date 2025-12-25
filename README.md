# ESP32-SPMS: Smart Plant Monitoring System v3.0

![Platform](https://img.shields.io/badge/Platform-ESP32%2F8266-blue)
![Language](https://img.shields.io/badge/Language-C%2B%2B-orange)
![Language](https://img.shields.io/badge/Language-C-grey)
![Language](https://img.shields.io/badge/Language-HTML-blue)
![License](https://img.shields.io/badge/License-Non--Commercial-red)
![Version](https://img.shields.io/badge/Version-3.0-green)

**ESP32-SPMS v3.0** is an advanced IoT solution for precision agriculture and smart gardening research. It leverages TinyML (Machine Learning) algorithms to make intelligent watering decisions based on real-time environmental data, featuring a modern web UI and OLED visualization.

Developed for **research purposes** at the **University of Jember**, Department of Electrical Engineering, Faculty of Engineering.

---

## Fun Fact

ApexCoderz is a subsidiary of **Aditya Projects.id**, which also oversees other established companies including Aditya Developer, Aditya Electronics, and Astropulse Audio.  
Codecztron is the moniker of Andri Kusuma Ramadani, a university student collaborating with ApexCoderz to ensure this program operates as close to perfection as possible.

---

## Key Features

- **TinyML Integration:** Calculates a "Need Water" score (0.0–1.0) based on weighted parameters (soil moisture, soil temperature, air temperature, humidity) with hysteresis control.
- **Interactive Web Dashboard:**
    - Real-time sensor monitoring via Server-Sent Events (SSE).
    - Historical data charting (24-hour buffer) using Chart.js.
    - Plant preset management.
    - Machine learning threshold configuration.
- **Advanced Connectivity:**
    - Seamless WiFi configuration via web portal.
    - **AP+STA Mode:** Device remains accessible while connected to a local network.
    - Static IP support.
- **OLED Display & Animations:**
    - Real-time status updates.
    - Fluid startup animations (Particle Burst, Glitch Reveal, Digital Dissolve).
    - Live connection feedback.
- **Persistent Storage:** Saves WiFi credentials, ML settings, and IP configurations using ESP32 Preferences.

---

## Hardware Requirements

- **Microcontroller:** ESP32 or ESP8266 development board
- **Module:** Logic level shifter
- **Sensors:**
    - **AHT10:** Air temperature & humidity (I2C)
    - **DS18B20:** Soil temperature (OneWire)
    - **Capacitive Soil Moisture Sensor:** Analog (v1.2 recommended)
- **Actuator:** 5V 1CH relay module (active LOW)
- **Display:** SSD1306 OLED 0.96" (I2C)

### Pin Configuration (`spms.config.h`)

| Component           | ESP32 Pin |
|---------------------|-----------|
| **OLED SDA**        | GPIO 21   |
| **OLED SCL**        | GPIO 22   |
| **Relay**           | GPIO 26   |
| **DS18B20**         | GPIO 4    |
| **Soil Moisture**   | GPIO 34   |

---

## Project Structure

The project follows a modular architecture for better maintainability and scalability:

ESP-SPMS-V3/                  
├── examples/               
│   └── ESP-SPMS-V3                              
│. . . .└──ESP-SPMS-V3.ino                  
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

---

## Contribution

Contributions are welcome! Please open an issue or submit a pull request for improvements or bug fixes.

---

## License

This project is licensed for **non-commercial use only**. See the `LICENSE` file for details.

