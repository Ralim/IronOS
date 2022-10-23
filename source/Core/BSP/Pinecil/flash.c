/*
 * flash.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "BSP.h"
#include "BSP_Flash.h"
#include "gd32vf103_libopt.h"
#include "string.h"
#define FMC_PAGE_SIZE ((uint16_t)0x400U)
#define SETTINGS_START_PAGE (0x08000000 + (127 * 1024))
void flash_save_buffer(const uint8_t *buffer, const uint16_t length) {

  /* unlock the flash program/erase controller */
  fmc_unlock();

  /* clear all pending flags */
  fmc_flag_clear(FMC_FLAG_END);
  fmc_flag_clear(FMC_FLAG_WPERR);
  fmc_flag_clear(FMC_FLAG_PGERR);
  resetWatchdog();
  fmc_page_erase((uint32_t)SETTINGS_START_PAGE);
  resetWatchdog();
  uint16_t *data = (uint16_t *)buffer;
  for (uint16_t i = 0; i < (length / 2); i++) {
    fmc_halfword_program((uint32_t)SETTINGS_START_PAGE + (i * 2), data[i]);
    fmc_flag_clear(FMC_FLAG_END);
    fmc_flag_clear(FMC_FLAG_WPERR);
    fmc_flag_clear(FMC_FLAG_PGERR);
    resetWatchdog();
  }
  fmc_lock();
}

void flash_read_buffer(uint8_t *buffer, const uint16_t length) {
  uint32_t *b  = (uint32_t *)buffer;
  uint32_t *b2 = (uint32_t *)SETTINGS_START_PAGE;
  for (int i = 0; i < length / 4; i++) {
    b[i] = b2[i];
  }
}
