#include "Model_Config.h"
#ifdef POW_PD
#include "BSP.h"
#include "I2CBB.hpp"
#include "Setup.h"
#include "fusb302b.h"
#include "fusb_user.h"
/*
 * Read a single byte from the FUSB302B
 *
 * cfg: The FUSB302B to communicate with
 * addr: The memory address from which to read
 *
 * Returns the value read from addr.
 */
uint8_t fusb_read_byte(uint8_t addr) {
  uint8_t data[1];
  if (!I2CBB::Mem_Read(FUSB302B_ADDR, addr, (uint8_t *)data, 1)) {
    return 0;
  }
  return data[0];
}

/*
 * Read multiple bytes from the FUSB302B
 *
 * cfg: The FUSB302B to communicate with
 * addr: The memory address from which to read
 * size: The number of bytes to read
 * buf: The buffer into which data will be read
 */
bool fusb_read_buf(uint8_t addr, uint8_t size, uint8_t *buf) { return I2CBB::Mem_Read(FUSB302B_ADDR, addr, buf, size); }

/*
 * Write a single byte to the FUSB302B
 *
 * cfg: The FUSB302B to communicate with
 * addr: The memory address to which we will write
 * byte: The value to write
 */
bool fusb_write_byte(uint8_t addr, uint8_t byte) { return I2CBB::Mem_Write(FUSB302B_ADDR, addr, (uint8_t *)&byte, 1); }

/*
 * Write multiple bytes to the FUSB302B
 *
 * cfg: The FUSB302B to communicate with
 * addr: The memory address to which we will write
 * size: The number of bytes to write
 * buf: The buffer to write
 */
bool fusb_write_buf(uint8_t addr, uint8_t size, const uint8_t *buf) { return I2CBB::Mem_Write(FUSB302B_ADDR, addr, buf, size); }

uint8_t fusb302_detect() {
  // Probe the I2C bus for its address
  return I2CBB::probe(FUSB302B_ADDR);
}

void setupFUSBIRQ() {
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pin   = GPIO_PIN_9;
  GPIO_InitStruct.Mode  = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 10, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}
#endif
