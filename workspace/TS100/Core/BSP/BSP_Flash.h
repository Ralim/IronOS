/*
 * BSP_Flash.h
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */
#include "stdint.h"
#ifndef BSP_BSP_FLASH_H_
#define BSP_BSP_FLASH_H_
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Wrappers to allow read/writing to a sector of flash that we use to store all of the user settings
 *
 * Should allow reading and writing to the flash
 */

//Erase the flash, then save the buffer. Returns 1 if worked
uint8_t flash_save_buffer(const uint8_t *buffer, const uint16_t length);

void flash_read_buffer(uint8_t *buffer, const uint16_t length);
#ifdef __cplusplus
}
#endif
#endif /* BSP_BSP_FLASH_H_ */
