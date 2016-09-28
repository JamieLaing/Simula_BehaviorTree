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
	restingBeats = 30;
	beatMsCheck = 0;
	beatUnderway = false;
	beatFlashDuration = 50;
	buttonFadeAmount = 10;
	buttonBrightness = 0;
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
		buttonBrightness = 0;
		beatUnderway = true;
		/*Serial.print(" BPM:");
		Serial.println(BPM);
		Serial.print(" BPS:");
		Serial.print(BPS);
		Serial.print(" msBeat:");
		Serial.println(msPerBeat);*/
	}
	//if (beatUnderway) {
	//	crcLights.setButtonLevel(buttonBrightness);
	//	Serial.print("Brightness:");
	//	Serial.println(buttonBrightness);
	//	buttonBrightness = buttonBrightness + buttonFadeAmount;
	//	//if (buttonBrightness <= 0 || buttonBrightness >= 150) {
	//	//	buttonFadeAmount = -buttonFadeAmount;
	//	//}
	//	if (buttonFadeAmount < 0) {
	//		beatUnderway = false;
	//		Serial.println("beat stopped.");
	//	}
	//	if (buttonBrightness >= 100 || buttonBrightness <= 0)
	//	{
	//		buttonFadeAmount = -buttonFadeAmount;
	//	}
	//}
}
void CRC_SimulationClass::showLedNone() {
	crcLights.setAllOff();
	currentAnimation = animationNone;
}
void CRC_SimulationClass::showLedBio() {
	currentAnimation = animationBio;
}