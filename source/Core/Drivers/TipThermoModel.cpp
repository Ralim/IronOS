/*
 * TipThermoModel.cpp
 *
 *  Created on: 7 Oct 2019
 *      Author: ralim
 */

#include "TipThermoModel.h"
#include "BSP.h"
#include "Settings.h"
#include "Utils.h"
#include "configuration.h"
#include "main.hpp"
#include "power.hpp"
/*
 * The hardware is laid out  as a non-inverting op-amp
 * There is a pullup of 39k(TS100) from the +ve input to 3.9V (1M pulup on TS100)
 *
 * The simplest case to model this, is to ignore the pullup resistors influence, and assume that its influence is mostly constant
 * -> Tip resistance *does* change with temp, but this should be much less than the rest of the system.
 *
 * When a thermocouple is equal temperature at both sides (hot and cold junction), then the output should be 0uV
 * Therefore, by measuring the uV when both are equal, the measured reading is the offset value.
 * This is a mix of the pull-up resistor, combined with tip manufacturing differences.
 *
 * All of the thermocouple readings are based on this expired patent
 * - > https://patents.google.com/patent/US6087631A/en
 *
 * This was bought to my attention by <Kuba Sztandera>
 */
volatile uint32_t lastuv = 0;
uint32_t          TipThermoModel::convertTipRawADCTouV(uint16_t rawADC, bool skipCalOffset) {
  // This takes the raw ADC samples, converts these to uV
  // Then divides this down by the gain to convert to the uV on the input to the op-amp (A+B terminals)
  // Then remove the calibration value that is stored as a tip offset
  uint32_t vddRailmVX10 = ADC_VDD_MV * 10; // The vreg is +-2%, but we have no higher accuracy available
  // 4096 * 8 readings for full scale
  // Convert the input ADC reading back into mV times 10 format.
  uint32_t rawInputmVX10 = (rawADC * vddRailmVX10) / (ADC_MAX_READING);

  uint32_t valueuV = rawInputmVX10 * 100; // shift into uV
  // Now to divide this down by the gain
  valueuV /= OP_AMP_GAIN_STAGE;

  if (getSettingValue(SettingsOptions::CalibrationOffset) && skipCalOffset == false) {
    // Remove uV tipOffset
    if (valueuV > getSettingValue(SettingsOptions::CalibrationOffset))
      valueuV -= getSettingValue(SettingsOptions::CalibrationOffset);
    else
      valueuV = 0;
  }
  lastuv = valueuV;
  return valueuV;
}

uint32_t TipThermoModel::convertTipRawADCToDegC(uint16_t rawADC) { return convertuVToDegC(convertTipRawADCTouV(rawADC)); }
uint32_t TipThermoModel::convertTipRawADCToDegF(uint16_t rawADC) { return convertuVToDegF(convertTipRawADCTouV(rawADC)); }

uint32_t TipThermoModel::convertuVToDegF(uint32_t tipuVDelta) { return convertCtoF(convertuVToDegC(tipuVDelta)); }

uint32_t TipThermoModel::convertCtoF(uint32_t degC) {
  //(Y °C × 9/5) + 32 =Y°F
  return (32 + ((degC * 9) / 5));
}

uint32_t TipThermoModel::convertFtoC(uint32_t degF) {
  //(Y°F − 32) × 5/9 = Y°C
  if (degF < 32) {
    return 0;
  }
  return ((degF - 32) * 5) / 9;
}
uint32_t TipThermoModel::getTipInC(bool sampleNow) {
  int32_t currentTipTempInC = TipThermoModel::convertTipRawADCToDegC(getTipRawTemp(sampleNow));
  currentTipTempInC += getHandleTemperature(sampleNow) / 10; // Add handle offset

  // Power usage indicates that our tip temp is lower than our thermocouple temp.
  // I found a number that doesn't unbalance the existing PID, causing overshoot.
  // This could be tuned in concert with PID parameters...

  if (currentTipTempInC < 0)
    return 0;
  return currentTipTempInC;
}

uint32_t TipThermoModel::getTipInF(bool sampleNow) {
  uint32_t currentTipTempInF = getTipInC(sampleNow);
  currentTipTempInF          = convertCtoF(currentTipTempInF);
  return currentTipTempInF;
}

uint32_t TipThermoModel::getTipMaxInC() {
  uint32_t maximumTipTemp = TipThermoModel::convertTipRawADCToDegC(ADC_MAX_READING - 1);
  maximumTipTemp += getHandleTemperature(0) / 10; // Add handle offset
  return maximumTipTemp - 1;
}
