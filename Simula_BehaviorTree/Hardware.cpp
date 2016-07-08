// 
// 
// 

#include "Hardware.h"
#include "Sensor_State.h"

void Hardware::init() {
	setupPins();
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
	pinMode(ampGain0, OUTPUT);
	pinMode(ampGain1, OUTPUT);
	pinMode(ampEnable, OUTPUT);
	digitalWrite(ampGain0, LOW);
	digitalWrite(ampGain1, LOW);
	digitalWrite(ampEnable, LOW);

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
	pinMode(pinActFrontPing, OUTPUT);
	pinMode(pinFrontPing, INPUT);
}
