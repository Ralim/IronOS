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
#include "uart_interface.h"
#include "bl702_ef_ctrl.h"
#include "bl702_glb.h"
#include "hal_gpio.h"
#include "dev_cfg.h"

#define CDC_IN_EP  0x82
#define CDC_OUT_EP 0x01
#define CDC_INT_EP 0x83

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN)

#define USB_CDC_RESET_FILTER_PATTERN "BOUFFALOLAB5555RESET"

uint8_t cdc_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x02, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, 0x02),
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
    0x20,                       /* bLength */
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
    'S', 0x00,                  /* wcChar9 */
    'e', 0x00,                  /* wcChar10 */
    'r', 0x00,                  /* wcChar11 */
    'i', 0x00,                  /* wcChar13 */
    'a', 0x00,                  /* wcChar14 */
    'l', 0x00,                  /* wcChar15 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x30,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'F', 0x00,                  /* wcChar0 */
    'a', 0x00,                  /* wcChar1 */
    'c', 0x00,                  /* wcChar2 */
    't', 0x00,                  /* wcChar3 */
    'o', 0x00,                  /* wcChar4 */
    'r', 0x00,                  /* wcChar5 */
    'y', 0x00,                  /* wcChar6 */
    'A', 0x00,                  /* wcChar7 */
    'I', 0x00,                  /* wcChar8 */
    'O', 0x00,                  /* wcChar9 */
    'T', 0x00,                  /* wcChar10 */
    ' ', 0x00,                  /* wcChar11 */
    'P', 0x00,                  /* wcChar12 */
    'r', 0x00,                  /* wcChar13 */
    'o', 0x00,                  /* wcChar14 */
    'g', 0x00,                  /* wcChar15 */
    ' ', 0x00,                  /* wcChar16 */
    'S', 0x00,                  /* wcChar17 */
    'e', 0x00,                  /* wcChar18 */
    'r', 0x00,                  /* wcChar19 */
    'i', 0x00,                  /* wcChar20 */
    'a', 0x00,                  /* wcChar21 */
    'l', 0x00,                  /* wcChar22 */
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
struct device *usb_fs;

uint8_t filter_buf[sizeof(USB_CDC_RESET_FILTER_PATTERN) + 1 + 1] = { 0 };

static void hexarr2string(uint8_t *hexarray, int length, uint8_t *string)
{
    unsigned char num2string_table[] = "0123456789ABCDEF";
    int i = 0;

    while (i < length) {
        *(string++) = num2string_table[((hexarray[i] >> 4) & 0x0f)];
        *(string++) = num2string_table[(hexarray[i] & 0x0f)];
        i++;
    }
}
uint32_t usbd_cdc_filter_callback(uint8_t *data, uint32_t len)
{
    uint32_t pattern_len = sizeof(USB_CDC_RESET_FILTER_PATTERN) - 1;
    int32_t delay = 10;

    MSG("usbd_cdc_filter_callback Len:%d\r\n", len);

    if (memcmp(data, USB_CDC_RESET_FILTER_PATTERN, pattern_len) == 0) {
        data[len] = 0;
        MSG("Filter get:%s\r\n", data);

        uart1_dtr_init();
        uart1_rts_init();

        if (data[pattern_len] == '0' || data[pattern_len] == 0) {
            dtr_pin_set(1);
            MSG("DTR0\r\n");
        }

        if (data[pattern_len] == '1' || data[pattern_len] == 1) {
            dtr_pin_set(0);
            MSG("DTR1\r\n");
        }

        if (data[pattern_len + 1] == '0' || data[pattern_len + 1] == 0) {
            rts_pin_set(1);
            MSG("RTS0\r\n");
        }

        if (data[pattern_len + 1] == '1' || data[pattern_len + 1] == 1) {
            rts_pin_set(0);
            MSG("RTS1\r\n");
        }

        //delay=atoi(( char *)&data[pattern_len+1]);
        if (delay <= 0) {
            delay = 10;
        }

        bflb_platform_delay_ms(delay);

        if (data[pattern_len + 1] == '0' || data[pattern_len + 1] == 0) {
            rts_pin_set(0);
            MSG("RTS1\r\n");
        }

        if (data[pattern_len + 1] == '1' || data[pattern_len + 1] == 1) {
            rts_pin_set(1);
            MSG("RTS0\r\n");
        }

        bflb_platform_delay_ms(delay);

        uart1_rts_deinit();
        bflb_platform_delay_ms(5);
        uart1_dtr_deinit();

        return 1;
    }

    return 0;
}

void usbd_cdc_acm_bulk_out(uint8_t ep)
{
    uint32_t filter_len = sizeof(USB_CDC_RESET_FILTER_PATTERN) + 1;

    usb_dc_receive_to_ringbuffer(usb_fs, &usb_rx_rb, ep);

    //MSG("Len:%d,%d\r\n",Ring_Buffer_Get_Length(&usb_rx_rb),filter_len);
    if (Ring_Buffer_Get_Length(&usb_rx_rb) == filter_len) {
        Ring_Buffer_Peek(&usb_rx_rb, filter_buf, filter_len);

        if (1 == usbd_cdc_filter_callback(filter_buf, filter_len)) {
            Ring_Buffer_Read(&usb_rx_rb, filter_buf, filter_len);
        }
    }
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
    //dtr_pin_set(!dtr);
}

void usbd_cdc_acm_set_rts(bool rts)
{
    //rts_pin_set(!rts);
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
    uint8_t chipid[8];
    uint8_t chipid2[6];

    bflb_platform_init(0);
    uart_ringbuffer_init();
    uart1_init();
    uart1_set_dtr_rts(UART_DTR_PIN, UART_RTS_PIN);
    uart1_dtr_deinit();
    uart1_rts_deinit();

    usbd_desc_register(cdc_descriptor);

    EF_Ctrl_Read_Chip_ID(chipid);
    hexarr2string(&chipid[2], 3, chipid2);
    // bflb_platform_dump(chipid,8);
    // bflb_platform_dump(chipid2,6);
    cdc_descriptor[0x12 + USB_CONFIG_SIZE + 0x04 + 0x12 + 0x20 + 0x24] = chipid2[0];
    cdc_descriptor[0x12 + USB_CONFIG_SIZE + 0x04 + 0x12 + 0x20 + 0x24 + 2] = chipid2[1];
    cdc_descriptor[0x12 + USB_CONFIG_SIZE + 0x04 + 0x12 + 0x20 + 0x24 + 4] = chipid2[2];
    cdc_descriptor[0x12 + USB_CONFIG_SIZE + 0x04 + 0x12 + 0x20 + 0x24 + 6] = chipid2[3];
    cdc_descriptor[0x12 + USB_CONFIG_SIZE + 0x04 + 0x12 + 0x20 + 0x24 + 8] = chipid2[4];
    cdc_descriptor[0x12 + USB_CONFIG_SIZE + 0x04 + 0x12 + 0x20 + 0x24 + 10] = chipid2[5];

    usbd_cdc_add_acm_interface(&cdc_class, &cdc_cmd_intf);
    usbd_cdc_add_acm_interface(&cdc_class, &cdc_data_intf);
    usbd_interface_add_endpoint(&cdc_data_intf, &cdc_out_ep);
    usbd_interface_add_endpoint(&cdc_data_intf, &cdc_in_ep);

    usb_fs = usb_dc_init();

    if (usb_fs) {
        device_control(usb_fs, DEVICE_CTRL_SET_INT, (void *)(USB_EP1_DATA_OUT_IT | USB_EP2_DATA_IN_IT));
    }

    while (!usb_device_is_configured()) {
    }

    while (1) {
        uart_send_from_ringbuffer();
    }
}
