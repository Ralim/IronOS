/**
 * @file usbd_video.c
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
#include "usbd_video.h"

extern struct video_probe_and_commit_controls probe;
extern struct video_probe_and_commit_controls commit;

int video_class_request_handler(struct usb_setup_packet *setup, uint8_t **data, uint32_t *len)
{
    USBD_LOG_DBG("Video Class request: "
                 "bRequest 0x%02x\r\n",
                 setup->bRequest);

    switch (setup->bRequest) {
        case VIDEO_REQUEST_SET_CUR:
            if (setup->wValue == 256) {
                memcpy((uint8_t *)&probe, *data, setup->wLength);
            } else if (setup->wValue == 512) {
                memcpy((uint8_t *)&commit, *data, setup->wLength);
            }

            break;

        case VIDEO_REQUEST_GET_CUR:
            if (setup->wValue == 256) {
                *data = (uint8_t *)&probe;
            } else if (setup->wValue == 512) {
                *data = (uint8_t *)&commit;
            }

            break;

        case VIDEO_REQUEST_GET_MIN:
            if (setup->wValue == 256) {
                *data = (uint8_t *)&probe;
            } else if (setup->wValue == 512) {
                *data = (uint8_t *)&commit;
            }

            break;

        case VIDEO_REQUEST_GET_MAX:
            if (setup->wValue == 256) {
                *data = (uint8_t *)&probe;
            } else if (setup->wValue == 512) {
                *data = (uint8_t *)&commit;
            }

            break;

        case VIDEO_REQUEST_GET_RES:

            break;

        case VIDEO_REQUEST_GET_LEN:

            break;

        case VIDEO_REQUEST_GET_INFO:

            break;

        case VIDEO_REQUEST_GET_DEF:
            if (setup->wLength == 256) {
                *data = (uint8_t *)&probe;
            } else if (setup->wLength == 512) {
                *data = (uint8_t *)&commit;
            }

            break;

        default:
            USBD_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
            return -1;
    }

    return 0;
}

void video_notify_handler(uint8_t event, void *arg)
{
    switch (event) {
        case USB_EVENT_RESET:

            break;

        case USB_EVENT_SOF:
            usbd_video_sof_callback();
            break;

        case USB_EVENT_SET_INTERFACE:
            usbd_video_set_interface_callback(((uint8_t *)arg)[3]);
            break;

        default:
            break;
    }
}

void usbd_video_add_interface(usbd_class_t *class, usbd_interface_t *intf)
{
    static usbd_class_t *last_class = NULL;

    if (last_class != class) {
        last_class = class;
        usbd_class_register(class);
    }

    intf->class_handler = video_class_request_handler;
    intf->custom_handler = NULL;
    intf->vendor_handler = NULL;
    intf->notify_handler = video_notify_handler;
    usbd_class_add_interface(class, intf);
}