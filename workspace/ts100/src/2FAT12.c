/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. ********************                       
File Name :      2FAT12.c
Version :        S100 APP Ver 2.11   
Description:
Author :         bure & Celery
Data:            2015/08/03
History:
2015/08/03   ͳһ������
*******************************************************************************/
#include <string.h>
#include "FAT12.h"
#include "Bios.h"


#define FAT_LEN      0x1800
#define FAT1_BASE    0x00001000   // FAT1����ʼ��ַ
#define FAT2_BASE    0x00002800   // FAT2����ʼ��ַ
#define ROOT_BASE    0x00004000   // ��Ŀ¼��ʼ��ַ
#define FILE_BASE    0x00008000   // �ļ�����ʼ��ַ
#define SEC_LEN      0x200        // ��������
#define FAT1_SEC     0x0C         // FAT1������
#define FAT2_SEC     0x0C         // FAT2������

#define OK           0            // �������
#define SEC_ERR      1            // ������д����
#define FAT_ERR      2            // FAT���д����
#define OVER         3            // �������
#define NEW          4            // ��Ŀ¼��
#define END          0xFFF        // ���ӽ���

#define OW           0            // ��д(���ݴ�0��1��д)
#define RW           1            // ��д


/*******************************************************************************
 ������һ�����Ӵغź󷵻�
*******************************************************************************/
u8 NextCluster(u16* pCluster)
{
    u16 FatNum;
    u32 Addr = FAT1_BASE +(*pCluster + *pCluster/2);

    *(pCluster+1)= *pCluster;                                   // ����ǰһ���غ�
//  *(pCluster+1)= 0;
    if((*pCluster >= END)||(*pCluster < 2)) return OK;
    if(ReadDiskData((u8*)&FatNum, Addr, 2)!= OK) return SEC_ERR;
    *pCluster = (*pCluster & 1)?(FatNum >>4):(FatNum & 0x0FFF); // ָ����һ���غ�
    return OK;
}
/*******************************************************************************
 ���ļ�����(512 Bytes), ������һ���غ�
*******************************************************************************/
u8 ReadFileSec(u8* pBuffer, u16* pCluster)
{
    u32 ReadAddr = FILE_BASE + SEC_LEN*(*pCluster-2);

    if(ReadDiskData(pBuffer, ReadAddr, 256)!=OK) return SEC_ERR; // ��ǰ������
    pBuffer += 256;
    ReadAddr += 256;
    if(ReadDiskData(pBuffer, ReadAddr, 256)!=OK) return SEC_ERR; // ���������
    if(NextCluster(pCluster)!=0) return FAT_ERR;                 // ȡ��һ���غ�
    return OK;
}
/*******************************************************************************
 д�ļ�����(512 Bytes)����д��ǰFAT�����ز��ҵ�����һ���غ�
*******************************************************************************/
u8 ProgFileSec(u8* pBuffer, u16* pCluster)
{
    u16 Tmp;
    u32 ProgAddr = FILE_BASE + SEC_LEN*(*pCluster-2);

    if(ProgDiskPage(pBuffer, ProgAddr)!= OK) return SEC_ERR; // дǰ������
    pBuffer += 256;
    ProgAddr += 256;
    if(ProgDiskPage(pBuffer, ProgAddr)!= OK) return SEC_ERR; // д�������
    Tmp = *pCluster;
    switch(Tmp) {
    case 0:                                                // ���дغ�
    case 1:                                                // ���дغ�
        if(SeekBlank (pBuffer, pCluster )!= OK) return OVER;
        if(SetCluster(pBuffer, pCluster )!= OK) return SEC_ERR;
        *(pCluster+1) = Tmp;
        return OK;
    case END:                                              // ���ӽ���
    default :
        if(NextCluster(pCluster)!= OK) return FAT_ERR;       // ȡ��һ���غ�
        return OK;
    }
}
/*******************************************************************************
 ���ҿ��дغţ���������У�Լ���������ֵ������ǰ���дغż���һ�����дغ�
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
        Tmp = pBuffer[Offset] + (pBuffer[Offset+1] <<8);
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
 ����һ���غ�д��FAT��ǰ�غ�����λ�ã�����ʱ��ǰ�غ�Ϊԭ��һ���غ�
*******************************************************************************/
u8 SetCluster(u8* pBuffer, u16* pCluster)
{
    u16  Offset, Tmp, i, k;
    u32  SecAddr;

    i = *pCluster;                        // ��ȡԭ��ǰ�غ�
    k = *(pCluster+1);                    // ��ȡ��һ�غ�
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
u8 FAT_SearchFile(u8* pBuffer, u8* pFileName, u16* pCluster, u32* pDirAddr,u32* flag)
{
    u16 i, n;

    *pCluster = 0;
    for(*pDirAddr=ROOT_BASE; *pDirAddr<FILE_BASE; ) {
        if(ReadDiskData(pBuffer, *pDirAddr, 256)!= OK) return SEC_ERR;
        for(n=0; n<256; n+=32) {
            for(i=0; i<4; i++) {
                if(pBuffer[n + i]!= 0) {
                    if(pBuffer[n + i]!= pFileName[i]) break;
                    if(i == 3) {                            // �ҵ��ļ���
                        *pCluster = *(u16*)(pBuffer + n + 0x1A); // �ļ���һ���غ�
                        return OK;
                    }
                } else return NEW;               // ������һ���հ�Ŀ¼��󷵻�
            }
            *pDirAddr += 32;
        }
    }
    return OVER;
}

/*******************************************************************************
 ��ģʽ���ļ��������ļ���һ���غż�Ŀ¼���ַ�� 0�غż���һ���հ�Ŀ¼���ַ
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
                    if(i == 10) {                            // �ҵ��ļ���
                        *pCluster = *(u16*)(pBuffer + n + 0x1A); // �ļ���һ���غ�
                        return OK;
                    }
                } else return NEW;               // ������һ���հ�Ŀ¼��󷵻�
            }
            *pDirAddr += 32;
        }
    }
    return OVER;
}
/*******************************************************************************
 дģʽ���ļ��������ļ���һ���غż�Ŀ¼���ַ
*******************************************************************************/
u8 OpenFileWr(u8* pBuffer, u8* pFileName, u16* pCluster, u32* pDirAddr)
{
    u16 i, n;

    i = OpenFileRd(pBuffer, pFileName, pCluster, pDirAddr);
    if(i != NEW) return i;
    else {                                                   // ��ǰ��Ϊ�հ�Ŀ¼��
        if(SeekBlank(pBuffer, pCluster)!= OK) return OVER;     // ��FAT��������
        n =*pDirAddr & 0xFF;                                   // nΪ��ǰҳĿ¼��
        if(ReadDiskData(pBuffer,(*pDirAddr)-n, 256)!= OK) return SEC_ERR;
        for(i=0; i<11; i++) pBuffer[n + i]= pFileName[i];      // ������Ŀ¼��
        *(u16*)(pBuffer + n + 0x1A)= *pCluster;
        if(ProgDiskPage(pBuffer,(*pDirAddr)-n)!= OK) return SEC_ERR;
        return OK;
    }
}
/*******************************************************************************
 �ر��ļ�����������д��FAT�����ļ�����д��Ŀ¼�����FAT1��FAT2
*******************************************************************************/
u8 CloseFile(u8* pBuffer, u32 Lenght, u16* pCluster, u32* pDirAddr)
{
    u16 n;

//  *pCluster     = *(pCluster+1); // ��ȡǰһ���غ�
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
