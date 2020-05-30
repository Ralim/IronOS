/*
 * FRToSI2C.cpp
 *
 *  Created on: 14Apr.,2018
 *      Author: Ralim
 */
#include <I2C_Wrapper.hpp>
#include "BSP.h"
#include "Setup.h"
#define I2CUSESDMA
SemaphoreHandle_t FRToSI2C::I2CSemaphore;
StaticSemaphore_t FRToSI2C::xSemaphoreBuffer;

void FRToSI2C::CpltCallback() {
	hi2c1.State = HAL_I2C_STATE_READY;  // Force state reset (even if tx error)
	if (I2CSemaphore) {
		xSemaphoreGiveFromISR(I2CSemaphore, NULL);
	}
}

bool FRToSI2C::Mem_Read(uint16_t DevAddress, uint16_t MemAddress,
		uint8_t *pData, uint16_t Size) {

	if (I2CSemaphore == NULL) {
		// no RToS, run blocking code
		HAL_I2C_Mem_Read(&hi2c1, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT,
				pData, Size, 5000);
		return true;
	} else {
		// RToS is active, run threading
		// Get the mutex so we can use the I2C port
		// Wait up to 1 second for the mutex
		if (xSemaphoreTake(I2CSemaphore, (TickType_t)500) == pdTRUE) {
#ifdef I2CUSESDMA
			if (HAL_I2C_Mem_Read(&hi2c1, DevAddress, MemAddress,
			I2C_MEMADD_SIZE_8BIT, pData, Size, 500) != HAL_OK) {

				I2C_Unstick();
				xSemaphoreGive(I2CSemaphore);
				return false;
			} else {
				xSemaphoreGive(I2CSemaphore);
				return true;
			}
#else

			if (HAL_I2C_Mem_Read(&hi2c1, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size,
					5000)==HAL_OK){
				xSemaphoreGive(I2CSemaphore);
				return true;
			}
			xSemaphoreGive(I2CSemaphore);
			return false;
#endif
		} else {
			return false;
		}
	}

}
void FRToSI2C::I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data) {
	Mem_Write(address, reg, &data, 1);
}

uint8_t FRToSI2C::I2C_RegisterRead(uint8_t add, uint8_t reg) {
	uint8_t tx_data[1];
	Mem_Read(add, reg, tx_data, 1);
	return tx_data[0];
}
void FRToSI2C::Mem_Write(uint16_t DevAddress, uint16_t MemAddress,
		uint8_t *pData, uint16_t Size) {

	if (I2CSemaphore == NULL) {
		// no RToS, run blocking code
		HAL_I2C_Mem_Write(&hi2c1, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT,
				pData, Size, 5000);
	} else {
		// RToS is active, run threading
		// Get the mutex so we can use the I2C port
		// Wait up to 1 second for the mutex
		if (xSemaphoreTake(I2CSemaphore, (TickType_t)500) == pdTRUE) {
			if (HAL_I2C_Mem_Write(&hi2c1, DevAddress, MemAddress,
			I2C_MEMADD_SIZE_8BIT, pData, Size, 500) != HAL_OK) {

				I2C_Unstick();
				xSemaphoreGive(I2CSemaphore);
			}
			xSemaphoreGive(I2CSemaphore);

		}
	}

}

void FRToSI2C::Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size) {
	if (I2CSemaphore == NULL) {
		// no RToS, run blocking code
		HAL_I2C_Master_Transmit(&hi2c1, DevAddress, pData, Size, 5000);
	} else {
		// RToS is active, run threading
		// Get the mutex so we can use the I2C port
		// Wait up to 1 second for the mutex
		if (xSemaphoreTake(I2CSemaphore, (TickType_t)50) == pdTRUE) {
#ifdef I2CUSESDMA

			if (HAL_I2C_Master_Transmit_DMA(&hi2c1, DevAddress, pData, Size)
					!= HAL_OK) {

				I2C_Unstick();
				xSemaphoreGive(I2CSemaphore);

			}
#else
			HAL_I2C_Master_Transmit(&hi2c1, DevAddress, pData, Size, 5000);
			xSemaphoreGive(I2CSemaphore);
#endif

		} else {
		}
	}

}

bool FRToSI2C::probe(uint16_t DevAddress) {
	uint8_t buffer[1];
	return HAL_I2C_Mem_Read(&hi2c1, DevAddress, 0x0F, I2C_MEMADD_SIZE_8BIT,
			buffer, 1, 1000) == HAL_OK;

}

void FRToSI2C::I2C_Unstick() {
	unstick_I2C();
}
