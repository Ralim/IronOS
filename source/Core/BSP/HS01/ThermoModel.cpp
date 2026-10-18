/*
 * ThermoModel.cpp
 *
 * FNIRSI HS-01
 */
#include "TipThermoModel.h"
#include "Utils.hpp"
#include "configuration.h"

#ifdef TEMP_uV_LOOKUP_HS01
// Stock firmware calibration: ADC pin voltage vs tip temperature, with a fixed 25 C cold junction (47 mV)
static const int32_t pinuVtoDegC[] = {
    //
    // uV at the ADC pin, Temperature (Celsius)
    0,       0,   //
    470000,  250, //
    580000,  300, //
    750000,  350, //
    990000,  400, //
    2100000, 848, // 0..2.1 V ADC range end, on the 400 C slope (4000 / 9900)
};

static const int     pinuVtoDegCItems = sizeof(pinuVtoDegC) / (2 * sizeof(pinuVtoDegC[0]));
static const int32_t coldJunctionuV   = 47000; // Table voltage of 25 C

// IronOS adds the handle temperature (fixed 25 C, see BSP.cpp) on top of the result, so return the rise above it
TemperatureType_t TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) { return Utils::InterpolateLookupTable(pinuVtoDegC, pinuVtoDegCItems, tipuVDelta + coldJunctionuV) - 25; }
#endif // TEMP_uV_LOOKUP_HS01
