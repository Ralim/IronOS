/*
 * power.cpp
 *
 *  Created on: 28 Oct, 2018
 *     Authors: Ben V. Brown, David Hilton
 */

#include <power.hpp>
#include <Settings.h>
#include <hardware.h>

history<uint16_t, 75> milliWattHistory = {{0}, 0, 0};

const uint8_t tipResistance = 87;
const uint8_t hz = 33;
const uint8_t maxPWM = 100;

int32_t tempToMilliWatts(int32_t rawTemp, uint16_t mass, uint8_t rawC) {
	 // mass is in milliJ/*C, rawC is raw per degree C
	int32_t milliJoules = mass * rawTemp / rawC;
	return milliJoules * hz;
}

void setTipMilliWatts(int32_t mw) {
	int32_t output = milliWattsToPWM(mw, systemSettings.voltageDiv / 10);
	setTipPWM(output);
	uint16_t actualMilliWatts = PWMToMilliWatts(output, systemSettings.voltageDiv / 10);

	milliWattHistory.update(actualMilliWatts);
}

uint8_t milliWattsToPWM(int32_t milliWatts, uint8_t divisor) {
	int32_t v = getInputVoltageX10(divisor); // 1000 = 10v
	int32_t availableMilliWatts = v*v / tipResistance;
	int32_t pwm = maxPWM * milliWatts / availableMilliWatts;

	if (pwm > maxPWM) {
		pwm = maxPWM;
	} else if (pwm < 0) {
		pwm = 0;
	}
	return pwm;
}

int32_t PWMToMilliWatts(uint8_t pwm, uint8_t divisor) {
	int32_t v = getInputVoltageX10(divisor);
	return pwm * (v*v / tipResistance) / maxPWM;
}
