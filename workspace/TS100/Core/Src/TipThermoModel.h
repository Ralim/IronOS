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
	//Returns the uV of the tip reading before the op-amp compensating for pullups
	static uint32_t convertTipRawADCTouV(uint16_t rawADC);
	static uint32_t convertTipRawADCToDegC(uint16_t rawADC);
	static uint32_t convertuVToDegC(uint32_t tipuVDelta);
};

#endif /* SRC_TIPTHERMOMODEL_H_ */
