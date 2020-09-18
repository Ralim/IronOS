/*
 * BMA223.cpp
 *
 *  Created on: 18 Sep. 2020
 *      Author: Ralim
 */

#include <BMA223.hpp>
#include "BMA223_defines.h"
#include <array>
#define BMA223_ADDRESS 0b00110000

bool BMA223::detect() {
	return FRToSI2C::probe(BMA223_ADDRESS);
}

static const FRToSI2C::I2C_REG i2c_registers[] = { //
		//
				{ BMA223_PMU_RANGE, 0b0011, 0 },    //2G range
				{ BMA223_PMU_BW, 0b1101, 0 },    //250Hz filter
				{ BMA223_PMU_LPW, 0x00, 0 },    //Full power
				{ BMA223_ACCD_HBW, 0b01000000, 0 },    //filtered data out
				{ BMA223_INT_OUT_CTRL, 0b1111, 0 },    //interrupt active high and OD to get it hi-z
				{ BMA223_OFC_CTRL, 0b00000111, 0 },    //High pass en

		//
		};
void BMA223::initalize() {
	//Setup acceleration readings
	//2G range
	//bandwidth = 250Hz
	//High pass filter on (Slow compensation)
	//Turn off IRQ output pins
	//Orientation recognition in symmetrical mode
	// Hysteresis is set to ~ 16 counts
	//Theta blocking is set to 0b10

	FRToSI2C::writeRegistersBulk(BMA223_ADDRESS, i2c_registers, sizeof(i2c_registers) / sizeof(i2c_registers[0]));

}

void BMA223::getAxisReadings(int16_t& x, int16_t& y, int16_t& z) {
	//The BMA is odd in that its output data width is only 8 bits
	//And yet there are MSB and LSB registers _sigh_.
	uint8_t sensorData[6];

	FRToSI2C::Mem_Read(BMA223_ADDRESS, BMA223_ACCD_X_LSB, sensorData, 6);

	x = sensorData[1] << 2;
	y = sensorData[3] << 2;
	z = sensorData[5] << 2;

}
