/*
 * BMA223.cpp
 *
 *  Created on: 18 Sep. 2020
 *      Author: Ralim
 */

#include <BMA223.hpp>
#include <array>

bool BMA223::detect() {
	return FRToSI2C::probe(BMA223_ADDRESS);
}

static const FRToSI2C::I2C_REG i2c_registers[] = { //
		//
				{ BMA223_PMU_RANGE, 0b00000011, 0 },    //2G range
				{ BMA223_PMU_BW, 0b00001101, 0 },    //250Hz filter
				{ BMA223_PMU_LPW, 0b00000000, 0 },    //Full power
				{ BMA223_ACCD_HBW, 0b00000000, 0 },    //filtered data out
				{ BMA223_INT_OUT_CTRL, 0b00001010, 0 },    //interrupt active low and OD to get it hi-z
				{ BMA223_INT_RST_LATCH, 0b10000000, 0 },    //interrupt active low and OD to get it hi-z
				{ BMA223_INT_EN_0, 0b01000000, 0 },    //Enable orientation
				{ BMA223_INT_A, 0b00100111, 0 },    //Setup orientation detection

		//
		};
bool BMA223::initalize() {
	//Setup acceleration readings
	//2G range
	//bandwidth = 250Hz
	//High pass filter on (Slow compensation)
	//Turn off IRQ output pins
	//Orientation recognition in symmetrical mode
	// Hysteresis is set to ~ 16 counts
	//Theta blocking is set to 0b10

	return FRToSI2C::writeRegistersBulk(BMA223_ADDRESS, i2c_registers, sizeof(i2c_registers) / sizeof(i2c_registers[0]));

}

void BMA223::getAxisReadings(int16_t& x, int16_t& y, int16_t& z) {
	//The BMA is odd in that its output data width is only 8 bits
	//And yet there are MSB and LSB registers _sigh_.
	uint8_t sensorData[6] = { 0, 0, 0, 0, 0, 0 };

	if (FRToSI2C::Mem_Read(BMA223_ADDRESS, BMA223_ACCD_X_LSB, sensorData, 6) == false) {
		x = y = z = 0;
		return;
	}

	x = sensorData[1] << 5;
	y = sensorData[3] << 5;
	z = sensorData[5] << 5;

}
