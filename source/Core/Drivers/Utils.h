/*
 * Utils.h
 *
 *  Created on: 28 Apr 2021
 *      Author: Ralim
 */

#ifndef CORE_DRIVERS_UTILS_H_
#define CORE_DRIVERS_UTILS_H_
#include <stdint.h>
class Utils {
public:
  static int32_t InterpolateLookupTable(const int32_t *lookupTable, const int noItems, const int32_t value);
  static int32_t LinearInterpolate(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x);
};

#endif /* CORE_DRIVERS_UTILS_H_ */
