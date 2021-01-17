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

bool FRToSI2C::Mem_Read(uint16_t DevAddress, uint16_t read_address, uint8_t *p_buffer, uint16_t number_of_byte) {
  if (!lock())
    return false;
  i2c_interrupt_disable(I2C0, I2C_INT_ERR);
  i2c_interrupt_disable(I2C0, I2C_INT_BUF);
  i2c_interrupt_disable(I2C0, I2C_INT_EV);
  dma_parameter_struct dma_init_struct;

  uint8_t  state            = I2C_START;
  uint8_t  in_rx_cycle      = 0;
  uint16_t timeout          = 0;
  uint8_t  tries            = 0;
  uint8_t  i2c_timeout_flag = 0;
  while (!(i2c_timeout_flag)) {
    switch (state) {
    case I2C_START:
      tries++;
      if (tries > 64) {
        i2c_stop_on_bus(I2C0);
        /* i2c master sends STOP signal successfully */
        while ((I2C_CTL0(I2C0) & 0x0200) && (timeout < I2C_TIME_OUT)) {
          timeout++;
        }
        unlock();
        return false;
      }
      if (0 == in_rx_cycle) {
        /* disable I2C0 */
        i2c_disable(I2C0);
        /* enable I2C0 */
        i2c_enable(I2C0);

        /* enable acknowledge */
        i2c_ack_config(I2C0, I2C_ACK_ENABLE);
        /* i2c master sends start signal only when the bus is idle */
        while (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
          timeout++;
        }
        if (timeout < I2C_TIME_OUT) {
          /* send the start signal */
          i2c_start_on_bus(I2C0);
          timeout = 0;
          state   = I2C_SEND_ADDRESS;
        } else {
          I2C_Unstick();
          timeout = 0;
          state   = I2C_START;
        }
      } else {
        i2c_start_on_bus(I2C0);
        timeout = 0;
        state   = I2C_SEND_ADDRESS;
      }
      break;
    case I2C_SEND_ADDRESS:
      /* i2c master sends START signal successfully */
      while ((!i2c_flag_get(I2C0, I2C_FLAG_SBSEND)) && (timeout < I2C_TIME_OUT)) {
        timeout++;
      }
      if (timeout < I2C_TIME_OUT) {
        if (RESET == in_rx_cycle) {
          i2c_master_addressing(I2C0, DevAddress, I2C_TRANSMITTER);
          state = I2C_CLEAR_ADDRESS_FLAG;
        } else {
          i2c_master_addressing(I2C0, DevAddress, I2C_RECEIVER);
          state = I2C_CLEAR_ADDRESS_FLAG;
        }
        timeout = 0;
      } else {
        timeout     = 0;
        state       = I2C_START;
        in_rx_cycle = 0;
      }
      break;
    case I2C_CLEAR_ADDRESS_FLAG:
      /* address flag set means i2c slave sends ACK */
      while ((!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)) && (timeout < I2C_TIME_OUT)) {
        timeout++;
        if (i2c_flag_get(I2C0, I2C_FLAG_AERR)) {
          i2c_flag_clear(I2C0, I2C_FLAG_AERR);
          i2c_stop_on_bus(I2C0);
          /* i2c master sends STOP signal successfully */
          while ((I2C_CTL0(I2C0) & 0x0200) && (timeout < I2C_TIME_OUT)) {
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
        state   = I2C_TRANSMIT_DATA;
      } else {
        i2c_stop_on_bus(I2C0);
        /* i2c master sends STOP signal successfully */
        while ((I2C_CTL0(I2C0) & 0x0200) && (timeout < I2C_TIME_OUT)) {
          timeout++;
        }
        // Address NACK'd
        unlock();
        return false;
      }
      break;
    case I2C_TRANSMIT_DATA:
      if (0 == in_rx_cycle) {
        /* wait until the transmit data buffer is empty */
        while ((!i2c_flag_get(I2C0, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
          timeout++;
        }
        if (timeout < I2C_TIME_OUT) {
          // Write out the 8 byte address
          i2c_data_transmit(I2C0, read_address);
          timeout = 0;
        } else {
          timeout     = 0;
          state       = I2C_START;
          in_rx_cycle = 0;
        }
        /* wait until BTC bit is set */
        while ((!i2c_flag_get(I2C0, I2C_FLAG_BTC)) && (timeout < I2C_TIME_OUT)) {
          timeout++;
        }
        if (timeout < I2C_TIME_OUT) {
          timeout     = 0;
          state       = I2C_START;
          in_rx_cycle = 1;
        } else {
          timeout     = 0;
          state       = I2C_START;
          in_rx_cycle = 0;
        }
      } else {
        /* one byte master reception procedure (polling) */
        if (number_of_byte < 2) {
          /* disable acknowledge */
          i2c_ack_config(I2C0, I2C_ACK_DISABLE);
          /* clear ADDSEND register by reading I2C_STAT0 then I2C_STAT1 register
           * (I2C_STAT0 has already been read) */
          i2c_flag_get(I2C0, I2C_FLAG_ADDSEND);
          /* send a stop condition to I2C bus*/
          i2c_stop_on_bus(I2C0);
          /* wait for the byte to be received */
          while (!i2c_flag_get(I2C0, I2C_FLAG_RBNE))
            ;
          /* read the byte received from the EEPROM */
          *p_buffer = i2c_data_receive(I2C0);
          /* decrement the read bytes counter */
          number_of_byte--;
          timeout = 0;
        } else { /* more than one byte master reception procedure (DMA) */
          dma_deinit(DMA0, DMA_CH6);
          dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
          dma_init_struct.memory_addr  = (uint32_t)p_buffer;
          dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
          dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
          dma_init_struct.number       = number_of_byte;
          dma_init_struct.periph_addr  = (uint32_t)&I2C_DATA(I2C0);
          dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
          dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
          dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
          dma_init(DMA0, DMA_CH6, &dma_init_struct);

          i2c_dma_last_transfer_config(I2C0, I2C_DMALST_ON);
          /* enable I2C0 DMA */
          i2c_dma_enable(I2C0, I2C_DMA_ON);
          /* enable DMA0 channel5 */
          dma_channel_enable(DMA0, DMA_CH6);
          /* wait until BTC bit is set */
          while (!dma_flag_get(DMA0, DMA_CH6, DMA_FLAG_FTF)) {}
          /* send a stop condition to I2C bus*/
          i2c_stop_on_bus(I2C0);
        }
        timeout = 0;
        state   = I2C_STOP;
      }
      break;
    case I2C_STOP:
      /* i2c master sends STOP signal successfully */
      while ((I2C_CTL0(I2C0) & 0x0200) && (timeout < I2C_TIME_OUT)) {
        timeout++;
      }
      if (timeout < I2C_TIME_OUT) {
        timeout          = 0;
        state            = I2C_END;
        i2c_timeout_flag = I2C_OK;
      } else {
        timeout     = 0;
        state       = I2C_START;
        in_rx_cycle = 0;
      }
      break;
    default:
      state            = I2C_START;
      in_rx_cycle      = 0;
      i2c_timeout_flag = I2C_OK;
      timeout          = 0;
      break;
    }
  }
  unlock();
  return true;
}

bool FRToSI2C::Mem_Write(uint16_t DevAddress, uint16_t MemAddress, uint8_t *p_buffer, uint16_t number_of_byte) {
  if (!lock())
    return false;

  i2c_interrupt_disable(I2C0, I2C_INT_ERR);
  i2c_interrupt_disable(I2C0, I2C_INT_EV);
  i2c_interrupt_disable(I2C0, I2C_INT_BUF);
  dma_parameter_struct dma_init_struct;

  uint8_t  state    = I2C_START;
  uint16_t timeout  = 0;
  bool     done     = false;
  bool     timedout = false;
  while (!(done || timedout)) {
    switch (state) {
    case I2C_START:
      /* i2c master sends start signal only when the bus is idle */
      while (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
        timeout++;
      }
      if (timeout < I2C_TIME_OUT) {
        i2c_start_on_bus(I2C0);
        timeout = 0;
        state   = I2C_SEND_ADDRESS;
      } else {
        I2C_Unstick();
        timeout = 0;
        state   = I2C_START;
      }
      break;
    case I2C_SEND_ADDRESS:
      /* i2c master sends START signal successfully */
      while ((!i2c_flag_get(I2C0, I2C_FLAG_SBSEND)) && (timeout < I2C_TIME_OUT)) {
        timeout++;
      }
      if (timeout < I2C_TIME_OUT) {
        i2c_master_addressing(I2C0, DevAddress, I2C_TRANSMITTER);
        timeout = 0;
        state   = I2C_CLEAR_ADDRESS_FLAG;
      } else {
        timedout = true;
        done     = true;
        timeout  = 0;
        state    = I2C_START;
      }
      break;
    case I2C_CLEAR_ADDRESS_FLAG:
      /* address flag set means i2c slave sends ACK */
      while ((!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)) && (timeout < I2C_TIME_OUT)) {
        timeout++;
        if (i2c_flag_get(I2C0, I2C_FLAG_AERR)) {
          i2c_flag_clear(I2C0, I2C_FLAG_AERR);
          i2c_stop_on_bus(I2C0);
          /* i2c master sends STOP signal successfully */
          while ((I2C_CTL0(I2C0) & 0x0200) && (timeout < I2C_TIME_OUT)) {
            timeout++;
          }
          // Address NACK'd
          unlock();
          return false;
        }
      }
      timeout = 0;
      if (timeout < I2C_TIME_OUT) {
        i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
        state = I2C_TRANSMIT_DATA;
      } else {
        // Dont retry as this means a NAK
        i2c_stop_on_bus(I2C0);
        /* i2c master sends STOP signal successfully */
        while ((I2C_CTL0(I2C0) & 0x0200) && (timeout < I2C_TIME_OUT)) {
          timeout++;
        }
        unlock();
        return false;
      }
      break;
    case I2C_TRANSMIT_DATA:
      /* wait until the transmit data buffer is empty */
      while ((!i2c_flag_get(I2C0, I2C_FLAG_TBE)) && (timeout < I2C_TIME_OUT)) {
        timeout++;
      }
      if (timeout < I2C_TIME_OUT) {
        /* send the EEPROM's internal address to write to : only one byte
         * address */
        i2c_data_transmit(I2C0, MemAddress);
        timeout = 0;
      } else {
        timedout = true;
        timeout  = 0;
        state    = I2C_START;
      }
      /* wait until BTC bit is set */
      while (!i2c_flag_get(I2C0, I2C_FLAG_BTC))
        ;
      dma_deinit(DMA0, DMA_CH5);
      dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
      dma_init_struct.memory_addr  = (uint32_t)p_buffer;
      dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
      dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
      dma_init_struct.number       = number_of_byte;
      dma_init_struct.periph_addr  = (uint32_t)&I2C_DATA(I2C0);
      dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
      dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
      dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
      dma_init(DMA0, DMA_CH5, &dma_init_struct);
      /* enable I2C0 DMA */
      i2c_dma_enable(I2C0, I2C_DMA_ON);
      /* enable DMA0 channel5 */
      dma_channel_enable(DMA0, DMA_CH5);
      /* wait until BTC bit is set */
      while (!dma_flag_get(DMA0, DMA_CH5, DMA_FLAG_FTF)) {}
      /* wait until BTC bit is set */
      while (!i2c_flag_get(I2C0, I2C_FLAG_BTC)) {}
      state = I2C_STOP;
      break;
    case I2C_STOP:
      /* send a stop condition to I2C bus */
      i2c_stop_on_bus(I2C0);
      /* i2c master sends STOP signal successfully */
      while ((I2C_CTL0(I2C0) & 0x0200) && (timeout < I2C_TIME_OUT)) {
        timeout++;
      }
      if (timeout < I2C_TIME_OUT) {
        timeout = 0;
        state   = I2C_END;
        done    = true;
      } else {
        timedout = true;
        done     = true;
        timeout  = 0;
        state    = I2C_START;
      }
      break;
    default:
      state   = I2C_START;
      timeout = 0;
      break;
    }
  }
  unlock();
  return timedout == false;
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
  if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
    return true;
  }
  return xSemaphoreTake(I2CSemaphore, TICKS_SECOND) == pdTRUE;
}

void FRToSI2C::unlock() {
  if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
    return;
  }
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

  uint8_t  state    = I2C_START;
  uint16_t timeout  = 0;
  bool     done     = false;
  bool     timedout = false;
  while (!(done || timedout)) {
    switch (state) {
    case I2C_START:
      /* i2c master sends start signal only when the bus is idle */
      while (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY) && (timeout < I2C_TIME_OUT)) {
        timeout++;
      }
      if (timeout < I2C_TIME_OUT) {
        i2c_start_on_bus(I2C0);
        timeout = 0;
        state   = I2C_SEND_ADDRESS;
      } else {
        I2C_Unstick();
        timeout = 0;
        state   = I2C_START;
      }
      break;
    case I2C_SEND_ADDRESS:
      /* i2c master sends START signal successfully */
      while ((!i2c_flag_get(I2C0, I2C_FLAG_SBSEND)) && (timeout < I2C_TIME_OUT)) {
        timeout++;
      }
      if (timeout < I2C_TIME_OUT) {
        i2c_master_addressing(I2C0, DevAddress, I2C_TRANSMITTER);
        timeout = 0;
        state   = I2C_CLEAR_ADDRESS_FLAG;
      } else {
        timedout = true;
        done     = true;
        timeout  = 0;
        state    = I2C_START;
      }
      break;
    case I2C_CLEAR_ADDRESS_FLAG:
      /* address flag set means i2c slave sends ACK */
      while ((!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)) && (timeout < I2C_TIME_OUT)) {
        timeout++;
        if (i2c_flag_get(I2C0, I2C_FLAG_AERR)) {
          i2c_flag_clear(I2C0, I2C_FLAG_AERR);
          i2c_stop_on_bus(I2C0);
          /* i2c master sends STOP signal successfully */
          while ((I2C_CTL0(I2C0) & 0x0200) && (timeout < I2C_TIME_OUT)) {
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
        state   = I2C_STOP;
      } else {
        // Dont retry as this means a NAK
        i2c_stop_on_bus(I2C0);
        /* i2c master sends STOP signal successfully */
        while ((I2C_CTL0(I2C0) & 0x0200) && (timeout < I2C_TIME_OUT)) {
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
      while ((I2C_CTL0(I2C0) & 0x0200) && (timeout < I2C_TIME_OUT)) {
        timeout++;
      }
      if (timeout < I2C_TIME_OUT) {
        timeout = 0;
        state   = I2C_END;
        done    = true;
      } else {
        timedout = true;
        done     = true;
        timeout  = 0;
        state    = I2C_START;
      }
      break;
    default:
      state   = I2C_START;
      timeout = 0;
      break;
    }
  }
  unlock();
  return timedout == false;
}
