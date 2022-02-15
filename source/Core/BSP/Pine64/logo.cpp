/*
 * logo.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "BSP.h"
#include "BootLogo.h"

// Second last page of flash set aside for logo image.
#define FLASH_LOGOADDR (0x08000000 + (126 * 1024))

void showBootLogoIfavailable() { BootLogo::handleShowingLogo((uint8_t *)FLASH_LOGOADDR); }
