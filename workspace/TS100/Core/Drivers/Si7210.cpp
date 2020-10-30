/*
 * Si7210.cpp
 *
 *  Created on: 5 Oct. 2020
 *      Author: Ralim
 */

#include <Si7210.h>
#include "Si7210_defines.h"
#include "I2C_Wrapper.hpp"
bool Si7210::detect() {
	uint8_t temp;
	return FRToSI2C::Mem_Read(SI7210_ADDRESS, SI7210_REG_ID, &temp, 1);
	//Cant use normal probe as reg 0x00 is not used
}

bool Si7210::init() {
	//Turn on auto increment and sanity check ID
	uint8_t temp;
	if (FRToSI2C::Mem_Read(SI7210_ADDRESS, SI7210_REG_ID, &temp, 1)) {
		// We don't really care what model it is etc, just probing to check its probably this iC
		if (temp != 0x00 && temp != 0xFF) {
			temp = 0x01; //turn on auto increment
			if (FRToSI2C::Mem_Write(SI7210_ADDRESS, SI7210_REG_INCR, &temp, 1)) {
				return true;
			}
		}
	}
	return false;
}

int16_t Si7210::read() {
	//Read the two regs
	int16_t temp = 0;
	FRToSI2C::Mem_Read(SI7210_ADDRESS, SI7210_REG_DATAH, (uint8_t*) &temp, 2);
	return __builtin_bswap16(temp);
}
