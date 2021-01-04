/*
 * MSA301.cpp
 *
 *  Created on: 3 Jan 2021
 *      Author: Ralim
 */

#include <MSA301.h>
#include "MSA301_defines.h"
#define MSA301_I2C_ADDRESS 0x4C
bool MSA301::detect() {
	return FRToSI2C::probe(MSA301_I2C_ADDRESS);
}

static const FRToSI2C::I2C_REG i2c_registers[] = { //
		//
				{ MSA301_REG_ODR, 0b00001000, 1 },    //X/Y/Z enabled @ 250Hz
				{ MSA301_REG_POWERMODE, 0b0001001, 1 },    // Normal mode
				{ MSA301_REG_RESRANGE, 0b00000001, 0 },    // 14bit resolution @ 4G range
				{ MSA301_REG_ORIENT_HY, 0b01000000, 0 },    // 4*62.5mg hyst, no blocking, symmetrical
				{ MSA301_REG_ORIENT_HY, 0b01000000, 0 },    // 4*62.5mg hyst, no blocking, symmetrical

		};

bool MSA301::initalize() {
	// Enable X/Y/Z
	// Normal mode
	// 250Hz filter
	// 4G range
	// 14 bit resolution

	return FRToSI2C::writeRegistersBulk(MSA301_I2C_ADDRESS, i2c_registers, sizeof(i2c_registers) / sizeof(i2c_registers[0]));
}

Orientation MSA301::getOrientation() {
	//read MSA301_REG_ORIENT_STATUS
}

void MSA301::getAxisReadings(int16_t &x, int16_t &y, int16_t &z) {
}
