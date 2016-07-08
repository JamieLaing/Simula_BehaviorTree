/*
 Name:		Simula_BehaviorTree.ino
 Created:	6/11/2016 2:05:02 PM
 Author:	jlaing
*/

#include "Hardware.h"
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

struct SENSOR_STATE sensorState;
struct Emotional_State emotionState;
Sensors sensors = Sensors();
Hardware hardware;
Motor motorLeft(hardware.enc1A, hardware.enc1B, hardware.mtr1Enable, hardware.mtr1In1, hardware.mtr1In2);
Motor motorRight(hardware.enc2A, hardware.enc2B, hardware.mtr2Enable, hardware.mtr2In1, hardware.mtr2In2);
Motors motors;

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



void setup() {
	Serial.begin(9600);
	randomSeed(analogRead(A3));
	hardware.init();
	sensors.init();
	motors.initializeMotors(&motorLeft, &motorRight);
	emotionState.frustration = 0;
	
	behaviorTree.setRootChild(&selector[0]);
	selector[0].addChildren({ &buttonStop, &batteryCheck, &selector[1], &selector[2], &cruise });
	selector[1].addChildren({ &cliffCenter, &cliffLeft, &cliffRight });
	selector[2].addChildren({ &perimeterCenter, &perimeterLeft, &perimeterRight });
	
	//wait for sensors to kick in.
	delay(50);
	Serial.println(F("Setup complete."));
}


void loop() {
	if (!sensors.sensorsUpdated()) {
		sensors.readIR();
	}
	
	if (!behaviorTree.run()) {
		Serial.println(F("Tree did not complete."));
	}
}
