/**
 * @file usbd_core.h
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
#ifndef _USBD_CORE_H
#define _USBD_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "usb_util.h"
#include "usb_def.h"
#include "usb_dc.h"

enum usb_event_type {
    /** USB error reported by the controller */
    USB_EVENT_ERROR,
    /** USB reset */
    USB_EVENT_RESET,
    /** Start of Frame received */
    USB_EVENT_SOF,
    /** USB connection established, hardware enumeration is completed */
    USB_EVENT_CONNECTED,
    /** USB configuration done */
    USB_EVENT_CONFIGURED,
    /** USB connection suspended by the HOST */
    USB_EVENT_SUSPEND,
    /** USB connection lost */
    USB_EVENT_DISCONNECTED,
    /** USB connection resumed by the HOST */
    USB_EVENT_RESUME,

    /** USB interface selected */
    USB_EVENT_SET_INTERFACE,
    /** USB interface selected */
    USB_EVENT_SET_REMOTE_WAKEUP,
    /** USB interface selected */
    USB_EVENT_CLEAR_REMOTE_WAKEUP,
    /** Set Feature ENDPOINT_HALT received */
    USB_EVENT_SET_HALT,
    /** Clear Feature ENDPOINT_HALT received */
    USB_EVENT_CLEAR_HALT,
    /** setup packet received */
    USB_EVENT_SETUP_NOTIFY,
    /** ep0 in packet received */
    USB_EVENT_EP0_IN_NOTIFY,
    /** ep0 out packet received */
    USB_EVENT_EP0_OUT_NOTIFY,
    /** ep in packet except ep0 received */
    USB_EVENT_EP_IN_NOTIFY,
    /** ep out packet except ep0 received */
    USB_EVENT_EP_OUT_NOTIFY,
    /** Initial USB connection status */
    USB_EVENT_UNKNOWN
};

/**
 * @brief Callback function signature for the USB Endpoint status
 */
typedef void (*usbd_endpoint_callback)(uint8_t ep);

/**
 * @brief Callback function signature for class specific requests
 *
 * Function which handles Class specific requests corresponding to an
 * interface number specified in the device descriptor table. For host
 * to device direction the 'len' and 'payload_data' contain the length
 * of the received data and the pointer to the received data respectively.
 * For device to host class requests, 'len' and 'payload_data' should be
 * set by the callback function with the length and the address of the
 * data to be transmitted buffer respectively.
 */
typedef int (*usbd_request_handler)(struct usb_setup_packet *setup,
                                    uint8_t **data, uint32_t *transfer_len);

/* callback function pointer structure for Application to handle events */
typedef void (*usbd_notify_handler)(uint8_t event, void *arg);

typedef struct usbd_endpoint {
    usb_slist_t list;
    uint8_t ep_addr;
    usbd_endpoint_callback ep_cb;
} usbd_endpoint_t;

typedef struct usbd_interface {
    usb_slist_t list;
    /** Handler for USB Class specific Control (EP 0) communications */
    usbd_request_handler class_handler;
    /** Handler for USB Vendor specific commands */
    usbd_request_handler vendor_handler;
    /** Handler for USB custom specific commands */
    usbd_request_handler custom_handler;
    /** Handler for USB event notify commands */
    usbd_notify_handler notify_handler;
    uint8_t intf_num;
    usb_slist_t ep_list;
} usbd_interface_t;

typedef struct usbd_class {
    usb_slist_t list;
    const char *name;
    usb_slist_t intf_list;
} usbd_class_t;

void usbd_event_notify_handler(uint8_t event, void *arg);

void usbd_desc_register(const uint8_t *desc);
void usbd_class_register(usbd_class_t *class);
void usbd_msosv1_desc_register(struct usb_msosv1_descriptor *desc);
void usbd_msosv2_desc_register(struct usb_msosv2_descriptor *desc);
void usbd_bos_desc_register(struct usb_bos_descriptor *desc);
void usbd_class_add_interface(usbd_class_t *class, usbd_interface_t *intf);
void usbd_interface_add_endpoint(usbd_interface_t *intf, usbd_endpoint_t *ep);
bool usb_device_is_configured(void);

#ifdef __cplusplus
}
#endif

#endif
