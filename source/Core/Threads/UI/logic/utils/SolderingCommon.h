#include "Types.h"
#include <stdint.h>
#ifndef SOLDERING_COMMON_H_
#define SOLDERING_COMMON_H_

bool              checkExitSoldering();
TemperatureType_t getTipTemp(void);

#endif // SOLDERING_COMMON_H_
