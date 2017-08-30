/*
 * MMA8652FC.h
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef MMA8652FC_HPP_
#define MMA8652FC_HPP_
#include "stm32f1xx_hal.h"
#include "MMA8652FC_defines.h"
class MMA8652FC {

public:

	MMA8652FC(I2C_HandleTypeDef* i2cHandle);
	void initalize();		// Initalize the system
	bool getOrientation();// Reads the I2C register and returns the orientation (true == left)
	void getAxisReadings(int16_t *x, int16_t *y, int16_t *z);

private:
	void setSensitivity(uint8_t threshold, uint8_t filterTime);	// Sets the sensitivity of the unit

	void I2C_RegisterWrite(uint8_t reg, uint8_t data);
	uint8_t I2C_RegisterRead(uint8_t reg);
	I2C_HandleTypeDef* i2c;

};

#endif /* MMA8652FC_HPP_ */
