/*!
    \file  usbd_core.h
    \brief USB device mode core functions protype

    \version 2019-6-5, V1.0.0, firmware for GD32 USBFS&USBHS
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#ifndef __USBD_CORE_H
#define __USBD_CORE_H

#include "drv_usb_core.h"
#include "drv_usb_dev.h"

typedef enum {
  USBD_OK = 0, /*!< status OK */
  USBD_BUSY,   /*!< status busy */
  USBD_FAIL,   /*!< status fail */
} usbd_status;

enum _usbd_status {
  USBD_DEFAULT    = 1, /*!< default status */
  USBD_ADDRESSED  = 2, /*!< address send status */
  USBD_CONFIGURED = 3, /*!< configured status */
  USBD_SUSPENDED  = 4  /*!< suspended status */
};

/* function declarations */

/* device connect */
void usbd_connect(usb_core_driver *udev);

/* device disconnect */
void usbd_disconnect(usb_core_driver *udev);

/* set USB device address */
void usbd_addr_set(usb_core_driver *udev, uint8_t addr);

/* initailizes the USB device-mode stack and load the class driver */
void usbd_init(usb_core_driver *udev, usb_core_enum core, usb_class_core *class_core);

/* endpoint initialization */
uint32_t usbd_ep_setup(usb_core_driver *udev, const usb_desc_ep *ep_desc);

/* configure the endpoint when it is disabled */
uint32_t usbd_ep_clear(usb_core_driver *udev, uint8_t ep_addr);

/* endpoint prepare to receive data */
uint32_t usbd_ep_recev(usb_core_driver *udev, uint8_t ep_addr, uint8_t *pbuf, uint16_t len);

/* endpoint prepare to transmit data */
uint32_t usbd_ep_send(usb_core_driver *udev, uint8_t ep_addr, uint8_t *pbuf, uint16_t len);

/* set an endpoint to STALL status */
uint32_t usbd_ep_stall(usb_core_driver *udev, uint8_t ep_addr);

/* clear endpoint STALLed status */
uint32_t usbd_ep_stall_clear(usb_core_driver *udev, uint8_t ep_addr);

/* flush the endpoint FIFOs */
uint32_t usbd_fifo_flush(usb_core_driver *udev, uint8_t ep_addr);

/* get the received data length */
uint16_t usbd_rxcount_get(usb_core_driver *udev, uint8_t ep_num);

#endif /* __USBD_CORE_H */
