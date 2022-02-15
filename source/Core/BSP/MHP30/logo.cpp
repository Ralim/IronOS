/*
 * logo.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "BSP.h"
#include "BootLogo.h"

static uint8_t logo_page[1024] __attribute__((section(".logo_page")));

void showBootLogoIfavailable() { BootLogo::handleShowingLogo((uint8_t *)logo_page); }