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

extern history<uint16_t, 75> milliWattHistory;

int32_t tempToMilliWatts(int32_t rawTemp, uint16_t mass, uint8_t rawC);
void setTipMilliWatts(int32_t mw);
uint8_t milliWattsToPWM(int32_t milliWatts, uint8_t divisor);
int32_t PWMToMilliWatts(uint8_t pwm, uint8_t divisor);

#endif /* POWER_HPP_ */
