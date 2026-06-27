/*
 * flash.c
 *
 *  Settings persistence for the Alientek T90 (Nations N32L40x).
 *  N32 main flash is WORD-only programmable (no halfword program for app flash)
 *  and erases in 2 KB pages; settings live in the reserved last-but-one 2 KB page.
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
  // Erase the single reserved 2 KB page. Act on the status: if the page is write-protected or the
  // erase otherwise fails, bail (and re-lock) instead of programming into an un-erased page.
  FLASH_STS status = FLASH_EraseOnePage((uint32_t)SETTINGS_START_PAGE);
  if (status != FLASH_COMPL) {
    FLASH_Lock();
    return;
  }

  // Program word-by-word; pad the tail up to a 4-byte boundary. Stop on the first failure.
  const uint16_t words = (length + 3) / 4;
  for (uint16_t i = 0; i < words; i++) {
    resetWatchdog();
    uint32_t word = 0;
    memcpy(&word, buffer + (i * 4), (length - (i * 4)) >= 4 ? 4 : (length - (i * 4)));
    status = FLASH_ProgramWord((uint32_t)SETTINGS_START_PAGE + (i * 4), word);
    if (status != FLASH_COMPL) {
      FLASH_Lock();
      return;
    }
  }
  FLASH_Lock();
  // The N32L40x serves flash reads through an iCache enabled at SystemInit (FLASH->AC ICAHEN).
  // After self-modifying this page the cache can hold stale lines, so a read of the freshly written
  // settings (the saveSettings verify memcmp, or loadSettings on the next boot) may return pre-erase
  // data. Reset the iCache so every subsequent read reflects the programmed cells.
  FLASH_iCacheRST();
}

void flash_read_buffer(uint8_t *buffer, const uint16_t length) { memcpy(buffer, (uint8_t *)SETTINGS_START_PAGE, length); }
