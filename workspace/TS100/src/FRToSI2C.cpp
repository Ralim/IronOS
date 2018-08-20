/*
 * FRToSI2C.cpp
 *
 *  Created on: 14Apr.,2018
 *      Author: Ralim
 */

#include "FRToSI2C.hpp"

void __attribute__ ((long_call, section (".data.ramfuncs"))) FRToSI2C::CpltCallback() {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	i2c->State = HAL_I2C_STATE_READY;//Force state reset
	if (I2CSemaphore) {
		xSemaphoreGiveFromISR(I2CSemaphore, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

void FRToSI2C::Mem_Read(uint16_t DevAddress, uint16_t MemAddress,
		uint16_t MemAddSize, uint8_t* pData, uint16_t Size) {
	if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED
			|| I2CSemaphore == NULL) {
		//no RToS, run blocking code
		HAL_I2C_Mem_Read(i2c, DevAddress, MemAddress, MemAddSize, pData, Size,
				5000);
	} else {
		//RToS is active, run threading
		//Get the mutex so we can use the I2C port
		//Wait up to 1 second for the mutex
		if ( xSemaphoreTake( I2CSemaphore, ( TickType_t ) 5000 ) == pdTRUE) {
			if (HAL_I2C_Mem_Read(i2c, DevAddress, MemAddress, MemAddSize, pData,
					Size, 5000) != HAL_OK) {
				NVIC_SystemReset();
			}
			xSemaphoreGive(I2CSemaphore);
		} else {
			NVIC_SystemReset();
		}

	}

}

void FRToSI2C::Mem_Write(uint16_t DevAddress, uint16_t MemAddress,
		uint16_t MemAddSize, uint8_t* pData, uint16_t Size) {
	if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED
			|| I2CSemaphore == NULL) {
		//no RToS, run blocking code
		HAL_I2C_Mem_Write(i2c, DevAddress, MemAddress, MemAddSize, pData, Size,
				5000);
	} else {
		//RToS is active, run threading
		//Get the mutex so we can use the I2C port
		//Wait up to 1 second for the mutex
		if ( xSemaphoreTake( I2CSemaphore, ( TickType_t ) 5000 ) == pdTRUE) {
			if (HAL_I2C_Mem_Write(i2c, DevAddress, MemAddress, MemAddSize,
					pData, Size, 5000) != HAL_OK) {
				NVIC_SystemReset();
			}
			xSemaphoreGive(I2CSemaphore);

		} else {
			NVIC_SystemReset();
		}
	}
}

void FRToSI2C::Transmit(uint16_t DevAddress, uint8_t* pData, uint16_t Size) {
	if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED
			|| I2CSemaphore == NULL) {
		//no RToS, run blocking code
		HAL_I2C_Master_Transmit(i2c, DevAddress, pData, Size, 5000);
	} else {
		//RToS is active, run threading
		//Get the mutex so we can use the I2C port
		//Wait up to 1 second for the mutex
		if ( xSemaphoreTake( I2CSemaphore, ( TickType_t ) 5000 ) == pdTRUE) {
			if (HAL_I2C_Master_Transmit_DMA(i2c, DevAddress, pData, Size)
					!= HAL_OK) {
				NVIC_SystemReset();
			}
			//xSemaphoreGive(I2CSemaphore);
		} else {
			NVIC_SystemReset();
		}
	}

}
