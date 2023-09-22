/*
 * TipThermoModel.h
 *
 *  Created on: 7 Oct 2019
 *      Author: ralim
 */

#ifndef SRC_TIPTHERMOMODEL_H_
#define SRC_TIPTHERMOMODEL_H_
#include "BSP.h"
#include "Types.h"
#include "stdint.h"
class TipThermoModel {
public:
  // These are the main two functions
  static TemperatureType_t getTipInC(bool sampleNow = false);
  static TemperatureType_t getTipInF(bool sampleNow = false);

  // Calculates the maximum temperature can can be read by the ADC range
  static TemperatureType_t getTipMaxInC();

  static TemperatureType_t convertTipRawADCToDegC(uint16_t rawADC);
  static TemperatureType_t convertTipRawADCToDegF(uint16_t rawADC);
  // Returns the uV of the tip reading before the op-amp compensating for pullups
  static uint32_t          convertTipRawADCTouV(uint16_t rawADC, bool skipCalOffset = false);
  static TemperatureType_t convertCtoF(TemperatureType_t degC);
  static TemperatureType_t convertFtoC(TemperatureType_t degF);

private:
  static TemperatureType_t convertuVToDegC(uint32_t tipuVDelta);
  static TemperatureType_t convertuVToDegF(uint32_t tipuVDelta);
};

#endif /* SRC_TIPTHERMOMODEL_H_ */
