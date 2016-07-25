/*
 Name:		Simula_BehaviorTree.ino
 Created:	6/11/2016 2:05:02 PM
 Author:	jlaing
*/

#include "CRC_PCA9635.h"
#include "CRC_Lights.h"
#include "Hardware.h"
#include <adafruit_vs1053.h>	//Download from https://github.com/adafruit/Adafruit_VS1053_Library/archive/master.zip
#include "Sensor_State.h"
#include "BehaviorTree.h"
#include "PingDistance.h"
#include "IR_BinaryDistance.h"
#include "IR_AnalogDistance.h"
#include "DistanceSensor.h"
#include "Motor.h"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <StandardCplusplus.h>

Sd2Card card;
SdVolume volume;
SdFile root;
File file;
Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(hardware.vs1053_reset, hardware.vs1053_cs, hardware.vs1053_dcs, hardware.vs1053_dreq, hardware.sdcard_cs);

struct SENSOR_STATE sensorState;
struct Emotional_State emotionState;
Sensors sensors = Sensors();
Hardware hardware;
Motor motorLeft(hardware.enc1A, hardware.enc1B, hardware.mtr1Enable, hardware.mtr1In1, hardware.mtr1In2);
Motor motorRight(hardware.enc2A, hardware.enc2B, hardware.mtr2Enable, hardware.mtr2In1, hardware.mtr2In2);
Motors motors;
CRC_LightsClass crcLights(hardware.i2cPca9635Left, hardware.i2cPca9635Right);


Behavior_Tree behaviorTree;
Behavior_Tree::Selector selector[3];
//Behavior_Tree::RandomSelector random[2];
//BehaviourTree::Sequence sequence[2];
Button_Stop buttonStop;
Battery_Check batteryCheck;
Cliff_Center cliffCenter;
Cliff_Left cliffLeft;
Cliff_Right cliffRight;
Cruise_Forward cruise;
Perimeter_Center perimeterCenter;
Perimeter_Left perimeterLeft;
Perimeter_Right perimeterRight;
Random_Action randomAction;

void setup() {
	Serial.begin(9600);
	Serial.println("Booting.");
	
	hardware.init();
	
	if (!musicPlayer.begin()) { // initialise the music player
		Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
	}
	else
	{
		Serial.println(F("VS1053 initialized."));
	}

	crcLights.init();
	//crcLights.setLeftLed(0, 255);
	//crcLights.setLeftLed(1, 255);
	//crcLights.setLeftLed(2, 255);
	//for (int i = 0; i < 10; i++) {
	//	//crcLights.setLed(i, 0x4B, 0x00, 0x82);
	//	crcLights.setLedHex(i, "#FFA500");
	//}
	//delay(2000);
	
	/*for (int l = 0; l < 2; l++) {
		for (int i = 0; i < 5; i++) {
			crcLights.setLed(i, 255, 0, 0);
		}
		for (int i = 6; i < 10; i++) {
			crcLights.setLed(i, 0, 0, 255);
		}
		delay(500);
		for (int i = 0; i < 5; i++) {
			crcLights.setLed(i, 0, 0, 255);
		}
		for (int i = 6; i < 10; i++) {
			crcLights.setLed(i, 255, 0, 0);
		}
		delay(500);
	}*/

	delay(500);
	for (int i = 0; i < 10; i++) {
		crcLights.setLed(i, 0, 0, 0);
	}
	delay(500);
	
	for (int k = 0; k < 20; k++) {
		for (int j = 10; j > 4; j--) {
			for (int i = 0; i < 10; i++) {
				crcLights.setLed(i, 0, 0, 0);
			}
			crcLights.setLed(j, 255, 0, 0);
			crcLights.setLed(j - 5, 255, 0, 0);
			delay(10);
		}
	}
	
	for (int i = 0; i < 10; i++) {
		crcLights.setLed(i, 0, 0, 0);
	}


	//sensors.activate();
	motors.initializeMotors(&motorLeft, &motorRight);
	
	behaviorTree.setRootChild(&selector[0]);
	selector[0].addChildren({ &buttonStop, &batteryCheck, &selector[1], &randomAction, &cruise });
	selector[1].addChildren({ &cliffCenter, &cliffLeft, &cliffRight, &perimeterCenter, &perimeterLeft, &perimeterRight });
	//selector[2].addChildren({  });

	//MP3 Player & Amplifier
	musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT);
	hardware.ampSetVolume(0); //0 = low, 3 = high
	musicPlayer.setVolume(60, 60); //0 = loudest, 60 = softest?  (not sure what softest value is)
	if (!SD.begin(hardware.sdcard_cs)) {
		Serial.println("SD card init failure.");
	}
	else
	{
		Serial.println("SD card initialized.");
		hardware.ampEnable();
		musicPlayer.startPlayingFile("effects/pwrup_05.mp3");
	}
	
	//wait for sensors to kick in.
	Serial.println(F("Setup complete."));

}


void loop() {
	if (!sensors.sensorsUpdated()) {
		sensors.readIR();
	}

	if(musicPlayer.playingMusic){
		//Serial.println(F("Playing."));
	}
	
	if (!behaviorTree.run()) {
		Serial.println(F("Tree did not complete."));
	}
}


