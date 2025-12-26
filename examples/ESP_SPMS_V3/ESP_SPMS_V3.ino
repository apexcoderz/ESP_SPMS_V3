/*
 * ============================================================================
 *  ESP32-SPMS SMART PLANT MONITORING SYSTEM v3.0
 * ============================================================================
 *
 * ════════════════════════════════════════════════════════════════════════════
 * COPYRIGHT NOTICE & SOFTWARE LICENSE
 * ════════════════════════════════════════════════════════════════════════════
 * This program is developed for Research Purposes under:
 * Project: ESP32-SPMS >> Smart Plant Monitoring Systems
 * Version: V3.0
 * Institution: University Of Jember
 * Faculty : Faculty of Engineering
 * Department: Department of Electrical Engineering
 * Lead Programmer: ACH Milladitya Lazuwardi << ApexCoderz >>
 * Contributor: Andri Kusumsa Ramadhani << Codecztron >>
 * Principal Investigator: Muhammad Riza Darmawan S.T., M.T.
 *
 * Copyright (c) 2026 Aditya Projects.id (ApexCoderz & Codecztron). All rights reserved.
 * This software and associated documentation files are proprietary to
 * Aditya Projects.id X Codecztron. Usage, copying, modification, merger, publication,
 * distribution, sublicensing, and/or sale of this software is strictly
 * prohibited without prior written consent from the Lead Programmer.
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 * ============================================================================
 *
 * ============================================================================
 *  ESP32-SPMS CHANGELOG
 * ============================================================================
 *  NEW FEATURES SPMS v3.0:
 *  Advanced ML Model dengan TinyML optimization
 *  Real-time Chart & Historical Data (24h - EEPROM)
 *  Static IP Configuration
 *  Plant-specific threshold presets
 *  Enhanced OLED info display (IP, WiFi status, etc)
 *  Optimized memory & performance
 *  interactive modern web UI with animations
 *  Advanced settings panel
 */

// including all library
#include <Arduino.h>
#include <ESP_SPMS_V3.h>
<<<<<<< HEAD
#define ApexCoderz
#define Codecztron
=======
#include ApexCoderz
#include Codecztron
>>>>>>> 5755c2ef5c0664a9166a1429601abe4b315286e8

// ============================================================================
// SETUP FUNCTION
// ============================================================================
void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println("\n\n═══════════════════════════════════════");
  Serial.println("         WELCOME TO SPMS V3.0             ");
  Serial.println("═══════════════════════════════════════\n");

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("$SYS> SPIFFS Mount Failed");
  }
  else
  {
    Serial.println("$SYS> SPIFFS Mounted");
  }

  // Initialize Preferences
  preferences.begin("plant-config", false);

  // Setup hardware
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Relay OFF (active LOW!)

  // Load Setup Programs
  setupDisplay();
  showSplashScreen();
  setupSensors();

  // Load all saved configurations
  loadWiFiCredentials();
  loadStaticIPConfig();
  loadMLConfig();

  setupWiFi();
  setupWebServer();

  // Initialize chart data
  memset(&chartData, 0, sizeof(chartData));

  Serial.println("\n$SYS> System Ready!  and runnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn! :'V");
  Serial.println("═══════════════════════════════════════\n");

  // Initial sensor read
  readSensors();
  addChartDataPoint();
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop()
{
  unsigned long currentMillis = millis();
  systemState.uptime = currentMillis;

#if defined(ApexCoderz) && defined(Codecztron)

  // SPLASH SCREEN + MACHINE STATE DISP
  if (currentState != FINISHED)
  {
    unsigned long elapsed = currentMillis - stateStartTime;

    switch (currentState)
    {
    case PARTICLE_BURST:
      updateParticles();
      drawParticles();
      delay(70);

      if (elapsed > 1000)
      {
        currentState = GLITCH_REVEAL;
        glitchFrame = 0;
        stateStartTime = currentMillis;
      }
      break;

    case GLITCH_REVEAL:
      drawGlitchReveal();
      glitchFrame++;
      delay(40);

      if (glitchFrame >= 25)
      {
        currentState = HOLOGRAM_FRAME;
        frameProgress = 0;
        stateStartTime = currentMillis;
      }
      break;

    case HOLOGRAM_FRAME:
      drawHologramFrame();
      frameProgress++;
      delay(30);

      if (frameProgress >= 40)
      {
        currentState = PULSE_GLOW;
        pulseFrame = 0;
        stateStartTime = currentMillis;
      }
      break;

    case PULSE_GLOW:
      drawPulseGlow();
      pulseFrame++;
      delay(40);

      if (pulseFrame >= 90)
      {
        currentState = DIGITAL_DISSOLVE;
        dissolveFrame = 0;
        initDissolve();
        stateStartTime = currentMillis;
      }
      break;

    case DIGITAL_DISSOLVE:
      drawDigitalDissolve();
      dissolveFrame++;
      delay(35);

      if (dissolveFrame >= 30)
      {
        currentState = INFO_DISPLAY;
        stateStartTime = currentMillis;
        layarOled.clearDisplay();
        layarOled.display();
      }
      break;

    case INFO_DISPLAY:
      // Show WiFi setup info for 5 seconds
      if (elapsed < 5000)
      {
        showInfoOnOLED();
        delay(100);
      }
      else
      {
        currentState = FINISHED;
      }
      break;

    case FINISHED:
      // Animation complete, proceed to normal operation
      break;
    }
  }

#else
  // Locked MWEHEHEHE << Do Not MODIFY!
  layarOled.clearDisplay();
  layarOled.setTextSize(1);
  layarOled.setTextColor(WHITE);
  layarOled.setCursor(0, 0);
  layarOled.println("SYSTEM LOCKED");
  layarOled.setCursor(0, 15);
  layarOled.println("Unauthorized Usage!");
  layarOled.setCursor(0, 30);
  layarOled.println("ApexCoderz");
  layarOled.display();
  while (true)
  {
    delay(1000);
  }
#endif

  // SENSOR READING
  if (currentMillis - lastSensorRead >= SENSOR_READ_INTERVAL)
  {
    readSensors();
    lastSensorRead = currentMillis;
  }

  // DISPLAY UPDATE
  if (currentMillis - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL && currentState == FINISHED && !systemState.isWifiConfiguring)
  {
    updateDisplay();
    displayMode = (displayMode + 1) % 5; // 5 display modes
    lastDisplayUpdate = currentMillis;
  }

  // ML MODEL UPDATE
  if (currentMillis - lastMLUpdate >= ML_UPDATE_INTERVAL)
  {
    runMLModel();
    lastMLUpdate = currentMillis;
  }

  // CHART DATA AUTOSAVE
  if (currentMillis - lastChartSave >= CHART_SAVE_INTERVAL)
  {
    addChartDataPoint();
    lastChartSave = currentMillis;
  }

  // WiFi RSSI AUTO UPDATER
  if (systemState.wifiConnected && !isAPMode)
  {
    systemState.wifiRSSI = WiFi.RSSI();
  }

  delay(10);
}