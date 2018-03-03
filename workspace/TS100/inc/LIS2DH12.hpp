/*
 * LIS2DH12.hpp
 *
 *  Created on: 27Feb.,2018
 *      Author: Ralim
 */

#ifndef LIS2DH12_HPP_
#define LIS2DH12_HPP_
#include "stm32f1xx_hal.h"
#include "LIS2DH12_defines.hpp"
class LIS2DH12 {
public:
	LIS2DH12(I2C_HandleTypeDef* i2cHandle);
	void initalize();
	uint8_t getOrientation();
	void getAxisReadings(int16_t *x, int16_t *y, int16_t *z);

private:
	void setSensitivity(uint8_t threshold, uint8_t filterTime);	// Sets the sensitivity of the unit

	void I2C_RegisterWrite(uint8_t reg, uint8_t data);
	uint8_t I2C_RegisterRead(uint8_t reg);
	I2C_HandleTypeDef* i2c;
};

#endif /* LIS2DH12_HPP_ */
