/*
 * I2CBB.cpp
 *
 *  Created on: 12 Jun 2020
 *      Author: Ralim
 */
#include "Model_Config.h"
#ifdef I2C_SOFT
#include "FreeRTOS.h"
#include <I2CBB.hpp>
SemaphoreHandle_t I2CBB::I2CSemaphore = NULL;
StaticSemaphore_t I2CBB::xSemaphoreBuffer;
void              I2CBB::init() {
  // Set GPIO's to output open drain
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStruct.Pin   = SDA2_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  HAL_GPIO_Init(SDA2_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  GPIO_InitStruct.Pin   = SCL2_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  HAL_GPIO_Init(SCL2_GPIO_Port, &GPIO_InitStruct);
  SOFT_SDA_HIGH();
  SOFT_SCL_HIGH();
  I2CSemaphore = xSemaphoreCreateMutexStatic(&xSemaphoreBuffer);
  unlock();
}

bool I2CBB::probe(uint8_t address) {
  if (!lock())
    return false;
  start();
  bool ack = send(address);
  stop();
  unlock();
  return ack;
}

bool I2CBB::Mem_Read(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size) {
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
  SOFT_SCL_LOW();
  SOFT_I2C_DELAY();
  //	stop();
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

bool I2CBB::Mem_Write(uint16_t DevAddress, uint16_t MemAddress, const uint8_t *pData, uint16_t Size) {
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
    ack = send(pData[0]);
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
    ack = send(pData[0]);
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

void I2CBB::TransmitReceive(uint16_t DevAddress, uint8_t *pData_tx, uint16_t Size_tx, uint8_t *pData_rx, uint16_t Size_rx) {
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
      ack = send(pData_tx[0]);
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
  SOFT_SCL_HIGH();
  SOFT_SDA_HIGH();
  SOFT_I2C_DELAY();
  SOFT_SDA_LOW();
  SOFT_I2C_DELAY();
  SOFT_SCL_LOW();
  SOFT_I2C_DELAY();
  SOFT_SDA_HIGH();
}

void I2CBB::stop() {
  /* I2C Stop condition, clock goes high when data is low */
  SOFT_SDA_LOW();
  SOFT_I2C_DELAY();
  SOFT_SCL_HIGH();
  SOFT_I2C_DELAY();
  SOFT_SDA_HIGH();
  SOFT_I2C_DELAY();
}

bool I2CBB::send(uint8_t value) {

  for (uint8_t i = 0; i < 8; i++) {
    write_bit(value & 0x80); // write the most-significant bit
    value <<= 1;
  }

  SOFT_SDA_HIGH();
  bool ack = (read_bit() == 0);
  return ack;
}

uint8_t I2CBB::read(bool ack) {
  uint8_t B = 0;

  uint8_t i;
  for (i = 0; i < 8; i++) {
    B <<= 1;
    B |= read_bit();
  }

  SOFT_SDA_HIGH();
  if (ack)
    write_bit(0);
  else
    write_bit(1);
  return B;
}

uint8_t I2CBB::read_bit() {
  uint8_t b;

  SOFT_SDA_HIGH();
  SOFT_I2C_DELAY();
  SOFT_SCL_HIGH();
  SOFT_I2C_DELAY();

  if (SOFT_SDA_READ())
    b = 1;
  else
    b = 0;

  SOFT_SCL_LOW();
  return b;
}

void I2CBB::unlock() { xSemaphoreGive(I2CSemaphore); }

bool I2CBB::lock() {
  if (I2CSemaphore == NULL) {
    asm("bkpt");
  }
  bool a = xSemaphoreTake(I2CSemaphore, (TickType_t)100) == pdTRUE;
  return a;
}

void I2CBB::write_bit(uint8_t val) {
  if (val) {
    SOFT_SDA_HIGH();
  } else {
    SOFT_SDA_LOW();
  }

  SOFT_I2C_DELAY();
  SOFT_SCL_HIGH();
  SOFT_I2C_DELAY();
  SOFT_SCL_LOW();
}

#endif
