/***************************************************
Uses: Provides higher level functions to control Simula's behavioral simulation.

This file is designed for the Simula project by Chicago Robotics Corp.
http://www.chicagorobotics.net/products

Copyright (c) 2016, Chicago Robotics Corp.
See README.md for license details
****************************************************/

#include "CRC_Simulation.h"

uint8_t currentAnimation;
const uint8_t animationNone = 0;
const uint8_t animationBio = 1;
const uint8_t animationRunwayFwd = 2;

CRC_SimulationClass::CRC_SimulationClass() {
	exertion = 0;
	restingBeats = 60;
	beatMsCheck = 0;
	heartbeatUnderway = false;
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
	//Looking for the classic, human-style two chamber
	//heartbeat effect.  Also seeking to modify heartbeat
	//through a representation of exertion level.
	uint16_t BPM = ((restingBeats * exertion) / 50) + restingBeats;
	uint16_t msPerBeat = BPM * 1000 / 60;
	if (now - msPerBeat > beatMsCheck)
	{
		beatMsCheck = now;
		//Is it time for another beat?  I sure hope so!
		Serial.print("msPerBeat:");
		Serial.print(msPerBeat);
		Serial.print(" BPM:");
		Serial.println(BPM);
	}
}
void CRC_SimulationClass::showLedNone() {
	crcLights.setAllOff();
	currentAnimation = animationNone;
}
void CRC_SimulationClass::showLedBio() {
	currentAnimation = animationBio;
}