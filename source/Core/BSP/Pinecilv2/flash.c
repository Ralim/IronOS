/*
 * flash.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "BSP.h"
#include "BSP_Flash.h"
#include "hal_flash.h"
#include "string.h"

#define FLASH_PAGE_SIZE (1024)

#define SETTINGS_START_PAGE (1023 * FLASH_PAGE_SIZE) // Hal auto offsets base addr

void flash_save_buffer(const uint8_t *buffer, const uint16_t length) {
  BL_Err_Type err = flash_erase(SETTINGS_START_PAGE, FLASH_PAGE_SIZE);
  err             = flash_write(SETTINGS_START_PAGE, buffer, length);
}

void flash_read_buffer(uint8_t *buffer, const uint16_t length) { flash_read(SETTINGS_START_PAGE, buffer, length); }
