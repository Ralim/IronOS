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
#include "hal_gpio.h"

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
#define BLOCK_COUNT 64
#define FLASH_ADDR  0x0000c000 /*addr start from 48k */
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

extern struct device *usb_dc_init(void);

int main(void)
{
    bflb_platform_init(0);

    gpio_set_mode(GPIO_PIN_15, GPIO_INPUT_PD_MODE);

    if (gpio_read(GPIO_PIN_15)) {
        MSG("*************Welcome to usb msc boot,update your app bin*************\r\n");

        usbd_desc_register(msc_ram_descriptor);
        usbd_msc_class_init(MSC_OUT_EP, MSC_IN_EP);

        usb_fs = usb_dc_init();

        if (usb_fs) {
            device_control(usb_fs, DEVICE_CTRL_SET_INT, (void *)(USB_EP4_DATA_OUT_IT | USB_EP5_DATA_IN_IT));
        }
        while (!usb_device_is_configured()) {
        }
    } else {
        uint8_t bin_header;
        flash_read(FLASH_ADDR + 10 * BLOCK_SIZE, &bin_header, 1);
        if ((bin_header == 0x00) && (bin_header == 0xff)) {
            MSG("*************Illegal bin!! Please update your bin*************\r\n");
            while (1) {
            }
        }

        MSG("*************Let's go to application*************\r\n");
        bflb_platform_delay_ms(20);
        bflb_platform_deinit();
        hal_jump2app(FLASH_ADDR + 10 * BLOCK_SIZE);
    }

    while (1) {
        bflb_platform_delay_ms(1000);
    }
}
