/*
 * Utils.hpp
 *
 *  Created on: 28 Apr 2021
 *      Author: Ralim
 */

#ifndef CORE_DRIVERS_UTILS_HPP_
#define CORE_DRIVERS_UTILS_HPP_
#include <stdint.h>
class Utils {
public:
  static int32_t InterpolateLookupTable(const int32_t *lookupTable, const int noItems, const int32_t value);
  static int32_t LinearInterpolate(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x);

  // Return the required current in X10 for the specified voltage
  static uint16_t RequiredCurrentForTipAtVoltage(uint16_t voltageX10);

};

#endif /* CORE_DRIVERS_UTILS_HPP_ */
