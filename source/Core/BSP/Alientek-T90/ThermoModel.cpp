/*
 * ThermoModel.cpp
 *
 *  Tip thermocouple uV -> degC curve for the Alientek T90 (T245 cartridge).
 *
 *  Mirrors the Sequre S60 structure: the shared front-end (Core/Drivers/TipThermoModel.cpp)
 *  converts the raw ADC reading to a tip-input uV delta using ADC_VDD_MV, ADC_MAX_READING,
 *  OP_AMP_GAIN_STAGE and CalibrationOffset; this file only maps that uV delta to degrees C.
 *
 *  The T245 element is a near-linear thermocouple over its working range, so we use the same
 *  closed-form linear fit as the S60 as a starting point and re-fit the slope on hardware.
 */
#include "TipThermoModel.h"
#include "Utils.hpp"
#include "configuration.h"

#ifdef TEMP_uV_LOOKUP_T245

// Linear fit degC = (uV * A) / B. Start from the S60 ratio (50/485 ~= 9.7 uV/degC) and
// re-fit A/B once the external op-amp gain (OP_AMP_GAIN_STAGE) and two reference points are
// measured on a T245 cartridge.
// TODO calibrate T245 slope on hardware (start from the S60 ratio 50/485).
#define T245_SLOPE_NUM 50
#define T245_SLOPE_DEN 485

TemperatureType_t TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) { return (tipuVDelta * T245_SLOPE_NUM) / T245_SLOPE_DEN; }

#endif // TEMP_uV_LOOKUP_T245
