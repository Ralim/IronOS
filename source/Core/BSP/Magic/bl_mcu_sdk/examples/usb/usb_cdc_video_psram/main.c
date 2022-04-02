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
#include "hal_cam.h"
#include "hal_mjpeg.h"
#include "hal_gpio.h"
#include "bsp_sf_psram.h"
#include "bsp_image_sensor.h"
#include "bl702_uart.h"

#define PSRAM_TEST   0
#define UART_SEND    0
#define USB_CDC_SEND 1

#define MJPEG_ENDADDR       (0x26300000)
#define CAMERA_RESOLUTION_X (320)
#define CAMERA_RESOLUTION_Y (240)
#define CAMERA_FRAME_SIZE   (CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y * 2)
#define CAMERA_WRITE_ADDR   (0x4201E000)
#define MJPEG_READ_ADDR     CAMERA_WRITE_ADDR //0x42022000 0x42020800  0x4201E000 0x26000000
#define MJPEG_READ_SIZE     5
#define YUV_USE             1
#if (YUV_USE == 1)
#define CAMERA_BUFFER_SIZE_WHEN_MJPEG (CAMERA_RESOLUTION_X * 2 * 8 * MJPEG_READ_SIZE)
#else
#define CAMERA_BUFFER_SIZE_WHEN_MJPEG (CAMERA_RESOLUTION_X * 8 * MJPEG_READ_SIZE)
#endif
#define MJPEG_WRITE_ADDR (0x26008000)
#define MJPEG_WRITE_SIZE (MJPEG_ENDADDR - MJPEG_WRITE_ADDR - CAMERA_BUFFER_SIZE_WHEN_MJPEG)

static mjpeg_device_t mjpeg_cfg = {
    .quality = 50,
    .yuv_format = MJPEG_YUV_FORMAT_YUV422_INTERLEAVE,
    .write_buffer_addr = MJPEG_WRITE_ADDR,
    .write_buffer_size = MJPEG_WRITE_SIZE,
    .read_buffer_addr = MJPEG_READ_ADDR,
    .read_buffer_size = MJPEG_READ_SIZE,
    .resolution_x = CAMERA_RESOLUTION_X,
    .resolution_y = CAMERA_RESOLUTION_Y,

    .packet_cut_mode = 0,
};

static cam_device_t camera_cfg = {
    .software_mode = CAM_AUTO_MODE,
    .frame_mode = CAM_FRAME_INTERLEAVE_MODE,
    .yuv_format = CAM_YUV_FORMAT_YUV422,
    .cam_write_ram_addr = CAMERA_WRITE_ADDR,
    .cam_write_ram_size = CAMERA_BUFFER_SIZE_WHEN_MJPEG,
    .cam_frame_size = CAMERA_FRAME_SIZE,

    .cam_write_ram_addr1 = 0,
    .cam_write_ram_size1 = 0,
    .cam_frame_size1 = 0,
};

#define CDC_IN_EP  0x82
#define CDC_OUT_EP 0x01
#define CDC_INT_EP 0x83

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN)

USB_DESC_SECTION const uint8_t cdc_descriptor[] = {
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

ATTR_DTCM_SECTION uint8_t frame_head[] = "picturestartjpeg";
ATTR_DTCM_SECTION uint8_t frame_tail[] = "pictureend";

uint32_t picture[256];

bool zlp_flag = false;
uint32_t actual_read_length = 1024 * 1024;
uint32_t total_recv_length = 0;
uint8_t out_buffer[64];
void CamUsbSend(uint8_t *pic, uint32_t len);
volatile uint8_t flag = 0;
void usbd_cdc_acm_bulk_out(uint8_t ep)
{
    static uint32_t out = 0;
    MSG("out:%d\r\n", out++);

    if (usbd_ep_read(ep, out_buffer, 64, &actual_read_length) < 0) {
        USBD_LOG_DBG("Read DATA Packet failed\r\n");
        usbd_ep_set_stall(ep);
        return;
    }

    usbd_ep_read(ep, NULL, 0, NULL);
}

void usbd_cdc_acm_bulk_in(uint8_t ep)
{
    flag = 1;
    // if ((zlp_flag == false) && actual_read_length)
    // {
    //     usbd_ep_write(0x82, out_buffer, actual_read_length, NULL);
    //     actual_read_length-=64;
    //     if(actual_read_length==0)
    //     zlp_flag = true;

    // }
    // else if (zlp_flag)
    // {
    //     usbd_ep_write(0x82, NULL, 0, NULL);
    //     zlp_flag = false;
    // }
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

struct device *usb_fs;
extern struct device *usb_dc_init(void);
void CamUsbSend(uint8_t *pic, uint32_t len)
{
    while (len >= 64) {
        usbd_ep_write(0x82, pic, 64, NULL);
        pic += 64;
        len -= 64;
    }

    if (len > 0) {
        usbd_ep_write(0x82, pic, len, NULL);
    } else {
        usbd_ep_write(0x82, NULL, 0, NULL);
    }
}

#if PSRAM_TEST
void test32(void)
{
    uint32_t i, val;

    for (i = 0; i < 256; i += 4) {
        *((volatile uint32_t *)(BSP_PSRAM_BASE + i)) = i / 4;
    }

    for (i = 0; i < 256; i += 4) {
        val = *((volatile uint32_t *)(BSP_PSRAM_BASE + i));
        MSG("addr = 0x%08X, val = 0x%08X\r\n", (BSP_PSRAM_BASE + i), *((volatile uint32_t *)(BSP_PSRAM_BASE + i)));

        if (i / 4 != val) {
            MSG("Error!\r\n");

            while (1)
                ;
        }
    }
}

void test8(void)
{
    uint32_t i;
    uint8_t val;

    for (i = 0; i < 256; i++) {
        *((volatile uint8_t *)(BSP_PSRAM_BASE + i)) = i;
    }

    for (i = 0; i < 256; i++) {
        val = *((volatile uint8_t *)(BSP_PSRAM_BASE + i));
        MSG("addr = 0x%08X, val = 0x%08X\r\n", (BSP_PSRAM_BASE + i), *((volatile uint8_t *)(BSP_PSRAM_BASE + i)));

        if ((uint8_t)i != val) {
            MSG("Error!\r\n");

            while (1)
                ;
        }
    }
}
#endif

int main(void)
{
    uint8_t *picture;
    uint32_t length;
    uint8_t q;
    uint8_t psram_id[8] = { 0 };

    bflb_platform_init(0);

    bsp_sf_psram_init(1);
    bsp_sf_psram_read_id(psram_id);
    MSG("BSP SF PSRAM ID: %02X %02X %02X %02X %02X %02X %02X %02X. \n",
        psram_id[0], psram_id[1], psram_id[2], psram_id[3], psram_id[4], psram_id[5], psram_id[6], psram_id[7]);

    usbd_desc_register(cdc_descriptor);

    usbd_cdc_add_acm_interface(&cdc_class, &cdc_cmd_intf);
    usbd_cdc_add_acm_interface(&cdc_class, &cdc_data_intf);
    usbd_interface_add_endpoint(&cdc_data_intf, &cdc_out_ep);
    usbd_interface_add_endpoint(&cdc_data_intf, &cdc_in_ep);

    memset(out_buffer, 'a', 64);
    usb_fs = usb_dc_init();

    if (usb_fs) {
        device_open(usb_fs, 0);
        device_control(usb_fs, DEVICE_CTRL_SET_INT, (void *)(USB_EP2_DATA_IN_IT | USB_EP1_DATA_OUT_IT));
    }

    trigger_init();
    cam_clk_out();

    MSG("MJPEG CASE IN INTERLEAVE MODE:\r\n");
    cam_hsync_crop(0, 2 * CAMERA_RESOLUTION_X);
    cam_vsync_crop(0, CAMERA_RESOLUTION_Y);

    if (SUCCESS != image_sensor_init(ENABLE, &camera_cfg, &mjpeg_cfg)) {
        MSG("Camera Init error!\n");
        BL_CASE_FAIL;
        while (1) {
        }
    }

    MSG("Camera Init success!\n");
    mjpeg_start();
    cam_start();

#if PSRAM_TEST
    // test32();
    // test8();
#endif

    while (1) {
#if USB_CDC_SEND
        static uint8_t aaa = 0;

        while (SUCCESS != mjpeg_get_one_frame(&picture, &length, &q)) {
        }

        if (flag == 0) {
            mjpeg_drop_one_frame();
            continue;
        }

        flag = 0;

        // MSG("pic %x, len %d \n", (uint32_t)picture, length);
        if (++aaa < 3) {
            mjpeg_drop_one_frame();
            continue;
        }

        aaa = 0;
        // Test_H();
        gpio_write(GPIO_PIN_22, 1);
        mjpeg_drop_one_frame();
        CamUsbSend(frame_head, sizeof(frame_head));

        if ((uint32_t)(picture + length) >= MJPEG_ENDADDR) {
            uint32_t len = (uint32_t)(picture + length) - MJPEG_ENDADDR;
            CamUsbSend(picture, MJPEG_ENDADDR - (uint32_t)picture);
            CamUsbSend((uint8_t *)MJPEG_WRITE_ADDR, len);
        } else {
            CamUsbSend(picture, length);
        }

        CamUsbSend(frame_tail, sizeof(frame_tail));
        // Test_L();
        gpio_write(GPIO_PIN_22, 0);
#endif

#if UART_SEND
        static uint8_t aaa = 0;

        while (SUCCESS != mjpeg_get_one_frame(&picture, &length, &q)) {
        }

        flag = 0;

        // MSG("pic %x, len %d \n", (uint32_t)picture, length);
        if (++aaa < 3) {
            mjpeg_drop_one_frame();
            continue;
        }

        aaa = 0;
        MSG("picturestartjpeg");

        if ((uint32_t)(picture + length) >= MJPEG_ENDADDR) {
            uint32_t len = (uint32_t)(picture + length) - MJPEG_ENDADDR;
            UART_SendData(0, picture, MJPEG_ENDADDR - (uint32_t)picture);
            UART_SendData(0, (uint8_t *)MJPEG_WRITE_ADDR, len);
            mjpeg_drop_one_frame();
            continue;
        } else {
            UART_SendData(0, picture, length);
        }

        MSG("pictureend");
#endif
    }
}
