/*
 * I2CBB.cpp
 *
 *  Created on: 12 Jun 2020
 *      Author: Ralim
 */

#include <I2CBB.hpp>
#include "FreeRTOS.h"
#define SCL_HIGH() 	HAL_GPIO_WritePin(SCL2_GPIO_Port, SCL2_Pin, GPIO_PIN_SET)
#define SCL_LOW() 	HAL_GPIO_WritePin(SCL2_GPIO_Port, SCL2_Pin, GPIO_PIN_RESET)
#define SDA_HIGH() 	HAL_GPIO_WritePin(SDA2_GPIO_Port, SDA2_Pin, GPIO_PIN_SET)
#define SDA_LOW() 	HAL_GPIO_WritePin(SDA2_GPIO_Port, SDA2_Pin, GPIO_PIN_RESET)
#define SDA_READ()  (HAL_GPIO_ReadPin(SDA2_GPIO_Port,SDA2_Pin)==GPIO_PIN_SET?1:0)
#define SCL_READ()  (HAL_GPIO_ReadPin(SCL2_GPIO_Port,SCL2_Pin)==GPIO_PIN_SET?1:0)
#define I2C_DELAY() {for(int xx=0;xx<700;xx++){asm("nop");}}
SemaphoreHandle_t I2CBB::I2CSemaphore = NULL;
StaticSemaphore_t I2CBB::xSemaphoreBuffer;
void I2CBB::init() {
	//Set GPIO's to output open drain
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStruct.Pin = SDA2_Pin | SCL2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(SDA2_GPIO_Port, &GPIO_InitStruct);
	SDA_HIGH();
	SCL_HIGH();
	I2CSemaphore = xSemaphoreCreateBinaryStatic(&xSemaphoreBuffer);
	xSemaphoreGive(I2CSemaphore);
	unlock();
}

bool I2CBB::probe(uint8_t address) {
	start();
	bool ack = send(address);
	stop();
	return ack;
}

bool I2CBB::Mem_Read(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData,
		uint16_t Size) {
	if (!lock())
		return false;
	start();
	bool ack = send(DevAddress);
	if (!ack) {
		stop();
		unlock();
		return false;
	}
	ack = send(MemAddress);
	if (!ack) {
		stop();
		unlock();
		return false;
	}
	start();
	ack = send(DevAddress | 1);
	if (!ack) {
		stop();
		unlock();
		return false;
	}
	while (Size) {
		pData[0] = read(Size > 1);
		pData++;
		Size--;
	}
	stop();
	unlock();
	return true;
}

bool I2CBB::Mem_Write(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData,
		uint16_t Size) {
	if (!lock())
		return false;
	start();
	bool ack = send(DevAddress);
	if (!ack) {
		stop();
		asm("bkpt");
		unlock();
		return false;
	}
	ack = send(MemAddress);
	if (!ack) {
		stop();
		asm("bkpt");
		unlock();
		return false;
	}
	while (Size) {
		resetWatchdog();
		bool ack = send(pData[0]);
		if (!ack) {
			stop();
			asm("bkpt");
			unlock();
			return false;
		}
		pData++;
		Size--;
	}
	stop();
	unlock();
	return true;
}

void I2CBB::Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size) {
	if (!lock())
		return;
	start();
	bool ack = send(DevAddress);
	if (!ack) {
		stop();
		unlock();
		return;
	}
	while (Size) {
		bool ack = send(pData[0]);
		if (!ack) {
			stop();
			unlock();
			return;
		}
		pData++;
		Size--;
	}
	stop();
	unlock();

}

void I2CBB::Receive(uint16_t DevAddress, uint8_t *pData, uint16_t Size) {
	if (!lock())
		return;
	start();
	bool ack = send(DevAddress | 1);
	if (!ack) {
		stop();
		unlock();
		return;
	}
	while (Size) {
		pData[0] = read(Size > 1);
		pData++;
		Size--;
	}
	stop();
	unlock();
}

void I2CBB::TransmitReceive(uint16_t DevAddress, uint8_t *pData_tx,
		uint16_t Size_tx, uint8_t *pData_rx, uint16_t Size_rx) {
	if (Size_tx == 0 && Size_rx == 0)
		return;
	if (lock() == false)
		return;
	if (Size_tx) {
		start();
		bool ack = send(DevAddress);
		if (!ack) {
			stop();
			unlock();
			return;
		}
		while (Size_tx) {
			bool ack = send(pData_tx[0]);
			if (!ack) {
				stop();
				unlock();
				return;
			}
			pData_tx++;
			Size_tx--;
		}
	}
	if (Size_rx) {
		start();
		bool ack = send(DevAddress | 1);
		if (!ack) {
			stop();
			unlock();
			return;
		}
		while (Size_rx) {
			pData_rx[0] = read(Size_rx > 1);
			pData_rx++;
			Size_rx--;
		}
	}
	stop();
	unlock();
}

void I2CBB::start() {
	/* I2C Start condition, data line goes low when clock is high */
	SCL_HIGH();
	SDA_HIGH();
	I2C_DELAY();
	SDA_LOW();
	I2C_DELAY();
	SCL_LOW();
	I2C_DELAY();
}

void I2CBB::stop() {
	/* I2C Stop condition, clock goes high when data is low */
	SDA_LOW();
	I2C_DELAY();
	SCL_HIGH();
	I2C_DELAY();
	SDA_HIGH();
	I2C_DELAY();
}

bool I2CBB::send(uint8_t value) {

	for (uint8_t i = 0; i < 8; i++) {
		write_bit(value & 0x80);   // write the most-significant bit
		value <<= 1;
	}

	bool ack = read_bit() == 0;
	return ack;
}

uint8_t I2CBB::read(bool ack) {
	uint8_t B = 0;

	uint8_t i;
	for (i = 0; i < 8; i++) {
		B <<= 1;
		B |= read_bit();
	}

	if (ack)
		write_bit(0);
	else
		write_bit(1);
	return B;
}

uint8_t I2CBB::read_bit() {
	uint8_t b;

	SDA_HIGH();
	I2C_DELAY();
	SCL_HIGH();
	I2C_DELAY();

	if (SDA_READ())
		b = 1;
	else
		b = 0;

	SCL_LOW();
	return b;
}

void I2CBB::unlock() {
	xSemaphoreGive(I2CSemaphore);
}

bool I2CBB::lock() {
	if (I2CSemaphore == NULL) {
		asm("bkpt");
	}
	return xSemaphoreTake(I2CSemaphore, (TickType_t) 50) == pdTRUE;
}

void I2CBB::write_bit(uint8_t val) {
	if (val > 0)
		SDA_HIGH();
	else
		SDA_LOW();

	I2C_DELAY();
	SCL_HIGH();
	I2C_DELAY();
	SCL_LOW();
}
