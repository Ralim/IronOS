/*
 * logo.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "BSP.h"
#include "OLED.hpp"

static uint8_t logo_page[1024] __attribute__((section(".logo_page")));

// Logo header signature.
#define LOGO_HEADER_VALUE 0xF00DAA55

uint8_t showBootLogoIfavailable() {
  // Do not show logo data if signature is not found.
  if (LOGO_HEADER_VALUE != *(reinterpret_cast<const uint32_t *>(logo_page))) {
    return 0;
  }

  OLED::drawAreaSwapped(0, 0, 96, 16, (uint8_t *)(logo_page + 4));
  OLED::refresh();
  return 1;
}
