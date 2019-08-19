/*
 * FRToSI2C.cpp
 *
 *  Created on: 14Apr.,2018
 *      Author: Ralim
 */
#include "hardware.h"
#include "FRToSI2C.hpp"
#define I2CUSESDMA
I2C_HandleTypeDef* FRToSI2C::i2c;
SemaphoreHandle_t FRToSI2C::I2CSemaphore;
StaticSemaphore_t FRToSI2C::xSemaphoreBuffer;

void FRToSI2C::CpltCallback() {
	i2c->State = HAL_I2C_STATE_READY;  // Force state reset (even if tx error)
	if (I2CSemaphore) {
		xSemaphoreGiveFromISR(I2CSemaphore, NULL);
	}
}

void FRToSI2C::Mem_Read(uint16_t DevAddress, uint16_t MemAddress,
		uint16_t MemAddSize, uint8_t* pData, uint16_t Size) {

	if (I2CSemaphore == NULL) {
		// no RToS, run blocking code
		HAL_I2C_Mem_Read(i2c, DevAddress, MemAddress, MemAddSize, pData, Size,
				5000);
	} else {
		// RToS is active, run threading
		// Get the mutex so we can use the I2C port
		// Wait up to 1 second for the mutex
		if (xSemaphoreTake(I2CSemaphore, (TickType_t)50) == pdTRUE) {
#ifdef I2CUSESDMA
			if (HAL_I2C_Mem_Read(i2c, DevAddress, MemAddress, MemAddSize, pData,
					Size, 500) != HAL_OK) {

				I2C1_ClearBusyFlagErratum();
				xSemaphoreGive(I2CSemaphore);
			}
			xSemaphoreGive(I2CSemaphore);
#else

			HAL_I2C_Mem_Read(i2c, DevAddress, MemAddress, MemAddSize, pData, Size,
					5000);
			xSemaphoreGive(I2CSemaphore);
#endif
		} else {
		}
	}

}
void FRToSI2C::I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data) {
	Mem_Write(address, reg, I2C_MEMADD_SIZE_8BIT, &data, 1);
}

uint8_t FRToSI2C::I2C_RegisterRead(uint8_t add, uint8_t reg) {
	uint8_t tx_data[1];
	Mem_Read(add, reg, I2C_MEMADD_SIZE_8BIT, tx_data, 1);
	return tx_data[0];
}
void FRToSI2C::Mem_Write(uint16_t DevAddress, uint16_t MemAddress,
		uint16_t MemAddSize, uint8_t* pData, uint16_t Size) {

	if (I2CSemaphore == NULL) {
		// no RToS, run blocking code
		HAL_I2C_Mem_Write(i2c, DevAddress, MemAddress, MemAddSize, pData, Size,
				5000);
	} else {
		// RToS is active, run threading
		// Get the mutex so we can use the I2C port
		// Wait up to 1 second for the mutex
		if (xSemaphoreTake(I2CSemaphore, (TickType_t)50) == pdTRUE) {
#ifdef I2CUSESDMA
			if (HAL_I2C_Mem_Write(i2c, DevAddress, MemAddress, MemAddSize,
					pData, Size, 500) != HAL_OK) {

				I2C1_ClearBusyFlagErratum();
				xSemaphoreGive(I2CSemaphore);
			}
			xSemaphoreGive(I2CSemaphore);
#else
			if (HAL_I2C_Mem_Write(i2c, DevAddress, MemAddress, MemAddSize, pData,
							Size, 5000) != HAL_OK) {
			}
			xSemaphoreGive(I2CSemaphore);
#endif
		} else {
		}
	}

}

void FRToSI2C::Transmit(uint16_t DevAddress, uint8_t* pData, uint16_t Size) {
	if (I2CSemaphore == NULL) {
		// no RToS, run blocking code
		HAL_I2C_Master_Transmit(i2c, DevAddress, pData, Size, 5000);
	} else {
		// RToS is active, run threading
		// Get the mutex so we can use the I2C port
		// Wait up to 1 second for the mutex
		if (xSemaphoreTake(I2CSemaphore, (TickType_t)50) == pdTRUE) {
#ifdef I2CUSESDMA

			if (HAL_I2C_Master_Transmit_DMA(i2c, DevAddress, pData, Size)
					!= HAL_OK) {

				I2C1_ClearBusyFlagErratum();
				xSemaphoreGive(I2CSemaphore);

			}
#else
			HAL_I2C_Master_Transmit(i2c, DevAddress, pData, Size, 5000);
			xSemaphoreGive(I2CSemaphore);
#endif

		} else {
		}
	}

}

void FRToSI2C::I2C1_ClearBusyFlagErratum() {
	GPIO_InitTypeDef GPIO_InitStruct;
	int timeout = 100;
	int timeout_cnt = 0;

	// 1. Clear PE bit.
	i2c->Instance->CR1 &= ~(0x0001);
	/**I2C1 GPIO Configuration
	 PB6     ------> I2C1_SCL
	 PB7     ------> I2C1_SDA
	 */
	//  2. Configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	GPIO_InitStruct.Pin = SCL_Pin;
	HAL_GPIO_Init(SCL_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = SDA_Pin;
	HAL_GPIO_Init(SDA_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET);

	while (GPIO_PIN_SET != HAL_GPIO_ReadPin(SDA_GPIO_Port, SDA_Pin)) {
		//Move clock to release I2C
		HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_RESET);
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET);

		timeout_cnt++;
		if (timeout_cnt > timeout)
			return;
	}

	// 12. Configure the SCL and SDA I/Os as Alternate function Open-Drain.
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	GPIO_InitStruct.Pin = SCL_Pin;
	HAL_GPIO_Init(SCL_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SDA_Pin;
	HAL_GPIO_Init(SDA_GPIO_Port, &GPIO_InitStruct);

	HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET);

	// 13. Set SWRST bit in I2Cx_CR1 register.
	i2c->Instance->CR1 |= 0x8000;

	asm("nop");

	// 14. Clear SWRST bit in I2Cx_CR1 register.
	i2c->Instance->CR1 &= ~0x8000;

	asm("nop");

	// 15. Enable the I2C peripheral by setting the PE bit in I2Cx_CR1 register
	i2c->Instance->CR1 |= 0x0001;

	// Call initialization function.
	HAL_I2C_Init(i2c);
}
