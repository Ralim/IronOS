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

#define FLASH_PAGE_SIZE ((uint16_t)1024)

#define SETTINGS_START_PAGE (1023 * 1024) // Hal auto offsets base addr
#define SETTINGS_READ_PAGE  0x23000000 + SETTINGS_START_PAGE
uint8_t flash_save_buffer(const uint8_t *buffer, const uint16_t length) {
  flash_erase(SETTINGS_START_PAGE, length);

  flash_write(SETTINGS_START_PAGE, buffer, sizeof(length));

  return 1;
}

void flash_read_buffer(uint8_t *buffer, const uint16_t length) {

  /* read 0x00010000 flash data */
  flash_read(SETTINGS_START_PAGE, buffer, sizeof(length));
  return;
  // #TODO
  uint32_t *b  = (uint32_t *)buffer;
  uint32_t *b2 = (uint32_t *)SETTINGS_START_PAGE;
  for (int i = 0; i < length / 4; i++) {
    b[i] = b2[i];
  }
}
