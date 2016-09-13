/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. ********************
 File Name : Flash.c
 Version   :                                        Author : bure
 *******************************************************************************/
#include "APP_Version.h"
#include "Flash.h"
#include "Bios.h"


/*******************************************************************************
 Function: FLASH_Prog
 Description: Programs the data into the system flash at the specified address
 *******************************************************************************/
u8 FLASH_Prog(u32 Address, u16 Data) {
	if (FLASH_WaitForLastOperation(WAIT_TIMES) != FLASH_TIMEOUT)
		FLASH_ClearFlag(
		FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	return FLASH_ProgramHalfWord(Address, Data);
}
/*******************************************************************************
 Function: FLASH_Erase
 Description: Erases a page of flash
 Inputs: Starting address for the page to erase
 *******************************************************************************/
void FLASH_Erase(u32 Address) {
	if (Address % FLASH_PAGE == 0) {               // FLASH Page start (1K/Page)
		if (FLASH_WaitForLastOperation(WAIT_TIMES) != FLASH_TIMEOUT) //wait for last op to finish
			FLASH_ClearFlag(
			FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
		FLASH_ErasePage(Address);                   // FLASH Page erase
	}
}
/*********************************  END OF FILE  ******************************/
