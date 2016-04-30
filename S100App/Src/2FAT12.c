/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. ********************                       
File Name :      2FAT12.c
Version :        S100 APP Ver 2.11   
Description:
Author :         bure & Celery
Data:            2015/08/03
History:
2015/08/03   统一命名；
*******************************************************************************/
#include <string.h>
#include "FAT12.h"
#include "BIOS.h"


#define FAT_LEN      0x1800
#define FAT1_BASE    0x00001000   // FAT1区开始地址
#define FAT2_BASE    0x00002800   // FAT2区开始地址
#define ROOT_BASE    0x00004000   // 根目录开始地址
#define FILE_BASE    0x00008000   // 文件区开始地址
#define SEC_LEN      0x200        // 扇区长度
#define FAT1_SEC     0x0C         // FAT1扇区数
#define FAT2_SEC     0x0C         // FAT2扇区数

#define OK           0            // 操作完成
#define SEC_ERR      1            // 扇区读写错误
#define FAT_ERR      2            // FAT表读写错误
#define OVER         3            // 操作溢出
#define NEW          4            // 新目录项
#define END          0xFFF        // 链接结束

#define OW           0            // 或写(数据从0变1改写)
#define RW           1            // 重写


/*******************************************************************************
 查找下一个链接簇号后返回
*******************************************************************************/
u8 NextCluster(u16* pCluster)
{
    u16 FatNum;
    u32 Addr = FAT1_BASE +(*pCluster + *pCluster/2);

    *(pCluster+1)= *pCluster;                                   // 保存前一个簇号
//  *(pCluster+1)= 0;
    if((*pCluster >= END)||(*pCluster < 2)) return OK;
    if(ReadDiskData((u8*)&FatNum, Addr, 2)!= OK) return SEC_ERR;
    *pCluster = (*pCluster & 1)?(FatNum >>4):(FatNum & 0x0FFF); // 指向下一个簇号
    return OK;
}
/*******************************************************************************
 读文件扇区(512 Bytes), 返回下一个簇号
*******************************************************************************/
u8 ReadFileSec(u8* pBuffer, u16* pCluster)
{
    u32 ReadAddr = FILE_BASE + SEC_LEN*(*pCluster-2);

    if(ReadDiskData(pBuffer, ReadAddr, 256)!=OK) return SEC_ERR; // 读前半扇区
    pBuffer += 256;
    ReadAddr += 256;
    if(ReadDiskData(pBuffer, ReadAddr, 256)!=OK) return SEC_ERR; // 读后半扇区
    if(NextCluster(pCluster)!=0) return FAT_ERR;                 // 取下一个簇号
    return OK;
}
/*******************************************************************************
 写文件扇区(512 Bytes)，填写当前FAT表及返回查找到的下一个簇号
*******************************************************************************/
u8 ProgFileSec(u8* pBuffer, u16* pCluster)
{
    u16 Tmp;
    u32 ProgAddr = FILE_BASE + SEC_LEN*(*pCluster-2);

    if(ProgDiskPage(pBuffer, ProgAddr)!= OK) return SEC_ERR; // 写前半扇区
    pBuffer += 256;
    ProgAddr += 256;
    if(ProgDiskPage(pBuffer, ProgAddr)!= OK) return SEC_ERR; // 写后半扇区
    Tmp = *pCluster;
    switch(Tmp) {
    case 0:                                                // 空闲簇号
    case 1:                                                // 空闲簇号
        if(SeekBlank (pBuffer, pCluster )!= OK) return OVER;
        if(SetCluster(pBuffer, pCluster )!= OK) return SEC_ERR;
        *(pCluster+1) = Tmp;
        return OK;
    case END:                                              // 链接结束
    default :
        if(NextCluster(pCluster)!= OK) return FAT_ERR;       // 取下一个簇号
        return OK;
    }
}
/*******************************************************************************
 查找空闲簇号，包含数据校对及纠错，返回值包括当前空闲簇号及下一个空闲簇号
*******************************************************************************/
u8 SeekBlank(u8* pBuffer, u16* pCluster)
{
    u16  Offset, Tmp, i, n = 0;
    u32  SecAddr;

    for(i=0; i<4096; i++) {
        Offset = i + i/2;
        if((Offset%256)== 0) {
            SecAddr = FAT1_BASE +(Offset &(~0xFF));
            if(ReadDiskData(pBuffer, SecAddr, 258)!= 0) return SEC_ERR;
        }
        Offset %= 256;
        Tmp = pBuffer[Offset] + pBuffer[Offset+1] <<8;
        Tmp = (i & 1)?(Tmp >>4):(Tmp & 0xFFF);
        if(Tmp == 0) {
            *pCluster++ = i;
            n++;
            if(n > 1) return OK;
        }
    }
    *(pCluster+1)= 0xFFF;
    return OK;
}
/*******************************************************************************
 将下一个簇号写入FAT表当前簇号链接位置，返回时当前簇号为原下一个簇号
*******************************************************************************/
u8 SetCluster(u8* pBuffer, u16* pCluster)
{
    u16  Offset, Tmp, i, k;
    u32  SecAddr;

    i = *pCluster;                        // 提取原当前簇号
    k = *(pCluster+1);                    // 提取下一簇号
    *pCluster = k;
    Offset = i + i/2;
    SecAddr = FAT1_BASE +(Offset &(~0xFF));
    Tmp = Offset & 0xFF;
    if(ReadDiskData(pBuffer, SecAddr, 256)!= 0) return SEC_ERR;
    if(i & 1) pBuffer[Tmp]=((k<<4)& 0xF0)+(pBuffer[Tmp]& 0x0F);
    else      pBuffer[Tmp]= k;
    if(Tmp++ < 256) {
        if(i & 1) pBuffer[Tmp]= k>>4;
        else      pBuffer[Tmp]=((k>>8)& 0x0F)+(pBuffer[Tmp]& 0xF0);
        if(ProgDiskPage(pBuffer, SecAddr)!= 0) return SEC_ERR;
    } else {
        if(ProgDiskPage(pBuffer, SecAddr)!= 0) return SEC_ERR;
        SecAddr += 256;
        if(ReadDiskData(pBuffer, SecAddr, 256)!= 0) return SEC_ERR;
        if(i & 1) pBuffer[0]= k>>4;
        else      pBuffer[0]=((k>>8)& 0x0F)+(pBuffer[0]& 0xF0);
        if(ProgDiskPage(pBuffer, SecAddr)!= 0) return SEC_ERR;
    }
    return OK;
}
/*******************************************************************************

*******************************************************************************/
u8 SearchFile(u8* pBuffer, u8* pFileName, u16* pCluster, u32* pDirAddr,u32* flag)
{
    u16 i, n;

    *pCluster = 0;
    for(*pDirAddr=ROOT_BASE; *pDirAddr<FILE_BASE; ) {
        if(ReadDiskData(pBuffer, *pDirAddr, 256)!= OK) return SEC_ERR;
        for(n=0; n<256; n+=32) {
            for(i=0; i<4; i++) {
                if(pBuffer[n + i]!= 0) {
                    if(pBuffer[n + i]!= pFileName[i]) break;
                    if(i == 3) {                            // 找到文件名
                        *pCluster = *(u16*)(pBuffer + n + 0x1A); // 文件第一个簇号
                        return OK;
                    }
                } else return NEW;               // 遇到第一个空白目录项后返回
            }
            *pDirAddr += 32;
        }
    }
    return OVER;
}

/*******************************************************************************
 读模式打开文件：返回文件第一个簇号及目录项地址或 0簇号及第一个空白目录项地址
*******************************************************************************/
u8 OpenFileRd(u8* pBuffer, u8* pFileName, u16* pCluster, u32* pDirAddr)
{
    u16 i, n;

    *pCluster = 0;
    for(*pDirAddr=ROOT_BASE; *pDirAddr<FILE_BASE; ) {
        if(ReadDiskData(pBuffer, *pDirAddr, 256)!= OK) return SEC_ERR;
        for(n=0; n<256; n+=32) {
            for(i=0; i<11; i++) {
                if(pBuffer[n + i]!= 0) {
                    if(pBuffer[n + i]!= pFileName[i]) break;
                    if(i == 10) {                            // 找到文件名
                        *pCluster = *(u16*)(pBuffer + n + 0x1A); // 文件第一个簇号
                        return OK;
                    }
                } else return NEW;               // 遇到第一个空白目录项后返回
            }
            *pDirAddr += 32;
        }
    }
    return OVER;
}
/*******************************************************************************
 写模式打开文件：返回文件第一个簇号及目录项地址
*******************************************************************************/
u8 OpenFileWr(u8* pBuffer, u8* pFileName, u16* pCluster, u32* pDirAddr)
{
    u16 i, n;

    i = OpenFileRd(pBuffer, pFileName, pCluster, pDirAddr);
    if(i != NEW) return i;
    else {                                                   // 当前项为空白目录项
        if(SeekBlank(pBuffer, pCluster)!= OK) return OVER;     // 若FAT表满返回
        n =*pDirAddr & 0xFF;                                   // n为当前页目录号
        if(ReadDiskData(pBuffer,(*pDirAddr)-n, 256)!= OK) return SEC_ERR;
        for(i=0; i<11; i++) pBuffer[n + i]= pFileName[i];      // 创建新目录项
        *(u16*)(pBuffer + n + 0x1A)= *pCluster;
        if(ProgDiskPage(pBuffer,(*pDirAddr)-n)!= OK) return SEC_ERR;
        return OK;
    }
}
/*******************************************************************************
 关闭文件，将结束符写入FAT表，将文件长度写入目录项，复制FAT1到FAT2
*******************************************************************************/
u8 CloseFile(u8* pBuffer, u32 Lenght, u16* pCluster, u32* pDirAddr)
{
    u16 n;

//  *pCluster     = *(pCluster+1); // 提取前一个簇号
    *(pCluster+1) = 0xFFF;
    SetCluster(pBuffer, pCluster);
    if(ReadDiskData(pBuffer, (*pDirAddr &(~0xFF)), 256)!= OK) return SEC_ERR;
    *(u8* )(pBuffer +(*pDirAddr & 0xFF)+ 0x0B)= 0x20;
    *(u32*)(pBuffer +(*pDirAddr & 0xFF)+ 0x1C)= Lenght;
    if(ProgDiskPage(pBuffer, (*pDirAddr &(~0xFF)))!= OK) return SEC_ERR;
    for(n=0; n<FAT1_SEC; n++) {
        if(ReadDiskData(pBuffer, FAT1_BASE+n*256, 256)!= OK) return SEC_ERR;
        if(ProgDiskPage(pBuffer, FAT2_BASE+n*256     )!= OK) return SEC_ERR;
    }
    return OK;
}
/******************************** END OF FILE *********************************/
