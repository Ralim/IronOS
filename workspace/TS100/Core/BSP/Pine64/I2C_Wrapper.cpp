/*
 * FRToSI2C.cpp
 *
 *  Created on: 14Apr.,2018
 *      Author: Ralim
 */
#include "BSP.h"
#include "Setup.h"
#include "IRQ.h"
#include <I2C_Wrapper.hpp>
SemaphoreHandle_t FRToSI2C::I2CSemaphore = nullptr;
StaticSemaphore_t FRToSI2C::xSemaphoreBuffer;
#define FLAG_TIMEOUT 1000
void FRToSI2C::CpltCallback() {
	//TODO
}

bool FRToSI2C::I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data) {
	return Mem_Write(address, reg, &data, 1);
}

uint8_t FRToSI2C::I2C_RegisterRead(uint8_t add, uint8_t reg) {
	uint8_t temp = 0;
	Mem_Read(add, reg, &temp, 1);
	return temp;
}

bool FRToSI2C::Mem_Read(uint16_t DevAddress, uint16_t read_address, uint8_t *p_buffer, uint16_t number_of_byte) {
	if (!lock())
		return false;
	i2c_interrupt_disable(I2C0, I2C_INT_ERR);
	i2c_interrupt_disable(I2C0, I2C_INT_BUF);
	i2c_interrupt_disable(I2C0, I2C_INT_EV);
	i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_AERR);
	i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_SMBALT);
	i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_SMBTO);
	i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_OUERR);
	i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_LOSTARB);
	i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_BERR);
	i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_PECERR);
	/* wait until I2C bus is idle */
	uint8_t timeout = 0;
	while (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY)) {
		timeout++;
		osDelay(1);
		if (timeout > 20) {
			unlock();
			return false;
		}
	}
	i2c_slave_address = DevAddress;
	i2c_read = p_buffer;
	i2c_read_dress = read_address;
	i2c_nbytes = number_of_byte;
	i2c_error_code = 0;
	i2c_process_flag = 1;
	i2c_write_process = I2C_SEND_ADDRESS_FIRST;
	i2c_read_process = I2C_SEND_ADDRESS_FIRST;

//	if (2 == number_of_byte) {
//		i2c_ackpos_config(I2C0, I2C_ACKPOS_NEXT);
//	}
	/* enable the I2C0 interrupt */
	i2c_interrupt_enable(I2C0, I2C_INT_ERR);
	i2c_interrupt_enable(I2C0, I2C_INT_EV);
	i2c_interrupt_enable(I2C0, I2C_INT_BUF);
	/* send a start condition to I2C bus */
	i2c_start_on_bus(I2C0);
	while ((i2c_nbytes > 0)) {
		osDelay(1);
		if (i2c_error_code != 0) {
			unlock();
			return false;
		}
	}
	unlock();
	return true;
}

bool FRToSI2C::Mem_Write(uint16_t DevAddress, uint16_t MemAddress, uint8_t *p_buffer, uint16_t number_of_byte) {
	if (!lock())
		return false;

	i2c_slave_address = DevAddress;
	i2c_write = p_buffer;
	i2c_write_dress = MemAddress;
	i2c_nbytes = number_of_byte;
	i2c_error_code = 0;
	i2c_process_flag = 0;
	i2c_write_process = I2C_SEND_ADDRESS_FIRST;
	i2c_read_process = I2C_SEND_ADDRESS_FIRST;
	i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_AERR);
	i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_SMBALT);
	i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_SMBTO);
	i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_OUERR);
	i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_LOSTARB);
	i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_BERR);
	i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_PECERR);
	/* enable the I2C0 interrupt */
	i2c_interrupt_enable(I2C0, I2C_INT_ERR);
	i2c_interrupt_enable(I2C0, I2C_INT_EV);
	i2c_interrupt_enable(I2C0, I2C_INT_BUF);
	/* wait until I2C bus is idle */
	uint8_t timeout = 0;
	while (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY)) {
		timeout++;
		osDelay(1);
		if (timeout > 20) {
			unlock();
			return false;
		}
	}

	/* send a start condition to I2C bus */
	//This sending will kickoff the IRQ's
	i2c_start_on_bus(I2C0);
	while ((i2c_nbytes > 0)) {
		osDelay(1);
		if (i2c_error_code != 0) {
			unlock();
			return false;
		}
	}
	unlock();
	return true;
}

bool FRToSI2C::Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size) {
	return Mem_Write(DevAddress, pData[0], pData + 1, Size - 1);
}

bool FRToSI2C::probe(uint16_t DevAddress) {
	uint8_t temp[1];
	return Mem_Read(DevAddress, 0x00, temp, sizeof(temp));
}

void FRToSI2C::I2C_Unstick() {
	unstick_I2C();
}

bool FRToSI2C::lock() {
	if (I2CSemaphore == nullptr) {
		return false;
	}
	return xSemaphoreTake(I2CSemaphore,1000) == pdTRUE;
}

void FRToSI2C::unlock() {
	xSemaphoreGive(I2CSemaphore);
}

bool FRToSI2C::writeRegistersBulk(const uint8_t address, const I2C_REG* registers, const uint8_t registersLength) {
	for (int index = 0; index < registersLength; index++) {
		if (!I2C_RegisterWrite(address, registers[index].reg, registers[index].val)) {
			return false;
		}
		if (registers[index].pause_ms) {
			delay_ms(registers[index].pause_ms);
		}
	}
	return true;
}
