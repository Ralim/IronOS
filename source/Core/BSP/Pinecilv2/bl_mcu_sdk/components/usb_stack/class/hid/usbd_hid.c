/**
 * @file usbd_hid.c
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
#include "usbd_hid.h"

#define HID_STATE_IDLE 0
#define HID_STATE_BUSY 1

struct usbd_hid_cfg_private {
    const uint8_t *hid_descriptor;
    const uint8_t *hid_report_descriptor;
    uint32_t hid_report_descriptor_len;
    uint8_t current_intf_num;
    uint8_t hid_state;
    uint8_t report;
    uint8_t idle_state;
    uint8_t protocol;

    uint8_t (*get_report_callback)(uint8_t report_id, uint8_t report_type);
    void (*set_report_callback)(uint8_t report_id, uint8_t report_type, uint8_t *report, uint8_t report_len);
    uint8_t (*get_idle_callback)(uint8_t report_id);
    void (*set_idle_callback)(uint8_t report_id, uint8_t duration);
    void (*set_protocol_callback)(uint8_t protocol);
    uint8_t (*get_protocol_callback)(void);

    usb_slist_t list;
} usbd_hid_cfg[4];

static usb_slist_t usbd_hid_class_head = USB_SLIST_OBJECT_INIT(usbd_hid_class_head);

static void usbd_hid_reset(void)
{
    usb_slist_t *i;
    usb_slist_for_each(i, &usbd_hid_class_head)
    {
        struct usbd_hid_cfg_private *hid_intf = usb_slist_entry(i, struct usbd_hid_cfg_private, list);
        hid_intf->hid_state = HID_STATE_IDLE;
        hid_intf->report = 0;
        hid_intf->idle_state = 0;
        hid_intf->protocol = 0;
    }
}

int hid_custom_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USBD_LOG_DBG("HID Custom request: "
                 "bRequest 0x%02x\r\n",
                 setup->bRequest);

    if (REQTYPE_GET_DIR(setup->bmRequestType) == USB_REQUEST_DEVICE_TO_HOST &&
        setup->bRequest == USB_REQUEST_GET_DESCRIPTOR) {
        uint8_t value = (uint8_t)(setup->wValue >> 8);
        uint8_t intf_num = (uint8_t)setup->wIndex;

        struct usbd_hid_cfg_private *current_hid_intf = NULL;
        usb_slist_t *i;
        usb_slist_for_each(i, &usbd_hid_class_head)
        {
            struct usbd_hid_cfg_private *hid_intf = usb_slist_entry(i, struct usbd_hid_cfg_private, list);

            if (hid_intf->current_intf_num == intf_num) {
                current_hid_intf = hid_intf;
                break;
            }
        }

        if (current_hid_intf == NULL) {
            return -2;
        }

        switch (value) {
            case HID_DESCRIPTOR_TYPE_HID:
                USBD_LOG_INFO("get HID Descriptor\r\n");
                *data = (uint8_t *)current_hid_intf->hid_descriptor;
                *len = current_hid_intf->hid_descriptor[0];
                break;

            case HID_DESCRIPTOR_TYPE_HID_REPORT:
                USBD_LOG_INFO("get Report Descriptor\r\n");
                *data = (uint8_t *)current_hid_intf->hid_report_descriptor;
                *len = current_hid_intf->hid_report_descriptor_len;
                break;

            case HID_DESCRIPTOR_TYPE_HID_PHYSICAL:
                USBD_LOG_INFO("get PHYSICAL Descriptor\r\n");

                break;

            default:
                return -2;
        }

        return 0;
    }

    return -1;
}

int hid_class_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USBD_LOG_DBG("HID Class request: "
                 "bRequest 0x%02x\r\n",
                 setup->bRequest);

    struct usbd_hid_cfg_private *current_hid_intf = NULL;
    usb_slist_t *i;
    usb_slist_for_each(i, &usbd_hid_class_head)
    {
        struct usbd_hid_cfg_private *hid_intf = usb_slist_entry(i, struct usbd_hid_cfg_private, list);
        uint8_t intf_num = (uint8_t)setup->wIndex;
        if (hid_intf->current_intf_num == intf_num) {
            current_hid_intf = hid_intf;
            break;
        }
    }

    if (current_hid_intf == NULL) {
        return -2;
    }

    switch (setup->bRequest) {
        case HID_REQUEST_GET_REPORT:
            if (current_hid_intf->get_report_callback)
                current_hid_intf->report = current_hid_intf->get_report_callback(setup->wValueL, setup->wValueH); /*report id ,report type*/

            *data = (uint8_t *)&current_hid_intf->report;
            *len = 1;
            break;
        case HID_REQUEST_GET_IDLE:
            if (current_hid_intf->get_idle_callback)
                current_hid_intf->idle_state = current_hid_intf->get_idle_callback(setup->wValueL);

            *data = (uint8_t *)&current_hid_intf->idle_state;
            *len = 1;
            break;
        case HID_REQUEST_GET_PROTOCOL:
            if (current_hid_intf->get_protocol_callback)
                current_hid_intf->protocol = current_hid_intf->get_protocol_callback();

            *data = (uint8_t *)&current_hid_intf->protocol;
            *len = 1;
            break;
        case HID_REQUEST_SET_REPORT:
            if (current_hid_intf->set_report_callback)
                current_hid_intf->set_report_callback(setup->wValueL, setup->wValueH, *data, *len); /*report id ,report type,report,report len*/

            current_hid_intf->report = **data;
            break;
        case HID_REQUEST_SET_IDLE:
            if (current_hid_intf->set_idle_callback)
                current_hid_intf->set_idle_callback(setup->wValueL, setup->wIndexH); /*report id ,duration*/

            current_hid_intf->idle_state = setup->wIndexH;
            break;
        case HID_REQUEST_SET_PROTOCOL:
            if (current_hid_intf->set_protocol_callback)
                current_hid_intf->set_protocol_callback(setup->wValueL); /*protocol*/

            current_hid_intf->protocol = setup->wValueL;
            break;

        default:
            USBD_LOG_WRN("Unhandled HID Class bRequest 0x%02x\r\n", setup->bRequest);
            return -1;
    }

    return 0;
}

static void hid_notify_handler(uint8_t event, void *arg)
{
    switch (event) {
        case USB_EVENT_RESET:
            usbd_hid_reset();
            break;

        default:
            break;
    }
}

void usbd_hid_reset_state(void)
{
    // usbd_hid_cfg.hid_state = HID_STATE_IDLE;
}

void usbd_hid_send_report(uint8_t ep, uint8_t *data, uint8_t len)
{
    // if(usbd_hid_cfg.hid_state == HID_STATE_IDLE)
    // {
    //     usbd_hid_cfg.hid_state = HID_STATE_BUSY;
    //     usbd_ep_write(ep, data, len, NULL);
    // }
}

void usbd_hid_descriptor_register(uint8_t intf_num, const uint8_t *desc)
{
    // usbd_hid_cfg.hid_descriptor = desc;
}

void usbd_hid_report_descriptor_register(uint8_t intf_num, const uint8_t *desc, uint32_t desc_len)
{
    usb_slist_t *i;
    usb_slist_for_each(i, &usbd_hid_class_head)
    {
        struct usbd_hid_cfg_private *hid_intf = usb_slist_entry(i, struct usbd_hid_cfg_private, list);

        if (hid_intf->current_intf_num == intf_num) {
            hid_intf->hid_report_descriptor = desc;
            hid_intf->hid_report_descriptor_len = desc_len;
            return;
        }
    }
}
// clang-format off
void usbd_hid_set_request_callback( uint8_t intf_num,
                                    uint8_t (*get_report_callback)(uint8_t report_id, uint8_t report_type),
                                    void (*set_report_callback)(uint8_t report_id, uint8_t report_type, uint8_t *report, uint8_t report_len),
                                    uint8_t (*get_idle_callback)(uint8_t report_id),
                                    void (*set_idle_callback)(uint8_t report_id, uint8_t duration),
                                    void (*set_protocol_callback)(uint8_t protocol),
                                    uint8_t (*get_protocol_callback)(void))
// clang-format on
{
    usb_slist_t *i;
    usb_slist_for_each(i, &usbd_hid_class_head)
    {
        struct usbd_hid_cfg_private *hid_intf = usb_slist_entry(i, struct usbd_hid_cfg_private, list);

        if (hid_intf->current_intf_num == intf_num) {
            if (get_report_callback)
                hid_intf->get_report_callback = get_report_callback;
            if (set_report_callback)
                hid_intf->set_report_callback = set_report_callback;
            if (get_idle_callback)
                hid_intf->get_idle_callback = get_idle_callback;
            if (set_idle_callback)
                hid_intf->set_idle_callback = set_idle_callback;
            if (set_protocol_callback)
                hid_intf->set_protocol_callback = set_protocol_callback;
            if (get_protocol_callback)
                hid_intf->get_protocol_callback = get_protocol_callback;
            return;
        }
    }
}

void usbd_hid_add_interface(usbd_class_t *class, usbd_interface_t *intf)
{
    static usbd_class_t *last_class = NULL;
    static uint8_t hid_num = 0;
    if (last_class != class) {
        last_class = class;
        usbd_class_register(class);
    }

    intf->class_handler = hid_class_request_handler;
    intf->custom_handler = hid_custom_request_handler;
    intf->vendor_handler = NULL;
    intf->notify_handler = hid_notify_handler;
    usbd_class_add_interface(class, intf);

    usbd_hid_cfg[hid_num].current_intf_num = intf->intf_num;
    usb_slist_add_tail(&usbd_hid_class_head, &usbd_hid_cfg[hid_num].list);
    hid_num++;
}
