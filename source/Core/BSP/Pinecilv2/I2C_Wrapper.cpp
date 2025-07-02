/*
 * FRToSI2C.cpp
 *
 *  Created on: 14Apr.,2018
 *      Author: Ralim
 */
#include "BSP.h"
#include "IRQ.h"
#include "Setup.h"
#include "bl_mcu_sdk/drivers/bl702_driver/std_drv/inc/bl702_dma.h"
extern "C" {
#include "bflb_platform.h"
#include "bl702_dma.h"
#include "bl702_glb.h"
#include "bl702_i2c.h"
}
#include <I2C_Wrapper.hpp>

// Semaphore for locking users of I2C
SemaphoreHandle_t FRToSI2C::I2CSemaphore = nullptr;
StaticSemaphore_t FRToSI2C::xSemaphoreBuffer;
#define I2C_TIME_OUT     (uint16_t)(12000)
#define I2C_NOTIFY_INDEX 1
#define I2C_TX_FIFO_ADDR (0x4000A300 + 0x88)
#define I2C_RX_FIFO_ADDR (0x4000A300 + 0x8C)

// Used by the irq handler

volatile uint8_t     *IRQDataPointer;
volatile uint8_t      IRQDataSizeLeft;
volatile bool         IRQFailureMarker;
volatile TaskHandle_t IRQTaskWaitingHandle = NULL;
/****** IRQ Handlers ******/
void i2c_irq_tx_fifo_low() {
  // Filling tx fifo
  // Fifo is 32 bit, LSB sent first
  // FiFo can store up to 2, 32-bit words
  // So we fill it until it has no free room (or we run out of data)

  while (IRQDataSizeLeft > 0 && I2C_GetTXFIFOAvailable() > 0) {
    // Can put in at least 1 byte

    // Build a 32-bit word from bytes
    uint32_t value   = 0;
    int      packing = IRQDataSizeLeft >= 4 ? 0 : 4 - IRQDataSizeLeft;
    for (int i = 0; i < 4 && IRQDataSizeLeft > 0; i++) {
      value >>= 8;
      value |= (*IRQDataPointer) << 24; // Shift to the left, adding new data to the higher byte
      IRQDataPointer++;                 // Shift to next byte
      IRQDataSizeLeft--;
    }
    // Handle shunting remaining bytes if not a full 4 to send
    for (int i = 0; i < packing; i++) {
      value >>= 8;
    }
    // Push the new value to the fifo
    *((volatile uint32_t *)I2C_TX_FIFO_ADDR) = value;
  }
  if (IRQDataSizeLeft == 0) {
    // Disable IRQ, were done
    I2C_IntMask(I2C0_ID, I2C_TX_FIFO_READY_INT, MASK);
  }
}

void i2c_rx_pop_fifo() {
  // Pop one word from the fifo and store it
  uint32_t value = *((uint32_t *)I2C_RX_FIFO_ADDR);

  for (int i = 0; i < 4 && IRQDataSizeLeft > 0; i++) {
    *IRQDataPointer = value & 0xFF;
    IRQDataPointer++;
    IRQDataSizeLeft--;
    value >>= 8;
  }
}

void i2c_irq_rx_fifo_ready() {
  // Draining the Rx FiFo
  while (I2C_GetRXFIFOAvailable() > 0) {
    i2c_rx_pop_fifo();
  }

  if (IRQDataSizeLeft == 0) {
    // Disable IRQ, were done
    I2C_IntMask(I2C0_ID, I2C_RX_FIFO_READY_INT, MASK);
  }
}

void i2c_irq_done_read() {
  IRQFailureMarker = false;
  // If there was a non multiple of 4 bytes to be read, they are pushed to the fifo now (end of transfer interrupt)
  // So we catch them here
  while (I2C_GetRXFIFOAvailable() > 0) {
    i2c_rx_pop_fifo();
  }

  // Mask IRQ's back off
  FRToSI2C::CpltCallback(); // Causes the lock to be released
}
void i2c_irq_done() {
  IRQFailureMarker = false;
  // Mask IRQ's back off
  FRToSI2C::CpltCallback(); // Causes the lock to be released
}
void i2c_irq_nack() {
  IRQFailureMarker = true;
  // Mask IRQ's back off
  FRToSI2C::CpltCallback(); // Causes the lock to be released
}

/****** END IRQ Handlers ******/
void FRToSI2C::CpltCallback() {
  // This is only triggered from IRQ context
  I2C_IntMask(I2C0_ID, I2C_TX_FIFO_READY_INT, MASK);
  I2C_IntMask(I2C0_ID, I2C_RX_FIFO_READY_INT, MASK);
  I2C_IntMask(I2C0_ID, I2C_TRANS_END_INT, MASK);
  I2C_IntMask(I2C0_ID, I2C_NACK_RECV_INT, MASK);

  CPU_Interrupt_Disable(I2C_IRQn); // Disable IRQ's

  I2C_Disable(I2C0_ID); // Disable I2C to tidy up

  // Unlock the semaphore && allow task switch if desired by RTOS
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  xSemaphoreGiveFromISR(I2CSemaphore, &xHigherPriorityTaskWoken);
  xTaskNotifyIndexedFromISR(IRQTaskWaitingHandle, I2C_NOTIFY_INDEX, IRQFailureMarker ? 2 : 1, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

bool FRToSI2C::I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data) { return Mem_Write(address, reg, &data, 1); }

uint8_t FRToSI2C::I2C_RegisterRead(uint8_t add, uint8_t reg) {
  uint8_t temp = 0;
  Mem_Read(add, reg, &temp, 1);
  return temp;
}

bool FRToSI2C::Mem_Read(uint16_t DevAddress, uint16_t read_address, uint8_t *p_buffer, uint16_t number_of_byte) {
  if (!lock()) {
    return false;
  }
  I2C_Transfer_Cfg i2cCfg = {0, DISABLE, 0, 0, 0, 0};
  BL_Err_Type      err    = ERROR;
  i2cCfg.slaveAddr        = DevAddress >> 1;
  i2cCfg.stopEveryByte    = DISABLE;
  i2cCfg.subAddr          = read_address;
  i2cCfg.dataSize         = number_of_byte;
  i2cCfg.data             = p_buffer;
  i2cCfg.subAddrSize      = 1; // one byte address

  // Store handles for IRQ
  IRQDataPointer       = p_buffer;
  IRQDataSizeLeft      = number_of_byte;
  IRQTaskWaitingHandle = xTaskGetCurrentTaskHandle();
  IRQFailureMarker     = false;

  I2C_Disable(I2C0_ID);
  // Setup and run
  I2C_Init(I2C0_ID, I2C_READ, &i2cCfg); // Setup hardware for the I2C init header with the device address
  I2C_IntMask(I2C0_ID, I2C_TRANS_END_INT, UNMASK);
  I2C_IntMask(I2C0_ID, I2C_NACK_RECV_INT, UNMASK);
  I2C_IntMask(I2C0_ID, I2C_RX_FIFO_READY_INT, UNMASK);
  I2C_Int_Callback_Install(I2C0_ID, I2C_TRANS_END_INT, i2c_irq_done_read);
  I2C_Int_Callback_Install(I2C0_ID, I2C_NACK_RECV_INT, i2c_irq_nack);
  I2C_Int_Callback_Install(I2C0_ID, I2C_RX_FIFO_READY_INT, i2c_irq_rx_fifo_ready);
  CPU_Interrupt_Enable(I2C_IRQn);

  CPU_Interrupt_Disable(BLE_IRQn);
  // Start
  I2C_Enable(I2C0_ID);

  // Wait for transfer in background
  uint32_t result = 0;
  xTaskNotifyWaitIndexed(I2C_NOTIFY_INDEX, 0xFFFFFFFF, 0xFFFFFFFF, &result, 0xFFFFFFFF);
  CPU_Interrupt_Enable(BLE_IRQn);

  return result == 1;
}

bool FRToSI2C::Mem_Write(uint16_t DevAddress, uint16_t MemAddress, uint8_t *p_buffer, uint16_t number_of_byte) {

  if (!lock()) {
    return false;
  }

  I2C_Transfer_Cfg i2cCfg = {0, DISABLE, 0, 0, 0, 0};
  BL_Err_Type      err    = ERROR;
  i2cCfg.slaveAddr        = DevAddress >> 1;
  i2cCfg.stopEveryByte    = DISABLE;
  i2cCfg.subAddr          = MemAddress;
  i2cCfg.dataSize         = number_of_byte;
  i2cCfg.data             = p_buffer;
  i2cCfg.subAddrSize      = 1; // one byte address

  // Store handles for IRQ
  IRQDataPointer       = p_buffer;
  IRQDataSizeLeft      = number_of_byte;
  IRQTaskWaitingHandle = xTaskGetCurrentTaskHandle();
  IRQFailureMarker     = false;

  // Setup and run
  I2C_Init(I2C0_ID, I2C_WRITE, &i2cCfg); // Setup hardware for the I2C init header with the device address
  I2C_IntMask(I2C0_ID, I2C_TRANS_END_INT, UNMASK);
  I2C_IntMask(I2C0_ID, I2C_NACK_RECV_INT, UNMASK);
  I2C_IntMask(I2C0_ID, I2C_TX_FIFO_READY_INT, UNMASK);
  I2C_Int_Callback_Install(I2C0_ID, I2C_TRANS_END_INT, i2c_irq_done);
  I2C_Int_Callback_Install(I2C0_ID, I2C_NACK_RECV_INT, i2c_irq_nack);
  I2C_Int_Callback_Install(I2C0_ID, I2C_TX_FIFO_READY_INT, i2c_irq_tx_fifo_low);
  CPU_Interrupt_Enable(I2C_IRQn);

  i2c_irq_tx_fifo_low();

  CPU_Interrupt_Disable(BLE_IRQn); // Shut up BLE while we do the transfer
  // Start
  I2C_Enable(I2C0_ID);

  // Wait for transfer in background
  uint32_t result = 0;
  xTaskNotifyWaitIndexed(I2C_NOTIFY_INDEX, 0xFFFFFFFF, 0xFFFFFFFF, &result, 0xFFFFFFFF);
  CPU_Interrupt_Enable(BLE_IRQn); // Now BLE can run

  return result == 1;
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

  if (!lock()) {
    return false;
  }
  uint8_t          temp[1] = {0};
  I2C_Transfer_Cfg i2cCfg  = {0, DISABLE, 0, 0, 0, 0};
  BL_Err_Type      err     = ERROR;
  i2cCfg.slaveAddr         = DevAddress >> 1;
  i2cCfg.stopEveryByte     = DISABLE;
  i2cCfg.subAddr           = 0;
  i2cCfg.dataSize          = 1;
  i2cCfg.data              = temp;
  i2cCfg.subAddrSize       = 0;

  err      = I2C_MasterReceiveBlocking(I2C0_ID, &i2cCfg);
  bool res = err == SUCCESS;
  if (!res) {
    I2C_Unstick();
  }
  unlock();
  return res;
}
