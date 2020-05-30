/*
 * FRToSI2C.hpp
 *
 *  Created on: 14Apr.,2018
 *      Author: Ralim
 */

#ifndef FRTOSI2C_HPP_
#define FRTOSI2C_HPP_

#include "cmsis_os.h"

/*
 * Wrapper class to work with the device I2C bus
 *
 * This provides mutex protection of the peripheral
 * Also allows hardware to use DMA should it want to
 *
 *
 */
class FRToSI2C {
public:

	static void init() {
		I2CSemaphore = nullptr;
	}

	static void FRToSInit() {
		I2CSemaphore = xSemaphoreCreateBinaryStatic(&xSemaphoreBuffer);
		xSemaphoreGive(I2CSemaphore);
	}

	static void CpltCallback(); //Normal Tx Callback

	static bool Mem_Read(uint16_t DevAddress, uint16_t MemAddress,
			uint8_t *pData, uint16_t Size);
	static void Mem_Write(uint16_t DevAddress, uint16_t MemAddress,
			uint8_t *pData, uint16_t Size);
	//Returns true if device ACK's being addressed
	static bool probe(uint16_t DevAddress);

	static void Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
	static void I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data);
	static uint8_t I2C_RegisterRead(uint8_t address, uint8_t reg);

private:
	static void I2C_Unstick();
	static SemaphoreHandle_t I2CSemaphore;
	static StaticSemaphore_t xSemaphoreBuffer;
};

#endif /* FRTOSI2C_HPP_ */
