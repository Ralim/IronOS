/*
 * ThermoModel.cpp
 *
 *  Created on: 1 May 2021
 *      Author: Ralim
 */
#include "Setup.h"
#include "TipThermoModel.h"
#include "Utils.h"
#include "configuration.h"
extern uint16_t tipSenseResistancex10Ohms;
uint32_t        TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) {
  // For the MHP30, we are mimicing the original code and using the resistor fitted to the base of the heater head,
  // this is measured at boot in pid task and in the disconnected tip check if tip is removed
  if (tipSenseResistancex10Ohms > 900 && tipSenseResistancex10Ohms <= 1100) {
    int32_t a = ((tipSenseResistancex10Ohms / 10) + 300) * (3300000 - tipuVDelta);
    int32_t b = a / 1000000;
    int32_t c = tipuVDelta - b;
    int32_t d = c * 243 / 1000;
    return d / 10;
  }
  return 0xFFFF;
}
