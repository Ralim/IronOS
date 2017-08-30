/*
 * MMA8652FC.cpp
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */

#include <MMA8652FC.hpp>

MMA8652FC::MMA8652FC(I2C_HandleTypeDef* i2cHandle) {
	i2c = i2cHandle;
}

void MMA8652FC::I2C_RegisterWrite(uint8_t reg, uint8_t data) {

	HAL_I2C_Mem_Write(i2c, MMA8652FC_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xFFFF);
}

uint8_t MMA8652FC::I2C_RegisterRead(uint8_t reg) {
	uint8_t tx_data[1];
	HAL_I2C_Mem_Read(i2c, MMA8652FC_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, tx_data, 1, 0xFFFF);

	return tx_data[0];
}
void MMA8652FC::initalize() {
	//send all the init commands to the unit
	I2C_RegisterWrite(CTRL_REG2, 0);    //Normal mode
	I2C_RegisterWrite( CTRL_REG2, 0x40);	// Reset all registers to POR values
	HAL_Delay(2);		// ~1ms delay
	I2C_RegisterWrite(FF_MT_CFG_REG, 0x78);    // Enable motion detection for X, Y, Z axis, latch disabled

	I2C_RegisterWrite(PL_CFG_REG, 0x40);	//Enable the orientation detection
	I2C_RegisterWrite(PL_COUNT_REG, 200);    //200 count debounce
	I2C_RegisterWrite(PL_BF_ZCOMP_REG, 0b01000111);    //Set the threshold to 42 degrees
	I2C_RegisterWrite(P_L_THS_REG, 0b10011100);    //Up the trip angles
	I2C_RegisterWrite( CTRL_REG4, 0x01 | (1 << 4));    // Enable dataready interrupt & orientation interrupt
	I2C_RegisterWrite( CTRL_REG5, 0x01);    // Route data ready interrupts to INT1 ->PB5 ->EXTI5, leaving orientation routed to INT2
	I2C_RegisterWrite( CTRL_REG2, 0x12);    //Set maximum resolution oversampling
	I2C_RegisterWrite( XYZ_DATA_CFG_REG, (1 << 4));    //select high pass filtered data
	I2C_RegisterWrite( HP_FILTER_CUTOFF_REG, 0x03);    //select high pass filtered data

	I2C_RegisterWrite( CTRL_REG1, 0x19);		// ODR=12 Hz, Active mode

}

void MMA8652FC::setSensitivity(uint8_t threshold, uint8_t filterTime) {
	uint8_t sens = 9 * 2 + 17;
	sens -= 2 * threshold;
	I2C_RegisterWrite( CTRL_REG1, 0);		//  sleep mode
	I2C_RegisterWrite(FF_MT_THS_REG, (sens & 0x7F));		// Set accumulation threshold
	I2C_RegisterWrite(FF_MT_COUNT_REG, filterTime);    // Set debounce threshold
	I2C_RegisterWrite( CTRL_REG1, 0x31);		// ODR=12 Hz, Active mode

}

bool MMA8652FC::getOrientation() {
	//First read the PL_STATUS register
	uint8_t plStatus = I2C_RegisterRead(PL_STATUS_REG);
	plStatus >>= 1;    //We don't need the up/down bit
	plStatus &= 0x03;    //mask to the two lower bits
	//0 == left handed
	//1 == right handed

	return !plStatus;
}
void MMA8652FC::getAxisReadings(int16_t *x, int16_t *y, int16_t *z) {
	uint8_t temp = 0;
	HAL_I2C_Mem_Read(i2c, MMA8652FC_I2C_ADDRESS, OUT_X_MSB_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) &temp, 1, 0xFFFF);
	(*x) = temp;
	(*x) <<= 8;
	HAL_I2C_Mem_Read(i2c, MMA8652FC_I2C_ADDRESS, OUT_X_LSB_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) &temp, 1, 0xFFFF);
	(*x) |= temp;

	HAL_I2C_Mem_Read(i2c, MMA8652FC_I2C_ADDRESS, OUT_Y_MSB_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) &temp, 1, 0xFFFF);
	(*y) = temp;
	(*y) <<= 8;
	HAL_I2C_Mem_Read(i2c, MMA8652FC_I2C_ADDRESS, OUT_Y_LSB_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) &temp, 1, 0xFFFF);
	(*y) |= temp;

	HAL_I2C_Mem_Read(i2c, MMA8652FC_I2C_ADDRESS, OUT_Z_MSB_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) &temp, 1, 0xFFFF);
	(*z) = temp;
	(*z) <<= 8;
	HAL_I2C_Mem_Read(i2c, MMA8652FC_I2C_ADDRESS, OUT_Z_LSB_REG, I2C_MEMADD_SIZE_8BIT, (uint8_t*) &temp, 1, 0xFFFF);
	(*z) |= temp;

}
