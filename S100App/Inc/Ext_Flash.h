/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. ********************                       
File Name :      EXT_Flash.h
Version :        S100 APP Ver 2.11   
Description:
Author :         bure 
Data:           
History:
*******************************************************************************/

#ifndef __EXT_FLASH_H
#define __EXT_FLASH_H
//#include "stm32f10x_lib.h"
#include "STM32F10x.h"

extern u8 flash_mode;
#define FLASH_2M  1
#define FLASH_8M  2
//#define W25Q64BV

#define WREN       0x06     // Write enable instruction 
#define READ       0x03     // Read from Memory instruction 
#define RDSR       0x05     // Read Status Register instruction  
#define PP         0x02     // Write to Memory instruction 
#define PE         0xDB     // Page Erase instruction 
#define PW         0x0A     // Page write instruction 
#define DP         0xB9     // Deep power-down instruction 
#define RDP        0xAB     // Release from deep power-down instruction 
//----W25Q64BV----------------------------------------------------------------//
#define CHIPE      0xC7     // Chip All Erase instruction 
#define BE64       0xD8     // Block 32k Erase instruction
#define BE32       0x52     // Block 64k Erase instruction 
#define SE         0x20     // Sector 4K Erase instruction 
#define Dummy_Byte 0xA5
#define EXT_FLASH_PageSize  256

//----------------------------------------------------------------------------//
#define WIP_Flag   0x01  // Write In Progress (WIP) flag 


#define ExtFlash_CS_LOW()     GPIO_ResetBits(GPIOB, GPIO_Pin_7)
#define ExtFlash_CS_HIGH()    GPIO_SetBits(GPIOB, GPIO_Pin_7)

void ExtFlash_PageWR(u8* pBuffer, u32 WriteAddr);
void ExtFlash_PageRD(u8* pBuffer, u32 ReadAddr, u16 Lenght);
void MAL_GetStatus (void);
u8   ExtFlash_ReadByte(void);
u8   ExtFlash_SendByte(u8 byte);
void ExtFlash_WriteEnable(void);
void ExtFlash_WaitForWriteEnd(void);
void ExtFlash_PageProg(u8* pBuffer, u32 WriteAddr,u8 CMD);
#endif

/*********************************  END OF FILE  ******************************/
