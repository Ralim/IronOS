/*
 * FRToSI2C.hpp
 *
 *  Created on: 14Apr.,2018
 *      Author: Ralim
 */

#ifndef FRTOSI2C_HPP_
#define FRTOSI2C_HPP_
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

class FRToSI2C {
public:

	static void init(I2C_HandleTypeDef *i2chandle) {
		i2c = i2chandle;
		I2CSemaphore = nullptr;
	}

	static void FRToSInit() {
		I2CSemaphore = xSemaphoreCreateBinaryStatic(&xSemaphoreBuffer);
		xSemaphoreGive(I2CSemaphore);
	}

	static void CpltCallback(); //Normal Tx Callback

	static void Mem_Read(uint16_t DevAddress, uint16_t MemAddress,
			uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
	static void Mem_Write(uint16_t DevAddress, uint16_t MemAddress,
			uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

	static void Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
	static void I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data);
	static uint8_t I2C_RegisterRead(uint8_t address, uint8_t reg);

private:
	static I2C_HandleTypeDef *i2c;
	static void I2C1_ClearBusyFlagErratum();
	static SemaphoreHandle_t I2CSemaphore;
	static StaticSemaphore_t xSemaphoreBuffer;
};

#endif /* FRTOSI2C_HPP_ */
