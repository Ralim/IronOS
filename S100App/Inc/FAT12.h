/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. ********************                       
File Name :      FAT12.h
Version :        S100 APP Ver 2.11   
Description:
Author :         bure 
Data:           
History:
*******************************************************************************/
#ifndef __FAT12_H
#define __FAT12_H

#include "stm32f10x_lib.h"


u8   ReadFileSec(u8* Buffer, u16* Cluster);
u8   ReadDiskData(u8* pBuffer, u32 ReadAddr, u16 Lenght);
u8   NextCluster(u16* Cluster);
u8   ProgFileSec(u8* Buffer, u16* Cluster);
u8   ProgDiskPage(u8* Buffer, u32 ProgAddr);
u8   SeekBlank(u8* Buffer, u16* Cluster);
u8   SetCluster(u8* Buffer, u16* Cluster);
u8   OpenFileRd(u8* Buffer, u8* FileName, u16* Cluster, u32* pDirAddr);
u8   OpenFileWr(u8* Buffer, u8* FileName, u16* Cluster, u32* pDirAddr);
u8   CloseFile(u8* Buffer, u32 Lenght, u16* Cluster, u32* pDirAddr);
void ExtFlash_PageWrite(u8* pBuffer, u32 WriteAddr, u8 Mode);

#endif
/********************************* END OF FILE ********************************/
