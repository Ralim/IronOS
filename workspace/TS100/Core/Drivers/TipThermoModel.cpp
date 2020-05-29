/*
 * TipThermoModel.cpp
 *
 *  Created on: 7 Oct 2019
 *      Author: ralim
 */

#include "TipThermoModel.h"
#include "Settings.h"
#include "BSP.h"
#include "../../configuration.h"

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


uint32_t TipThermoModel::convertTipRawADCTouV(uint16_t rawADC) {
	// This takes the raw ADC samples, converts these to uV
	// Then divides this down by the gain to convert to the uV on the input to the op-amp (A+B terminals)
	// Then remove the calibration value that is stored as a tip offset
	uint32_t vddRailmVX10 = 33000;	//The vreg is +-2%, but we have no higher accuracy available
	// 4096 * 8 readings for full scale
	// Convert the input ADC reading back into mV times 10 format.
	uint32_t rawInputmVX10 = (rawADC * vddRailmVX10) / (4096 * 8);

	uint32_t valueuV = rawInputmVX10 * 100;	// shift into uV
	//Now to divide this down by the gain
	valueuV = (valueuV) / OP_AMP_GAIN_STAGE;
	//Remove uV tipOffset
	if (valueuV >= systemSettings.CalibrationOffset)
		valueuV -= systemSettings.CalibrationOffset;
	else
		valueuV = 0;

	return valueuV;
}

uint32_t TipThermoModel::convertTipRawADCToDegC(uint16_t rawADC) {
	return convertuVToDegC(convertTipRawADCTouV(rawADC));
}
#ifdef ENABLED_FAHRENHEIT_SUPPORT
uint32_t TipThermoModel::convertTipRawADCToDegF(uint16_t rawADC) {
	return convertuVToDegF(convertTipRawADCTouV(rawADC));
}
#endif

//Table that is designed to be walked to find the best sample for the lookup

//Extrapolate between two points
// [x1, y1] = point 1
// [x2, y2] = point 2
//  x = input value
// output is x's extrapolated y value
int32_t LinearInterpolate(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
		int32_t x) {
	return y1 + (((((x - x1) * 1000) / (x2 - x1)) * (y2 - y1))) / 1000;
}

uint32_t TipThermoModel::convertuVToDegC(uint32_t tipuVDelta) {
	//based on new measurements, tip is quite linear
	//
	tipuVDelta *= 10;
	tipuVDelta /= systemSettings.TipGain;

#ifdef MODEL_TS80
	tipuVDelta /= OP_AMP_GAIN_STAGE_TS100 / OP_AMP_GAIN_STAGE_TS80;
#endif

	return tipuVDelta;
}

#ifdef ENABLED_FAHRENHEIT_SUPPORT
uint32_t TipThermoModel::convertuVToDegF(uint32_t tipuVDelta) {
	return convertCtoF(convertuVToDegC(tipuVDelta));
}

uint32_t TipThermoModel::convertCtoF(uint32_t degC) {
	//(Y °C × 9/5) + 32 =Y°F
	return 32 + ((degC * 9) / 5);
}

uint32_t TipThermoModel::convertFtoC(uint32_t degF) {
	//(Y°F − 32) × 5/9 = Y°C
	if (degF < 32)
		return 0;
	return ((degF - 32) * 5) / 9;
}
#endif

uint32_t TipThermoModel::getTipInC(bool sampleNow) {
	uint32_t currentTipTempInC = TipThermoModel::convertTipRawADCToDegC(
			getTipRawTemp(sampleNow));
	currentTipTempInC += getHandleTemperature() / 10; //Add handle offset
	return currentTipTempInC;
}
#ifdef ENABLED_FAHRENHEIT_SUPPORT
uint32_t TipThermoModel::getTipInF(bool sampleNow) {
	uint32_t currentTipTempInF = TipThermoModel::convertTipRawADCToDegF(
			getTipRawTemp(sampleNow));
	currentTipTempInF += convertCtoF(getHandleTemperature() / 10); //Add handle offset
	return currentTipTempInF;
}
#endif

uint32_t TipThermoModel::getTipMaxInC() {
	uint32_t maximumTipTemp = TipThermoModel::convertTipRawADCToDegC(
			0x7FFF - (80 * 5)); //back off approx 5 deg c from ADC max
	maximumTipTemp += getHandleTemperature() / 10; //Add handle offset
	return maximumTipTemp - 1;
}
