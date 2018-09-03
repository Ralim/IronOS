/*
 * LIS2DH12.hpp
 *
 *  Created on: 27Feb.,2018
 *      Author: Ralim
 */

#ifndef LIS2DH12_HPP_
#define LIS2DH12_HPP_
#include "stm32f1xx_hal.h"
#include "FRToSI2C.hpp"
#include "LIS2DH12_defines.hpp"
#include "hardware.h"

class LIS2DH12 {
public:
	static void initalize();
	static Orientation getOrientation() { return static_cast<Orientation>((FRToSI2C::I2C_RegisterRead(LIS2DH_I2C_ADDRESS,LIS_INT2_SRC) >> 2) - 1); }
	static void getAxisReadings(int16_t *x, int16_t *y, int16_t *z);

private:
};

#endif /* LIS2DH12_HPP_ */
