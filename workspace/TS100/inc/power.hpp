/*
 * Power.hpp
 *
 *  Created on: 28 Oct, 2018
 *     Authors: Ben V. Brown, David Hilton
 */

#include "stdint.h"
#include <history.hpp>

#ifndef POWER_HPP_
#define POWER_HPP_

const uint8_t hz = 32;//PID loop rate
const uint8_t oscillationPeriod = 3.5 * hz; // dampening look back tuning
extern history<uint32_t, oscillationPeriod> milliWattHistory;
void setupPower(uint8_t resistance);

int32_t tempToMilliWatts(int32_t rawTemp, uint16_t mass, uint8_t rawC);
void setTipMilliWatts(int32_t mw);
uint8_t milliWattsToPWM(int32_t milliWatts, uint8_t divisor,uint8_t sample=0);
int32_t PWMToMilliWatts(uint8_t pwm, uint8_t divisor);

#endif /* POWER_HPP_ */
