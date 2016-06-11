/*
 Name:		Simula_BehaviorTree.ino
 Created:	6/11/2016 2:05:02 PM
 Author:	jlaing
*/

#include "BehaviorTree.h"
#include "PingDistance.h"
#include "UnitState.h"
#include "IR_BinaryDistance.h"
#include "IR_AnalogDistance.h"
#include "DistanceSensor.h"
#include "Motor.h"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <StandardCplusplus.h>

Unit_State unitState = Unit_State();
bool prevIrLeftState;
bool prevIrRightState;

void setup() {
	Serial.begin(9600);
	Serial.println("Booting.");
	
	BehaviourTree behaviorTree;
	BehaviourTree::Selector selector[3];
	BehaviourTree::Sequence sequence[4];
	Action walkToDoor("Walk to door", 99), openDoor1("Open door", 15), unlockDoor("Unlock door", 25), openDoor2("Open door after unlocking it", 99), smashDoor("Smash door", 5),
		walkThroughDoor("Walk through door", 60), closeDoor("Close door", 100), walkToWindow("Walk to Window", 99), openWindow1("Open window", 50), unlockWindow("Unlock window", 50),
		openWindow2("Open window after unlocking it", 85), smashWindow("Smash window", 50), climbThroughWindow("Climb through window", 85), closeWindow("Close window", 100);

	behaviorTree.setRootChild(&selector[0]);
	selector[0].addChildren({ &sequence[0],&sequence[2] });
	sequence[0].addChildren({ &walkToDoor, &selector[1], &walkThroughDoor, &closeDoor });
	selector[1].addChildren({ &openDoor1, &sequence[1], &smashDoor });
	sequence[1].addChildren({ &unlockDoor, &openDoor2 });
	sequence[2].addChildren({ &walkToWindow, &selector[2], &climbThroughWindow, &closeWindow });
	selector[2].addChildren({ &openWindow1, &sequence[3], &smashWindow });
	sequence[3].addChildren({ &unlockWindow, &openWindow2 });

	if (behaviorTree.run())
		Serial.println("Congratulations!  You made it out!");
	else
		Serial.println("Sorry.  You are trapped here for life.");
}


void loop() {
	/*bool irCurrentLeftState = unitState.edgeLeft.objectDetected();
	if (prevIrLeftState != irCurrentLeftState) {
		Serial.print("Edge left: ");
		Serial.println(irCurrentLeftState);
		prevIrLeftState = irCurrentLeftState;
	}

	bool irCurrentRightState = unitState.edgeRight.objectDetected();
	if (prevIrRightState != irCurrentRightState) {
		Serial.print("Edge Right: ");
		Serial.println(irCurrentRightState);
		prevIrRightState = irCurrentRightState;
	}*/
	//unit_state.motorLeft.setPower(120);
}
