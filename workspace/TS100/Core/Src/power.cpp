/*
 * power.cpp
 *
 *  Created on: 28 Oct, 2018
 *     Authors: Ben V. Brown, David Hilton
 */

#include <power.hpp>
#include <Settings.h>
#include <hardware.h>

uint8_t tipResistance = 85; //x10 ohms, 8.5 typical for ts100, 4.5 typical for ts80
const uint16_t powerPWM = 255;
const uint16_t totalPWM = 255 + 60; //htim2.Init.Period, the full PWM cycle

history<uint32_t, oscillationPeriod> milliWattHistory = { { 0 }, 0, 0 };

void setupPower(uint8_t res) {
	tipResistance = res;
}

int32_t tempToMilliWatts(int32_t rawTemp, uint16_t mass, uint8_t rawC) {
	// mass is in milliJ/*C, rawC is raw per degree C
	// returns milliWatts needed to raise/lower a mass by rawTemp
	//  degrees in one cycle.
	int32_t milliJoules = mass * rawTemp / rawC;
	return milliJoules * hz;
}

void setTipMilliWatts(int32_t mw) {
	//Enforce Max Watts Limiter # TODO

	int32_t output = milliWattsToPWM(mw, systemSettings.voltageDiv / 10,1);
	setTipPWM(output);
	uint16_t actualMilliWatts = PWMToMilliWatts(output,
			systemSettings.voltageDiv / 10);

	milliWattHistory.update(actualMilliWatts);
}

uint8_t milliWattsToPWM(int32_t milliWatts, uint8_t divisor, uint8_t sample) {
	//P = V^2 / R, v*v = v^2 * 100
	//				R = R*10
	// P therefore is in V^2*10/R = W*10.
	// Scale input milliWatts to the pwm rate
	if (milliWatts == 0)
		return 0;
	int32_t v = getInputVoltageX10(divisor, sample);	// 100 = 10v
	int32_t availableMilliWatts = v * v / tipResistance;

	//int32_t pwm = ((powerPWM * totalPWM / powerPWM) * milliWatts)	/ availableMilliWatts;
	int32_t pwm = (totalPWM * milliWatts) / availableMilliWatts;
	if (pwm > powerPWM) {
		pwm = powerPWM;
	} else if (pwm < 0) {
		pwm = 0;
	}


	return pwm;
}

int32_t PWMToMilliWatts(uint8_t pwm, uint8_t divisor) {
	int32_t v = getInputVoltageX10(divisor, 0);
	return pwm * (v * v / tipResistance) / (powerPWM * totalPWM / powerPWM);
}
