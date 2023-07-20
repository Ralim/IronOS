#ifndef SOLDERING_COMMON_H
#define SOLDERING_COMMON_H
#include <stdint.h>

void     detailedPowerStatus();
void     basicSolderingStatus(bool boostModeOn);
bool     checkExitSoldering();
uint16_t getTipTemp(void);

#endif // SOLDERING_COMMON_H
