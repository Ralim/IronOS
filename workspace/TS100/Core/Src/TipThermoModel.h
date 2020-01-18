/*
 * TipThermoModel.h
 *
 *  Created on: 7 Oct 2019
 *      Author: ralim
 */

#ifndef SRC_TIPTHERMOMODEL_H_
#define SRC_TIPTHERMOMODEL_H_
#include "stdint.h"
#include "hardware.h"
class TipThermoModel {
public:
	//These are the main two functions
	static uint32_t getTipInC(bool sampleNow = false);
	static uint32_t getTipInF(bool sampleNow = false);

	//Calculates the maximum temperature can can be read by the ADC range
	static uint32_t getTipMaxInC();

	static uint32_t convertTipRawADCToDegC(uint16_t rawADC);
	static uint32_t convertTipRawADCToDegF(uint16_t rawADC);
	//Returns the uV of the tip reading before the op-amp compensating for pullups
	static uint32_t convertTipRawADCTouV(uint16_t rawADC);
	static uint32_t convertCtoF(uint32_t degC);
	static uint32_t convertFtoC(uint32_t degF);

private:
	static uint32_t convertuVToDegC(uint32_t tipuVDelta);
	static uint32_t convertuVToDegF(uint32_t tipuVDelta);
};

#endif /* SRC_TIPTHERMOMODEL_H_ */
