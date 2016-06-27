// Hardware.h

#ifndef _HARDWARE_h
#define _HARDWARE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Hardware {
public:
	// Define Pin's by Hardware Revisions
	// Default to the ALPHA Units Pinouts
#ifndef _CRC_BOARD_VER_
#define _CRC_BOARD_VER_    ALPHA
	const byte enc1A = 3;
	const byte enc1B = 2;
	const byte pinButton = 5;
	const byte pinLED = 13;
	const byte enc2A = 18;
	const byte enc2B = 19;

	const byte mtr1In1 = 24;
	const byte mtr1In2 = 22;
	const byte mtr1Enable = 44;

	const byte mtr2In1 = 23;
	const byte mtr2In2 = 25;
	const byte mtr2Enable = 45;

	// VS1053 Control
	const byte vs1053_dcs = 8;
	const byte vs1053_reset = 9;
	const byte vs1053_cs = 10;   // SPI CS for VS 1053 Control Port
	const byte vs1053_dreq = 12; // SPI CS for VS 1053 Data Port
	const byte ampGain0 = 37;
	const byte ampGain1 = 40;
	const byte ampEnable = 41;

	const byte sdcard_cs = 4; // SPI Chip Select for SD Card

	const byte pinBatt = A2;

	// IR Sensor pins

	const byte pinEdge1 = A0;
	const byte pinActEdge1 = 27;
	const byte pinEdge2 = A1;
	const byte pinActEdge2 = 31;

	const byte pinPerim1 = A4;
	const byte pinActPerim1 = 26;
	const byte pinPerim2 = A5;
	const byte pinActPerim2 = 28;
	const byte pinPerim3 = A6;
	const byte pinActPerim3 = 30;
	const byte pinPerim4 = A7;
	const byte pinActPerim4 = 32;
	const byte pinFrntIr = A8;
	const byte pinActFrntIR = 29;

	// Ping Sensors
	const byte pinFrontPing = 6;
	const byte pinActFrontPing = 7;

	// Free Pins with Breakouts
	// const byte pinSpeaker = 11; // Unused, Breakout to Daughter Board

	// Reserved Pins with Breakouts
	// const byte SPI_miso = 50; // Reserved - Standard Arduino SPI
	// const byte SPI_mosi = 51; // Reserved - Standard Arduino SPI
	// const byte SPI_clk = 52;  // Reserved - Standard Arduino SPI

#endif

	void init();
private:
	void setupPins();
};

extern Hardware hardware;

#endif

