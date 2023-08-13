/*
 * ThermoModel.cpp
 *
 *  Created on: 1 May 2021
 *      Author: Ralim
 */
#include "TipThermoModel.h"
#include "Utils.h"
#include "configuration.h"

TemperatureType_t TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) { return (tipuVDelta * 50) / 485; }
