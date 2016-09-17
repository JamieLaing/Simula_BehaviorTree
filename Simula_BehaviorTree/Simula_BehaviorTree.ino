+/*
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
#include <Adafruit_LSM9DS0.h>	//Download from https://github.com/adafruit/Adafruit_LSM9DS0_Library/archive/master.zip
#include <Adafruit_Sensor.h>	//Download from https://github.com/adafruit/Adafruit_Sensor/archive/master.zip

Sd2Card card;
SdVolume volume;
SdFile root;
File file;

struct UNIT_STATE unitState;
//struct Emotional_State emotionState;
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
Orientation_Check orientationCheck;

void setup() {
	hardware.seedRandomGenerator();
	Serial.begin(115200);
	Serial.println(F("Booting."));
	sensors.lsm = Adafruit_LSM9DS0();
	Serial.println(F("LSM instantiated."));
	hardware.init();
	Serial.println(F("Hardware initialized."));
	crcLights.init();
	Serial.println(F("Lights initialized."));

	if (crcAudio.init()) {
		unitState.audioPlayer = true;
		Serial.println(F("Audio initialized."));
	}
	else {
		Serial.println(F("Audio chip not detected."));
	}
	if (!sensors.lsm.begin())
	{
		Serial.println(F("Oops ... unable to initialize the LSM9DS0. Check your wiring!"));
	}
	else
	{
		Serial.println(F("Setting IMU attributes."));
		// 1.) Set the accelerometer range
		sensors.lsm.setupAccel(sensors.lsm.LSM9DS0_ACCELRANGE_2G);
		//lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_4G);
		//lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_6G);
		//lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_8G);
		//lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_16G);

		// 2.) Set the magnetometer sensitivity
		sensors.lsm.setupMag(sensors.lsm.LSM9DS0_MAGGAIN_2GAUSS);
		//lsm.setupMag(lsm.LSM9DS0_MAGGAIN_4GAUSS);
		//lsm.setupMag(lsm.LSM9DS0_MAGGAIN_8GAUSS);
		//lsm.setupMag(lsm.LSM9DS0_MAGGAIN_12GAUSS);

		// 3.) Setup the gyroscope
		sensors.lsm.setupGyro(sensors.lsm.LSM9DS0_GYROSCALE_245DPS);
		//lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_500DPS);
		//lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_2000DPS);
		
		Serial.println(F("IMU configured."));
	}

	//Check battery voltage.
	float postVoltage = hardware.readBatteryVoltage();

	crcLights.showRunwayWithDelay();
	motors.initializeMotors(&motorLeft, &motorRight);
	
	behaviorTree.setRootChild(&selector[0]);
	selector[0].addChildren({ &buttonStop, &batteryCheck, &orientationCheck, &selector[1], &randomAction, &cruise });
	selector[1].addChildren({ &cliffCenter, &cliffLeft, &cliffRight, &perimeterCenter, &perimeterLeft, &perimeterRight });
	//selector[2].addChildren({  });

	//MP3 Player & Amplifier
	//hardware.ampSetVolume(1);
	crcAudio.setAmpGain(1); //0 = low, 3 = high
	crcAudio.setVolume(40, 40); //0 = loudest, 60 = softest ?
	if (!SD.begin(hardware.sdcard_cs)) {
		Serial.println(F("SD card init failure."));
	}
	else
	{
		Serial.println(F("SD card initialized."));
		crcAudio.playRandomAudio("effects/PwrUp_", 10, ".mp3");
	}
	
	//wait for sensors to kick in.
	Serial.println(F("Setup complete."));
	crcLights.showBreathing();
}

void loop() {
	crcLights.tick();
	crcAudio.tick();
	if (!unitState.treeDeactivated)
	{
		sensors.lsm.read();
		if (!sensors.irReadingUpdated()) {
			sensors.readIR();
		}
	}
	
	if (!behaviorTree.run()) {
		Serial.println(F("Tree did not complete."));
	}
}


