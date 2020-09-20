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

	static void FRToSInit() {
		if (I2CSemaphore == nullptr) {
			I2CSemaphore = xSemaphoreCreateBinaryStatic(&xSemaphoreBuffer);
			xSemaphoreGive(I2CSemaphore);
		}
	}

	static void CpltCallback(); //Normal Tx Callback

	static bool Mem_Read(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size);
	static bool Mem_Write(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size);
	//Returns true if device ACK's being addressed
	static bool probe(uint16_t DevAddress);

	static bool Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
	static void Receive(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
	static void TransmitReceive(uint16_t DevAddress, uint8_t *pData_tx, uint16_t Size_tx, uint8_t *pData_rx, uint16_t Size_rx);
	static bool I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data);
	static uint8_t I2C_RegisterRead(uint8_t address, uint8_t reg);

	typedef struct {
		const uint8_t reg; // The register to write to
		uint8_t val; // The value to write to this register
		const uint8_t pause_ms; //How many ms to pause _after_ writing this reg
	} I2C_REG;
	static bool writeRegistersBulk(const uint8_t address, const I2C_REG* registers, const uint8_t registersLength);
private:
	static void unlock();
	static bool lock();
	static void I2C_Unstick();
	static SemaphoreHandle_t I2CSemaphore;
	static StaticSemaphore_t xSemaphoreBuffer;
};

#endif /* FRTOSI2C_HPP_ */
