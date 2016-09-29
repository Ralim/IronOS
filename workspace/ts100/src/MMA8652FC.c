/*
 * MMA8652FC.*
 * Files for the built in accelerometer from NXP.
 * This sets the unit up in motion detection mode with an interrupt on movement
 * This interrupt is fed to PB5 which catches it via EXTI5
 *
 * http://cache.freescale.com/files/sensors/doc/data_sheet/MMA8652FC.pdf
 *
 * EXTI Motion config setup values lifted from AN4070from NXP
 *
 * Ben V. Brown - <ralim@ralimtek.com>
 */
#include <stdio.h>
#include "Bios.h"
#include "MMA8652FC.h"
#include "I2C.h"

void I2C_RegisterWrite(uint8_t reg, uint8_t data) {
	u8 tx_data[2];
	tx_data[0] = reg;
	tx_data[1] = data;
	I2C_PageWrite(tx_data, 2, DEVICE_ADDR);
}

uint8_t I2C_RegisterRead(uint8_t reg) {
	u8 tx_data[3];
	tx_data[0] = reg;
	I2C_PageRead(tx_data, 1, DEVICE_ADDR, reg);
	return tx_data[0];
}

void StartUp_Accelerometer(void) {
	I2C_RegisterWrite(CTRL_REG2, 0); //Normal mode
	I2C_RegisterWrite( CTRL_REG2, 0x40);	// Reset all registers to POR values
	delayMs(2);		// ~1ms delay
	I2C_RegisterWrite(FF_MT_CFG_REG, 0x78);	// Enable motion detection for X and Y axis, latch enabled
	I2C_RegisterWrite(FF_MT_THS_REG, 0x0F);		// Set threshold
	I2C_RegisterWrite(FF_MT_COUNT_REG, 0x01);	// Set debounce to 100ms

	I2C_RegisterWrite( CTRL_REG4, 0x04);		// Enable motion interrupt
	I2C_RegisterWrite( CTRL_REG5, 0x04);// Route motion interrupts to INT1 ->PB5 ->EXTI
	I2C_RegisterWrite( CTRL_REG1, 0x19);		// ODR=100 Hz, Active mode
}
