#include "BSP.h"
#include "FreeRTOS.h"
#include "Settings.h"
#include "task.h"
#include "ts21.h"
#include <string.h>

static_assert(sizeof(systemSettingsType) <= 256, "TS21 settings must fit a single fast-erase page");

static volatile uint32_t &flashRegister(unsigned offset) { return *(volatile uint32_t *)(0x40022000 + offset); }

static void waitFlash() {
  for (unsigned tries = 0; tries < 1000000; ++tries) {
    if (!(flashRegister(0x0C) & 1)) {
      if (flashRegister(0x0C) & 0x14) {
        ts21_fault(TS21_ERROR_FLASH);
      }
      return;
    }
    ts21_watchdog_refresh();
  }
  ts21_fault(TS21_ERROR_FLASH);
}

void flash_read_buffer(uint8_t *buffer, const uint16_t length) {
  if (buffer == nullptr || length > 256) {
    ts21_fault(TS21_ERROR_ARGUMENT);
  }
  memcpy(buffer, (const void *)SETTINGS_START_PAGE, length);
}

void flash_save_buffer(const uint8_t *buffer, const uint16_t length) {
  if (buffer == nullptr || length > 256) {
    ts21_fault(TS21_ERROR_ARGUMENT);
  }
  // Fast erase addresses one 256-byte page; normal erase would destroy 4 KiB.
  alignas(4) uint16_t page[128];
  memcpy(page, (const void *)SETTINGS_START_PAGE, sizeof(page));
  memcpy(page, buffer, length);
  uint32_t state;
  __asm volatile("csrrci %0, mstatus, 8" : "=r"(state)::"memory");
  ts21_heater_inhibit();
  flashRegister(4)    = 0x45670123;
  flashRegister(4)    = 0xCDEF89AB;
  flashRegister(0x24) = 0x45670123;
  flashRegister(0x24) = 0xCDEF89AB;
  flashRegister(0x0C) = 0x34;
  waitFlash();
  flashRegister(0x10) = 0x20000;
  flashRegister(0x14) = SETTINGS_START_PAGE;
  flashRegister(0x10) = 0x20040;
  waitFlash();
  flashRegister(0x0C) = 0x20;
  flashRegister(0x10) = 1;
  for (unsigned i = 0; i < sizeof(page) / sizeof(page[0]); ++i) {
    ((volatile uint16_t *)SETTINGS_START_PAGE)[i] = page[i];
    waitFlash();
    flashRegister(0x0C) = 0x20;
    resetWatchdog();
  }
  flashRegister(0x10) = 0x8080;
  if (memcmp((const void *)SETTINGS_START_PAGE, page, sizeof(page))) {
    ts21_fault(TS21_ERROR_FLASH);
  }
  // A settings erase/program can outlast one tick; resume from the live timebase.
  if (SysTick->CTLR & 2) {
    SysTick->CMP = ts21_timer_now() + TS21_TIMER_HZ / configTICK_RATE_HZ;
    SysTick->SR  = 0;
  }
  __asm volatile("csrw mstatus, %0" ::"r"(state) : "memory");
}
