/*
 * ThermoModel.cpp
 *
 *  Created on: 1 May 2021
 *      Author: Ralim
 */
#include "TipThermoModel.h"
#include "Utils.h"
#include "configuration.h"

#ifdef TEMP_uV_LOOKUP_PT100_1K_PULLUP

#endif // TEMP_uV_LOOKUP_PT100_1K_PULLUP
#ifdef TEMP_uV_LOOKUP_S60
TemperatureType_t TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) { return (tipuVDelta * 50) / 485; }
#endif // TEMP_uV_LOOKUP_S60
