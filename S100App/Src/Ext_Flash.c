/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. ********************                       
File Name :      EXT_Flash.c
Version :        S100 APP Ver 2.11   
Description:
Author :         bure 
Data:           
History:
*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include "Ext_Flash.h"
#include "BIOS.h"


#define OK           0            // 操作完成
#define SEC_ERR      1            // 扇区读写错误
#define TMAX         100000       // 超时限制

u32 Mass_Memory_Size;
u32 Mass_Block_Size;
u32 Mass_Block_Count;
u32 Tout;


u8 flash_mode;

void ExtFLASH_SectorErase(u32 SectorAddr);

/*******************************************************************************
 SPI_FLASH_SectorErase  : Sector Erases the specified FLASH Page.(4k/sector)
*******************************************************************************/
void ExtFLASH_SectorErase(u32 SectorAddr)
{
    ExtFlash_WriteEnable();
    ExtFlash_CS_LOW();
    ExtFlash_SendByte(SE);
    ExtFlash_SendByte((SectorAddr & 0xFF0000) >> 16);   //Send high address byte
    ExtFlash_SendByte((SectorAddr & 0xFF00) >> 8);      //Send medium address byte
    ExtFlash_SendByte(SectorAddr & 0xFF);               //Send low address byte
    ExtFlash_CS_HIGH();
    ExtFlash_WaitForWriteEnd();               // Wait the end of Flash writing
}


void ExtFlash_PageWR(u8* pBuffer, u32 WriteAddr)
{
    u32 addr,i,j;
    u8* ptr;
    u8  page=0,flag=0,write_mode;
    u8  buffer[256];

    flag=0;
    if(flash_mode==FLASH_8M) {
        addr=WriteAddr & 0xFFF000;
        page=16;
    } else {
        page=1;
        addr=WriteAddr & 0xFFFF00;
    }
    while(page>0) {
        ExtFlash_PageRD((u8*)&buffer,addr, 256);
        for(j=0; j<255; j++) {
            if(buffer[j++]!=0xff) {
                flag=1;
                break;
            }
        }
        addr+=256;
        page--;
    }
    if(flash_mode==FLASH_8M) {
        page=16;
        addr=WriteAddr & 0xFFF000;
        if(flag==1)ExtFLASH_SectorErase(addr);
        write_mode=PP;
    } else {
        page=1;
        addr=WriteAddr & 0xFFFF00;
        if(flag==1)write_mode=PW;
        else write_mode=PP;
    }
    ptr=pBuffer;
    for(i=0; i<page; i++) {
        ExtFlash_PageProg(ptr, addr,write_mode);
        addr+=256;
        ptr+=256;
    }
}

/*******************************************************************************
 写FLASH页(256 Bytes)： Mode=0: 从0变1数据改写   Mode=1: 数据重写
*******************************************************************************/
void ExtFlash_PageProg(u8* pBuffer, u32 WriteAddr,u8 CMD)
{
    u16   Lenght = 256;

    ExtFlash_CS_HIGH();
    ExtFlash_WaitForWriteEnd();
    ExtFlash_WriteEnable();
    ExtFlash_CS_LOW();
    ExtFlash_SendByte(CMD);
    ExtFlash_SendByte((WriteAddr & 0xFF0000) >> 16);
    ExtFlash_SendByte((WriteAddr & 0xFF00) >> 8);
    ExtFlash_SendByte(WriteAddr & 0xFF);
    while(Lenght--) {         // while there is data to be written on the FLASH
        ExtFlash_SendByte((*pBuffer));
        pBuffer++;
    }
    ExtFlash_CS_HIGH();
    ExtFlash_WaitForWriteEnd();
}

/*******************************************************************************
 SPI_FLASH_BufferRead
*******************************************************************************/
void ExtFlash_PageRD(u8* pBuffer, u32 ReadAddr, u16 Lenght)
{
    ExtFlash_CS_HIGH();
    ExtFlash_WaitForWriteEnd();
    ExtFlash_CS_LOW();
    ExtFlash_SendByte(READ);
    ExtFlash_SendByte((ReadAddr & 0xFF0000) >> 16);
    ExtFlash_SendByte((ReadAddr& 0xFF00) >> 8);
    ExtFlash_SendByte(ReadAddr & 0xFF);

    while(Lenght--) { // while there is data to be read
        *pBuffer = (ExtFlash_SendByte(Dummy_Byte));
        pBuffer++;
    }
    ExtFlash_CS_HIGH();
}
/*******************************************************************************
 SPI_FLASH_ReadByte
*******************************************************************************/
u8 ExtFlash_ReadByte(void)
{
    return (ExtFlash_SendByte(Dummy_Byte));
}

/*******************************************************************************
 SPI_FLASH_SendByte
*******************************************************************************/
u8 ExtFlash_SendByte(u8 byte)
{
    Tout = 0;
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET) {
        if(Tout++ > TMAX) return 255;
    }
    SPI_I2S_SendData(SPI3, byte);
    Tout = 0;
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET) {
        if(Tout++ > TMAX) return 255;
    }
    return SPI_I2S_ReceiveData(SPI3);
}
/*******************************************************************************
 SPI_FLASH_WriteEnable
*******************************************************************************/
void ExtFlash_WriteEnable(void)
{
    ExtFlash_CS_LOW();
    ExtFlash_SendByte(WREN);
    ExtFlash_CS_HIGH();
}
/*******************************************************************************
 SPI_FLASH_WaitForWriteEnd
*******************************************************************************/
void ExtFlash_WaitForWriteEnd(void)
{
    u8 FLASH_Status = 0;

    ExtFlash_CS_LOW();
    ExtFlash_SendByte(RDSR);
    Tout = 0;
    do {
        FLASH_Status = ExtFlash_SendByte(Dummy_Byte);
        if(Tout++ > TMAX) return;
    } while((FLASH_Status & WIP_Flag) == SET);              // Write in progress
    ExtFlash_CS_HIGH();
}
/*******************************************************************************
 MAL_GetStatus
*******************************************************************************/
void MAL_GetStatus (void)
{
    if(flash_mode==FLASH_8M) {
        Mass_Block_Count = 2048;        //FLASH_SIZE/FLASH_PAGE_SIZE;
        Mass_Block_Size  = 512*8;         //FLASH_PAGE_SIZE; 4096
        Mass_Memory_Size = 512*4096*4 ;   //FLASH_SIZE; 0x800000;
    } else {
        Mass_Block_Count = 4096;        //FLASH_SIZE/FLASH_PAGE_SIZE; 4096
        Mass_Block_Size  = 512;         //FLASH_PAGE_SIZE;
        Mass_Memory_Size = 512*4096 ;   //FLASH_SIZE; 0x200000;
    }
}

/*********************************  END OF FILE  ******************************/
