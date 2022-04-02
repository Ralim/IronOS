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
#include "usbd_cdc.h"
#include "usbd_msc.h"
#include "uart_interface.h"

#define CDC_IN_EP  0x82
#define CDC_OUT_EP 0x01
#define CDC_INT_EP 0x83
#define MSC_IN_EP  0x85
#define MSC_OUT_EP 0x04

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN + 8 + MSC_DESCRIPTOR_LEN)

USB_DESC_SECTION const uint8_t cdc_msc_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x03, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, 0x02),
    ///////////////////////////////////////
    /// interface association descriptor
    ///////////////////////////////////////
    0x08,                                      /* bLength */
    USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION, /* bDescriptorType */
    0x02,                                      /* bFirstInterface */
    0x01,                                      /* bInterfaceCount */
    0x08,                                      /* bFunctionClass */
    0x06,                                      /* bFunctionSubClass */
    0x50,                                      /* bFunctionProtocol */
    0x00,                                      /* iFunction */

    MSC_DESCRIPTOR_INIT(0x02, MSC_OUT_EP, MSC_IN_EP, 0x00),
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
    0x22,                       /* bLength */
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
    'C', 0x00,                  /* wcChar9 */
    'D', 0x00,                  /* wcChar10 */
    'C', 0x00,                  /* wcChar11 */
    '_', 0x00,                  /* wcChar12 */
    'M', 0x00,                  /* wcChar14 */
    'S', 0x00,                  /* wcChar15 */
    'C', 0x00,                  /* wcChar16 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'b', 0x00,                  /* wcChar0 */
    'l', 0x00,                  /* wcChar1 */
    '7', 0x00,                  /* wcChar2 */
    '0', 0x00,                  /* wcChar3 */
    '2', 0x00,                  /* wcChar4 */
    '1', 0x00,                  /* wcChar5 */
    '2', 0x00,                  /* wcChar6 */
    '3', 0x00,                  /* wcChar7 */
    '4', 0x00,                  /* wcChar8 */
    '5', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x02,
    0x02,
    0x01,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

#define BLOCK_SIZE  512
#define BLOCK_COUNT 32

typedef struct
{
    uint8_t BlockSpace[BLOCK_SIZE];
} BLOCK_TYPE;

// The CDC recv buffer size should equal to the out endpoint size
// or we will need a timeout to flush the recv buffer
BLOCK_TYPE mass_block[BLOCK_COUNT] __attribute__((section(".system_ram")));

extern struct usbd_interface_cfg usb_msc;

void usbd_msc_get_cap(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
    *block_num = BLOCK_COUNT;
    *block_size = BLOCK_SIZE;
}
int usbd_msc_sector_read(uint32_t sector, uint8_t *buffer, uint32_t length)
{
    memcpy(buffer, mass_block[sector].BlockSpace, length);
    return 0;
}

int usbd_msc_sector_write(uint32_t sector, uint8_t *buffer, uint32_t length)
{
    memcpy(mass_block[sector].BlockSpace, buffer, length);
    return 0;
}

struct device *usb_fs;

extern struct usbd_interface_cfg usb_cdc;

void usbd_cdc_acm_bulk_out(uint8_t ep)
{
    usb_dc_receive_to_ringbuffer(usb_fs, &usb_rx_rb, ep);
}

void usbd_cdc_acm_bulk_in(uint8_t ep)
{
    usb_dc_send_from_ringbuffer(usb_fs, &uart1_rx_rb, ep);
}
void usbd_cdc_acm_set_line_coding(uint32_t baudrate, uint8_t databits, uint8_t parity, uint8_t stopbits)
{
    uart1_config(baudrate, databits, parity, stopbits);
}

void usbd_cdc_acm_set_dtr(bool dtr)
{
    dtr_pin_set(dtr);
}

void usbd_cdc_acm_set_rts(bool rts)
{
    rts_pin_set(rts);
}

usbd_class_t cdc_class;

usbd_interface_t cdc_cmd_intf;
usbd_interface_t cdc_data_intf;

usbd_endpoint_t cdc_out_ep = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = usbd_cdc_acm_bulk_out
};

usbd_endpoint_t cdc_in_ep = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in
};

extern struct device *usb_dc_init(void);

int main(void)
{
    bflb_platform_init(0);
    uart_ringbuffer_init();
    uart1_init();
    uart1_dtr_init();
    uart1_rts_init();

    usbd_desc_register(cdc_msc_descriptor);

    usbd_cdc_add_acm_interface(&cdc_class, &cdc_cmd_intf);
    usbd_cdc_add_acm_interface(&cdc_class, &cdc_data_intf);
    usbd_interface_add_endpoint(&cdc_data_intf, &cdc_out_ep);
    usbd_interface_add_endpoint(&cdc_data_intf, &cdc_in_ep);

    usbd_msc_class_init(MSC_OUT_EP, MSC_IN_EP);

    usb_fs = usb_dc_init();

    if (usb_fs) {
        device_control(usb_fs, DEVICE_CTRL_SET_INT, (void *)(USB_EP1_DATA_OUT_IT | USB_EP2_DATA_IN_IT | USB_EP4_DATA_OUT_IT | USB_EP5_DATA_IN_IT));
    }

    while (1) {
        uart_send_from_ringbuffer();
    }
}
