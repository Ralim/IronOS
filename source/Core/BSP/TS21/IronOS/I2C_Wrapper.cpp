#include "I2C_Wrapper.hpp"
#include "ts21.h"

SemaphoreHandle_t FRToSI2C::I2CSemaphore = nullptr;
StaticSemaphore_t FRToSI2C::xSemaphoreBuffer;

bool FRToSI2C::Mem_Write(uint16_t address, uint16_t control, uint8_t *data, uint16_t size) {
  if (address != 0x78 || (control != 0 && control != 0x40)) {
    ts21_fault(TS21_ERROR_ARGUMENT);
  }
  if (xSemaphoreTake(I2CSemaphore, pdMS_TO_TICKS(250)) != pdTRUE) {
    ts21_fault(TS21_ERROR_I2C_BUS);
  }
  const bool result = ts21_oled_write((uint8_t)control, data, size);
  xSemaphoreGive(I2CSemaphore);
  if (!result) {
    ts21_fault(TS21_ERROR_I2C_BUS);
  }
  return result;
}

bool FRToSI2C::I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t value) { return Mem_Write(address, reg, &value, 1); }
