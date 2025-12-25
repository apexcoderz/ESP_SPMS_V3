/*!
 * SPMS Library
 * Copyright (c) 2024-2026 ApexCoderz
 * Licensed under Custom Non-Commercial License. 
 * See LICENSE file for details.
 * 
 * @file spms.display_module.cpp
 */

#include "ESP_SPMS_V3.h"
#include "spms.display_module.h"
#include "spms.globals.h"
// DISPLAY SETUP
void setupDisplay() {
  if(!layarOled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("$SYS> OLED not detected!");
    while(1);
  }
  layarOled.dim(false);
  layarOled.clearDisplay();
  layarOled.display();
  layarOled.setTextColor(SSD1306_WHITE);
  
  randomSeed(analogRead(0));
  initParticles();
  stateStartTime = millis();
  
  Serial.println("$SYS> OLED Display initialized");
}

// SPLASH SCREEN
void showSplashScreen() {
  // Large title
  layarOled.clearDisplay();
  layarOled.setTextSize(3);
  layarOled.setCursor(10, 8);
  layarOled.print("S-PMS");
  layarOled.display();
  delay(2000);

  // Loading bar
  layarOled.clearDisplay();
  layarOled.setTextSize(1);
  layarOled.setCursor(25, 8);
  layarOled.print("V3 is Loading...");
  for(int i = 0; i < 10; i++) {
    layarOled.fillRect(14 + i*10, 20, 8, 4, SSD1306_WHITE);
    layarOled.display();
    delay(80);
  }
  // INFO > By The Wayy this program is mostly made by human << me as aditya >>
  delay(500);
}

// DISPLAY UPDATE - 5 Modes
void updateDisplay() {
  layarOled.clearDisplay();
  layarOled.setTextSize(1);
  
  switch(displayMode) {
    case 0: // Air Temperature
      layarOled.setTextSize(1);
      layarOled.setCursor(0, 0);
      layarOled.print("SUHU UDARA");
      layarOled.setTextSize(2);
      layarOled.setCursor(0, 14);
      layarOled.print(sensorData.airTemp, 1);
      layarOled.print("C");
      break;
      
    case 1: // Air Humidity
      layarOled.setTextSize(1);
      layarOled.setCursor(0, 0);
      layarOled.print("KELEMBABAN");
      layarOled.setTextSize(2);
      layarOled.setCursor(0, 14);
      layarOled.print(sensorData.airHumidity, 0);
      layarOled.print("%");
      break;
      
    case 2: // Soil Temperature
      layarOled.setTextSize(1);
      layarOled.setCursor(0, 0);
      layarOled.print("SUHU TANAH");
      layarOled.setTextSize(2);
      layarOled.setCursor(0, 14);
      layarOled.print(sensorData.soilTemp, 1);
      layarOled.print("C");
      break;
      
    case 3: { // Soil Moisture + Bar
      layarOled.setTextSize(1);
      layarOled.setCursor(0, 0);
      layarOled.print("TANAH:");
      layarOled.print(sensorData.soilMoisturePercent);
      layarOled.print("%");
      
      // Progress bar
      int barWidth = map(sensorData.soilMoisturePercent, 0, 100, 0, 128);
      layarOled.fillRect(0, 10, barWidth, 6, SSD1306_WHITE);
      layarOled.drawRect(0, 10, 128, 6, SSD1306_WHITE);
      
      // ML Score & Status
      layarOled.setCursor(0, 20);
      layarOled.print("AI:");
      layarOled.print(systemState.mlScore, 2);
      layarOled.print(" ");
      layarOled.print(systemState.relayState ? "[ON]" : "[OFF]");
      break; }
      
    case 4: { // WiFi & System Info
      layarOled.setTextSize(1);
      layarOled.setCursor(0, 0);
      
      if (isAPMode) {
        layarOled.print("MODE: AP");
        layarOled.setCursor(0, 10);
        layarOled.print("SSID:");
        layarOled.print(apSSID.substring(0, 10));
        layarOled.setCursor(0, 20);
        layarOled.print("IP:");
        layarOled.print(systemState.ipAddress);
      } else {
        layarOled.print("WiFi:");
        layarOled.print(systemState.ssid.substring(0, 8));
        layarOled.setCursor(0, 10);
        layarOled.print("IP:");
        layarOled.print(systemState.ipAddress);
        layarOled.setCursor(0, 20);
        layarOled.print("RSSI:");
        layarOled.print(systemState.wifiRSSI);
        layarOled.print("dBm");
      }
      break; }
  }
  
  layarOled.display();
}

// INFO DISPLAY
void showInfoOnOLED() {
  layarOled.clearDisplay();
  layarOled.setTextSize(1);
  if (isAPMode) {
    // AP Mode Info
    layarOled.setCursor(0, 0);
    layarOled.print("MODE: Access Point");
    layarOled.setCursor(0, 10);
    layarOled.print("SSID: ");
    layarOled.print(apSSID);
    layarOled.setCursor(0, 20);
    layarOled.print("IP: ");
    layarOled.print(systemState.ipAddress);
  } else {
    // Station Mode Info
    layarOled.setCursor(0, 0);
    layarOled.print("WiFi: ");
    layarOled.print(systemState.ssid);
    layarOled.setCursor(0, 10);
    layarOled.print("IP:  ");
    layarOled.print(systemState.ipAddress);
    layarOled.setCursor(0, 20);
    layarOled.print("Akses via browser!");
  }
  
  layarOled.display();
}
// PARTICLE BURST ANIMATION
void initParticles() {
  int centerX = 64; // OLED_WIDTH / 2
  int centerY = 16; // OLED_HEIGHT / 2
  
  for (int i = 0; i < MAX_PARTICLES; i++) {
    float angle = (i * 2.0 * PI) / MAX_PARTICLES;
    float speed = 1.5 + (i % 3) * 0.5;
    
    particles[i].x = centerX;
    particles[i].y = centerY;
    particles[i].vx = cos(angle) * speed;
    particles[i].vy = sin(angle) * speed;
    particles[i].active = true;
    // meoww aditya is here sir!
  }
}

// I'm heree -  AP
void updateParticles() {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (particles[i].active) {
      particles[i].x += particles[i].vx;
      particles[i].y += particles[i].vy;
      
      // Deactivate if out of bounds
      if (particles[i].x < 0 || particles[i].x >= 128 || 
          particles[i].y < 0 || particles[i].y >= 32) {
        particles[i].active = false;
      }
    }
  }
}

void drawParticles() {
  layarOled.clearDisplay();
  
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (particles[i].active) {
      // Draw particle
      layarOled.drawPixel((int)particles[i].x, (int)particles[i].y, SSD1306_WHITE);
      
      // Trail effect
      int prevX = particles[i].x - particles[i].vx;
      int prevY = particles[i].y - particles[i].vy;
      if (prevX >= 0 && prevX < 128 && prevY >= 0 && prevY < 32) {
        layarOled.drawPixel(prevX, prevY, SSD1306_WHITE);
      }
    }
  }
  
  layarOled.display();
}

// GLITCH REVEAL ANIMATION / GRA
void drawGlitchReveal() {
  layarOled.clearDisplay();
  
  const String TEXT = "ADITYA PROJECT";
  const int TEXT_SIZE = 1;
  const int CHAR_WIDTH = 6;
  const int TEXT_X = (128 - TEXT.length() * CHAR_WIDTH) / 2;
  const int TEXT_Y = 12;
  
  int revealChars = map(glitchFrame, 0, 25, 0, TEXT.length());
  
  // Scanlines background
  for (int y = 0; y < 32; y += 3) {
    layarOled.drawFastHLine(0, y, 128, SSD1306_WHITE);
  }
  
  layarOled.setTextSize(TEXT_SIZE);
  layarOled.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  
  // Draw revealed characters
  for (int i = 0; i < revealChars && i < TEXT.length(); i++) {
    int xPos = TEXT_X + i * CHAR_WIDTH;
    layarOled.setCursor(xPos, TEXT_Y);
    layarOled.print(TEXT[i]);
    
    // Random glitch effect
    if (glitchFrame % 3 == 0 && random(100) < 20) {
      int offsetY = random(-2, 3);
      layarOled.setCursor(xPos, TEXT_Y + offsetY);
      layarOled.print(TEXT[i]);
    }
  }
  
  // Unrevealed characters (noise)
  for (int i = revealChars; i < TEXT.length(); i++) {
    if (random(100) < 30) {
      layarOled.setCursor(TEXT_X + i * CHAR_WIDTH, TEXT_Y);
      layarOled.print((char)random(33, 126));
    }
  }
  // INFO > But indeed thid program was also helped by an AI for 10% of the code
  layarOled.display();
}

// HOLOGRAM FRAME ANIMATION
void drawHologramFrame() {
  layarOled.clearDisplay();
  
  const String TEXT = "ADITYA PROJECT";
  const int TEXT_SIZE = 1;
  const int CHAR_WIDTH = 6;
  const int TEXT_X = (128 - TEXT.length() * CHAR_WIDTH) / 2;
  const int TEXT_Y = 12;
  
  // Main text
  layarOled.setTextSize(TEXT_SIZE);
  layarOled.setTextColor(SSD1306_WHITE);
  layarOled.setCursor(TEXT_X, TEXT_Y);
  layarOled.print(TEXT);
  
  // Animated corner brackets
  int cornerSize = map(frameProgress, 0, 40, 0, 10);
  
  // Top-left
  layarOled.drawLine(5, 5, 5 + cornerSize, 5, SSD1306_WHITE);
  layarOled.drawLine(5, 5, 5, 5 + cornerSize, SSD1306_WHITE);
  
  // Top-right
  layarOled.drawLine(122, 5, 122 - cornerSize, 5, SSD1306_WHITE);
  layarOled.drawLine(122, 5, 122, 5 + cornerSize, SSD1306_WHITE);
  
  // Bottom-left
  layarOled.drawLine(5, 26, 5 + cornerSize, 26, SSD1306_WHITE);
  layarOled.drawLine(5, 26, 5, 26 - cornerSize, SSD1306_WHITE);
  
  // Bottom-right
  layarOled.drawLine(122, 26, 122 - cornerSize, 26, SSD1306_WHITE);
  layarOled.drawLine(122, 26, 122, 26 - cornerSize, SSD1306_WHITE);
  
  // Progress bar
  if (frameProgress > 20) {
    int barWidth = map(frameProgress - 20, 0, 20, 0, 88);
    layarOled.fillRect(20, 29, barWidth, 2, SSD1306_WHITE);
  }
  layarOled.display();
}

// PULSE GLOW ANIMATION
void drawPulseGlow() {
  layarOled.clearDisplay();
  
  const String TEXT = "ADITYA PROJECT";
  const int TEXT_SIZE = 1;
  const int CHAR_WIDTH = 6;
  const int TEXT_X = (128 - TEXT.length() * CHAR_WIDTH) / 2;
  const int TEXT_Y = 12;
  
  // Main text
  layarOled.setTextSize(TEXT_SIZE);
  layarOled.setTextColor(SSD1306_WHITE);
  layarOled.setCursor(TEXT_X, TEXT_Y);
  layarOled.print(TEXT);
  
  // Persistent frame
  layarOled.drawRect(5, 5, 118, 22, SSD1306_WHITE);
  
  // Enhanced corners
  layarOled.drawLine(5, 5, 10, 5, SSD1306_WHITE);
  layarOled.drawLine(5, 5, 5, 10, SSD1306_WHITE);
  layarOled.drawLine(122, 5, 117, 5, SSD1306_WHITE);
  layarOled.drawLine(122, 5, 122, 10, SSD1306_WHITE);
  layarOled.drawLine(5, 26, 10, 26, SSD1306_WHITE);
  layarOled.drawLine(5, 26, 5, 21, SSD1306_WHITE);
  layarOled.drawLine(122, 26, 117, 26, SSD1306_WHITE);
  layarOled.drawLine(122, 26, 122, 21, SSD1306_WHITE);
  
  // Pulse effect (expanding rectangles)
  float phase = (pulseFrame % 30) / 30.0;
  int offset = phase * 15;
  
  if (offset < 12) {
    layarOled.drawRect(5 - offset, 5 - offset/2, 
                     118 + 2*offset, 22 + offset, 
                     SSD1306_WHITE);
  }
  
  layarOled.display();
}

// DIGITAL DISSOLVE ANIMATION
void initDissolve() {
  layarOled.clearDisplay();
  
  const String TEXT = "ADITYA PROJECT";
  const int TEXT_SIZE = 1;
  const int CHAR_WIDTH = 6;
  const int TEXT_X = (128 - TEXT.length() * CHAR_WIDTH) / 2;
  const int TEXT_Y = 12;
  // eyy could you find me again! aditya
  
  layarOled.setTextSize(TEXT_SIZE);
  layarOled.setTextColor(SSD1306_WHITE);
  layarOled.setCursor(TEXT_X, TEXT_Y);
  layarOled.print(TEXT);
  
  layarOled.drawRect(5, 5, 118, 22, SSD1306_WHITE);
  
  // Capture to pixelMap
  for (int x = 0; x < 128; x++) {
    for (int y = 0; y < 32; y++) {
      pixelMap[x][y] = layarOled.getPixel(x, y);
    }
  }
}

void drawDigitalDissolve() {
  layarOled.clearDisplay();
  
  float dissolveProbability = dissolveFrame / 30.0;
  
  for (int x = 0; x < 128; x++) {
    for (int y = 0; y < 32; y++) {
      if (pixelMap[x][y]) {
        if (random(1000) / 1000.0 > dissolveProbability) {
          layarOled.drawPixel(x, y, SSD1306_WHITE);
        }
      }
    }
  }
  
  layarOled.display();
}
