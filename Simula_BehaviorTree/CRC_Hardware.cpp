/***************************************************
Uses: Hardware abstraction layer and definitions for the
Simula Board. Provides the constant definitions for each
board revision, as well as module initializations.

This file is designed for the Simula project by Chicago Robotics Corp.
http://www.chicagorobotics.net/products

Copyright (c) 2016, Chicago Robotics Corp.
See README.md for license details
****************************************************/

#include <SD.h>
#include <SPI.h>
#include <Wire.h>

#include "CRC_Hardware.h"
#include "Sensor_State.h"


void CRC_HardwareClass::init() {
	setupPins();
	randomSeed(analogRead(A3));  //Get voltage reading from an unused pin.
	setupSPI();
	setupI2C();
}
void CRC_HardwareClass::setupPins()
{
	//Set up audio amplifier and audio player
	pinMode(vs1053_dreq, INPUT);
	pinMode(vs1053_reset, OUTPUT);
	digitalWrite(vs1053_reset, LOW);

	pinMode(vs1053_dcs, OUTPUT);
	digitalWrite(vs1053_dcs, HIGH);

	pinMode(vs1053_cs, OUTPUT);
	digitalWrite(vs1053_cs, LOW);

	// SD Card
	pinMode(sdcard_cs, OUTPUT);
	digitalWrite(sdcard_cs, HIGH);

	// Setting amplifier gain low.
	pinMode(pinAmpGain0, OUTPUT);
	pinMode(pinAmpGain1, OUTPUT);
	pinMode(pinAmpEnable, OUTPUT);
	digitalWrite(pinAmpGain0, LOW);
	digitalWrite(pinAmpGain1, LOW);
	digitalWrite(pinAmpEnable, LOW);

	// 
	pinMode(pinLED, OUTPUT);
	pinMode(pinButton, INPUT_PULLUP);

	// Motor Pins
	pinMode(mtr1In1, OUTPUT);
	pinMode(mtr1In2, OUTPUT);
	pinMode(mtr1Enable, OUTPUT);
	pinMode(mtr2In1, OUTPUT);
	pinMode(mtr2In2, OUTPUT);
	pinMode(mtr2Enable, OUTPUT);

	// Sensor Pins
	pinMode(pinEdge1, INPUT);
	pinMode(pinEdge2, INPUT);
	pinMode(pinPerim1, INPUT);
	pinMode(pinPerim2, INPUT);
	pinMode(pinPerim3, INPUT);
	pinMode(pinPerim4, INPUT);

	//Battery pin
	pinMode(pinBatt, INPUT);

	// Ping Pins
	pinMode(pinPingTrigger, OUTPUT);
	pinMode(pinPingEcho, INPUT);
}
void CRC_HardwareClass::ampSetVolume(int volumeLevel) {
	//0 = lowest volume, 3 = highest
	pinMode(hardware.pinAmpGain0, OUTPUT);
	pinMode(hardware.pinAmpGain1, OUTPUT);
	pinMode(hardware.pinAmpEnable, OUTPUT);


	switch (volumeLevel)
	{
	case 0:
		digitalWrite(hardware.pinAmpGain0, LOW);
		digitalWrite(hardware.pinAmpGain1, LOW);
		break;
	case 1:
		digitalWrite(hardware.pinAmpGain0, HIGH);
		digitalWrite(hardware.pinAmpGain1, LOW);
		break;
	case 2:
		digitalWrite(hardware.pinAmpGain0, LOW);
		digitalWrite(hardware.pinAmpGain1, HIGH);
		break;
	case 3:
		digitalWrite(hardware.pinAmpGain0, HIGH);
		digitalWrite(hardware.pinAmpGain1, HIGH);
		break;
	default:
		digitalWrite(hardware.pinAmpGain0, LOW);
		digitalWrite(hardware.pinAmpGain1, LOW);
		//default to lowest volume.
	}
}
void CRC_HardwareClass::ampEnable() {
	digitalWrite(pinAmpEnable, HIGH);
}
void CRC_HardwareClass::ampDisable() {
	digitalWrite(pinAmpEnable, LOW);
}
void CRC_HardwareClass::setupI2C()
{
	Wire.begin();
	Wire.setTimeout(500);
}
void CRC_HardwareClass::setupSPI()
{
	SPI.begin();
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV128);
}
void CRC_HardwareClass::startScanStatus(unsigned long startTime)
{
	// Scan Free Ram START
	// Keep this block as is at start of this method
	extern int __heap_start, *__brkval;
	int v;
	unitState.freeRam = (uint16_t)(&v - (__brkval == 0 ? (uint16_t)&__heap_start : (uint16_t)__brkval));
	// Scan Free Ram END
};
void CRC_HardwareClass::endScanStatus(unsigned long startTime)
{
	unsigned long endTime = millis();
	unsigned long loopTime = endTime - startTime;
	unitState.loopLastTimeMillis = loopTime; // Last Time in millis
	unitState.loopMinTimeMillis = min(unitState.loopMinTimeMillis + 1, loopTime);  // Min Time in millis
	unitState.loopMaxTimeMillis = max(unitState.loopMaxTimeMillis, loopTime);  // Max Time in millis
}