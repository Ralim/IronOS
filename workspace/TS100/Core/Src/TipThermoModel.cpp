/*
 * TipThermoModel.cpp
 *
 *  Created on: 7 Oct 2019
 *      Author: ralim
 */

#include "TipThermoModel.h"
#include "Settings.h"
#include "hardware.h"

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

#ifdef MODEL_TS100
#define OP_AMP_Rf 		750*1000 		/*750  Kilo-ohms -> From schematic, R1*/
#define OP_AMP_Rin 		2370			/*2.37 Kilo-ohms -> From schematic, R2*/

#else
#define OP_AMP_Rf 		180*1000 		/*180  Kilo-ohms -> From schematic, R6*/
#define OP_AMP_Rin 		2000			/*2.0  Kilo-ohms -> From schematic, R3*/

#endif

#define  op_amp_gain_stage  (1+(OP_AMP_Rf/OP_AMP_Rin))
uint32_t TipThermoModel::convertTipRawADCTouV(uint16_t rawADC) {
	// This takes the raw ADC samples, converts these to uV
	// Then divides this down by the gain to convert to the uV on the input to the op-amp (A+B terminals)
	// Then remove the calibration value that is stored as a tip offset
	uint32_t vddRailmVX10 = 33000;	//TODO use ADC Vref to calculate this
	// 4096 * 8 readings for full scale
	// Convert the input ADC reading back into mV times 10 format.
	uint32_t rawInputmVX10 = (rawADC * vddRailmVX10) / (4096 * 8);

	uint32_t valueuV = rawInputmVX10 * 100;	// shift into uV
	//Now to divide this down by the gain
	valueuV = (valueuV) / op_amp_gain_stage;
	//Remove uV tipOffset
	if (valueuV > systemSettings.CalibrationOffset)
		valueuV -= systemSettings.CalibrationOffset;
	else
		valueuV = 0;
	//TODO
	return valueuV;
}

uint32_t TipThermoModel::convertTipRawADCToDegC(uint16_t rawADC) {
	return convertuVToDegC(convertTipRawADCTouV(rawADC));
}
uint32_t TipThermoModel::convertTipRawADCToDegF(uint16_t rawADC) {
	return convertuVToDegF(convertTipRawADCTouV(rawADC));
}

//Table that is designed to be walked to find the best sample for the lookup

struct HakkoThermocoupleLookup {
	// 0 is the uV reading
	// 1 is the deg C X10
	// This was created from numbers transcribed from the patent by <Kuba Sztandera>
	constexpr HakkoThermocoupleLookup() :
			values() {
		values[0][0] = 0;
		values[0][1] = 0;
		values[1][0] = 0;
		values[1][1] = 0;
		values[2][0] = 175;
		values[2][1] = 100;
		values[3][0] = 381;
		values[3][1] = 200;
		values[4][0] = 587;
		values[4][1] = 300;
		values[5][0] = 804;
		values[5][1] = 400;
		values[6][0] = 1005;
		values[6][1] = 500;
		values[7][0] = 1007;
		values[7][1] = 600;
		values[8][0] = 1107;
		values[8][1] = 700;
		values[9][0] = 1310;
		values[9][1] = 800;
		values[10][0] = 1522;
		values[10][1] = 900;
		values[11][0] = 1731;
		values[11][1] = 1000;
		values[12][0] = 1939;
		values[12][1] = 1100;
		values[13][0] = 2079;
		values[13][1] = 1200;
		values[14][0] = 2265;
		values[14][1] = 1300;
		values[15][0] = 2470;
		values[15][1] = 1400;
		values[16][0] = 2676;
		values[16][1] = 1500;
		values[17][0] = 2899;
		values[17][1] = 1600;
		values[18][0] = 3081;
		values[18][1] = 1700;
		values[19][0] = 3186;
		values[19][1] = 1800;
		values[20][0] = 3422;
		values[20][1] = 1900;
		values[21][0] = 3622;
		values[21][1] = 2000;
		values[22][0] = 3830;
		values[22][1] = 2100;
		values[23][0] = 4044;
		values[23][1] = 2200;
		values[24][0] = 4400;
		values[24][1] = 2300;
		values[25][0] = 4691;
		values[25][1] = 2400;
		values[26][0] = 4989;
		values[26][1] = 2500;
		values[27][0] = 5289;
		values[27][1] = 2600;
		values[28][0] = 5583;
		values[28][1] = 2700;
		values[29][0] = 5879;
		values[29][1] = 2800;
		values[30][0] = 6075;
		values[30][1] = 2900;
		values[31][0] = 6332;
		values[31][1] = 3000;
		values[32][0] = 6521;
		values[32][1] = 3100;
		values[33][0] = 6724;
		values[33][1] = 3200;
		values[34][0] = 6929;
		values[34][1] = 3300;
		values[35][0] = 7132;
		values[35][1] = 3400;
		values[36][0] = 7356;
		values[36][1] = 3500;
		values[37][0] = 7561;
		values[37][1] = 3600;
		values[38][0] = 7774;
		values[38][1] = 3700;
		values[39][0] = 7992;
		values[39][1] = 3800;
		values[40][0] = 8200;
		values[40][1] = 3900;
		values[41][0] = 8410;
		values[41][1] = 4000;
		values[42][0] = 8626;
		values[42][1] = 4100;
		values[43][0] = 8849;
		values[43][1] = 4200;
		values[44][0] = 9060;
		values[44][1] = 4300;
		values[45][0] = 9271;
		values[45][1] = 4400;
		values[46][0] = 9531;
		values[46][1] = 4500;
		values[47][0] = 9748;
		values[47][1] = 4600;
		values[48][0] = 10210;
		values[48][1] = 4700;
		values[49][0] = 10219;
		values[49][1] = 4800;
		values[50][0] = 10429;
		values[50][1] = 4900;
		values[51][0] = 10649;
		values[51][1] = 5000;

	}
	uint32_t count = 52;
	uint32_t values[52][2];

};
constexpr auto ThermalTable = HakkoThermocoupleLookup();
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
	//Perform lookup on table of values to find the closest two measurement points, and then linearly interpolate these

	//This assumes results in the table are increasing order
	// TODO -> Should this be made into a binary search? Is it much faster??
	for (uint32_t i = 1; i < ThermalTable.count; i++) {
		if (((uint32_t) ThermalTable.values[i][0]) > tipuVDelta) {
			//Then extrapolate
			//Where i= the lower raw sample, i-1 is the higher raw sample
			return LinearInterpolate(						//
					ThermalTable.values[i][0],				// x1
					ThermalTable.values[i][1],				// y1
					ThermalTable.values[i - 1][0],			// x2
					ThermalTable.values[i - 1][1],			// y2
					tipuVDelta) / 10;			// raw sample to be interpolated

		}
	}
	return 500;						// fail high -> will turn off heater
}

uint32_t TipThermoModel::convertuVToDegF(uint32_t tipuVDelta) {
	//(Y °C × 9/5) + 32 =Y°F
	for (uint32_t i = 1; i < ThermalTable.count; i++) {
		if (((uint32_t) ThermalTable.values[i][0]) < tipuVDelta) {
			//Then extrapolate
			//Where i= the lower raw sample, i-1 is the higher raw sample
			return ((LinearInterpolate(						//
					ThermalTable.values[i][0],				// x1
					ThermalTable.values[i][1],				// y1
					ThermalTable.values[i - 1][0],			// x2
					ThermalTable.values[i - 1][1],			// y2
					tipuVDelta) 				// raw sample to be interpolated
			* 9) / 50) + 32;					// Convert C ->> F for 'mericans

		}
	}
	return 932;								// fail high -> will turn off heater
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

uint32_t TipThermoModel::getTipInC(bool sampleNow) {
	uint32_t currentTipTempInC = TipThermoModel::convertTipRawADCToDegC(
			getTipRawTemp(sampleNow));
	currentTipTempInC += getHandleTemperature() / 10; //Add handle offset
	return currentTipTempInC;
}

uint32_t TipThermoModel::getTipInF(bool sampleNow) {
	uint32_t currentTipTempInF = TipThermoModel::convertTipRawADCToDegF(
			getTipRawTemp(sampleNow));
	currentTipTempInF += convertCtoF(getHandleTemperature() / 10); //Add handle offset
	return currentTipTempInF;
}
