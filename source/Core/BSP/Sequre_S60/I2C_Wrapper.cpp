/*
 * FRToSI2C.cpp
 *
 *  Created on: 14Apr.,2018
 *      Author: Ralim
 */
#include "BSP.h"
#include "Setup.h"
#include <I2C_Wrapper.hpp>
SemaphoreHandle_t FRToSI2C::I2CSemaphore = nullptr;
StaticSemaphore_t FRToSI2C::xSemaphoreBuffer;

void FRToSI2C::CpltCallback() {
  hi2c1.State = HAL_I2C_STATE_READY; // Force state reset (even if tx error)
  if (I2CSemaphore) {
    xSemaphoreGiveFromISR(I2CSemaphore, NULL);
  }
}

bool FRToSI2C::Mem_Read(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size) {

  if (!lock())
    return false;
  if (HAL_I2C_Mem_Read(&hi2c1, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, 500) != HAL_OK) {

    I2C_Unstick();
    unlock();
    return false;
  }

  unlock();
  return true;
}
bool FRToSI2C::I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data) { return Mem_Write(address, reg, &data, 1); }

uint8_t FRToSI2C::I2C_RegisterRead(uint8_t add, uint8_t reg) {
  uint8_t tx_data[1];
  Mem_Read(add, reg, tx_data, 1);
  return tx_data[0];
}
bool FRToSI2C::Mem_Write(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size) {

  if (!lock())
    return false;
  if (HAL_I2C_Mem_Write(&hi2c1, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, 500) != HAL_OK) {

    I2C_Unstick();
    unlock();
    return false;
  }

  unlock();
  return true;
}

bool FRToSI2C::Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size) {
  if (!lock())
    return false;
  if (HAL_I2C_Master_Transmit_IT(&hi2c1, DevAddress, pData, Size) != HAL_OK) {
    I2C_Unstick();
    unlock();
    return false;
  }
  return true;
}

bool FRToSI2C::probe(uint16_t DevAddress) {
  if (!lock())
    return false;
  uint8_t buffer[1];
  bool    worked = HAL_I2C_Mem_Read(&hi2c1, DevAddress, 0x0F, I2C_MEMADD_SIZE_8BIT, buffer, 1, 1000) == HAL_OK;
  unlock();
  return worked;
}

void FRToSI2C::I2C_Unstick() { unstick_I2C(); }

void FRToSI2C::unlock() { xSemaphoreGive(I2CSemaphore); }

bool FRToSI2C::lock() { return xSemaphoreTake(I2CSemaphore, (TickType_t)50) == pdTRUE; }

bool FRToSI2C::writeRegistersBulk(const uint8_t address, const I2C_REG *registers, const uint8_t registersLength) {
  for (int index = 0; index < registersLength; index++) {
    if (!I2C_RegisterWrite(address, registers[index].reg, registers[index].val)) {
      return false;
    }
    if (registers[index].pause_ms)
      delay_ms(registers[index].pause_ms);
  }
  return true;
}
