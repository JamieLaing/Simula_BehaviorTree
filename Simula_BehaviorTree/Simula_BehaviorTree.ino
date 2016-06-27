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
struct TREE_STATE treeState;
Sensors sensors = Sensors();
Hardware hardware;
Motor motorLeft(hardware.enc1A, hardware.enc1B, hardware.mtr1Enable, hardware.mtr1In1, hardware.mtr1In2);
Motor motorRight(hardware.enc2A, hardware.enc2B, hardware.mtr2Enable, hardware.mtr2In1, hardware.mtr2In2);
Motors motors;

Behavior_Tree behaviorTree;
Behavior_Tree::Selector selector[2];
//BehaviourTree::Sequence sequence[2];
Button_Stop buttonStop;
Cliff_Center cliffCenter;
Cliff_Left cliffLeft;
Cliff_Right cliffRight;
Cruise_Forward cruise;
Perimeter_Front perimeterFront;
Perimeter_Left perimeterLeft;
Perimeter_Right perimeterRight;


void setup() {
	Serial.begin(9600);
	randomSeed(analogRead(A3));
	hardware.init();
	sensors.init();
	motors.setMotors(&motorLeft, &motorRight);
	
	behaviorTree.setRootChild(&selector[0]);
	selector[0].addChildren({ &buttonStop, &perimeterFront, &perimeterLeft, &perimeterRight });

	//selector[0].addChildren({ &buttonStop, &selector[1], &cruise });
	//selector[1].addChildren({ &cliffCenter, &cliffLeft, &cliffRight });
	//wait for sensors to kick in.
	delay(50);
	Serial.println(F("Setup complete."));
}


void loop() {
	if (!sensors.sensorsUpdated()) {
		sensors.readIR();
	}
	treeState.currentTime = millis();
	
	if (!behaviorTree.run()) {
		Serial.println(F("Tree did not complete."));
	}
}
