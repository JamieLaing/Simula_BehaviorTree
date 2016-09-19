// CRC_Simulation.h

#ifndef _CRC_SIMULATION_h
#define _CRC_SIMULATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Simulation {
public:
	uint8_t exertion;  //A 0-100 range representing exertion level.
};

extern Simulation simulation;

#endif

