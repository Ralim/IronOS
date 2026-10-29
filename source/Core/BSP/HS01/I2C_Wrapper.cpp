/*
 * I2C_Wrapper.cpp
 *
 * Bit-banged I2C for the OLED (no I2C peripheral). SDA is open drain emulated through the direction register.
 */
#include "BSP.h"
#include "Pins.h"
#include "Setup.h"
#include <I2C_Wrapper.hpp>

SemaphoreHandle_t FRToSI2C::I2CSemaphore = nullptr;
StaticSemaphore_t FRToSI2C::xSemaphoreBuffer;

#define SCL_HIGH() GPIOB_SetBits(SCL_Pin)
#define SCL_LOW()  GPIOB_ResetBits(SCL_Pin)
#define SDA_HIGH() (R32_PB_DIR &= ~SDA_Pin)
#define SDA_LOW()  (R32_PB_DIR |= SDA_Pin)
#define SDA_READ() (GPIOB_ReadPortPin(SDA_Pin) ? 1 : 0)

// Half a bit, roughly 200-280 kHz at 60 MHz. Shorter delays gave display glitches
static inline void i2cDelay() {
  for (int i = 0; i < 30; i++) {
    asm volatile("nop");
  }
}

static void i2cStart() {
  /* I2C Start condition, data line goes low when clock is high */
  SCL_HIGH();
  SDA_HIGH();
  i2cDelay();
  SDA_LOW();
  i2cDelay();
  SCL_LOW();
  i2cDelay();
  SDA_HIGH();
}

static void i2cStop() {
  /* I2C Stop condition, clock goes high when data is low */
  SDA_LOW();
  i2cDelay();
  SCL_HIGH();
  i2cDelay();
  SDA_HIGH();
  i2cDelay();
}

static void i2cWriteBit(uint8_t val) {
  if (val) {
    SDA_HIGH();
  } else {
    SDA_LOW();
  }
  i2cDelay();
  SCL_HIGH();
  i2cDelay();
  SCL_LOW();
}

static uint8_t i2cReadBit() {
  SDA_HIGH();
  i2cDelay();
  SCL_HIGH();
  i2cDelay();
  uint8_t b = SDA_READ();
  SCL_LOW();
  return b;
}

static bool i2cSend(uint8_t value) {
  for (uint8_t i = 0; i < 8; i++) {
    i2cWriteBit(value & 0x80); // write the most-significant bit
    value <<= 1;
  }
  SDA_HIGH();
  return i2cReadBit() == 0;
}

static uint8_t i2cRead(bool ack) {
  uint8_t B = 0;
  for (uint8_t i = 0; i < 8; i++) {
    B <<= 1;
    B |= i2cReadBit();
  }
  SDA_HIGH();
  i2cWriteBit(ack ? 0 : 1);
  return B;
}

void unstick_I2C() {
  // Clock the bus until a stuck slave lets go of SDA
  SDA_HIGH();
  for (int i = 0; i < 9 && !SDA_READ(); i++) {
    SCL_LOW();
    i2cDelay();
    SCL_HIGH();
    i2cDelay();
  }
  i2cStop();
}

void I2CInit() {
  GPIOB_SetBits(SCL_Pin);
  GPIOB_ModeCfg(SCL_Pin, GPIO_ModeOut_PP_5mA);
  GPIOB_ResetBits(SDA_Pin);
  GPIOB_ModeCfg(SDA_Pin, GPIO_ModeIN_PU);
  unstick_I2C();
}

void FRToSI2C::CpltCallback() {}

bool FRToSI2C::I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data) { return Mem_Write(address, reg, &data, 1); }

uint8_t FRToSI2C::I2C_RegisterRead(uint8_t add, uint8_t reg) {
  uint8_t temp = 0;
  Mem_Read(add, reg, &temp, 1);
  return temp;
}

bool FRToSI2C::Mem_Read(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size) {
  if (!lock()) {
    return false;
  }
  i2cStart();
  bool ack = i2cSend(DevAddress) && i2cSend(MemAddress);
  if (ack) {
    SCL_LOW();
    i2cDelay();
    i2cStart();
    ack = i2cSend(DevAddress | 1);
  }
  if (ack) {
    while (Size) {
      pData[0] = i2cRead(Size > 1);
      pData++;
      Size--;
    }
  }
  i2cStop();
  unlock();
  return ack;
}

bool FRToSI2C::Mem_Write(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size) {
  if (!lock()) {
    return false;
  }
  i2cStart();
  bool ack = i2cSend(DevAddress) && i2cSend(MemAddress);
  while (ack && Size) {
    ack = i2cSend(pData[0]);
    pData++;
    Size--;
  }
  i2cStop();
  unlock();
  return ack;
}

bool FRToSI2C::Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size) { return Mem_Write(DevAddress, pData[0], pData + 1, Size - 1); }

void FRToSI2C::Receive(uint16_t DevAddress, uint8_t *pData, uint16_t Size) {
  if (!lock()) {
    return;
  }
  i2cStart();
  if (i2cSend(DevAddress | 1)) {
    while (Size) {
      pData[0] = i2cRead(Size > 1);
      pData++;
      Size--;
    }
  }
  i2cStop();
  unlock();
}

void FRToSI2C::TransmitReceive(uint16_t DevAddress, uint8_t *pData_tx, uint16_t Size_tx, uint8_t *pData_rx, uint16_t Size_rx) {
  if (Size_tx) {
    Transmit(DevAddress, pData_tx, Size_tx);
  }
  if (Size_rx) {
    Receive(DevAddress, pData_rx, Size_rx);
  }
}

bool FRToSI2C::probe(uint16_t DevAddress) { return wakePart(DevAddress); }

void FRToSI2C::I2C_Unstick() { unstick_I2C(); }

bool FRToSI2C::lock() { return xSemaphoreTake(I2CSemaphore, (TickType_t)100) == pdTRUE; }

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
  // wakepart is a special case where only the device address is sent
  if (!lock()) {
    return false;
  }
  i2cStart();
  bool ack = i2cSend(DevAddress);
  i2cStop();
  unlock();
  return ack;
}
