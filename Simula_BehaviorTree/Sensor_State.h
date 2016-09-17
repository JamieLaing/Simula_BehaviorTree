// Sensor_State.h

#ifndef _SENSOR_STATE_h
#define _SENSOR_STATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Adafruit_LSM9DS0.h>

struct UNIT_STATE {
	uint16_t totalRam = 0;      // Total RAM in bytes
	uint16_t freeRam = 0;       // Free RAM in bytes
	int8_t leftMotor = 0;       // -100 -> 100
	int8_t rightMotor = 0;       // -100 -> 100	
	boolean sdCard = false;      // SD Card initialized/available state
	boolean audioPlayer = false; // Audi Player state
	boolean audioPlaying = false; // Is the audio player playing
	uint8_t wireless = 0x00;     // Wireless Status
	unsigned long loopLastTimeMillis = 0; // Last Time in millis
	unsigned long loopMinTimeMillis = 0;  // Min Time in millis
	unsigned long loopMaxTimeMillis = 0;  // Max Time in millis
	boolean treeDeactivated = true;  //Default to pressed condition, stopping tree execution

	//Distance sensors
	boolean irLeftCliff;		// Left cliff sensor reading
	boolean irRightCliff;		// Right cliff sensor reading
	uint8_t irLeftCM;			// Left IR CM reading
	uint8_t irLeftFrontCM;		// Left front IR CM reading
	uint8_t irFrontCM;			// Front IR CM reading
	uint8_t irRightFrontCM;		// Right front IR CM reading
	uint8_t irRightCM;			// Right IR CM reading
	uint8_t pingFrontCM;		// Front Ping CM Reading

	//LSM readings
};

extern struct UNIT_STATE unitState;

class Sensors {
protected:
	unsigned long lastIrPollSensors;
public:
	void activate();
	void deactivate();
	void readIR();
	boolean irReadingUpdated();
	Adafruit_LSM9DS0 lsm;
};

extern Sensors sensors;

#endif

