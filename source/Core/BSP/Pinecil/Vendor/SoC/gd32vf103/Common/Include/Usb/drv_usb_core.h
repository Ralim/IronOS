/*!
    \file  drv_usb_core.h
    \brief USB core low level driver header file

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

#ifndef __DRV_USB_CORE_H
#define __DRV_USB_CORE_H

#include "drv_usb_regs.h"
#include "usb_ch9_std.h"

#define USB_FS_EP0_MAX_LEN 64U /* maximum packet size of EndPoint0 */

#define HC_MAX_PACKET_COUNT 140U /* maximum packet count */

#define EP_ID(x)  ((uint8_t)((x)&0x7FU)) /* endpoint number */
#define EP_DIR(x) ((uint8_t)((x) >> 7))  /* endpoint direction */

enum _usb_eptype {
  USB_EPTYPE_CTRL = 0U, /*!< control endpoint type */
  USB_EPTYPE_ISOC = 1U, /*!< isochronous endpoint type */
  USB_EPTYPE_BULK = 2U, /*!< bulk endpoint type */
  USB_EPTYPE_INTR = 3U, /*!< interrupt endpoint type */
  USB_EPTYPE_MASK = 3U, /*!< endpoint type mask */
};

typedef enum {
  USB_OTG_OK = 0, /*!< USB OTG status OK*/
  USB_OTG_FAIL    /*!< USB OTG status fail*/
} usb_otg_status;

typedef enum {
  USB_OK = 0, /*!< USB status OK*/
  USB_FAIL    /*!< USB status fail*/
} usb_status;

typedef enum {
  USB_USE_FIFO, /*!< USB use FIFO transfer mode */
  USB_USE_DMA   /*!< USB use DMA transfer mode */
} usb_transfer_mode;

typedef struct {
  uint8_t core_enum;     /*!< USB core type */
  uint8_t core_speed;    /*!< USB core speed */
  uint8_t num_pipe;      /*!< USB host channel numbers */
  uint8_t num_ep;        /*!< USB device endpoint numbers */
  uint8_t transfer_mode; /*!< USB transfer mode */
  uint8_t phy_itf;       /*!< USB core PHY interface */
  uint8_t sof_enable;    /*!< USB SOF output */
  uint8_t low_power;     /*!< USB low power */
} usb_core_basic;

/* function declarations */

/* config core capabilities */
usb_status usb_basic_init(usb_core_basic *usb_basic, usb_core_regs *usb_regs, usb_core_enum usb_core);

/*initializes the USB controller registers and prepares the core device mode or host mode operation*/
usb_status usb_core_init(usb_core_basic usb_basic, usb_core_regs *usb_regs);

/* read a packet from the Rx FIFO associated with the endpoint */
void *usb_rxfifo_read(usb_core_regs *core_regs, uint8_t *dest_buf, uint16_t byte_count);

/* write a packet into the Tx FIFO associated with the endpoint */
usb_status usb_txfifo_write(usb_core_regs *usb_regs, uint8_t *src_buf, uint8_t fifo_num, uint16_t byte_count);

/* flush a Tx FIFO or all Tx FIFOs */
usb_status usb_txfifo_flush(usb_core_regs *usb_regs, uint8_t fifo_num);

/* flush the entire Rx FIFO */
usb_status usb_rxfifo_flush(usb_core_regs *usb_regs);

/* get the global interrupts */
static inline uint32_t usb_coreintr_get(usb_core_regs *usb_regs) { return usb_regs->gr->GINTEN & usb_regs->gr->GINTF; }

#endif /* __DRV_USB_CORE_H */
