// 
// 
// 

#include "Motor.h"

Motor::Motor(int encoderPin1, int encoderPin2, int mtrEnable, int mtrIn1, int mtrIn2)
	: Encoder(encoderPin1, encoderPin2) {
	_mtrEnable = mtrEnable;
	_mtrIn1 = mtrIn1;
	_mtrIn2 = mtrIn2;
	_previousPosition = 0;
	_previousRateCheckMillis = 0;
	_stallPower = 0;
	pinMode(_mtrEnable, OUTPUT);
	pinMode(_mtrIn1, OUTPUT);
	pinMode(_mtrIn2, OUTPUT);
	powerOff();
	motorOff = true;

}

void Motor::setPower(int power) {
	boolean in1 = HIGH;
	boolean in2 = LOW;

	if (power > 255) {
		Serial.print("Power max exceeded.  Power: ");
		Serial.println(power);
	}
	else {
		if (power > 0)
		{
			in1 = LOW;
			in2 = HIGH;
		}
		analogWrite(_mtrEnable, abs(power));
		digitalWrite(_mtrIn1, in1);
		digitalWrite(_mtrIn2, in2);
		motorOff = false;
	}
}

void Motor::powerOff() {
	if (!motorOff)
	{
		analogWrite(_mtrEnable, 0);
		digitalWrite(_mtrIn1, LOW);
		digitalWrite(_mtrIn2, LOW);
		motorOff = true;
	}
}

bool Motor::positionChanged() {
	bool _positionChanged = false;
	int32_t _newPosition = read();
	if (_newPosition != _previousPosition) {
		_previousPosition = _newPosition;
		_positionChanged = true;
	}
	return _positionChanged;
}

void Motor::accelerateToEncoderTarget(int32_t encoderTarget, int powerTarget) {
	unsigned long _currentMillis = millis();
	if (read() < encoderTarget) {
		if (_currentMillis - _previousRateCheckMillis >= _rateCheckInterval) {
			_previousRateCheckMillis = _currentMillis;
			if (!positionChanged()) {
				_stallPower += 5;
				Serial.print("Stall power: ");
				Serial.println(_stallPower);
			}
		}
		setPower(_stallPower);
	}
	else
	{
		powerOff();
	}
}

void Motor::setEncoderRate(int32_t pulsesPerSecond) {
	//1000 millis per second
	int _interval = 20;
	int _convertedPulses = pulsesPerSecond * _interval / 1000;
	unsigned long _currentMillis = millis();
	if (_currentMillis - _previousRateCheckMillis >= _interval) {
		_previousRateCheckMillis = _currentMillis;
		int32_t _currentPosition = read();
		int32_t _pulsesPerInterval = _currentPosition - _previousPosition;
		_previousPosition = _currentPosition;
		if (_pulsesPerInterval < _convertedPulses) {
			_currentPower += 1;
			setPower(_currentPower);
		}
		if (_pulsesPerInterval > _convertedPulses)
		{
			_currentPower -= 1;
			setPower(_currentPower);
		}
		Serial.print("Current power: ");
		Serial.println(_currentPower);
		Serial.print("measuredPPS: ");
		Serial.println(_pulsesPerInterval * 1000 / _interval);
		Serial.print("targetPPS: ");
		Serial.println(pulsesPerSecond);
	}

}
