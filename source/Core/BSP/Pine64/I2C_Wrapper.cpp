/*
 * FRToSI2C.cpp
 *
 *  Created on: 14Apr.,2018
 *      Author: Ralim
 */
#include "BSP.h"
#include "IRQ.h"
#include "Setup.h"
#include <I2C_Wrapper.hpp>
SemaphoreHandle_t FRToSI2C::I2CSemaphore = nullptr;
StaticSemaphore_t FRToSI2C::xSemaphoreBuffer;
#define I2C_TIME_OUT (uint16_t)(12000)
void FRToSI2C::CpltCallback() {
	// TODO
}

bool FRToSI2C::I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data) {
	return Mem_Write(address, reg, &data, 1);
}

uint8_t FRToSI2C::I2C_RegisterRead(uint8_t add, uint8_t reg) {
	uint8_t temp = 0;
	Mem_Read(add, reg, &temp, 1);
	return temp;
}

enum i2c_step {
//Write+read steps
	Write_start, //Sending start on bus
	Write_device_address, //start sent, send device address
	Write_device_memory_address, //device address sent, write the memory location
	Write_device_data_start, // Write all of the remaining data using DMA
	Write_device_data_finish, // Write all of the remaining data using DMA

	Read_start, //second read
	Read_device_address, // Send device address again for the read
	Read_device_data_start, //read device data via DMA
	Read_device_data_finish, //read device data via DMA
	Send_stop, // send the stop at the end of the transaction
	Wait_stop, // Wait for stop to send and we are done
	Done, //Finished
	Error_occured, //Error occured on the bus

};
struct i2c_state {
	i2c_step currentStep;
	bool isMemoryWrite;
	dma_parameter_struct dma_init_struct;

};
volatile i2c_state currentState;

bool perform_i2c_transaction(uint16_t DevAddress, uint16_t memory_address, uint8_t *p_buffer, uint16_t number_of_byte, bool isWrite) {
	{
		//TODO is this required
		/* disable I2C0 */
		i2c_disable(I2C0);
		/* enable I2C0 */
		i2c_enable(I2C0);
	}
	i2c_interrupt_disable(I2C0, I2C_INT_ERR);
	i2c_interrupt_disable(I2C0, I2C_INT_BUF);
	i2c_interrupt_disable(I2C0, I2C_INT_EV);
	currentState.isMemoryWrite = isWrite;
	//Setup DMA
	if (currentState.isMemoryWrite) {
		dma_deinit(DMA0, DMA_CH5);
		currentState.dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
		currentState.dma_init_struct.memory_addr = (uint32_t) p_buffer;
		currentState.dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
		currentState.dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
		currentState.dma_init_struct.number = number_of_byte;
		currentState.dma_init_struct.periph_addr = (uint32_t) &I2C_DATA(I2C0);
		currentState.dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
		currentState.dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
		currentState.dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
		dma_init(DMA0, DMA_CH5, (dma_parameter_struct*) &currentState.dma_init_struct);
	} else {
		dma_deinit(DMA0, DMA_CH6);
		currentState.dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
		currentState.dma_init_struct.memory_addr = (uint32_t) p_buffer;
		currentState.dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
		currentState.dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
		currentState.dma_init_struct.number = number_of_byte;
		currentState.dma_init_struct.periph_addr = (uint32_t) &I2C_DATA(I2C0);
		currentState.dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
		currentState.dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
		currentState.dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
		dma_init(DMA0, DMA_CH6, (dma_parameter_struct*) &currentState.dma_init_struct);
	}

	if (!currentState.isMemoryWrite) {
		i2c_dma_last_transfer_config(I2C0, I2C_DMALST_ON);
	}
	currentState.currentStep = Write_start; //Always start in write mode
	TickType_t timeout = xTaskGetTickCount() + TICKS_SECOND;
	while ((currentState.currentStep != Done) && (currentState.currentStep != Error_occured)) {
		if (xTaskGetTickCount() > timeout) {
			i2c_stop_on_bus(I2C0);
			return false;
		}
		switch (currentState.currentStep) {
		case Error_occured:

			i2c_stop_on_bus(I2C0);
			return false;
			break;
		case Write_start:

			/* enable acknowledge */
			i2c_ack_config(I2C0, I2C_ACK_ENABLE);
			/* i2c master sends start signal only when the bus is idle */
			if (!i2c_flag_get(I2C0, I2C_FLAG_I2CBSY)) {
				/* send the start signal */
				i2c_start_on_bus(I2C0);
				currentState.currentStep = Write_device_address;
			}
			break;
		case Read_start:
			/* wait until BTC bit is set */
			if (i2c_flag_get(I2C0, I2C_FLAG_BTC)) {
				i2c_start_on_bus(I2C0);
				currentState.currentStep = Read_device_address;
			}
			break;
		case Write_device_address:
			/* i2c master sends START signal successfully */
			if (i2c_flag_get(I2C0, I2C_FLAG_SBSEND)) {
				i2c_master_addressing(I2C0, DevAddress, I2C_TRANSMITTER);
				currentState.currentStep = Write_device_memory_address;
			}
			break;
		case Read_device_address:
			if (i2c_flag_get(I2C0, I2C_FLAG_SBSEND)) {
				i2c_master_addressing(I2C0, DevAddress, I2C_RECEIVER);
				currentState.currentStep = Read_device_data_start;
			}
			break;
		case Write_device_memory_address:
			//Send the device memory location
			if (i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)) { //addr sent
				if (i2c_flag_get(I2C0, I2C_FLAG_AERR)) {
					//Arb error - we lost the bus / nacked
					currentState.currentStep = Error_occured;
				}
				if (i2c_flag_get(I2C0, I2C_FLAG_TBE)) {
					i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
					// Write out the 8 byte address
					i2c_data_transmit(I2C0, memory_address);
					if (currentState.isMemoryWrite) {
						currentState.currentStep = Write_device_data_start;
					} else {
						currentState.currentStep = Read_start;
					}
				}
			}
			break;
		case Write_device_data_start:

			/* wait until BTC bit is set */
			if (i2c_flag_get(I2C0, I2C_FLAG_BTC)) {
				/* enable I2C0 DMA */
				i2c_dma_enable(I2C0, I2C_DMA_ON);
				/* enable DMA0 channel5 */
				dma_channel_enable(DMA0, DMA_CH5);
				currentState.currentStep = Write_device_data_finish;
			}
			break;

		case Write_device_data_finish:					//Wait for complete then goto stop
			/* wait until BTC bit is set */
			if (dma_flag_get(DMA0, DMA_CH5, DMA_FLAG_FTF)) {
				/* wait until BTC bit is set */
				if (i2c_flag_get(I2C0, I2C_FLAG_BTC)) {
					currentState.currentStep = Send_stop;
				}
			}

			break;
		case Read_device_data_start:
			if (i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)) { //addr sent
				if (i2c_flag_get(I2C0, I2C_FLAG_AERR)) {
					//Arb error - we lost the bus / nacked
					currentState.currentStep = Error_occured;
				}
				i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
				/* one byte master reception procedure (polling) */
				if (number_of_byte == 0) {

					currentState.currentStep = Send_stop;
				} else if (number_of_byte == 1) {
					/* disable acknowledge */
					i2c_ack_config(I2C0, I2C_ACK_DISABLE);
					/* clear ADDSEND register by reading I2C_STAT0 then I2C_STAT1 register
					 * (I2C_STAT0 has already been read) */
					i2c_flag_get(I2C0, I2C_FLAG_ADDSEND);					//sat0
					i2c_flag_get(I2C0, I2C_FLAG_I2CBSY);					//sat1
					/* send a stop condition to I2C bus*/
					i2c_stop_on_bus(I2C0);
					/* wait for the byte to be received */
					while (!i2c_flag_get(I2C0, I2C_FLAG_RBNE))
						;
					/* read the byte received from the EEPROM */
					*p_buffer = i2c_data_receive(I2C0);
					currentState.currentStep = Wait_stop;
				} else { /* more than one byte master reception procedure (DMA) */
					/* enable I2C0 DMA */
					i2c_dma_enable(I2C0, I2C_DMA_ON);
					/* enable DMA0 channel5 */
					dma_channel_enable(DMA0, DMA_CH6);
					currentState.currentStep = Read_device_data_finish;
				}
			}
			break;
		case Read_device_data_finish:					//Wait for complete then goto stop
			/* wait until BTC bit is set */
			if (dma_flag_get(DMA0, DMA_CH6, DMA_FLAG_FTF)) {
				currentState.currentStep = Send_stop;
			}

			break;
		case Send_stop:
			/* send a stop condition to I2C bus*/
			i2c_stop_on_bus(I2C0);
			currentState.currentStep = Wait_stop;
			break;
		case Wait_stop:
			/* i2c master sends STOP signal successfully */
			if ((I2C_CTL0(I2C0) & 0x0200) != 0x0200) {
				currentState.currentStep = Done;
			}
			break;
		default:
			//If we get here something is amiss
			return false;
		}
	}
	return true;
}

bool FRToSI2C::Mem_Read(uint16_t DevAddress, uint16_t read_address, uint8_t *p_buffer, uint16_t number_of_byte) {
	if (!lock())
		return false;
	bool res = perform_i2c_transaction(DevAddress, read_address, p_buffer, number_of_byte, false);
	if (!res) {
		I2C_Unstick();
	}
	unlock();
	return res;
}

bool FRToSI2C::Mem_Write(uint16_t DevAddress, uint16_t MemAddress, uint8_t *p_buffer, uint16_t number_of_byte) {
	if (!lock())
		return false;
	bool res = perform_i2c_transaction(DevAddress, MemAddress, p_buffer, number_of_byte, true);
	if (!res) {
		I2C_Unstick();
	}
	unlock();
	return res;
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
	return xSemaphoreTake(I2CSemaphore, TICKS_SECOND) == pdTRUE;
}

void FRToSI2C::unlock() {
	xSemaphoreGive(I2CSemaphore);
}

bool FRToSI2C::writeRegistersBulk(const uint8_t address, const I2C_REG *registers, const uint8_t registersLength) {
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

bool FRToSI2C::wakePart(uint16_t DevAddress) {
// wakepart is a special case  where only the device address is sent
	if (!lock())
		return false;

	i2c_interrupt_disable(I2C0, I2C_INT_ERR);
	i2c_interrupt_disable(I2C0, I2C_INT_EV);
	i2c_interrupt_disable(I2C0, I2C_INT_BUF);

	uint8_t state = I2C_START;
	uint16_t timeout = 0;
	bool done = false;
	bool timedout = false;
	while (!(done || timedout)) {
		switch (state) {
		case I2C_START:
			/* i2c master sends start signal only when the bus is idle */
			while (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT )) {
				timeout++;
			}
			if (timeout < I2C_TIME_OUT) {
				i2c_start_on_bus(I2C0);
				timeout = 0;
				state = I2C_SEND_ADDRESS;
			} else {
				I2C_Unstick();
				timeout = 0;
				state = I2C_START;
			}
			break;
		case I2C_SEND_ADDRESS:
			/* i2c master sends START signal successfully */
			while ((!i2c_flag_get(I2C0, I2C_FLAG_SBSEND)) && (timeout < I2C_TIME_OUT )) {
				timeout++;
			}
			if (timeout < I2C_TIME_OUT) {
				i2c_master_addressing(I2C0, DevAddress, I2C_TRANSMITTER);
				timeout = 0;
				state = I2C_CLEAR_ADDRESS_FLAG;
			} else {
				timedout = true;
				done = true;
				timeout = 0;
				state = I2C_START;
			}
			break;
		case I2C_CLEAR_ADDRESS_FLAG:
			/* address flag set means i2c slave sends ACK */
			while ((!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)) && (timeout < I2C_TIME_OUT )) {
				timeout++;
				if (i2c_flag_get(I2C0, I2C_FLAG_AERR)) {
					i2c_flag_clear(I2C0, I2C_FLAG_AERR);
					i2c_stop_on_bus(I2C0);
					/* i2c master sends STOP signal successfully */
					while ((I2C_CTL0(I2C0) & 0x0200) && (timeout < I2C_TIME_OUT )) {
						timeout++;
					}
					// Address NACK'd
					unlock();
					return false;
				}
			}
			if (timeout < I2C_TIME_OUT) {
				i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
				timeout = 0;
				state = I2C_STOP;
			} else {
				// Dont retry as this means a NAK
				i2c_stop_on_bus(I2C0);
				/* i2c master sends STOP signal successfully */
				while ((I2C_CTL0(I2C0) & 0x0200) && (timeout < I2C_TIME_OUT )) {
					timeout++;
				}
				unlock();
				return false;
			}
			break;

		case I2C_STOP:
			/* send a stop condition to I2C bus */
			i2c_stop_on_bus(I2C0);
			/* i2c master sends STOP signal successfully */
			while ((I2C_CTL0(I2C0) & 0x0200) && (timeout < I2C_TIME_OUT )) {
				timeout++;
			}
			if (timeout < I2C_TIME_OUT) {
				timeout = 0;
				state = I2C_END;
				done = true;
			} else {
				timedout = true;
				done = true;
				timeout = 0;
				state = I2C_START;
			}
			break;
		default:
			state = I2C_START;
			timeout = 0;
			break;
		}
	}
	unlock();
	return timedout == false;
}

void I2C_EV_IRQ() {

}
void I2C_ER_IRQ() {
//Error callbacks

}
