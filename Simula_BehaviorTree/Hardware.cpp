// 
// 
// 

#include "Hardware.h"
#include "Sensor_State.h"
#include <SD.h>
#include <SPI.h>
#include <Wire.h>

void Hardware::init() {
	setupPins();
	randomSeed(analogRead(A3));  //Get voltage reading from an unused pin.
	setupSPI();
	setupI2C();
}

void Hardware::setupPins()
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
void Hardware::ampSetVolume(int volumeLevel) {
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
void Hardware::ampEnable() {
	digitalWrite(pinAmpEnable, HIGH);
}
void Hardware::ampDisable() {
	digitalWrite(pinAmpEnable, LOW);
}
void Hardware::setupI2C()
{
	Wire.begin();
	Wire.setTimeout(500);
}
void Hardware::setupSPI()
{
	SPI.begin();
	SPI.setDataMode(SPI_MODE0);
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV128);
}