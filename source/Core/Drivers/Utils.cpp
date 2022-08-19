/*
 * Utils.cpp
 *
 *  Created on: 28 Apr 2021
 *      Author: Ralim
 */

#include <Utils.h>

int32_t Utils::InterpolateLookupTable(const int32_t *lookupTable, const int noItems, const int32_t value) {
  if (value) {
    for (int i = 1; i < (noItems - 1); i++) {
      // If current tip temp is less than current lookup, then this current lookup is the higher point to interpolate
      if (value < lookupTable[i * 2]) {
        return LinearInterpolate(lookupTable[(i - 1) * 2], lookupTable[((i - 1) * 2) + 1], lookupTable[i * 2], lookupTable[(i * 2) + 1], value);
      }
    }
    return LinearInterpolate(lookupTable[(noItems - 2) * 2], lookupTable[((noItems - 2) * 2) + 1], lookupTable[(noItems - 1) * 2], lookupTable[((noItems - 1) * 2) + 1], value);
  }
  return 0;
}

int32_t Utils::LinearInterpolate(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x) { return y1 + (((((x - x1) * 1000) / (x2 - x1)) * (y2 - y1))) / 1000; }
