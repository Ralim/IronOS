/*
 * MMA8652FC.cpp
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */

#include <array>

#include "MMA8652FC.hpp"
#include "cmsis_os.h"

typedef struct {
	const uint8_t reg;
	const uint8_t val;
} MMA_REG;

static const MMA_REG i2c_registers[] = { { CTRL_REG2, 0 },    //Normal mode
		{ CTRL_REG2, 0x40 },      // Reset all registers to POR values
		{ FF_MT_CFG_REG, 0x78 }, // Enable motion detection for X, Y, Z axis, latch disabled
		{ PL_CFG_REG, 0x40 },     //Enable the orientation detection
		{ PL_COUNT_REG, 200 },    //200 count debounce
		{ PL_BF_ZCOMP_REG, 0b01000111 }, //Set the threshold to 42 degrees
		{ P_L_THS_REG, 0b10011100 },    //Up the trip angles
		{ CTRL_REG4, 0x01 | (1 << 4) }, // Enable dataready interrupt & orientation interrupt
		{ CTRL_REG5, 0x01 }, // Route data ready interrupts to INT1 ->PB5 ->EXTI5, leaving orientation routed to INT2
		{ CTRL_REG2, 0x12 },   //Set maximum resolution oversampling
		{ XYZ_DATA_CFG_REG, (1 << 4) }, //select high pass filtered data
		{ HP_FILTER_CUTOFF_REG, 0x03 }, //select high pass filtered data
		{ CTRL_REG1, 0x19 } // ODR=12 Hz, Active mode
};

void MMA8652FC::initalize() {
	size_t index = 0;

	//send all the init commands to the unit

	FRToSI2C::I2C_RegisterWrite(MMA8652FC_I2C_ADDRESS, i2c_registers[index].reg,
			i2c_registers[index].val);
	index++;
	FRToSI2C::I2C_RegisterWrite(MMA8652FC_I2C_ADDRESS, i2c_registers[index].reg,
			i2c_registers[index].val);
	index++;

	delay_ms(2);		// ~1ms delay

	while (index < (sizeof(i2c_registers) / sizeof(i2c_registers[0]))) {
		FRToSI2C::I2C_RegisterWrite(MMA8652FC_I2C_ADDRESS,
				i2c_registers[index].reg, i2c_registers[index].val);
		index++;
	}
}

Orientation MMA8652FC::getOrientation() {
	//First read the PL_STATUS register
	uint8_t plStatus = FRToSI2C::I2C_RegisterRead(MMA8652FC_I2C_ADDRESS,
	PL_STATUS_REG);
	if ((plStatus & 0b10000000) == 0b10000000) {
		plStatus >>= 1;    //We don't need the up/down bit
		plStatus &= 0x03;    //mask to the two lower bits

		//0 == left handed
		//1 == right handed

		return static_cast<Orientation>(plStatus);
	}

	return ORIENTATION_FLAT;
}

void MMA8652FC::getAxisReadings(int16_t &x, int16_t &y, int16_t &z) {
	std::array<int16_t, 3> sensorData;

	FRToSI2C::Mem_Read(MMA8652FC_I2C_ADDRESS, OUT_X_MSB_REG,
			reinterpret_cast<uint8_t*>(sensorData.begin()),
			sensorData.size() * sizeof(int16_t));

	x = static_cast<int16_t>(__builtin_bswap16(
			*reinterpret_cast<uint16_t*>(&sensorData[0])));
	y = static_cast<int16_t>(__builtin_bswap16(
			*reinterpret_cast<uint16_t*>(&sensorData[1])));
	z = static_cast<int16_t>(__builtin_bswap16(
			*reinterpret_cast<uint16_t*>(&sensorData[2])));
}

bool MMA8652FC::detect() {
	return FRToSI2C::probe(MMA8652FC_I2C_ADDRESS);
}
