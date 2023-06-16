/*
 * flash.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "BSP.h"
#include "BSP_Flash.h"
#include "stm32f1xx_hal.h"
#include "string.h"

#define SETTINGS_START_PAGE (0x08000000 + (63 * 1024))

void flash_save_buffer(const uint8_t *buffer, const uint16_t length) {
  FLASH_EraseInitTypeDef pEraseInit;
  pEraseInit.TypeErase    = FLASH_TYPEERASE_PAGES;
  pEraseInit.Banks        = FLASH_BANK_1;
  pEraseInit.NbPages      = 1;
  pEraseInit.PageAddress  = (uint32_t)SETTINGS_START_PAGE;
  uint32_t failingAddress = 0;
  resetWatchdog();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR | FLASH_FLAG_BSY);
  HAL_FLASH_Unlock();
  HAL_Delay(1);
  resetWatchdog();
  HAL_FLASHEx_Erase(&pEraseInit, &failingAddress);
  //^ Erase the page of flash (1024 bytes on this stm32)
  // erased the chunk
  // now we program it
  uint16_t *data = (uint16_t *)buffer;
  HAL_FLASH_Unlock();
  for (uint16_t i = 0; i < (length / 2); i++) {
    resetWatchdog();
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, SETTINGS_START_PAGE+ (i*sizeof(uint16_t)), data[i]);
  }
  HAL_FLASH_Lock();
  
}

void flash_read_buffer(uint8_t *buffer, const uint16_t length) { memcpy(buffer, (uint8_t*)SETTINGS_START_PAGE, length); }
