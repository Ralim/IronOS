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

// thermal mass = 1690 milliJ/*C for my tip.
//  ->  Wattsx10*Seconds to raise Temp from room temp to +100*C, divided by 100*C.
// we divide mass by 20 to let the I term dominate near the set point.
//  This is necessary because of the temp noise and thermal lag in the system.
// Once we have feed-forward temp estimation we should be able to better tune this.

#ifdef MODEL_TS100
const uint16_t tipMass = 450; // divide here so division is compile-time.
const uint8_t tipResistance = 85; //x10 ohms, 8.5 typical for ts100, 4.5 typical for ts80

#endif
#ifdef MODEL_TS80
const uint16_t tipMass = 450;
const uint8_t tipResistance = 45; //x10 ohms, 8.5 typical for ts100, 4.5 typical for ts80

#endif
const uint8_t oscillationPeriod = 6 * PID_TIM_HZ; // I term look back value
extern history<uint32_t, oscillationPeriod> milliWattHistory;

int32_t tempToMilliWatts(int32_t rawTemp, uint8_t rawC);
void setTipMilliWatts(int32_t mw);
uint8_t milliWattsToPWM(int32_t milliWatts, uint8_t divisor,
		uint8_t sample = 0);
int32_t PWMToMilliWatts(uint8_t pwm, uint8_t divisor, uint8_t sample = 0);

#endif /* POWER_HPP_ */
