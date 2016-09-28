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
	restingBeats = 60;
	beatMsCheck = 0;
	beatUnderway = false;
	beatFlashDuration = 150;
	beatBrightness = 100;
}
void CRC_SimulationClass::tick() {
	unsigned long now = millis();
	switch (currentAnimation) {
	case animationBio:
		buttonHeartbeat(now);
		//buttonBreath(now);
		break;
	case animationRunwayFwd:
		//animationRunwayFwd(now);
		break;
	default:
		break;
	}
}
void CRC_SimulationClass::buttonHeartbeat(unsigned long &now) {
	float BPM = ((restingBeats * exertion) / 50) + restingBeats;
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
void CRC_SimulationClass::showLedNone() {
	crcLights.setAllOff();
	currentAnimation = animationNone;
}
void CRC_SimulationClass::showLedBio() {
	currentAnimation = animationBio;
}