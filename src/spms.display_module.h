/*!
 * SPMS Library
 * Copyright (c) 2024-2026 ApexCoders
 * Licensed under Custom Non-Commercial License. 
 * See LICENSE file for details.
 * 
 * @file spms.display_module.h
 */

#ifndef SPMS_DISPLAY_MODULE_H
#define SPMS_DISPLAY_MODULE_H

// Display Function
extern void setupDisplay();
extern void showSplashScreen();
extern void updateDisplay();
extern void showInfoOnOLED();

// Animation Functions
extern void initParticles();
extern void updateParticles();
extern void drawParticles();
extern void drawGlitchReveal();
extern void drawHologramFrame();
extern void drawPulseGlow();
extern void initDissolve();
extern void drawDigitalDissolve();

#endif