/**
 * @file usbd_cdc.c
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
#include "usbd_core.h"
#include "usbd_cdc.h"

const char *stop_name[] = { "1", "1.5", "2" };
const char *parity_name[] = { "N", "O", "E", "M", "S" };

/* Device data structure */
struct cdc_acm_cfg_private {
    /* CDC ACM line coding properties. LE order */
    struct cdc_line_coding line_coding;
    /* CDC ACM line state bitmap, DTE side */
    uint8_t line_state;
    /* CDC ACM serial state bitmap, DCE side */
    uint8_t serial_state;
    /* CDC ACM notification sent status */
    uint8_t notification_sent;
    /* CDC ACM configured flag */
    bool configured;
    /* CDC ACM suspended flag */
    bool suspended;
    uint32_t uart_first_init_flag;

} usbd_cdc_acm_cfg;

static void usbd_cdc_acm_reset(void)
{
    usbd_cdc_acm_cfg.line_coding.dwDTERate = 2000000;
    usbd_cdc_acm_cfg.line_coding.bDataBits = 8;
    usbd_cdc_acm_cfg.line_coding.bParityType = 0;
    usbd_cdc_acm_cfg.line_coding.bCharFormat = 0;
    usbd_cdc_acm_cfg.configured = false;
    usbd_cdc_acm_cfg.uart_first_init_flag = 0;
}

/**
 * @brief Handler called for Class requests not handled by the USB stack.
 *
 * @param setup    Information about the request to execute.
 * @param len       Size of the buffer.
 * @param data      Buffer containing the request result.
 *
 * @return  0 on success, negative errno code on fail.
 */
static int cdc_acm_class_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USBD_LOG_DBG("CDC Class request: "
                 "bRequest 0x%02x\r\n",
                 setup->bRequest);

    switch (setup->bRequest) {
        case CDC_REQUEST_SET_LINE_CODING:

            /*******************************************************************************/
            /* Line Coding Structure                                                       */
            /*-----------------------------------------------------------------------------*/
            /* Offset | Field       | Size | Value  | Description                          */
            /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
            /* 4      | bCharFormat |   1  | Number | Stop bits                            */
            /*                                        0 - 1 Stop bit                       */
            /*                                        1 - 1.5 Stop bits                    */
            /*                                        2 - 2 Stop bits                      */
            /* 5      | bParityType |  1   | Number | Parity                               */
            /*                                        0 - None                             */
            /*                                        1 - Odd                              */
            /*                                        2 - Even                             */
            /*                                        3 - Mark                             */
            /*                                        4 - Space                            */
            /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
            /*******************************************************************************/
            if (usbd_cdc_acm_cfg.uart_first_init_flag == 0) {
                usbd_cdc_acm_cfg.uart_first_init_flag = 1;
                return 0;
            }

            memcpy(&usbd_cdc_acm_cfg.line_coding, *data, sizeof(usbd_cdc_acm_cfg.line_coding));
            USBD_LOG_DBG("CDC_SET_LINE_CODING <%d %d %s %s>\r\n",
                         usbd_cdc_acm_cfg.line_coding.dwDTERate,
                         usbd_cdc_acm_cfg.line_coding.bDataBits,
                         parity_name[usbd_cdc_acm_cfg.line_coding.bParityType],
                         stop_name[usbd_cdc_acm_cfg.line_coding.bCharFormat]);
            usbd_cdc_acm_set_line_coding(usbd_cdc_acm_cfg.line_coding.dwDTERate, usbd_cdc_acm_cfg.line_coding.bDataBits,
                                         usbd_cdc_acm_cfg.line_coding.bParityType, usbd_cdc_acm_cfg.line_coding.bCharFormat);
            break;

        case CDC_REQUEST_SET_CONTROL_LINE_STATE:
            usbd_cdc_acm_cfg.line_state = (uint8_t)setup->wValue;
            bool dtr = (setup->wValue & 0x01);
            bool rts = (setup->wValue & 0x02);
            USBD_LOG_DBG("DTR 0x%x,RTS 0x%x\r\n",
                         dtr, rts);
            usbd_cdc_acm_set_dtr(dtr);
            usbd_cdc_acm_set_rts(rts);
            break;

        case CDC_REQUEST_GET_LINE_CODING:
            *data = (uint8_t *)(&usbd_cdc_acm_cfg.line_coding);
            *len = sizeof(usbd_cdc_acm_cfg.line_coding);
            USBD_LOG_DBG("CDC_GET_LINE_CODING %d %d %d %d\r\n",
                         usbd_cdc_acm_cfg.line_coding.dwDTERate,
                         usbd_cdc_acm_cfg.line_coding.bCharFormat,
                         usbd_cdc_acm_cfg.line_coding.bParityType,
                         usbd_cdc_acm_cfg.line_coding.bDataBits);
            break;

        default:
            USBD_LOG_WRN("Unhandled CDC Class bRequest 0x%02x\r\n", setup->bRequest);
            return -1;
    }

    return 0;
}

static void cdc_notify_handler(uint8_t event, void *arg)
{
    switch (event) {
        case USB_EVENT_RESET:
            usbd_cdc_acm_reset();
            break;

        default:
            break;
    }
}

__weak void usbd_cdc_acm_set_line_coding(uint32_t baudrate, uint8_t databits, uint8_t parity, uint8_t stopbits)
{
}
__weak void usbd_cdc_acm_set_dtr(bool dtr)
{
}
__weak void usbd_cdc_acm_set_rts(bool rts)
{
}

void usbd_cdc_add_acm_interface(usbd_class_t *class, usbd_interface_t *intf)
{
    static usbd_class_t *last_class = NULL;

    if (last_class != class) {
        last_class = class;
        usbd_class_register(class);
    }

    intf->class_handler = cdc_acm_class_request_handler;
    intf->custom_handler = NULL;
    intf->vendor_handler = NULL;
    intf->notify_handler = cdc_notify_handler;
    usbd_class_add_interface(class, intf);
}
