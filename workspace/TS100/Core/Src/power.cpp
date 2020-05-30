/*
 * power.cpp
 *
 *  Created on: 28 Oct, 2018
 *     Authors: Ben V. Brown, David Hilton <- Mostly David
 */

#include <power.hpp>
#include <Settings.h>
#include <BSP.h>

const uint16_t powerPWM = 255;
const uint16_t totalPWM = 255 + 17; //htim2.Init.Period, the full PWM cycle

expMovingAverage<uint32_t, wattHistoryFilter> x10WattHistory = { 0 };

int32_t tempToX10Watts(int32_t rawTemp) {
	// mass is in milliJ/*C, rawC is raw per degree C
	// returns milliWatts needed to raise/lower a mass by rawTemp
	//  degrees in one cycle.
	int32_t milliJoules = tipMass * rawTemp;
	return milliJoules;
}

void setTipX10Watts(int32_t mw) {
	int32_t output = X10WattsToPWM(mw, 1);
	setTipPWM(output);
	uint32_t actualMilliWatts = PWMToX10Watts(output, 0);

	x10WattHistory.update(actualMilliWatts);
}

uint32_t availableW10(uint8_t sample) {
	//P = V^2 / R, v*v = v^2 * 100
	//				R = R*10
	// P therefore is in V^2*100/R*10 = W*10.
	uint32_t v = getInputVoltageX10(systemSettings.voltageDiv, sample);	// 100 = 10v
	uint32_t availableWattsX10 = (v * v) / tipResistance;
	//However, 100% duty cycle is not possible as there is a dead time while the ADC takes a reading
	//Therefore need to scale available milliwats by this

	// avMw=(AvMw*powerPWM)/totalPWM.
	availableWattsX10 = availableWattsX10 * powerPWM;
	availableWattsX10 /= totalPWM;

	//availableMilliWattsX10 is now an accurate representation
	return availableWattsX10;
}

uint8_t X10WattsToPWM(int32_t milliWatts, uint8_t sample) {
	// Scale input milliWatts to the pwm range available
	if (milliWatts < 1) {
		//keep the battery voltage updating the filter
		getInputVoltageX10(systemSettings.voltageDiv, sample);
		return 0;
	}
	//	if (milliWatts > (int(systemSettings.pidPowerLimit) * 10))
//		milliWatts = (int(systemSettings.pidPowerLimit) * 10);
	//Calculate desired milliwatts as a percentage of availableW10
	uint32_t pwm = (powerPWM * milliWatts) / availableW10(sample);
	if (pwm > powerPWM) {
		pwm = powerPWM;	//constrain to max PWM counter, shouldnt be possible, but small cost for safety to avoid wraps
	}
	return pwm;
}

int32_t PWMToX10Watts(uint8_t pwm, uint8_t sample) {
	uint32_t maxMW = availableW10(sample); //Get the milliwatts for the max pwm period
	//Then convert pwm into percentage of powerPWM to get the percentage of the max mw
	return (((uint32_t) pwm) * maxMW) / powerPWM;

}
