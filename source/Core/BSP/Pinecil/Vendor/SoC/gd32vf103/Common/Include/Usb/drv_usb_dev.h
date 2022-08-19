/*!
    \file  drv_usb_dev.h
    \brief USB device low level driver header file

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

#ifndef __DRV_USB_DEV_H
#define __DRV_USB_DEV_H

#include "drv_usb_core.h"

enum usb_ctl_status {
  USB_CTL_IDLE = 0U,     /*!< USB control transfer idle state */
  USB_CTL_DATA_IN,       /*!< USB control transfer data in state */
  USB_CTL_LAST_DATA_IN,  /*!< USB control transfer last data in state */
  USB_CTL_DATA_OUT,      /*!< USB control transfer data out state */
  USB_CTL_LAST_DATA_OUT, /*!< USB control transfer last data out state */
  USB_CTL_STATUS_IN,     /*!< USB control transfer status in state*/
  USB_CTL_STATUS_OUT     /*!< USB control transfer status out state */
};

#define EP_IN(x)  ((uint8_t)(0x80U | (x))) /*!< device IN endpoint */
#define EP_OUT(x) ((uint8_t)(x))           /*!< device OUT endpoint */

/* USB descriptor */
typedef struct _usb_desc {
  uint8_t *dev_desc;    /*!< device descriptor */
  uint8_t *config_desc; /*!< config descriptor */
  uint8_t *bos_desc;    /*!< BOS descriptor */

  void *const *strings; /*!< string descriptor */
} usb_desc;

/* USB power management */
typedef struct _usb_pm {
  uint8_t power_mode;        /*!< power mode */
  uint8_t power_low;         /*!< power low */
  uint8_t dev_remote_wakeup; /*!< remote wakeup */
  uint8_t remote_wakeup_on;  /*!< remote wakeup on */
} usb_pm;

/* USB control information */
typedef struct _usb_control {
  usb_req req; /*!< USB standard device request */

  uint8_t ctl_state; /*!< USB control transfer state */
  uint8_t ctl_zlp;   /*!< zero lenth package */
} usb_control;

typedef struct {
  struct {
    uint8_t num : 4; /*!< the endpoint number.it can be from 0 to 6 */
    uint8_t pad : 3; /*!< padding between number and direction */
    uint8_t dir : 1; /*!< the endpoint direction */
  } ep_addr;

  uint8_t ep_type;  /*!< USB endpoint type */
  uint8_t ep_stall; /*!< USB endpoint stall status */

  uint8_t  frame_num; /*!< number of frame */
  uint16_t max_len;   /*!< Maximum packet lenth */

  /* transaction level variables */
  uint8_t *xfer_buf;   /*!< transmit buffer */
  uint32_t xfer_len;   /*!< transmit buffer length */
  uint32_t xfer_count; /*!< transmit buffer count */

  uint32_t remain_len; /*!< remain packet lenth */

  uint32_t dma_addr; /*!< DMA address */
} usb_transc;

typedef struct _usb_core_driver usb_dev;

typedef struct _usb_class_core {
  uint8_t command;   /*!< device class request command */
  uint8_t alter_set; /*!< alternative set */

  uint8_t (*init)(usb_dev *udev, uint8_t config_index);   /*!< initialize handler */
  uint8_t (*deinit)(usb_dev *udev, uint8_t config_index); /*!< de-initialize handler */

  uint8_t (*req_proc)(usb_dev *udev, usb_req *req); /*!< device request handler */

  uint8_t (*data_in)(usb_dev *udev, uint8_t ep_num);  /*!< device data in handler */
  uint8_t (*data_out)(usb_dev *udev, uint8_t ep_num); /*!< device data out handler */

  uint8_t (*SOF)(usb_dev *udev); /*!< Start of frame handler */

  uint8_t (*incomplete_isoc_in)(usb_dev *udev);  /*!< Incomplete synchronization IN transfer handler */
  uint8_t (*incomplete_isoc_out)(usb_dev *udev); /*!< Incomplete synchronization OUT transfer handler */
} usb_class_core;

typedef struct _usb_perp_dev {
  uint8_t config;   /*!< configuration */
  uint8_t dev_addr; /*!< device address */

  __IO uint8_t cur_status;    /*!< current status */
  __IO uint8_t backup_status; /*!< backup status */

  usb_transc transc_in[USBFS_MAX_TX_FIFOS];  /*!< endpoint IN transaction */
  usb_transc transc_out[USBFS_MAX_TX_FIFOS]; /*!< endpoint OUT transaction */

  usb_pm      pm;      /*!< power management */
  usb_desc    desc;    /*!< USB descriptors */
  usb_control control; /*!< USB control information */

  usb_class_core *class_core; /*!< class driver */
} usb_perp_dev;

typedef struct _usb_core_driver {
  usb_core_basic bp;   /*!< USB basic parameters */
  usb_core_regs  regs; /*!< USB registers */
  usb_perp_dev   dev;  /*!< USB peripheral device */
} usb_core_driver;

/* function declarations */

/* initialize USB core registers for device mode */
usb_status usb_devcore_init(usb_core_driver *udev);

/* enable the USB device mode interrupts */
usb_status usb_devint_enable(usb_core_driver *udev);

/* active the usb transaction */
usb_status usb_transc_active(usb_core_driver *udev, usb_transc *transc);

/* deactive the usb transaction */
usb_status usb_transc_deactivate(usb_core_driver *udev, usb_transc *transc);

/* configure usb transaction to start IN transfer */
usb_status usb_transc_inxfer(usb_core_driver *udev, usb_transc *transc);

/* configure usb transaction to start OUT transfer */
usb_status usb_transc_outxfer(usb_core_driver *udev, usb_transc *transc);

/* set the usb transaction STALL status */
usb_status usb_transc_stall(usb_core_driver *udev, usb_transc *transc);

/* clear the usb transaction STALL status */
usb_status usb_transc_clrstall(usb_core_driver *udev, usb_transc *transc);

/* read device all OUT endpoint interrupt register */
uint32_t usb_oepintnum_read(usb_core_driver *udev);

/* read device OUT endpoint interrupt flag register */
uint32_t usb_oepintr_read(usb_core_driver *udev, uint8_t ep_num);

/* read device all IN endpoint interrupt register */
uint32_t usb_iepintnum_read(usb_core_driver *udev);

/* read device IN endpoint interrupt flag register */
uint32_t usb_iepintr_read(usb_core_driver *udev, uint8_t ep_num);

/* config the USB device to be disconnected */
void usb_dev_disconnect(usb_core_driver *udev);

/* config the USB device to be connected */
void usb_dev_connect(usb_core_driver *udev);

/* set the USB device address */
void usb_devaddr_set(usb_core_driver *pudev, uint8_t dev_addr);

/* configures OUT endpoint 0 to receive SETUP packets */
void usb_ctlep_startout(usb_core_driver *udev);

/* active remote wakeup signalling */
void usb_rwkup_active(usb_core_driver *udev);

/* reset remote wakeup signalling */
void usb_rwkup_reset(usb_core_driver *udev);

/* set remote wakeup signalling */
void usb_rwkup_set(usb_core_driver *udev);

/* active USB core clock */
void usb_clock_active(usb_core_driver *udev);

/* usb device suspend */
void usb_dev_suspend(usb_core_driver *udev);

/* stop the device and clean up fifos */
void usb_dev_stop(usb_core_driver *udev);

#endif /* __DRV_USB_DEV_H */
