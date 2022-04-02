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
#include "hal_usb.h"
#include "usbd_core.h"
#include "usbd_msc.h"
#include "hal_flash.h"
#include "ff.h"

#define MSC_IN_EP  0x85
#define MSC_OUT_EP 0x04

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define USB_CONFIG_SIZE (9 + MSC_DESCRIPTOR_LEN)

USB_DESC_SECTION const uint8_t msc_ram_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    MSC_DESCRIPTOR_INIT(0x00, MSC_OUT_EP, MSC_IN_EP, 0x02),
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x12,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'B', 0x00,                  /* wcChar0 */
    'o', 0x00,                  /* wcChar1 */
    'u', 0x00,                  /* wcChar2 */
    'f', 0x00,                  /* wcChar3 */
    'f', 0x00,                  /* wcChar4 */
    'a', 0x00,                  /* wcChar5 */
    'l', 0x00,                  /* wcChar6 */
    'o', 0x00,                  /* wcChar7 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x24,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'B', 0x00,                  /* wcChar0 */
    'o', 0x00,                  /* wcChar1 */
    'u', 0x00,                  /* wcChar2 */
    'f', 0x00,                  /* wcChar3 */
    'f', 0x00,                  /* wcChar4 */
    'a', 0x00,                  /* wcChar5 */
    'l', 0x00,                  /* wcChar6 */
    'o', 0x00,                  /* wcChar7 */
    ' ', 0x00,                  /* wcChar8 */
    'M', 0x00,                  /* wcChar9 */
    'S', 0x00,                  /* wcChar10 */
    'C', 0x00,                  /* wcChar11 */
    ' ', 0x00,                  /* wcChar12 */
    'D', 0x00,                  /* wcChar13 */
    'E', 0x00,                  /* wcChar14 */
    'M', 0x00,                  /* wcChar15 */
    'O', 0x00,                  /* wcChar16 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '1', 0x00,                  /* wcChar3 */
    '0', 0x00,                  /* wcChar4 */
    '3', 0x00,                  /* wcChar5 */
    '1', 0x00,                  /* wcChar6 */
    '0', 0x00,                  /* wcChar7 */
    '0', 0x00,                  /* wcChar8 */
    '0', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

/*The following parameters need to be modified simultaneously in bsp/bsp_common/fatfs/fatfs_usb.c!!*/
#define BLOCK_SIZE  4096
#define BLOCK_COUNT 256
#define FLASH_ADDR  0x00040000 /*addr start from 256k */
void usbd_msc_get_cap(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
    *block_num = BLOCK_COUNT;
    *block_size = BLOCK_SIZE;
}
int usbd_msc_sector_read(uint32_t sector, uint8_t *buffer, uint32_t length)
{
    flash_read(FLASH_ADDR + sector * BLOCK_SIZE, buffer, length);
    return 0;
}

int usbd_msc_sector_write(uint32_t sector, uint8_t *buffer, uint32_t length)
{
    flash_erase(FLASH_ADDR + sector * BLOCK_SIZE, BLOCK_SIZE);
    flash_write(FLASH_ADDR + sector * BLOCK_SIZE, buffer, length);
    return 0;
}

struct device *usb_fs;
FATFS fs;
FIL fnew;
UINT fnum;
FRESULT res_sd = 0;
uint8_t workbuf[4096];
uint8_t ReadBuffer[4096] = { 0 };
MKFS_PARM fs_para = {
    .fmt = FM_FAT32, /* Format option (FM_FAT, FM_FAT32, FM_EXFAT and FM_SFD) */
    .n_fat = 1,      /* Number of FATs */
    .align = 0,      /* Data area alignment (sector) */
    .n_root = 1,     /* Number of root directory entries */
    .au_size = 4096, /* Cluster size (byte) */
};
extern struct device *usb_dc_init(void);
extern void fatfs_usb_driver_register(void);
int main(void)
{
    bflb_platform_init(0);

    fatfs_usb_driver_register();

    usbd_desc_register(msc_ram_descriptor);
    usbd_msc_class_init(MSC_OUT_EP, MSC_IN_EP);

    usb_fs = usb_dc_init();

    if (usb_fs) {
        device_control(usb_fs, DEVICE_CTRL_SET_INT, (void *)(USB_EP4_DATA_OUT_IT | USB_EP5_DATA_IN_IT));
    }
    while (!usb_device_is_configured()) {
    }

    while (1) {
        cpu_global_irq_disable();
        /*mount*/
        f_mount(&fs, "2:", 1);
        res_sd = f_open(&fnew, "2:1234.c", FA_OPEN_EXISTING | FA_READ);
        if (res_sd == FR_OK) {
            res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
            bflb_platform_dump(ReadBuffer, fnum);
            f_close(&fnew);
            /*unmount*/
            f_mount(NULL, "2:", 1);

        } else {
            MSG("open error\r\n");
        }
        cpu_global_irq_enable();
        bflb_platform_delay_ms(2000);
    }
}
