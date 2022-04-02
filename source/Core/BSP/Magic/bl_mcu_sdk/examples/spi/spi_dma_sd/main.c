/**
 * @file main.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include "bflb_platform.h"
#include "hal_spi.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "hal_mtimer.h"
#include "ff.h"

uint32_t barebones_clock()
{
    return (mtimer_get_time_ms());
}

#define GETMYTIME(_t)        (*_t = barebones_clock())
#define MYTIMEDIFF(fin, ini) ((fin) - (ini))

static uint32_t start_time_val, stop_time_val, read_start_time_val, read_stop_time_val;
uint32_t total_time;
void start_time(void)
{
    GETMYTIME(&start_time_val);
}

void stop_time(void)
{
    GETMYTIME(&stop_time_val);
}

void read_start_time(void)
{
    GETMYTIME(&read_start_time_val);
}

void read_stop_time(void)
{
    GETMYTIME(&read_stop_time_val);
}

uint32_t get_time(void)
{
    uint32_t runTimer = (uint32_t)(MYTIMEDIFF(stop_time_val, start_time_val));
    return runTimer;
}

uint32_t get_read_time(void)
{
    uint32_t runTimer = (uint32_t)(MYTIMEDIFF(read_stop_time_val, read_start_time_val));
    return runTimer;
}

// static FRESULT miscellaneous(void);
// static FRESULT file_check(char* path);
// static FRESULT scan_files (char* path);
extern void fatfs_sd_driver_register(void);
MKFS_PARM fs_para = {
    .fmt = FM_FAT32, /* Format option (FM_FAT, FM_FAT32, FM_EXFAT and FM_SFD) */
    .n_fat = 1,      /* Number of FATs */
    .align = 0,      /* Data area alignment (sector) */
    .n_root = 1,     /* Number of root directory entries */
    .au_size = 512,  /* Cluster size (byte) */
};
ATTR_DTCM_SECTION char WriteBuffer[] = "The path of the righteous man is beset on all sides by the inequities of the tyranny of evil man.\
Blessed is he who, in the name of charity and good will, shepherds the weak through the valley of darkness, \
for he is truly his brother's keeper and the finder of lost children. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And you will know my name is the lord when i lay my vengeance upon thee. And This test data is Over! 2048 2048 2048 2048.\
The path of the righteous man is beset on all sides by the inequities of the tyranny of evil man.\
Blessed is he who, in the name of charity and good will, shepherds the weak through the valley of darkness, \
for he is truly his brother's keeper and the finder of lost children. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And i will strike down upon thee with great vengeance and furious anger those who attempt to poison and destroy my brothers\r\n. \
And you will know my name is the lord when i lay my vengeance upon thee. And This test data is Over! 2048 2048 2048 2048..";

FATFS fs;
FIL fnew;
FRESULT res_sd;
UINT fnum;
uint8_t ReadBuffer[4096] = { 0 };
FILINFO fno;
char fpath[100];

uint32_t workbuf[4096];

int main(void)
{
    bflb_platform_init(0);
    fatfs_sd_driver_register();
    MSG("FATFS case");
    //mount file system
    res_sd = f_mount(&fs, "1:", 1);
    MSG("f_mount:%d\r\n", res_sd);

    /*----------------------- format test ---------------------------*/
    /*if no file system,then format and create file system*/
    if (res_sd == FR_NO_FILESYSTEM) {
        MSG("No filesystem yet, to be formatted...\r\n");
        /*format*/
        res_sd = f_mkfs("1:", &fs_para, workbuf, 512 * 2);

        if (res_sd == FR_OK) {
            MSG("done with formatting.\r\n");
            /*after format, unmount first*/
            MSG("first start to unmount.\r\n");
            res_sd = f_mount(NULL, "1:", 1);
            /*then mount again*/
            MSG("then start to remount.\r\n");
            res_sd = f_mount(&fs, "1:", 1);
        } else {
            MSG("fail to format.\r\n");

            while (1)
                ;
        }
    } else if (res_sd != FR_OK) {
        MSG("fail to mount filesystem,error= %d\r\n", res_sd);
        MSG("SD card might fail to initialise.\r\n");

        while (1)
            ;
    } else {
        MSG("Succeed to mount filesystem. Go on to read write test.\r\n");
    }

#if 1
    /*-----------------------file system write test-----------------------------*/
    /* open file, create one if not exist*/
    MSG("\r\n*****be about to write test...******\r\n");
    res_sd = f_open(&fnew, "1:ftest.txt", FA_CREATE_ALWAYS | FA_WRITE);

    if (res_sd == FR_OK) {
        MSG("open or create ftest.txt, and write to it.\r\n");
        start_time();

        /*write into file*/
        for (uint16_t i = 0; i < 512; i++) {
            res_sd = f_write(&fnew, WriteBuffer, sizeof(WriteBuffer), &fnum);
        }

        /*close file*/
        f_close(&fnew);
        stop_time();

        if (res_sd == FR_OK) {
            MSG("Done with writing, write %d bytes. \r\n", fnum);
            MSG("Write contents: \r\n%s\r\n", WriteBuffer);
        } else {
            MSG("Fail to write files(%d)\n", res_sd);
        }
    } else {
        MSG("Fail to open or create files.\r\n");
    }

    total_time = get_time();
    MSG("write %d Byte * %d time: %d \n", sizeof(WriteBuffer), 512, total_time);
#endif

    /*------------------- file system read test------------------------------------*/
    MSG("******be about to read test...******\r\n");
    res_sd = f_open(&fnew, "1:ftest.txt", FA_OPEN_EXISTING | FA_READ);

    if (res_sd == FR_OK) {
        MSG("Done with open file.\r\n");
        read_start_time();

        for (uint16_t i = 0; i < 512; i++) {
            res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
        }

        read_stop_time();

        if (res_sd == FR_OK) {
            MSG("Done with reading file, read %d bytes.\r\n", fnum);
            MSG("Read contents: \r\n%s \r\n", ReadBuffer);
        } else {
            MSG("Fail to read file: (%d)\n", res_sd);
        }

        total_time = get_read_time();
        MSG("\n write %d Byte * %d time: %d \n", sizeof(ReadBuffer), 512, total_time);
    } else {
        MSG("Fail to open files.\r\n");
    }

    /*close file*/
    f_close(&fnew);
    /*unmount*/
    f_mount(NULL, "1:", 1);

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}

// static FRESULT miscellaneous(void)
// {
//     DIR dir;
//     FATFS* pfs;
//     DWORD fre_clust, fre_sect, tot_sect;

//     MSG("\n*************** get device info***************\r\n");
//     res_sd = f_getfree("1:", &fre_clust, &pfs);

//     /* toal sector number and free sector number*/
//     tot_sect = (pfs->n_fatent - 2) * pfs->csize;
//     fre_sect = fre_clust * pfs->csize;

//     MSG("total capacity%10lu KB\n available capacity: %10lu KB\n", tot_sect >>1, fre_sect >>1);

//     MSG("\n**********create directory and **********\r\n");

//     res_sd=f_opendir(&dir,"1:TestDir");
//     if(res_sd!=FR_OK)
//     {
//         /*new dir if fail to open*/
//         MSG("\nf_mkdir\r\n");
//         res_sd=f_mkdir("1:TestDir");
//     }
//     else
//     {
//         /*close dir if exist*/
//         res_sd=f_closedir(&dir);
//         /*delete file*/
//         f_unlink("1:TestDir/ftest.txt");
//     }
//     if(res_sd==FR_OK)
//     {
//         /*move and rename file*/
//         res_sd=f_rename("1:ftest.txt","1:TestDir/ftest.txt");
//         if(res_sd==FR_OK){
//             MSG("\nf_rename success\r\n");
//         }else{
//             MSG("\nf_rename fail\r\n");
//         }
//     }

//     return res_sd;
// }
// static FRESULT file_check(char* path)
// {

//     /*get file info*/
//     res_sd=f_stat(path,&fno);
//     if(res_sd==FR_OK)
//     {
//         MSG("%s file infomation:\r\n",path);
//         MSG("\tFile size: %ld(byte)\n", fno.fsize);
//         MSG("\ttimestap:  %u/%02u/%02u, %02u:%02u\n",
//                              (fno.fdate >> 9) + 1980, fno.fdate >> 5 & 15, fno.fdate & 31,fno.ftime >> 11, fno.ftime >> 5 & 63);
//         MSG("\tttribute: %c%c%c%c%c\n\n",
//                              (fno.fattrib & AM_DIR) ? 'D' : '-',      /* Directory */
//                              (fno.fattrib & AM_RDO) ? 'R' : '-',      /* Read only */
//                              (fno.fattrib & AM_HID) ? 'H' : '-',      /* Hidden */
//                              (fno.fattrib & AM_SYS) ? 'S' : '-',      /* System */
//                              (fno.fattrib & AM_ARC) ? 'A' : '-');     /* Archive */
//     }else{
//         MSG("file_check fail:\r\n",path);
//     }
//     return res_sd;
// }

// static FRESULT scan_files (char* path)
// {
//     FRESULT res;
//     FILINFO fno;
//     DIR dir;
//     int i;
//     char* fn;

//     res = f_opendir(&dir, path);
//     if (res == FR_OK)
//     {
//         i = strlen(path);
//         for (;;)
//         {
//             //read contents under dir, next read will get next content
//             res = f_readdir(&dir, &fno);
//             //last name is blank, jump
//             if (res != FR_OK || fno.fname[0] == 0) break;
//             fn = fno.fname;
//             //jump current dir
//             if (*fn == '.') continue;
//             //recursive dir
//             if (fno.fattrib & AM_DIR)
//             {
//                 //get full dir name
//                 sprintf(&path[i], "/%s", fn);
//                 //recursive
//                 res = scan_files(path);
//                 path[i] = 0;
//                 if (res != FR_OK)
//                     break;
//             }
//             else
//             {
//                 MSG("\t %s/%s\r\n", path, fn);

//             }
//         }
//     }else{
//         MSG("f_opendir fail\r\n");
//     }
//     return res;
// }
