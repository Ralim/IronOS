/*
 * Power.hpp
 *
 *  Created on: 28 Oct, 2018
 *     Authors: Ben V. Brown, David Hilton (David's Idea)
 */

#include "stdint.h"
#include <history.hpp>
#include "hardware.h"
#ifndef POWER_HPP_
#define POWER_HPP_

const uint8_t oscillationPeriod = 4 * PID_TIM_HZ; // I term look back value
extern history<uint32_t, oscillationPeriod> milliWattHistory;

int32_t tempToMilliWatts(int32_t rawTemp, uint8_t rawC);
void setTipMilliWatts(int32_t mw);
uint8_t milliWattsToPWM(int32_t milliWatts, uint8_t divisor,
		uint8_t sample = 0);
int32_t PWMToMilliWatts(uint8_t pwm, uint8_t divisor, uint8_t sample = 0);

#endif /* POWER_HPP_ */
