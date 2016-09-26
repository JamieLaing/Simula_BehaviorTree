// CRC_Simulation.h

#ifndef _CRC_SIMULATION_h
#define _CRC_SIMULATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class CRC_SimulationClass {
private:
	uint8_t exertion;  //0-100 range representing exertion level.
	uint8_t restingBeats;  //resting BPM.
	unsigned long beatMsCheck;
	bool heartbeatUnderway;
	void buttonHeartbeat(unsigned long &now);
	uint8_t buttonBrightness;
	uint8_t buttonFadeAmount;
	uint8_t buttonFadeDelay;
public:
	CRC_SimulationClass();
	void tick();
	void showLedBio();
	void showLedNone();
};

extern CRC_SimulationClass simulation;

#endif

