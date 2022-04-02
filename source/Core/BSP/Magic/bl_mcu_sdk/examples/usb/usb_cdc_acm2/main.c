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

#define CDC_IN_EP  0x82
#define CDC_OUT_EP 0x01
#define CDC_INT_EP 0x83

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN + CDC_ACM_DESCRIPTOR_LEN)

USB_DESC_SECTION const uint8_t cdc_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x02, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x04, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, 0x85, CDC_OUT_EP, CDC_IN_EP, 0x02),
    CDC_ACM_DESCRIPTOR_INIT(0x02, 0x86, 0x04, 0x83, 0x02),
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
    'C', 0x00,                  /* wcChar9 */
    'D', 0x00,                  /* wcChar10 */
    'C', 0x00,                  /* wcChar11 */
    ' ', 0x00,                  /* wcChar13 */
    'D', 0x00,                  /* wcChar14 */
    'E', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    'O', 0x00,                  /* wcChar17 */
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
    0x02,
    0x02,
    0x01,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

bool zlp_flag = false;

uint32_t actual_write_length = 0;
uint32_t total_recv_length = 0;
uint32_t rx_pos = 0;
uint8_t tx_pos = 0;
uint8_t out_buffer[4096];

void usbd_cdc_acm_bulk_out(uint8_t ep)
{
    uint32_t actual_read_length = 0;

    if (usbd_ep_read(ep, &out_buffer[rx_pos], 64, &actual_read_length) < 0) {
        USBD_LOG_DBG("Read DATA Packet failed\r\n");
        usbd_ep_set_stall(ep);
        return;
    }

    usbd_ep_read(ep, NULL, 0, NULL);
    rx_pos += actual_read_length;
    total_recv_length += actual_read_length;
    actual_write_length = total_recv_length;

    if (rx_pos > 4096) {
        rx_pos = 0;
    }
}

void usbd_cdc_acm_bulk_in(uint8_t ep)
{
    if ((zlp_flag == false) && actual_write_length) {
        if (actual_write_length > 64) {
            usbd_ep_write(ep, (uint8_t *)&out_buffer[tx_pos], 64, NULL);
            actual_write_length -= 64;
            tx_pos += 64;
        } else {
            usbd_ep_write(ep, (uint8_t *)&out_buffer[tx_pos], actual_write_length, NULL);
            actual_write_length = 0;
            tx_pos = 0;
            total_recv_length = 0;
        }

        if (!actual_write_length && !(total_recv_length % 64)) {
            MSG("zlp\r\n");
            zlp_flag = true;
        }
    } else if (zlp_flag) {
        usbd_ep_write(ep, NULL, 0, NULL);
        zlp_flag = false;
    }
}

usbd_class_t cdc_class1;
usbd_interface_t cdc_cmd_intf1;
usbd_interface_t cdc_data_intf1;

usbd_endpoint_t cdc_out_ep1 = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = usbd_cdc_acm_bulk_out
};

usbd_endpoint_t cdc_in_ep1 = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in
};

usbd_class_t cdc_class2;
usbd_interface_t cdc_cmd_intf2;
usbd_interface_t cdc_data_intf2;

usbd_endpoint_t cdc_out_ep2 = {
    .ep_addr = 0x04,
    .ep_cb = usbd_cdc_acm_bulk_out
};

usbd_endpoint_t cdc_in_ep2 = {
    .ep_addr = 0x83,
    .ep_cb = usbd_cdc_acm_bulk_in
};

struct device *usb_fs;

extern struct device *usb_dc_init(void);
int main(void)
{
    bflb_platform_init(0);

    usbd_desc_register(cdc_descriptor);

    usbd_cdc_add_acm_interface(&cdc_class1, &cdc_cmd_intf1);
    usbd_cdc_add_acm_interface(&cdc_class1, &cdc_data_intf1);
    usbd_interface_add_endpoint(&cdc_data_intf1, &cdc_out_ep1);
    usbd_interface_add_endpoint(&cdc_data_intf1, &cdc_in_ep1);

    usbd_cdc_add_acm_interface(&cdc_class2, &cdc_cmd_intf2);
    usbd_cdc_add_acm_interface(&cdc_class2, &cdc_data_intf2);
    usbd_interface_add_endpoint(&cdc_data_intf2, &cdc_out_ep2);
    usbd_interface_add_endpoint(&cdc_data_intf2, &cdc_in_ep2);

    usb_fs = usb_dc_init();

    if (usb_fs) {
        device_control(usb_fs, DEVICE_CTRL_SET_INT, (void *)(USB_EP1_DATA_OUT_IT | USB_EP2_DATA_IN_IT | USB_EP4_DATA_OUT_IT | USB_EP3_DATA_IN_IT));
    }

    while (1) {
    }
}
