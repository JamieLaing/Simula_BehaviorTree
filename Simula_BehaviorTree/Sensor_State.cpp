// 
// 
// 

#include "Sensor_State.h"
#include "IR_BinaryDistance.h"
#include "IR_AnalogDistance.h"
#include "Hardware.h"

void Sensors::init() {
	//Activate sensors
	digitalWrite(hardware.pinActEdge1, HIGH);
	digitalWrite(hardware.pinActEdge2, HIGH);
	digitalWrite(hardware.pinActPerim1, HIGH);
	digitalWrite(hardware.pinActPerim2, HIGH);
	digitalWrite(hardware.pinActPerim3, HIGH);
	digitalWrite(hardware.pinActPerim4, HIGH);
	digitalWrite(hardware.pinActFrntIR, HIGH);
	lastIrPollSensors = 0;
	Serial.println(F("Sensors initialized."));
}

void Sensors::readIR() {
	//Serial.println("Reading IR.");
	IR_BinaryDistance edgeLeft = IR_BinaryDistance(hardware.pinActEdge1, hardware.pinEdge1);
	IR_BinaryDistance edgeRight = IR_BinaryDistance(hardware.pinActEdge2, hardware.pinEdge2);
	IR_AnalogDistance perimLeft = IR_AnalogDistance(hardware.pinActPerim1, hardware.pinPerim1);
	IR_AnalogDistance perimLeftFront = IR_AnalogDistance(hardware.pinActPerim2, hardware.pinPerim2);
	IR_AnalogDistance perimFront = IR_AnalogDistance(hardware.pinActFrntIR, hardware.pinFrntIr);
	IR_AnalogDistance perimRightFront = IR_AnalogDistance(hardware.pinActPerim3, hardware.pinPerim3);
	IR_AnalogDistance perimRight = IR_AnalogDistance(hardware.pinActPerim4, hardware.pinPerim4);
	//PingDistance frontPing = PingDistance(pinTrigger, pinEcho);

	sensorState.irLeftCM = perimLeft.readDistance();
	sensorState.irLeftFrontCM = perimLeftFront.readDistance();
	//Serial.print("Left front:");
	//Serial.println(sensorState.irLeftFrontCM);
	sensorState.irFrontCM = perimFront.readDistance();
	sensorState.irRightFrontCM = perimRightFront.readDistance();
	//Serial.print("Right front:");
	//Serial.println(sensorState.irRightFrontCM);
	sensorState.irRightCM = perimRight.readDistance();

	sensorState.irLeftCliff = !edgeLeft.objectDetected();
	sensorState.irRightCliff = !edgeRight.objectDetected();
	lastIrPollSensors = millis();
}

boolean Sensors::sensorsUpdated() {
	unsigned long now = millis();
	unsigned long diff = now - lastIrPollSensors;

	if (lastIrPollSensors == 0)
	{
		//Serial.println("Sensors not initialized.");
		// First Read of Sensors - pre-debounce
		return false;
	}

	if (diff < 50)
	{
		// No Need to poll again yet
		// Let's debounce over a period of Millis
		//Serial.println("Sensor reading fresh.");
		return true;
	}

	if (diff > 1200)
	{
		Serial.println(F("Long loop."));
		// We lost some loop time, force a blocking poll
		delay(5);
		return false;
		//CRC_Logger.logF(CRC_Logger.LOG_TRACE, F("Forced IR Read: %ul"), diff);
	}
	return false;
}
