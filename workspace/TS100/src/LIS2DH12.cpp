/*
 * LIS2DH12.cpp
 *
 *  Created on: 27Feb.,2018
 *      Author: Ralim
 */

#include <LIS2DH12.hpp>
#include "cmsis_os.h"
LIS2DH12::LIS2DH12(I2C_HandleTypeDef* i2cHandle) {
	i2c = i2cHandle;
}

void LIS2DH12::initalize() {
	I2C_RegisterWrite(LIS_CTRL_REG1, 0x17); //25Hz
	I2C_RegisterWrite(LIS_CTRL_REG2, 0b00001000); //Highpass filter off
	I2C_RegisterWrite(LIS_CTRL_REG3, 0b01100000); //Setup interrupt pins
	I2C_RegisterWrite(LIS_CTRL_REG4, 0b00001000); //Block update mode off,HR on
	I2C_RegisterWrite(LIS_CTRL_REG5, 0b00000010);
	I2C_RegisterWrite(LIS_CTRL_REG6, 0b01100010);

	//Basically setup the unit to run, and enable 4D orientation detection
	I2C_RegisterWrite(LIS_INT2_CFG, 0b01111110); //setup for movement detection
	I2C_RegisterWrite(LIS_INT2_THS, 0x28);
	I2C_RegisterWrite(LIS_INT2_DURATION, 64);
	I2C_RegisterWrite(LIS_INT1_CFG, 0b01111110); //setup for movement detection
	I2C_RegisterWrite(LIS_INT1_THS, 0x28);
	I2C_RegisterWrite(LIS_INT1_DURATION, 64);

}

//0=no change, 1= right handed, 2= left handed
uint8_t LIS2DH12::getOrientation() {
	// 8=right handed,4=left,16=flat
	//So we ignore if not 8/4
	taskENTER_CRITICAL();
	uint8_t pos = I2C_RegisterRead(LIS_INT2_SRC);
	taskEXIT_CRITICAL();
	if (pos == 8)
		return 1;
	else if (pos == 4)
		return 2;
	else
		return 0;
}

void LIS2DH12::getAxisReadings(int16_t* x, int16_t* y, int16_t* z) {
	uint8_t tempArr[6];
	taskENTER_CRITICAL();
	while (HAL_I2C_Mem_Read(i2c, LIS2DH_I2C_ADDRESS, 0xA8,
	I2C_MEMADD_SIZE_8BIT, (uint8_t*) tempArr, 6, 5000) != HAL_OK) {
		HAL_Delay(5);
	}
	taskEXIT_CRITICAL();
	(*x) = ((uint16_t) (tempArr[1] << 8 | tempArr[0]));
	(*y) = ((uint16_t) (tempArr[3] << 8 | tempArr[2]));
	(*z) = ((uint16_t) (tempArr[5] << 8 | tempArr[4]));
}

void LIS2DH12::setSensitivity(uint8_t threshold, uint8_t filterTime) {
}

void LIS2DH12::I2C_RegisterWrite(uint8_t reg, uint8_t data) {
	HAL_I2C_Mem_Write(i2c, LIS2DH_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &data,
			1, 500);
}

uint8_t LIS2DH12::I2C_RegisterRead(uint8_t reg) {
	uint8_t tx_data[1];
	HAL_I2C_Mem_Read(i2c, LIS2DH_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT,
			tx_data, 1, 500);
	return tx_data[0];
}
