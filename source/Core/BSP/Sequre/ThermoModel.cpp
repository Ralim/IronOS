/*
 * ThermoModel.cpp
 *
 *  Created on: 1 May 2021
 *      Author: Ralim
 */
#include "TipThermoModel.h"
#include "Utils.h"
#include "configuration.h"

#if defined(TEMP_uV_LOOKUP_S60)
TemperatureType_t TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) { return (tipuVDelta * 50) / 485; }
#elif defined(TEMP_uV_LOOKUP_S99)
// 42.85 uV / K -> 1/42.85 K/uV = 20/857
// TemperatureType_t TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) { return (tipuVDelta * 20) / 857; }
// Measurement is probably with heating element in series, thats why the reading differs from the approx 42.85 uV/K
TemperatureType_t TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) { return (tipuVDelta * 1) / 30; }
#endif
