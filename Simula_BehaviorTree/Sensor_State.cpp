// 
// 
// 

#include "Sensor_State.h"
#include "IR_BinaryDistance.h"
#include "IR_BinaryDistance.h"
#include "Hardware.h"

void Sensors::init() {
	digitalWrite(hardware.pinActEdge1, HIGH);
	digitalWrite(hardware.pinActEdge2, HIGH);
	lastIrPollSensors = 0;
	Serial.println("Sensors initialized.");
}

void Sensors::readIR() {
	//Serial.println("Reading IR.");
	IR_BinaryDistance edgeLeft = IR_BinaryDistance(hardware.pinActEdge1, hardware.pinEdge1);
	//IR_AnalogDistance perim1IR = IR_AnalogDistance(hardware.pinActPerim1, hardware.pinPerim1);
	//IR_AnalogDistance perim2IR = IR_AnalogDistance(hardware.pinActPerim2, hardware.pinPerim2);
	//IR_AnalogDistance frontIR = IR_AnalogDistance(hardware.pinActFrntIR, hardware.pinFrntIr);
	//PingDistance frontPing = PingDistance(pinTrigger, pinEcho);
	//IR_AnalogDistance perim3IR = IR_AnalogDistance(hardware.pinActPerim3, hardware.pinPerim3);
	//IR_AnalogDistance perim4IR = IR_AnalogDistance(hardware.pinActPerim4, hardware.pinPerim4);
	IR_BinaryDistance edgeRight = IR_BinaryDistance(hardware.pinActEdge2, hardware.pinEdge2);
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
		Serial.println("Long loop.");
		// We lost some loop time, force a blocking poll
		delay(5);
		return false;
		//CRC_Logger.logF(CRC_Logger.LOG_TRACE, F("Forced IR Read: %ul"), diff);
	}
	return false;
}
