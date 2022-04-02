/**
 * @file fatfs_spi_sd.c
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

#include "diskio.h"
#include "bsp_spi_sd.h"
#include "string.h"

extern const char *FR_Table[];

int sd_disk_status(void)
{
    return 0;
}
int sd_disk_initialize(void)
{
    return SD_Init(&SD_CardInfo);
}
int sd_disk_read(BYTE *buff, LBA_t sector, UINT count)
{
    return SD_ReadBlock(sector, buff, count);
}
int sd_disk_write(const BYTE *buff, LBA_t sector, UINT count)
{
    return SD_WriteBlock(sector, (BYTE *)buff, count);
}
int sd_disk_ioctl(BYTE cmd, void *buff)
{
    int result = 0;

    switch (cmd) {
        case CTRL_SYNC:
            result = RES_OK;
            break;

        case GET_SECTOR_SIZE:
            *(WORD *)buff = SD_CardInfo.CardBlockSize;
            result = RES_OK;
            break;

        case GET_BLOCK_SIZE:
            *(DWORD *)buff = SD_CardInfo.CardBlockSize;
            result = RES_OK;
            break;

        case GET_SECTOR_COUNT:
            *(DWORD *)buff = SD_CardInfo.CardCapacity / SD_CardInfo.CardBlockSize;
            result = RES_OK;
            break;

        default:
            result = RES_PARERR;
            break;
    }

    return result;
}

DSTATUS Translate_Result_Code(int result)
{
    // MSG("%s\r\n",FR_Table[result]);
    return result;
}

void fatfs_sd_driver_register(void)
{
    FATFS_DiskioDriverTypeDef sdDiskioDriver;

    memset(&sdDiskioDriver, 0, sizeof(FATFS_DiskioDriverTypeDef));

    sdDiskioDriver.MMC_disk_status = sd_disk_status;
    sdDiskioDriver.MMC_disk_initialize = sd_disk_initialize;
    sdDiskioDriver.MMC_disk_write = sd_disk_write;
    sdDiskioDriver.MMC_disk_read = sd_disk_read;
    sdDiskioDriver.MMC_disk_ioctl = sd_disk_ioctl;
    sdDiskioDriver.Translate_Result_Code = Translate_Result_Code;
    disk_driver_callback_init(&sdDiskioDriver);
}