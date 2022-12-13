

#include "OperatingModeUtilities.h"
#include <stdint.h>
uint16_t min(uint16_t a, uint16_t b) {
  if (a > b)
    return b;
  else
    return a;
}