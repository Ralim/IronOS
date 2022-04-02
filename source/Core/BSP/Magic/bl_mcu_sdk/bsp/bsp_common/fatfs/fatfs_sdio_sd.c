/**
 * @file fatfs_sdio_sd.c
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

/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"     /* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "bsp_sdio_sdcard.h"

static sd_card_t gSDCardInfo;
extern const char *FR_Table[];

int MMC_disk_status()
{
    return 0;
}

int MMC_disk_initialize()
{
    static uint8_t inited = 0;

    if (inited == 0) {
        if (SDH_Init(SDH_DATA_BUS_WIDTH_4BITS, &gSDCardInfo) == SD_OK) {
            inited = 1;
            return 0;
        } else {
            return -1;
        }
    }

    return 0;
}

int MMC_disk_read(BYTE *buff, LBA_t sector, UINT count)
{
    if (SD_OK == SDH_ReadMultiBlocks(buff, sector, gSDCardInfo.blockSize, count)) {
        return 0;
    } else {
        return -1;
    }
}

int MMC_disk_write(const BYTE *buff, LBA_t sector, UINT count)
{
    if (SD_OK == SDH_WriteMultiBlocks((uint8_t *)buff, sector, gSDCardInfo.blockSize, count)) {
        return 0;
    } else {
        return -1;
    }
}

int MMC_disk_ioctl(BYTE cmd, void *buff)
{
    switch (cmd) {
        // Get R/W sector size (WORD)
        case GET_SECTOR_SIZE:
            *(WORD *)buff = gSDCardInfo.blockSize;
            break;

        // Get erase block size in unit of sector (DWORD)
        case GET_BLOCK_SIZE:
            *(DWORD *)buff = 1;
            break;

        case GET_SECTOR_COUNT:
            *(DWORD *)buff = gSDCardInfo.blockCount;
            break;

        case CTRL_SYNC:
            break;

        default:
            break;
    }

    return 0;
}

DWORD get_fattime(void)
{
    return ((DWORD)(2015 - 1980) << 25) /* Year 2015 */
           | ((DWORD)1 << 21)           /* Month 1 */
           | ((DWORD)1 << 16)           /* Mday 1 */
           | ((DWORD)0 << 11)           /* Hour 0 */
           | ((DWORD)0 << 5)            /* Min 0 */
           | ((DWORD)0 >> 1);           /* Sec 0 */
}

DSTATUS Translate_Result_Code(int result)
{
    // MSG("%s\r\n",FR_Table[result]);
    return result;
}

void fatfs_sdio_driver_register(void)
{
    FATFS_DiskioDriverTypeDef pNewDiskioDriver;

    memset(&pNewDiskioDriver, 0, sizeof(FATFS_DiskioDriverTypeDef));

    pNewDiskioDriver.MMC_disk_status = MMC_disk_status;
    pNewDiskioDriver.MMC_disk_initialize = MMC_disk_initialize;
    pNewDiskioDriver.MMC_disk_write = MMC_disk_write;
    pNewDiskioDriver.MMC_disk_read = MMC_disk_read;
    pNewDiskioDriver.MMC_disk_ioctl = MMC_disk_ioctl;
    pNewDiskioDriver.Translate_Result_Code = Translate_Result_Code;
    disk_driver_callback_init(&pNewDiskioDriver);
}
