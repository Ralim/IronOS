/*
 * flash.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

#include "BSP_Flash.h"
#include "BSP.h"
#include "string.h"
#include "stm32f1xx_hal.h"
/*Flash start OR'ed with the maximum amount of flash - 1024 bytes*/
/*We use the last 1024 byte page*/
#define FLASH_ADDR (0x8000000 |0xFC00)
uint8_t flash_save_buffer(const uint8_t *buffer, const uint16_t length) {
	FLASH_EraseInitTypeDef pEraseInit;
	pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	pEraseInit.Banks = FLASH_BANK_1;
	pEraseInit.NbPages = 1;
	pEraseInit.PageAddress = FLASH_ADDR;
	uint32_t failingAddress = 0;
	resetWatchdog();
	__HAL_FLASH_CLEAR_FLAG(
			FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR | FLASH_FLAG_BSY);
	HAL_FLASH_Unlock();
	HAL_Delay(10);
	resetWatchdog();
	HAL_FLASHEx_Erase(&pEraseInit, &failingAddress);
	//^ Erase the page of flash (1024 bytes on this stm32)
	// erased the chunk
	// now we program it
	uint16_t *data = (uint16_t*) buffer;
	HAL_FLASH_Unlock();
	for (uint8_t i = 0; i < (length / 2); i++) {
		resetWatchdog();
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FLASH_ADDR + (i * 2),
				data[i]);
	}
	HAL_FLASH_Lock();
	return 1;
}

void flash_read_buffer(uint8_t *buffer, const uint16_t length) {

	uint16_t *data = (uint16_t*) buffer;
	for (uint8_t i = 0; i < (length / 2); i++) {
		data[i] = *((uint16_t*) (FLASH_ADDR + (i * 2)));
	}
}
