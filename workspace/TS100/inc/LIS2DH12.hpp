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
	LIS2DH12(FRToSI2C* i2cHandle) : i2c(i2cHandle) {}
	void initalize();
	Orientation getOrientation() { return static_cast<Orientation>((I2C_RegisterRead(LIS_INT2_SRC) >> 2) - 1); }
	void getAxisReadings(int16_t *x, int16_t *y, int16_t *z);

private:
	void I2C_RegisterWrite(uint8_t reg, uint8_t data);
	uint8_t I2C_RegisterRead(uint8_t reg);
	FRToSI2C* i2c;
};

#endif /* LIS2DH12_HPP_ */
