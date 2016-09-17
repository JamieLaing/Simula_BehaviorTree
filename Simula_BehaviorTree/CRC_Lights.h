/***************************************************
Uses: Provides higher level functions to Control the LED outputs

This file is designed for the Simula project by Chicago Robotics Corp.
http://www.chicagorobotics.net/products

Copyright (c) 2016, Chicago Robotics Corp.
See README.md for license details
****************************************************/

#ifndef _LIGHTS_h
#define _LIGHTS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CRC_PCA9635.h"

class CRC_LightsClass
{

private:
	int currentFrame = 0;  //the current frame in the animation.  incremented by tick().
	uint8_t currentAnimation;
	unsigned long frameDuration = 0;  //The duration for a particular animation.
	unsigned long frameStartTime = 0;
	CRC_PCA9635 ledLeft;
	CRC_PCA9635 ledRight;
	uint8_t buttonBrightness;
	uint8_t buttonFadeAmount;
	uint8_t buttonFadeDelay;
	unsigned long buttonFadeTimecheck;
	int breathBrightness;
	int breathFadeAmount;
	uint8_t breathCount;
	uint8_t breathFadeDelay;
	unsigned long breathFadeTimecheck;
	inline void setLed(CRC_PCA9635 & ledBank, uint8_t ledNum, uint8_t level);
	void buttonBreath(unsigned long &now);
	void ledBreath(unsigned long &now);
	
public:
	CRC_LightsClass(uint8_t leftAddress, uint8_t rightAddress);
	
	void init();
	void setLeftLed(uint8_t ledNum, uint8_t level); // 0=Off, 1000 = On, between 1-256 = Level
	void setRightLed(uint8_t ledNum, uint8_t level); // 0=Off, 1000 = On, between 1-256 = Level
	void showRunwayWithDelay();
	void showRunway2();
	void showBreathing();
	void tick();  //increment state of lights
	void setButtonLevel(uint8_t level);
	void setAllOff();
	boolean allOff;
	boolean setLed(uint8_t ledId, uint8_t red, uint8_t green, uint8_t blue);
	boolean setLedHex(uint8_t ledId, String hexString);
};

extern CRC_LightsClass crcLights;

#endif

