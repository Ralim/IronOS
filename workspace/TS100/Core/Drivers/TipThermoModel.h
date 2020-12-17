/*
 * TipThermoModel.h
 *
 *  Created on: 7 Oct 2019
 *      Author: ralim
 */

#ifndef SRC_TIPTHERMOMODEL_H_
#define SRC_TIPTHERMOMODEL_H_
#include "stdint.h"
#include "BSP.h"
#include "unit.h"
class TipThermoModel {
public:
	//These are the main two functions
	static uint32_t getTipInC(bool sampleNow = false);
#ifdef ENABLED_FAHRENHEIT_SUPPORT
	static uint32_t getTipInF(bool sampleNow = false);
#endif

	//Calculates the maximum temperature can can be read by the ADC range
	static uint32_t getTipMaxInC();

	static uint32_t convertTipRawADCToDegC(uint16_t rawADC);
#ifdef ENABLED_FAHRENHEIT_SUPPORT
	static uint32_t convertTipRawADCToDegF(uint16_t rawADC);
#endif
	//Returns the uV of the tip reading before the op-amp compensating for pullups
	static uint32_t convertTipRawADCTouV(uint16_t rawADC);
#ifdef ENABLED_FAHRENHEIT_SUPPORT
	static uint32_t convertCtoF(uint32_t degC);
	static uint32_t convertFtoC(uint32_t degF);
#endif

private:
	static uint32_t convertuVToDegC(uint32_t tipuVDelta);
#ifdef ENABLED_FAHRENHEIT_SUPPORT
	static uint32_t convertuVToDegF(uint32_t tipuVDelta);
#endif
};

#endif /* SRC_TIPTHERMOMODEL_H_ */
