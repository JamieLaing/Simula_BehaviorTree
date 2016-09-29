/***************************************************
Uses: Provides higher level functions to control Simula's behavioral simulation.

This file is designed for the Simula project by Chicago Robotics Corp.
http://www.chicagorobotics.net/products

Copyright (c) 2016, Chicago Robotics Corp.
See README.md for license details
****************************************************/

#include "CRC_Simulation.h"

CRC_SimulationClass::CRC_SimulationClass() {
	exertion = 0;
	restingBPM = 60;
	beatMsCheck = 0;
	beatUnderway = false;
	beatFlashDuration = 150;
	beatBrightness = 100;

	restingBreaths = 18; //breaths per minute;
	breathsMsCheck = 0;
	breathUnderway = false;
	breathBrightness = 0;
	breathFadeAmount = 2;
	breathFadeDelay = 70;
	breathFadeTimecheck = millis();
}
void CRC_SimulationClass::tick() {
	unsigned long now = millis();
	switch (currentAnimation) {
	case animationBio:
		buttonHeartbeat(now);
		ledBreath(now);
		break;
	case animationRunwayFwd:
		//animationRunwayFwd(now);
		break;
	default:
		break;
	}
}
void CRC_SimulationClass::buttonHeartbeat(unsigned long &now) {
	float BPM = ((restingBPM * exertion) / 50) + restingBPM;
	float BPS = BPM / 60;
	float msPerBeat = 1000 / BPS;
	if (now - msPerBeat > beatMsCheck)
	{
		beatMsCheck = now;
		beatUnderway = true;
		crcLights.setButtonLevel(beatBrightness);
	}
	if ((now > beatMsCheck + beatFlashDuration) && beatUnderway) {
		crcLights.setButtonLevel(0);
		beatUnderway = false;
	}
}
void CRC_SimulationClass::ledExertionBreath(unsigned long &now) {
	float breathsPM = ((restingBreaths * exertion) / 50) + restingBreaths;
	float breathsPS = breathsPM / 60;
	float msPerBreath = 1000 / breathsPS;
	if (now - msPerBreath > breathsMsCheck)
	{
		breathsMsCheck = now;
		breathUnderway = true;
	}
	
}
void CRC_SimulationClass::ledBreath(unsigned long &now) {
	if (now - breathFadeDelay > breathFadeTimecheck) {
		breathFadeTimecheck = now;

		for (int i = 0; i < 10; i++) {
			if (((i % 2) == 0) && (breathFadeAmount > 0))
			{
				crcLights.setLed(i, breathBrightness, breathBrightness, breathBrightness);
			}
			if (((i % 2) != 0) && (breathFadeAmount < 0))
			{
				crcLights.setLed(i, breathBrightness, breathBrightness, breathBrightness);
			}
		}

		breathBrightness = breathBrightness + breathFadeAmount;
		if (breathBrightness <= 0 || breathBrightness >= 150) {
			breathFadeAmount = -breathFadeAmount;
			//turn all LEDs off
			for (int i = 0; i < 10; i++) {
				crcLights.setLed(i, 0, 0, 0);
			}
		}
	}
}
void CRC_SimulationClass::showLedNone() {
	crcLights.setAllOff();
	currentAnimation = animationNone;
}
void CRC_SimulationClass::showLedBio() {
	currentAnimation = animationBio;
}