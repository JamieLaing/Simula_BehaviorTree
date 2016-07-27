/*
 Name:		Simula_BehaviorTree.ino
 Created:	6/11/2016 2:05:02 PM
 Author:	jlaing
*/

#include "CRC_AudioManager.h"
#include "CRC_PCA9635.h"
#include "CRC_Lights.h"
#include "CRC_Hardware.h"
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

struct UNIT_STATE unitState;
struct Emotional_State emotionState;
Sensors sensors = Sensors();
CRC_HardwareClass hardware;
Motor motorLeft(hardware.enc1A, hardware.enc1B, hardware.mtr1Enable, hardware.mtr1In1, hardware.mtr1In2);
Motor motorRight(hardware.enc2A, hardware.enc2B, hardware.mtr2Enable, hardware.mtr2In1, hardware.mtr2In2);
Motors motors;
CRC_LightsClass crcLights(hardware.i2cPca9635Left, hardware.i2cPca9635Right);
CRC_AudioManagerClass crcAudio;


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
	Serial.println(F("Booting."));
	
	hardware.init();
	crcLights.init();

	if (crcAudio.init()) {
		unitState.audioPlayer = true;
	}
	else {
		Serial.println(F("VS1053 not detected."));
	}
	crcLights.showRunway();

	//sensors.activate();
	motors.initializeMotors(&motorLeft, &motorRight);
	
	behaviorTree.setRootChild(&selector[0]);
	selector[0].addChildren({ &buttonStop, &batteryCheck, &selector[1], &randomAction, &cruise });
	selector[1].addChildren({ &cliffCenter, &cliffLeft, &cliffRight, &perimeterCenter, &perimeterLeft, &perimeterRight });
	//selector[2].addChildren({  });

	//MP3 Player & Amplifier
	hardware.ampSetVolume(1); //0 = low, 3 = high
	crcAudio.setVolume(10, 10); //0 = loudest, 60 = softest?  (not sure what softest value is)
	if (!SD.begin(hardware.sdcard_cs)) {
		Serial.println(F("SD card init failure."));
	}
	else
	{
		Serial.println(F("SD card initialized."));
		hardware.ampEnable();
		crcAudio.startAudioFile("effects/pwrup_05.mp3");
	}
	
	//wait for sensors to kick in.
	Serial.println(F("Setup complete."));

}


void loop() {
	crcAudio.updateAudioState();

	if (!sensors.sensorsUpdated()) {
		sensors.readIR();
	}

	
	if (!behaviorTree.run()) {
		Serial.println(F("Tree did not complete."));
	}
}


