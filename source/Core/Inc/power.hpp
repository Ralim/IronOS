/*
 * Power.hpp
 *
 *  Created on: 28 Oct, 2018
 *     Authors: Ben V. Brown, David Hilton (David's Idea)
 */

#include "BSP.h"
#include "configuration.h"
#include "expMovingAverage.h"
#include "stdint.h"
#include <history.hpp>
#ifndef POWER_HPP_
#define POWER_HPP_

// thermal mass = 1690 milliJ/*C for my tip.
//  ->  Wattsx10*Seconds to raise Temp from room temp to +100*C, divided by 100*C.
// we divide mass by 20 to let the I term dominate near the set point.
//  This is necessary because of the temp noise and thermal lag in the system.
// Once we have feed-forward temp estimation we should be able to better tune this.

const uint8_t                                        wattHistoryFilter = 24; // I term look back weighting
extern expMovingAverage<uint32_t, wattHistoryFilter> x10WattHistory;

int32_t tempToX10Watts(int32_t rawTemp);
void    setTipX10Watts(int32_t mw);
uint8_t X10WattsToPWM(int32_t milliWatts, uint8_t sample = 0);
#endif /* POWER_HPP_ */
