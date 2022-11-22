#ifndef OPERATING_MODE_UTILITIES_H_
#define OPERATING_MODE_UTILITIES_H_
#include "OLED.hpp"
#include <stdbool.h>

bool     checkForUnderVoltage(void);
uint32_t getSleepTimeout(void);
bool     shouldBeSleeping(bool inAutoStart);

#endif