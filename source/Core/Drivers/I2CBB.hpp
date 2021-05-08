/*
 * I2CBB.hpp
 *
 *  Created on: 12 Jun 2020
 *      Author: Ralim
 */

#ifndef BSP_MINIWARE_I2CBB_HPP_
#define BSP_MINIWARE_I2CBB_HPP_
#include "Model_Config.h"
#ifdef I2C_SOFT
#include "BSP.h"
#include "FreeRTOS.h"
#include "Pins.h"
#include "Setup.h"
#include "Software_I2C.h"
#include "semphr.h"

class I2CBB {
public:
  static void init();
  // Probe if device ACK's address or not
  static bool probe(uint8_t address);
  // Issues a complete 8bit register read
  static bool Mem_Read(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size);
  // Implements a register write
  static bool    Mem_Write(uint16_t DevAddress, uint16_t MemAddress, const uint8_t *pData, uint16_t Size);
  static void    Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
  static void    Receive(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
  static void    TransmitReceive(uint16_t DevAddress, uint8_t *pData_tx, uint16_t Size_tx, uint8_t *pData_rx, uint16_t Size_rx);
  static bool    I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data);
  static uint8_t I2C_RegisterRead(uint8_t address, uint8_t reg);
  typedef struct {
    const uint8_t reg;      // The register to write to
    uint8_t       val;      // The value to write to this register
    const uint8_t pause_ms; // How many ms to pause _after_ writing this reg
  } I2C_REG;
  static bool writeRegistersBulk(const uint8_t address, const I2C_REG *registers, const uint8_t registersLength);

private:
  static SemaphoreHandle_t I2CSemaphore;
  static StaticSemaphore_t xSemaphoreBuffer;
  static void              unlock();
  static bool              lock();
  static void              start();
  static void              stop();
  static bool              send(uint8_t value);
  static uint8_t           read(bool ack);
  static uint8_t           read_bit();
  static void              write_bit(uint8_t val);
};
#endif
#endif /* BSP_MINIWARE_I2CBB_HPP_ */
