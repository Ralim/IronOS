#include "Types.h"
#include <stdint.h>
#ifndef SOLDERING_COMMON_H_
#define SOLDERING_COMMON_H_

void              detailedPowerStatus();
void              basicSolderingStatus(bool boostModeOn);
bool              checkExitSoldering();
TemperatureType_t getTipTemp(void);

#endif // SOLDERING_COMMON_H_
