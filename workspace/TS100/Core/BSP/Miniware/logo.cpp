/*
 * logo.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "BSP.h"
#include "OLED.hpp"
// Second last page of flash set aside for logo image.
#define FLASH_LOGOADDR (0x8000000 | 0xF800)

// Logo header signature.
#define LOGO_HEADER_VALUE 0xF00DAA55

uint8_t showBootLogoIfavailable() {
// Do not show logo data if signature is not found.
	if (LOGO_HEADER_VALUE
			!= *(reinterpret_cast<const uint32_t*>(FLASH_LOGOADDR))) {
		return 0;
	}

	OLED::drawAreaSwapped(0, 0, 96, 16, (uint8_t*) (FLASH_LOGOADDR + 4));
	OLED::refresh();
	return 1;
}

