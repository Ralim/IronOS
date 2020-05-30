/*
 * Power.hpp
 *
 *  Created on: 28 Oct, 2018
 *     Authors: Ben V. Brown, David Hilton (David's Idea)
 */

#include "stdint.h"
#include <history.hpp>
#include "BSP.h"
#include "expMovingAverage.h"
#ifndef POWER_HPP_
#define POWER_HPP_

// thermal mass = 1690 milliJ/*C for my tip.
//  ->  Wattsx10*Seconds to raise Temp from room temp to +100*C, divided by 100*C.
// we divide mass by 20 to let the I term dominate near the set point.
//  This is necessary because of the temp noise and thermal lag in the system.
// Once we have feed-forward temp estimation we should be able to better tune this.

#ifdef MODEL_TS100
const int32_t tipMass = 45; // X10 watts to raise 1 deg C in 1 second
const uint8_t tipResistance = 85; //x10 ohms, 8.5 typical for ts100, 4.5 typical for ts80

#endif
#ifdef MODEL_TS80
const uint32_t tipMass = 40;
const uint8_t tipResistance = 45; //x10 ohms, 8.5 typical for ts100, 4.5 typical for ts80

#endif
const uint8_t wattHistoryFilter = 24; // I term look back weighting
extern expMovingAverage<uint32_t, wattHistoryFilter> x10WattHistory;

int32_t tempToX10Watts(int32_t rawTemp);
void setTipX10Watts(int32_t mw);
uint8_t X10WattsToPWM(int32_t milliWatts, uint8_t sample = 0);
int32_t PWMToX10Watts(uint8_t pwm, uint8_t sample = 0);
uint32_t availableW10(uint8_t sample) ;
#endif /* POWER_HPP_ */
