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

bool FRToSI2C::I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data) { return Mem_Write(address, reg, &data, 1); }

uint8_t FRToSI2C::I2C_RegisterRead(uint8_t add, uint8_t reg) {
  uint8_t temp = 0;
  Mem_Read(add, reg, &temp, 1);
  return temp;
}

enum class i2c_step {
  // Write+read steps
  Write_start,                 // Sending start on bus
  Write_device_address,        // start sent, send device address
  Write_device_memory_address, // device address sent, write the memory location
  Write_device_data_start,     // Write all of the remaining data using DMA
  Write_device_data_finish,    // Write all of the remaining data using DMA

  Read_start,              // second read
  Read_device_address,     // Send device address again for the read
  Read_device_data_start,  // read device data via DMA
  Read_device_data_finish, // read device data via DMA
  Send_stop,               // send the stop at the end of the transaction
  Wait_stop,               // Wait for stop to send and we are done
  Done,                    // Finished
  Error_occured,           // Error occured on the bus

};
struct i2c_state {
  i2c_step             currentStep;
  bool                 isMemoryWrite;
  bool                 wakePart;
  uint8_t              deviceAddress;
  uint8_t              memoryAddress;
  uint8_t *            buffer;
  uint16_t             numberOfBytes;
  dma_parameter_struct dma_init_struct;
};
i2c_state currentState;

void perform_i2c_step() {
  // Performs next step of the i2c state machine
  if (i2c_flag_get(I2C0, I2C_FLAG_AERR)) {
    i2c_flag_clear(I2C0, I2C_FLAG_AERR);
    // Arb error - we lost the bus / nacked
    currentState.currentStep = i2c_step::Error_occured;
  }
  switch (currentState.currentStep) {
  case i2c_step::Error_occured:
    i2c_stop_on_bus(I2C0);
    break;
  case i2c_step::Write_start:

    /* enable acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
    /* i2c master sends start signal only when the bus is idle */
    if (!i2c_flag_get(I2C0, I2C_FLAG_I2CBSY)) {
      /* send the start signal */
      i2c_start_on_bus(I2C0);
      currentState.currentStep = i2c_step::Write_device_address;
    }
    break;

  case i2c_step::Write_device_address:
    /* i2c master sends START signal successfully */
    if (i2c_flag_get(I2C0, I2C_FLAG_SBSEND)) {
      i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND); // Clear sbsend by reading ctrl banks
      i2c_master_addressing(I2C0, currentState.deviceAddress, I2C_TRANSMITTER);
      currentState.currentStep = i2c_step::Write_device_memory_address;
    }
    break;
  case i2c_step::Write_device_memory_address:
    // Send the device memory location

    if (i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)) { // addr sent
      i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

      if (currentState.wakePart) {
        // We are stopping here
        currentState.currentStep = i2c_step::Send_stop;
        return;
      }
      i2c_flag_clear(I2C0, I2C_FLAG_BTC);
      // Write out the 8 byte address
      i2c_data_transmit(I2C0, currentState.memoryAddress);
      if (currentState.isMemoryWrite) {
        currentState.currentStep = i2c_step::Write_device_data_start;
      } else {
        currentState.currentStep = i2c_step::Read_start;
      }
    }

    break;
  case i2c_step::Write_device_data_start:
    /* wait until the transmission data register is empty */
    if (i2c_flag_get(I2C0, I2C_FLAG_BTC)) {
      dma_deinit(DMA0, DMA_CH5);
      dma_init(DMA0, DMA_CH5, &currentState.dma_init_struct);
      i2c_dma_last_transfer_config(I2C0, I2C_DMALST_ON);
      dma_circulation_disable(DMA0, DMA_CH5);
      /* enable I2C0 DMA */
      i2c_dma_enable(I2C0, I2C_DMA_ON);
      /* enable DMA0 channel5 */
      dma_channel_enable(DMA0, DMA_CH5);
      currentState.currentStep = i2c_step::Write_device_data_finish;
    }
    break;

  case i2c_step::Write_device_data_finish: // Wait for complete then goto stop
    /* wait until BTC bit is set */
    if (dma_flag_get(DMA0, DMA_CH5, DMA_FLAG_FTF)) {
      /* wait until BTC bit is set */
      if (i2c_flag_get(I2C0, I2C_FLAG_BTC)) {
        currentState.currentStep = i2c_step::Send_stop;
      }
    }
    break;
  case i2c_step::Read_start:
    if (i2c_flag_get(I2C0, I2C_FLAG_BTC)) {
      /* wait until BTC bit is set */
      i2c_start_on_bus(I2C0);
      currentState.currentStep = i2c_step::Read_device_address;
    }
    break;
  case i2c_step::Read_device_address:
    if (i2c_flag_get(I2C0, I2C_FLAG_SBSEND)) {
      i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
      if (currentState.numberOfBytes == 1) {
        /* disable acknowledge */
        i2c_master_addressing(I2C0, currentState.deviceAddress, I2C_RECEIVER);
        while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)) {}
        i2c_ack_config(I2C0, I2C_ACK_DISABLE);
        i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
        /* wait for the byte to be received */
        while (!i2c_flag_get(I2C0, I2C_FLAG_RBNE)) {}
        /* read the byte received from the EEPROM */
        *currentState.buffer = i2c_data_receive(I2C0);
        while (i2c_flag_get(I2C0, I2C_FLAG_RBNE)) {
          i2c_data_receive(I2C0);
        }
        i2c_stop_on_bus(I2C0);
        while ((I2C_CTL0(I2C0) & I2C_CTL0_STOP)) {
          asm("nop");
        }
        currentState.currentStep = i2c_step::Done;
      } else if (currentState.numberOfBytes == 2) {
        /* disable acknowledge */
        i2c_master_addressing(I2C0, currentState.deviceAddress, I2C_RECEIVER);
        while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)) {}
        i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
        /* wait for the byte to be received */
        while (!i2c_flag_get(I2C0, I2C_FLAG_RBNE)) {}
        i2c_ackpos_config(I2C0, I2C_ACKPOS_CURRENT);
        i2c_ack_config(I2C0, I2C_ACK_DISABLE);

        /* read the byte received from the EEPROM */
        *currentState.buffer = i2c_data_receive(I2C0);
        currentState.buffer++;

        /* wait for the byte to be received */
        while (!i2c_flag_get(I2C0, I2C_FLAG_RBNE)) {}
        /* read the byte received from the EEPROM */
        *currentState.buffer = i2c_data_receive(I2C0);
        while (i2c_flag_get(I2C0, I2C_FLAG_RBNE)) {
          i2c_data_receive(I2C0);
        }
        i2c_stop_on_bus(I2C0);
        while ((I2C_CTL0(I2C0) & I2C_CTL0_STOP)) {
          asm("nop");
        }
        currentState.currentStep = i2c_step::Done;
      } else {
        i2c_master_addressing(I2C0, currentState.deviceAddress, I2C_RECEIVER);
        currentState.currentStep = i2c_step::Read_device_data_start;
      }
    }
    break;
  case i2c_step::Read_device_data_start:
    if (i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)) { // addr sent
      i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
      /* one byte master reception procedure (polling) */
      if (currentState.numberOfBytes == 0) {
        currentState.currentStep = i2c_step::Send_stop;
      } else { /* more than one byte master reception procedure (DMA) */

        while (currentState.numberOfBytes) {

          if (3 == currentState.numberOfBytes) {
            /* wait until BTC bit is set */
            while (!i2c_flag_get(I2C0, I2C_FLAG_BTC)) {}
            i2c_ackpos_config(I2C0, I2C_ACKPOS_CURRENT);
            /* disable acknowledge */
            i2c_ack_config(I2C0, I2C_ACK_DISABLE);
          } else if (2 == currentState.numberOfBytes) {
            /* wait until BTC bit is set */
            while (!i2c_flag_get(I2C0, I2C_FLAG_BTC)) {}
            /* disable acknowledge */
            i2c_ack_config(I2C0, I2C_ACK_DISABLE);
            /* send a stop condition to I2C bus */
            i2c_stop_on_bus(I2C0);
          }
          /* wait until RBNE bit is set */
          while (!i2c_flag_get(I2C0, I2C_FLAG_RBNE)) {}
          /* read a byte from the EEPROM */
          *currentState.buffer = i2c_data_receive(I2C0);

          /* point to the next location where the byte read will be saved */
          currentState.buffer++;

          /* decrement the read bytes counter */
          currentState.numberOfBytes--;
        }
        currentState.currentStep = i2c_step::Wait_stop;
        // currentState.currentStep = i2c_step::Read_device_data_finish;
      }
    }
    break;
  case i2c_step::Read_device_data_finish: // Wait for complete then goto stop
    /* wait until BTC bit is set */

    break;
  case i2c_step::Send_stop:
    /* send a stop condition to I2C bus*/
    i2c_stop_on_bus(I2C0);
    currentState.currentStep = i2c_step::Wait_stop;
    break;
  case i2c_step::Wait_stop:
    /* i2c master sends STOP signal successfully */
    if ((I2C_CTL0(I2C0) & I2C_CTL0_STOP) != I2C_CTL0_STOP) {
      currentState.currentStep = i2c_step::Done;
    }
    break;
  default:
    // If we get here something is amiss
    return;
  }
}

bool perform_i2c_transaction(uint16_t DevAddress, uint16_t memory_address, uint8_t *p_buffer, uint16_t number_of_byte, bool isWrite, bool isWakeOnly) {

  currentState.isMemoryWrite = isWrite;
  currentState.wakePart      = isWakeOnly;
  currentState.deviceAddress = DevAddress;
  currentState.memoryAddress = memory_address;
  currentState.numberOfBytes = number_of_byte;
  currentState.buffer        = p_buffer;
  // Setup DMA
  currentState.dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
  currentState.dma_init_struct.memory_addr  = (uint32_t)p_buffer;
  currentState.dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
  currentState.dma_init_struct.number       = number_of_byte;
  currentState.dma_init_struct.periph_addr  = (uint32_t)&I2C_DATA(I2C0);
  currentState.dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
  currentState.dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
  currentState.dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;

  if (currentState.isMemoryWrite) {
    currentState.dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
  } else {
    currentState.dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
  }
  // Clear flags
  I2C_STAT0(I2C0) = 0;
  I2C_STAT1(I2C0) = 0;
  i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
  i2c_ackpos_config(I2C0, I2C_ACKPOS_CURRENT);
  i2c_data_receive(I2C0);
  i2c_data_receive(I2C0);
  currentState.currentStep = i2c_step::Write_start; // Always start in write mode
  TickType_t timeout       = xTaskGetTickCount() + TICKS_100MS;
  while ((currentState.currentStep != i2c_step::Done) && (currentState.currentStep != i2c_step::Error_occured)) {
    if (xTaskGetTickCount() > timeout) {
      i2c_stop_on_bus(I2C0);
      return false;
    }
    perform_i2c_step();
  }
  return currentState.currentStep == i2c_step::Done;
}

bool FRToSI2C::Mem_Read(uint16_t DevAddress, uint16_t read_address, uint8_t *p_buffer, uint16_t number_of_byte) {
  if (!lock())
    return false;
  bool res = perform_i2c_transaction(DevAddress, read_address, p_buffer, number_of_byte, false, false);
  if (!res) {
    I2C_Unstick();
  }
  unlock();
  return res;
}

bool FRToSI2C::Mem_Write(uint16_t DevAddress, uint16_t MemAddress, uint8_t *p_buffer, uint16_t number_of_byte) {
  if (!lock())
    return false;
  bool res = perform_i2c_transaction(DevAddress, MemAddress, p_buffer, number_of_byte, true, false);
  if (!res) {
    I2C_Unstick();
  }
  unlock();
  return res;
}

bool FRToSI2C::Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size) { return Mem_Write(DevAddress, pData[0], pData + 1, Size - 1); }

bool FRToSI2C::probe(uint16_t DevAddress) {
  uint8_t temp[1];
  return Mem_Read(DevAddress, 0x00, temp, sizeof(temp));
}

void FRToSI2C::I2C_Unstick() { unstick_I2C(); }

bool FRToSI2C::lock() {
  if (I2CSemaphore == nullptr) {
    return false;
  }
  return xSemaphoreTake(I2CSemaphore, TICKS_SECOND) == pdTRUE;
}

void FRToSI2C::unlock() { xSemaphoreGive(I2CSemaphore); }

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
  bool res = perform_i2c_transaction(DevAddress, 0, NULL, 0, false, true);
  if (!res) {
    I2C_Unstick();
  }
  unlock();
  return res;
}

void I2C_EV_IRQ() {}
void I2C_ER_IRQ() {
  // Error callbacks
}
