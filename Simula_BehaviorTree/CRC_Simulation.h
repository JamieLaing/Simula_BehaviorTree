// CRC_Simulation.h

#ifndef _CRC_SIMULATION_h
#define _CRC_SIMULATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "CRC_Lights.h"

class CRC_SimulationClass {
private:
	uint8_t exertion;  //0-100 range representing exertion level.
	uint8_t restingBeats;  //resting BPM.
	unsigned long beatMsCheck;
	bool beatUnderway;
	int beatFlashDuration;
	uint8_t beatBrightness;

	void buttonHeartbeat(unsigned long &now);
	uint8_t currentAnimation;
	static const uint8_t animationNone = 0;
	static const uint8_t animationBio = 1;
	static const uint8_t animationRunwayFwd = 2;
public:
	CRC_SimulationClass();
	void tick();
	void showLedBio();
	void showLedNone();
};

extern CRC_SimulationClass simulation;

#endif

