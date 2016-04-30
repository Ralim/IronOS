/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. ********************
 File Name : Flash.h
 Version  :                                     Author : bure
*******************************************************************************/
#ifndef __EXT_FLASH_H
#define __EXT_FLASH_H

#define PAGESIZE   256

#define WREN       0x06   // Write enable instruction
#define READ       0x03   // Read from Memory instruction
#define RDSR       0x05   // Read Status Register instruction
#define PP         0x02   // Write to Memory instruction
#define PW         0x0A   // Page write instruction

#define OK         0      // 操作完成
#define SEC_ERR    1      // 扇区读写错误
#define TMAX       100000 // 超时限制

#define WIP_Flag   0x01   // Write In Progress (WIP) flag
#define Dummy_Byte 0xA5

u8   FLASH_Prog(u32 Address, u16 Data);
void FLASH_Erase(u32 Address);
void ExtFlashPageWR(u8* pBuffer, u32 WriteAddr);
void ExtFlashDataRD(u8* pBuffer, u32 ReadAddr, u16 Lenght);
#endif

/*********************************  END OF FILE  ******************************/
