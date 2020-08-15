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
/*Flash start OR'ed with the maximum amount of flash - 1024 bytes*/
/*We use the last 1024 byte page*/
#define FLASH_ADDR (0x8000000 | 0xFC00)
uint8_t flash_save_buffer(const uint8_t *buffer, const uint16_t length) {
  //TODO
  return 1;
}

void flash_read_buffer(uint8_t *buffer, const uint16_t length) {

  //TODO
}
