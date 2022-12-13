/**
 * @file hal_usb.h
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
#ifndef __HAL_USB__H__
#define __HAL_USB__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "ring_buffer.h"
#include "drv_device.h"
#include "bl702_config.h"

#define DEVICE_CTRL_USB_DC_SET_ACK            0X10
#define DEVICE_CTRL_USB_DC_ENUM_ON            0X11
#define DEVICE_CTRL_USB_DC_ENUM_OFF           0X12
#define DEVICE_CTRL_USB_DC_GET_EP_TX_FIFO_CNT 0x13
#define DEVICE_CTRL_USB_DC_GET_EP_RX_FIFO_CNT 0x14
#define DEVICE_CTRL_USB_DC_SET_TX_DMA         0x15
#define DEVICE_CTRL_USB_DC_SET_RX_DMA         0x16

enum usb_index_type {
#ifdef BSP_USING_USB
    USB_INDEX,
#endif
    USB_MAX_INDEX
};

/**
 * USB endpoint Transfer Type mask.
 */
#define USBD_EP_TYPE_CTRL 0
#define USBD_EP_TYPE_ISOC 1
#define USBD_EP_TYPE_BULK 2
#define USBD_EP_TYPE_INTR 3
#define USBD_EP_TYPE_MASK 3

/**
 * USB endpoint direction and number.
 */
#define USB_EP_DIR_MASK 0x80U
#define USB_EP_DIR_IN   0x80U
#define USB_EP_DIR_OUT  0x00U

#define USB_EP_OUT_MSK 0x7FU
#define USB_EP_IN_MSK  0x80U

/** Get endpoint index (number) from endpoint address */
#define USB_EP_GET_IDX(ep) ((ep) & ~USB_EP_DIR_MASK)
/** Get direction from endpoint address */
#define USB_EP_GET_DIR(ep) ((ep)&USB_EP_DIR_MASK)
/** Get endpoint address from endpoint index and direction */
#define USB_EP_GET_ADDR(idx, dir) ((idx) | ((dir)&USB_EP_DIR_MASK))
/** True if the endpoint is an IN endpoint */
#define USB_EP_DIR_IS_IN(ep) (USB_EP_GET_DIR(ep) == USB_EP_DIR_IN)
/** True if the endpoint is an OUT endpoint */
#define USB_EP_DIR_IS_OUT(ep) (USB_EP_GET_DIR(ep) == USB_EP_DIR_OUT)

#define USB_SET_EP_OUT(ep) (ep & USB_EP_OUT_MSK)
#define USB_SET_EP_IN(ep)  (ep | USB_EP_IN_MSK)

#define USB_OUT_EP_NUM 8
#define USB_IN_EP_NUM  8

#define USB_CTRL_EP_MPS        64 /**< maximum packet size (MPS) for EP 0 */
#define USB_FS_MAX_PACKET_SIZE 64 /**< full speed MPS for bulk EP */

/* Default USB control EP, always 0 and 0x80 */
#define USB_CONTROL_OUT_EP0 0
#define USB_CONTROL_IN_EP0  0x80

#define USB_DC_EP_TYPE_CTRL 0x5 /*0*/
#define USB_DC_EP_TYPE_ISOC 0x2 /*1*/
#define USB_DC_EP_TYPE_BULK 0x4 /*2*/
#define USB_DC_EP_TYPE_INTR 0x0 /*3*/

#define USB_DC_EP1_IN_DR  (0x4000D800 + 0x118)
#define USB_DC_EP1_OUT_DR (0x4000D800 + 0x11c)
#define USB_DC_EP2_IN_DR  (0x4000D800 + 0x128)
#define USB_DC_EP2_OUT_DR (0x4000D800 + 0x12c)
#define USB_DC_EP3_IN_DR  (0x4000D800 + 0x138)
#define USB_DC_EP3_OUT_DR (0x4000D800 + 0x13c)
#define USB_DC_EP4_IN_DR  (0x4000D800 + 0x148)
#define USB_DC_EP4_OUT_DR (0x4000D800 + 0x14c)
#define USB_DC_EP5_IN_DR  (0x4000D800 + 0x158)
#define USB_DC_EP5_OUT_DR (0x4000D800 + 0x15c)
#define USB_DC_EP6_IN_DR  (0x4000D800 + 0x168)
#define USB_DC_EP6_OUT_DR (0x4000D800 + 0x16c)
#define USB_DC_EP7_IN_DR  (0x4000D800 + 0x178)
#define USB_DC_EP7_OUT_DR (0x4000D800 + 0x17c)

enum usb_dc_event_type {
    /** USB error reported by the controller */
    USB_DC_EVENT_ERROR,
    /** USB reset */
    USB_DC_EVENT_RESET,
    /** Start of Frame received */
    USB_DC_EVENT_SOF,
    /** USB connection established, hardware enumeration is completed */
    USB_DC_EVENT_CONNECTED,
    /** USB configuration done */
    USB_DC_EVENT_CONFIGURED,
    /** USB connection suspended by the HOST */
    USB_DC_EVENT_SUSPEND,
    /** USB connection lost */
    USB_DC_EVENT_DISCONNECTED,
    /** USB connection resumed by the HOST */
    USB_DC_EVENT_RESUME,

    /** USB interface selected */
    USB_DC_EVENT_SET_INTERFACE,
    /** USB interface selected */
    USB_DC_EVENT_SET_REMOTE_WAKEUP,
    /** USB interface selected */
    USB_DC_EVENT_CLEAR_REMOTE_WAKEUP,
    /** Set Feature ENDPOINT_HALT received */
    USB_DC_EVENT_SET_HALT,
    /** Clear Feature ENDPOINT_HALT received */
    USB_DC_EVENT_CLEAR_HALT,
    /** setup packet received */
    USB_DC_EVENT_SETUP_NOTIFY,
    /** ep0 in packet received */
    USB_DC_EVENT_EP0_IN_NOTIFY,
    /** ep0 out packet received */
    USB_DC_EVENT_EP0_OUT_NOTIFY,
    /** ep in packet except ep0 received */
    USB_DC_EVENT_EP_IN_NOTIFY,
    /** ep out packet except ep0 received */
    USB_DC_EVENT_EP_OUT_NOTIFY,
    /** Initial USB connection status */
    USB_DC_EVENT_UNKNOWN
};

enum usb_dc_ep_it_type {
    USB_SOF_IT = 1 << 0,
    USB_EP1_DATA_IN_IT = 1 << 10,
    USB_EP1_DATA_OUT_IT = 1 << 11,
    USB_EP2_DATA_IN_IT = 1 << 12,
    USB_EP2_DATA_OUT_IT = 1 << 13,
    USB_EP3_DATA_IN_IT = 1 << 14,
    USB_EP3_DATA_OUT_IT = 1 << 15,
    USB_EP4_DATA_IN_IT = 1 << 16,
    USB_EP4_DATA_OUT_IT = 1 << 17,
    USB_EP5_DATA_IN_IT = 1 << 18,
    USB_EP5_DATA_OUT_IT = 1 << 19,
    USB_EP6_DATA_IN_IT = 1 << 20,
    USB_EP6_DATA_OUT_IT = 1 << 21,
    USB_EP7_DATA_IN_IT = 1 << 22,
    USB_EP7_DATA_OUT_IT = 1 << 23,
};

enum usb_error_type {
    USB_DC_OK = 0,
    USB_DC_EP_DIR_ERR = 1,
    USB_DC_EP_EN_ERR = 2,
    USB_DC_EP_TIMEOUT_ERR = 3,
    USB_DC_ADDR_ERR = 4,
    USB_DC_RB_SIZE_SMALL_ERR = 5,
    USB_DC_ZLP_ERR = 6,
};
/**
 * @brief USB Endpoint Configuration.
 *
 * Structure containing the USB endpoint configuration.
 */
struct usb_dc_ep_cfg {
    /** The number associated with the EP in the device
     *  configuration structure
     *       IN  EP = 0x80 | \<endpoint number\>
     *       OUT EP = 0x00 | \<endpoint number\>
     */
    uint8_t ep_addr;
    /** Endpoint max packet size */
    uint16_t ep_mps;
    /** Endpoint Transfer Type.
     * May be Bulk, Interrupt, Control or Isochronous
     */
    uint8_t ep_type;
};

/*
 * USB endpoint  structure.
 */
typedef struct
{
    uint8_t ep_ena;
    uint32_t is_stalled;
    struct usb_dc_ep_cfg ep_cfg;
} usb_dc_ep_state_t;

typedef struct usb_dc_device {
    struct device parent;
    uint8_t id;
    usb_dc_ep_state_t in_ep[8];  /*!< IN endpoint parameters             */
    usb_dc_ep_state_t out_ep[8]; /*!< OUT endpoint parameters            */
    void *tx_dma;
    void *rx_dma;
} usb_dc_device_t;

int usb_dc_register(enum usb_index_type index, const char *name);

int usb_dc_set_dev_address(const uint8_t addr);
int usb_dc_ep_open(struct device *dev, const struct usb_dc_ep_cfg *ep_cfg);
int usb_dc_ep_close(const uint8_t ep);
int usb_dc_ep_set_stall(const uint8_t ep);
int usb_dc_ep_clear_stall(const uint8_t ep);
int usb_dc_ep_is_stalled(struct device *dev, const uint8_t ep, uint8_t *stalled);
int usb_dc_ep_write(struct device *dev, const uint8_t ep, const uint8_t *data, uint32_t data_len, uint32_t *ret_bytes);
int usb_dc_ep_read(struct device *dev, const uint8_t ep, uint8_t *data, uint32_t data_len, uint32_t *read_bytes);
int usb_dc_receive_to_ringbuffer(struct device *dev, Ring_Buffer_Type *rb, uint8_t ep);
int usb_dc_send_from_ringbuffer(struct device *dev, Ring_Buffer_Type *rb, uint8_t ep);
#ifdef __cplusplus
}
#endif
#endif