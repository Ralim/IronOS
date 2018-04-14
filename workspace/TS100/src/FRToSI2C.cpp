/*
 * FRToSI2C.cpp
 *
 *  Created on: 14Apr.,2018
 *      Author: Ralim
 */

#include "FRToSI2C.hpp"

FRToSI2C::FRToSI2C(I2C_HandleTypeDef* i2chandle) {
	i2c = i2chandle;

}

void FRToSI2C::MasterTxCpltCallback() {
	xSemaphoreGive(I2CSemaphore);
}

void FRToSI2C::MemRxCpltCallback() {
	xSemaphoreGive(I2CSemaphore);
}
void FRToSI2C::MemTxCpltCallback() {
	xSemaphoreGive(I2CSemaphore);
}

void FRToSI2C::Mem_Read(uint16_t DevAddress, uint16_t MemAddress,
		uint16_t MemAddSize, uint8_t* pData, uint16_t Size) {
	if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED
			|| RToSUP == false) {
		//no RToS, run blocking code
		HAL_I2C_Mem_Read(i2c, DevAddress, MemAddress, MemAddSize, pData, Size,
				5000);
	} else {
		//RToS is active, run threading
		//Get the mutex so we can use the I2C port
		//Wait up to 1 second for the mutex
		if ( xSemaphoreTake( I2CSemaphore, ( TickType_t ) 1000 ) == pdTRUE) {
			HAL_I2C_Mem_Read(i2c, DevAddress, MemAddress, MemAddSize, pData,
					Size, 5000);
			xSemaphoreGive(I2CSemaphore);

		}

	}

}

void FRToSI2C::Mem_Write(uint16_t DevAddress, uint16_t MemAddress,
		uint16_t MemAddSize, uint8_t* pData, uint16_t Size) {
	if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED
			|| RToSUP == false) {
		//no RToS, run blocking code
		HAL_I2C_Mem_Write(i2c, DevAddress, MemAddress, MemAddSize, pData, Size,
				5000);
	} else {
		//RToS is active, run threading
		//Get the mutex so we can use the I2C port
		//Wait up to 1 second for the mutex
		if ( xSemaphoreTake( I2CSemaphore, ( TickType_t ) 1000 ) == pdTRUE) {
			HAL_I2C_Mem_Write(i2c, DevAddress, MemAddress, MemAddSize, pData,
					Size, 5000);
			xSemaphoreGive(I2CSemaphore);

		}

	}

}

void FRToSI2C::FRToSInit() {
	I2CSemaphore = xSemaphoreCreateMutex();
	xSemaphoreGive(I2CSemaphore);
	RToSUP = true;
}

void FRToSI2C::Transmit(uint16_t DevAddress, uint8_t* pData, uint16_t Size) {
	if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED
			|| RToSUP == false) {
		//no RToS, run blocking code
		HAL_I2C_Master_Transmit(i2c, DevAddress, pData, Size, 5000);
	} else {
		//RToS is active, run threading
		//Get the mutex so we can use the I2C port
		//Wait up to 1 second for the mutex
		if ( xSemaphoreTake( I2CSemaphore, ( TickType_t ) 1000 ) == pdTRUE) {
			HAL_I2C_Master_Transmit(i2c, DevAddress, pData, Size, 5000);

			xSemaphoreGive(I2CSemaphore);

		}
	}

}
