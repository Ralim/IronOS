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
	u8 tx_data[1];
	I2C_Master_Read(DEVICE_ADDR << 1, reg, tx_data, 1);
	return tx_data[0];
}

uint8_t getOrientation() {
//First read the PL_STATUS register
	uint8_t plStatus = I2C_RegisterRead(PL_STATUS_REG);
	plStatus >>= 1; //We dont need the up/down bit
	plStatus &= 0x03; //mask to the two lower bits
	//0 == left handed
	//1 == right handed

	return plStatus;
}

void StartUp_Accelerometer(uint8_t sensitivity) {
	I2C_RegisterWrite(CTRL_REG2, 0); //Normal mode
	I2C_RegisterWrite( CTRL_REG2, 0x40);	// Reset all registers to POR values
	delayMs(2);		// ~1ms delay
	I2C_RegisterWrite(FF_MT_CFG_REG, 0x78);	// Enable motion detection for X and Y axis, latch enabled
	uint8_t sens = 9 * 6 + 5;
	sens -= 6 * sensitivity;

	I2C_RegisterWrite(FF_MT_THS_REG, 0x80 | sens);		// Set threshold
	I2C_RegisterWrite(FF_MT_COUNT_REG, 0x02);	// Set debounce to 100ms
	I2C_RegisterWrite(PL_CFG_REG, 0x40);	//Enable the orientation detection
	I2C_RegisterWrite(PL_COUNT_REG, 200);	//200 count debounce
	I2C_RegisterWrite(PL_BF_ZCOMP_REG, 0b01000111);	//Set the threshold to 42 degrees
	I2C_RegisterWrite(P_L_THS_REG, 0b10011100);	//Up the trip angles
	I2C_RegisterWrite( CTRL_REG4, 0x04 | (1<<4));	// Enable motion interrupt & orientation interrupt
	I2C_RegisterWrite( CTRL_REG5, 0x04);// Route motion interrupts to INT1 ->PB5 ->EXTI5, leaving orientation routed to INT2
	I2C_RegisterWrite( CTRL_REG1, 0x11);		// ODR=800 Hz, Active mode

}
