/*
 * ThermoModel.cpp
 *
 *  Tip thermocouple uV -> degC curve for the Alientek T90 (T245 cartridge).
 *
 *  Mirrors the Sequre S60 structure: the shared front-end (Core/Drivers/TipThermoModel.cpp)
 *  converts the raw ADC reading to a tip-input uV delta using ADC_VDD_MV, ADC_MAX_READING,
 *  OP_AMP_GAIN_STAGE and CalibrationOffset; this file only maps that uV delta to degrees C.
 *
 *  The uV -> degC curve below is reverse-engineered from the T90B stock firmware (its op-amp output
 *  mV -> degC lookup), so the slope/curvature match the factory; absolute offset is trimmed by the
 *  on-device tip calibration.
 */
#include "TipThermoModel.h"
#include "Utils.hpp"
#include "configuration.h"

#ifdef TEMP_uV_LOOKUP_T245

// T245 (JBC C245) tip thermocouple uV -> degC, reverse-engineered from the Alientek T90B stock
// firmware. The factory uses a piecewise-linear lookup (op-amp output mV -> absolute degC) plus a
// cold-junction term, with an external op-amp gain ~= 250 (see OP_AMP_GAIN_STAGE). The table below is
// that curve in the IronOS delta model: tipuV (after the gain divide) -> degrees ABOVE the cold
// junction, which getTipInC() then sums with the handle (cold-junction) temperature. The factory's
// ~38 degC built-in intercept (op-amp zero offset) is removed so the curve passes through the origin;
// the small per-unit residual is trimmed by the on-device tip calibration. Pairs are {uV, degC}.
const int32_t uVtoDegC[] = {
    0,    0,   //
    135,  16,  //
    533,  64,  //
    927,  112, //
    1395, 162, //
    1846, 212, //
    2363, 260, //
    2889, 312, //
    3414, 367, //
    3939, 410, //
    4464, 461, //
    4989, 508, //
};
const int uVtoDegCItems = sizeof(uVtoDegC) / (2 * sizeof(uVtoDegC[0]));

TemperatureType_t TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) { return Utils::InterpolateLookupTable(uVtoDegC, uVtoDegCItems, tipuVDelta); }

#endif // TEMP_uV_LOOKUP_T245
