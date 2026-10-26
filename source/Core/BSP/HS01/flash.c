/*
 * flash.c
 *
 * Settings live at the start of the data flash, like the stock firmware. Interrupts stay off while it is busy.
 */

#include "BSP.h"
#include "BSP_Flash.h"
#include "CH57x_common.h"
#include "FreeRTOS.h"
#include "task.h"

#define SETTINGS_FLASH_ADDR 0

void flash_save_buffer(const uint8_t *buffer, const uint16_t length) {
  const uint32_t eraseLength = ((length + EEPROM_MIN_ER_SIZE - 1) / EEPROM_MIN_ER_SIZE) * EEPROM_MIN_ER_SIZE;
  resetWatchdog();
  TMR0_PWMActDataWidth(0); // Heater off while the frame timer is blocked, the next frame restores it
  taskENTER_CRITICAL();
  EEPROM_ERASE(SETTINGS_FLASH_ADDR, eraseLength);
  EEPROM_WRITE(SETTINGS_FLASH_ADDR, (void *)buffer, length);
  taskEXIT_CRITICAL();
  resetWatchdog();
}

void flash_read_buffer(uint8_t *buffer, const uint16_t length) {
  taskENTER_CRITICAL();
  EEPROM_READ(SETTINGS_FLASH_ADDR, buffer, length);
  taskEXIT_CRITICAL();
}
