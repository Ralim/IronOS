/*
 * flash.c
 *
 *  Settings persistence for the Alientek T90 (Nations N32L40x).
 *  N32 main flash is WORD-only programmable (no halfword program for app flash)
 *  and erases in 2 KB pages; settings live in the reserved last 2 KB sector.
 */

#include "BSP.h"
#include "BSP_Flash.h"
#include "n32l40x.h"
#include "string.h"

void flash_save_buffer(const uint8_t *buffer, const uint16_t length) {
  // The settings struct must fit the single reserved 2 KB page; refuse anything larger so a
  // bad length can never erase/program past the settings page into the logo page or the app.
  if (length > 2048) {
    return;
  }
  resetWatchdog();
  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_STS_CLRFLAG); // N32 all-error-flags constant (incl. PVERR/EVERR), not the F1 subset
  resetWatchdog();
  // Erase the single reserved 2 KB page.
  FLASH_EraseOnePage((uint32_t)SETTINGS_START_PAGE);

  // Program word-by-word; pad the tail up to a 4-byte boundary.
  const uint16_t words = (length + 3) / 4;
  for (uint16_t i = 0; i < words; i++) {
    resetWatchdog();
    uint32_t word = 0;
    memcpy(&word, buffer + (i * 4), (length - (i * 4)) >= 4 ? 4 : (length - (i * 4)));
    FLASH_ProgramWord((uint32_t)SETTINGS_START_PAGE + (i * 4), word);
  }
  FLASH_Lock();
}

void flash_read_buffer(uint8_t *buffer, const uint16_t length) { memcpy(buffer, (uint8_t *)SETTINGS_START_PAGE, length); }
