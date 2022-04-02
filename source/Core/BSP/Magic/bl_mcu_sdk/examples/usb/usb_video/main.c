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
#include "usbd_video.h"
#include "pic_data.h"
#include "hal_dma.h"
#include "hal_cam.h"
#include "hal_mjpeg.h"
#include "bsp_sf_psram.h"
#include "bsp_image_sensor.h"

#define USING_DMA
#define USING_CAMERA
//#define PSRAM_TEST
//#define TIM_TEST

#define WIDTH             (unsigned int)(640)
#define HEIGHT            (unsigned int)(480)
#define VIDEO_PACKET_SIZE (unsigned int)(1021 + 2)

#define CAM_FPS        (20)
#define INTERVAL       (unsigned long)(10000000 / CAM_FPS)
#define MIN_BIT_RATE   (unsigned long)(WIDTH * HEIGHT * 16 * CAM_FPS) //16 bit
#define MAX_BIT_RATE   (unsigned long)(WIDTH * HEIGHT * 16 * CAM_FPS)
#define MAX_FRAME_SIZE (unsigned long)(WIDTH * HEIGHT * 2)

#define MJPEG_ENDADDR       (0x26500000)
#define CAMERA_RESOLUTION_X (640)
#define CAMERA_RESOLUTION_Y (480)
#define CAMERA_FRAME_SIZE   (CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y * 2)
#define CAMERA_WRITE_ADDR   (0x4201E000)
#define MJPEG_READ_ADDR     CAMERA_WRITE_ADDR //0x42022000 0x42020800  0x4201E000 0x26000000
#define MJPEG_READ_SIZE     (5)
#define YUV_USE             (1)
#if (YUV_USE == 1)
#define CAMERA_BUFFER_SIZE_WHEN_MJPEG (CAMERA_RESOLUTION_X * 2 * 8 * MJPEG_READ_SIZE)
#else
#define CAMERA_BUFFER_SIZE_WHEN_MJPEG (CAMERA_RESOLUTION_X * 8 * MJPEG_READ_SIZE)
#endif
#define MJPEG_WRITE_ADDR (0x26008000)
#define MJPEG_WRITE_SIZE (MJPEG_ENDADDR - MJPEG_WRITE_ADDR - CAMERA_BUFFER_SIZE_WHEN_MJPEG)

#define FRAM_HEAD_LENGTH    (1021)
#define PACKET_HEAD_LENGTH  (2)
#define PACKET_BODY_LENGTH  (1021)
#define PACKET_TAIL_LENGTH  (0)
#define PACKET_TOTAL_LENGTH (PACKET_HEAD_LENGTH + PACKET_BODY_LENGTH + PACKET_TAIL_LENGTH)

struct video_probe_and_commit_controls probe = {
    .hintUnion.bmHint = 0,
    .hintUnion1.bmHint = 0,
    .bFormatIndex = 1,
    .bFrameIndex = 1,
    .dwFrameInterval = INTERVAL,
    .wKeyFrameRate = 0,
    .wPFrameRate = 0,
    .wCompQuality = 0,
    .wCompWindowSize = 0,
    .wDelay = 0,
    .dwMaxVideoFrameSize = MAX_FRAME_SIZE,
    .dwMaxPayloadTransferSize = 0,
    .dwClockFrequency = 0,
    .bmFramingInfo = 0,
    .bPreferedVersion = 0,
    .bMinVersion = 0,
    .bMaxVersion = 0,
};
struct video_probe_and_commit_controls commit = {
    .hintUnion.bmHint = 0,
    .hintUnion1.bmHint = 0,
    .bFormatIndex = 1,
    .bFrameIndex = 1,
    .dwFrameInterval = INTERVAL,
    .wKeyFrameRate = 0,
    .wPFrameRate = 0,
    .wCompQuality = 0,
    .wCompWindowSize = 0,
    .wDelay = 0,
    .dwMaxVideoFrameSize = MAX_FRAME_SIZE,
    .dwMaxPayloadTransferSize = 0,
    .dwClockFrequency = 0,
    .bmFramingInfo = 0,
    .bPreferedVersion = 0,
    .bMinVersion = 0,
    .bMaxVersion = 0,
};

#define USB_VIDEO_DESC_SIZ (unsigned long)(9 +  \
                                           8 +  \
                                           9 +  \
                                           13 + \
                                           17 + \
                                           9 +  \
                                           9 +  \
                                           14 + \
                                           11 + \
                                           38 + \
                                           9 +  \
                                           7)

#define VC_TERMINAL_SIZ (unsigned int)(13 + 17 + 9)
#define VC_HEADER_SIZ   (unsigned int)(14 + 11 + 38)

#define USBD_VID           0xffff
#define USBD_PID           0xffff
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

USB_DESC_SECTION const uint8_t video_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0001, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_VIDEO_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    /* Interface Association Descriptor */
    0x08,                                      // bLength                  8
    USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION, // bDescriptorType         11
    0x00,                                      /* bFirstInterface: Interface number of the VideoControl interface that is associated with this function*/
    0x02,                                      /* Number of contiguous Video interfaces that are associated with this function */
    0x0E,                                      /* bFunction Class: CC_VIDEO*/
    0x03,                                      /* bFunction sub Class: SC_VIDEO_INTERFACE_COLLECTION */
    0x00,                                      /* bFunction protocol : PC_PROTOCOL_UNDEFINED*/
    0x02,                                      /* iFunction */

    /* VideoControl Interface Descriptor */
    /* Standard VC Interface Descriptor  = interface 0 */
    0x09,                          /* bLength */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType */
    0x00,                          /* bInterfaceNumber */
    0x00,                          /* bAlternateSetting */
    0x00,                          /* bNumEndpoints:1 endpoint (interrupt endpoint) */
    0x0e,                          /* bInterfaceClass : CC_VIDEO */
    0x01,                          /* bInterfaceSubClass : SC_VIDEOCONTROL */
    0x00,                          /* bInterfaceProtocol : PC_PROTOCOL_UNDEFINED */
    0x02,                          /* iInterface:Index to string descriptor that contains the string <Your Product Name> */

    /*Class-specific VideoControl Interface Descriptor */
    0x0d,                   /* bLength */
    0x24,                   /* bDescriptorType : CS_INTERFACE */
    0x01,                   /* bDescriptorSubType : VC_HEADER subtype */
    WBVAL(0x0100),          /* bcdUVC : Revision of class specification that this device is based upon. For this example, the device complies with Video Class specification version 1.1 */
    WBVAL(VC_TERMINAL_SIZ), /* wTotalLength : 30 */
    DBVAL(0x005B8D80),      /* dwClockFrequency : 0x005b8d80 -> 6,000,000 == 6MHz*/
    0x01,                   /* bInCollection : Number of streaming interfaces. */
    0x01,                   /* baInterfaceNr(1) : VideoStreaming interface 1 belongs to this VideoControl interface.*/

    /* Input Terminal Descriptor (Camera) */
    0x11,                                       // bLength                 17 15 + 2 controls
    0x24,                                       // bDescriptorType         36 (INTERFACE)
    VIDEO_VC_INPUT_TERMINAL_DESCRIPTOR_SUBTYPE, // bDescriptorSubtype       2 (INPUT_TERMINAL)
    0x01,                                       // bTerminalID              1 ID of this Terminal
    WBVAL(VIDEO_ITT_CAMERA),                    // wTerminalType       0x0201 Camera Sensor
    0x00,                                       // bAssocTerminal           0 no Terminal associated
    0x00,                                       // iTerminal                0 no description available
    WBVAL(0x0000),                              // wObjectiveFocalLengthMin 0
    WBVAL(0x0000),                              // wObjectiveFocalLengthMax 0
    WBVAL(0x0000),                              // wOcularFocalLength       0
    0x02,                                       // bControlSize             2
    0x00, 0x00,                                 // bmControls          0x0000 no controls supported

    /* Output Terminal Descriptor */
    0x09,                                        // bLength                  9
    0x24,                                        // bDescriptorType         36 (INTERFACE)
    VIDEO_VC_OUTPUT_TERMINAL_DESCRIPTOR_SUBTYPE, // bDescriptorSubtype       3 (OUTPUT_TERMINAL)
    0x02,                                        // bTerminalID              2 ID of this Terminal
    WBVAL(VIDEO_TT_STREAMING),                   // wTerminalType       0x0101 USB streaming terminal
    0x00,                                        // bAssocTerminal           0 no Terminal assiciated
    0x01,                                        // bSourceID                1 input pin connected to output pin unit 1
    0x00,                                        // iTerminal                0 no description available

    /* Video Streaming (VS) Interface Descriptor */
    0x09,                          /* bLength */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType : INTERFACE */
    0x01,                          /* bInterfaceNumber: Index of this interface */
    0x00,                          /* bAlternateSetting: Index of this alternate setting */
    0x00,                          /* bNumEndpoints : 0 endpoints – no bandwidth used*/
    0x0e,                          /* bInterfaceClass : CC_VIDEO */
    0x02,                          /* bInterfaceSubClass : SC_VIDEOSTREAMING */
    0x00,                          /* bInterfaceProtocol : PC_PROTOCOL_UNDEFINED */
    0x00,                          /* iInterface : unused */

    /*Class-specific VideoStream Header Descriptor (Input) */
    0x0e,                 /* bLength */
    0x24,                 /* bDescriptorType : CS_INTERFACE */
    0x01,                 /* bDescriptorSubType : VC_HEADER subtype */
    0x01,                 /* bNumFormats : One format descriptor follows. */
    WBVAL(VC_HEADER_SIZ), /* wTotalLength :63 */
    0x81,                 /* bEndpointAddress : 0x82 */
    0x00,                 /* bmInfo : No dynamic format change supported. */
    0x02,                 /* bTerminalLink : This VideoStreaming interface supplies terminal ID 3 (Output Terminal). */
    0x00,                 /* bStillCaptureMethod : Device supports still image capture method 0. */
    0x01,                 /* bTriggerSupport : Hardware trigger supported for still image capture */
    0x00,                 /* bTriggerUsage : Hardware trigger should initiate a still image capture. */
    0x01,                 /* bControlSize : Size of the bmaControls field */
    0x00,                 /* bmaControls : No VideoStreaming specific controls are supported.*/

    /*Payload Format(MJPEG) Descriptor */
    0x0b, /* bLength */
    0x24, /* bDescriptorType : CS_INTERFACE */
    0x06, /* bDescriptorSubType : VS_FORMAT_MJPEG subtype */
    0x01, /* bFormatIndex : First (and only) format descriptor */
    0x01, /* bNumFrameDescriptors : One frame descriptor for this format follows. */
    0x01, /* bmFlags : Uses fixed size samples.. */
    0x01, /* bDefaultFrameIndex : Default frame index is 1. */
    0x00, /* bAspectRatioX : Non-interlaced stream – not required. */
    0x00, /* bAspectRatioY : Non-interlaced stream – not required. */
    0x00, /* bmInterlaceFlags : Non-interlaced stream */
    0x00, /* bCopyProtect : No restrictions imposed on the duplication of this video stream. */

    /*Class-specific VideoStream Frame Descriptor */
    0x26,                                                                                                      /* bLength */
    0x24,                                                                                                      /* bDescriptorType : CS_INTERFACE */
    0x07,                                                                                                      /* bDescriptorSubType : VS_FRAME_MJPEG */
    0x01,                                                                                                      /* bFrameIndex : First (and only) frame descriptor */
    0x02,                                                                                                      /* bmCapabilities : Still images using capture method 0 are supported at this frame setting.D1: Fixed frame-rate. */
    WBVAL(WIDTH),                                                                                              /* wWidth (2bytes): Width of frame is 128 pixels. */
    WBVAL(HEIGHT),                                                                                             /* wHeight (2bytes): Height of frame is 64 pixels. */
    DBVAL(MIN_BIT_RATE), /* dwMinBitRate (4bytes): Min bit rate in bits/s  */                                  // 128*64*16*5 = 655360 = 0x000A0000 //5fps
    DBVAL(MAX_BIT_RATE), /* dwMaxBitRate (4bytes): Max bit rate in bits/s  */                                  // 128*64*16*5 = 655360 = 0x000A0000
    DBVAL(MAX_FRAME_SIZE), /* dwMaxVideoFrameBufSize (4bytes): Maximum video or still frame size, in bytes. */ // 128*64*2 = 16384 = 0x00004000
    DBVAL(INTERVAL), /* dwDefaultFrameInterval : 1,000,000 * 100ns -> 10 FPS */                                // 5 FPS -> 200ms -> 200,000 us -> 2,000,000 X 100ns = 0x001e8480
    0x00,                                                                                                      /* bFrameIntervalType : Continuous frame interval */
    DBVAL(INTERVAL),                                                                                           /* dwMinFrameInterval : 1,000,000 ns  *100ns -> 10 FPS */
    DBVAL(INTERVAL),                                                                                           /* dwMaxFrameInterval : 1,000,000 ns  *100ns -> 10 FPS */
    0x00, 0x00, 0x00, 0x00,                                                                                    /* dwFrameIntervalStep : No frame interval step supported. */

    /* Standard VS Interface Descriptor  = interface 1 */
    // alternate setting 1 = operational setting
    0x09,                          /* bLength */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType: INTERFACE descriptor type */
    0x01,                          /* bInterfaceNumber: Index of this interface */
    0x01,                          /* bAlternateSetting: Index of this alternate setting */
    0x01,                          /* bNumEndpoints: endpoints, 1 – data endpoint */
    0x0e,                          /* bInterfaceClass: CC_VIDEO */
    0x02,                          /* bInterfaceSubClass: SC_VIDEOSTREAMING */
    0x00,                          /* bInterfaceProtocol: PC_PROTOCOL_UNDEFINED */
    0x00,                          /* iInterface: Unused */
    /* 9 bytes, total size 137 */
    /* 1.2.2.2 Standard VideoStream Isochronous Video Data Endpoint Descriptor */
    0x07,                         /* bLength */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: ENDPOINT */
    0x81,                         /* bEndpointAddress: IN endpoint 2 */
    0x01,                         /* bmAttributes: Isochronous transfer type. Asynchronous synchronization type. */
    WBVAL(VIDEO_PACKET_SIZE),     /* wMaxPacketSize */
    0x01,                         /* bInterval: One frame interval */

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
    0x28,                       /* bLength */
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
    'V', 0x00,                  /* wcChar9 */
    'E', 0x00,                  /* wcChar10 */
    'D', 0x00,                  /* wcChar11 */
    'I', 0x00,                  /* wcChar12 */
    'O', 0x00,                  /* wcChar13 */
    ' ', 0x00,                  /* wcChar14 */
    'D', 0x00,                  /* wcChar15 */
    'E', 0x00,                  /* wcChar16 */
    'M', 0x00,                  /* wcChar17 */
    'O', 0x00,                  /* wcChar18 */
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
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

static struct device *usb_fs;
static struct device *dma_ch2;

// static uint8_t header[2] = {0x02,0x00};
ATTR_DTCM_SECTION uint8_t packet_buffer1[VIDEO_PACKET_SIZE] = { 0x02, 0x00 };
ATTR_DTCM_SECTION uint8_t packet_buffer2[VIDEO_PACKET_SIZE] = { 0x02, 0x00 };
static uint32_t picture_pos = 0;
static uint32_t packets_cnt = 0;
static uint32_t packets_in_frame;
static uint32_t last_packet_size;
static uint8_t tx_enable_flag = 0;
static uint8_t play_status = 0;
uint32_t jpeg_count = 0;

void usbd_video_set_interface_callback(uint8_t value)
{
    struct device* cam0 = device_find("camera0");

    if (value) {
        play_status = 1;
        MSG("OPEN\r\n");
        mjpeg_start();
        device_control(cam0, DEVICE_CTRL_RESUME, NULL);

    } else {
        play_status = 0;
        MSG("CLOSE\r\n");
        device_control(cam0, DEVICE_CTRL_SUSPEND, NULL);
        mjpeg_stop();
    }
}
void usbd_video_sof_callback(void)
{
    if (play_status == 1) {
        device_control(usb_fs, DEVICE_CTRL_USB_DC_SET_ACK, (void *)0x81);
        play_status = 2;
        MSG("READY\r\n");
    }
}

#ifdef PSRAM_TEST
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

#define JPG_HEAD_LENGTH_422 605
static const uint8_t jpgHeadBuf[JPG_HEAD_LENGTH_422] = {
    0xFF, 0xD8, 0xFF, 0xDB, 0x00, 0x43, 0x00, 0x10, 0x0B, 0x0C, 0x0E, 0x0C, 0x0A, 0x10, 0x0E, 0x0D,
    0x0E, 0x12, 0x11, 0x10, 0x13, 0x18, 0x28, 0x1A, 0x18, 0x16, 0x16, 0x18, 0x31, 0x23, 0x25, 0x1D,
    0x28, 0x3A, 0x33, 0x3D, 0x3C, 0x39, 0x33, 0x38, 0x37, 0x40, 0x48, 0x5C, 0x4E, 0x40, 0x44, 0x57,
    0x45, 0x37, 0x38, 0x50, 0x6D, 0x51, 0x57, 0x5F, 0x62, 0x67, 0x68, 0x67, 0x3E, 0x4D, 0x71, 0x79,
    0x70, 0x64, 0x78, 0x5C, 0x65, 0x67, 0x63, 0xFF, 0xDB, 0x00, 0x43, 0x01, 0x11, 0x12, 0x12, 0x18,
    0x15, 0x18, 0x2F, 0x1A, 0x1A, 0x2F, 0x63, 0x42, 0x38, 0x42, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
    0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0xFF, 0xC0, 0x00, 0x11,
    0x08, 0x01, 0xE0, 0x02, 0x80, 0x03, 0x01, 0x21, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 0x01, 0xFF,
    0xC4, 0x00, 0x1F, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0xFF, 0xC4, 0x00, 0xB5, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04,
    0x04, 0x00, 0x00, 0x01, 0x7D, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41,
    0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1,
    0xC1, 0x15, 0x52, 0xD1, 0xF0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19,
    0x1A, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44,
    0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64,
    0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84,
    0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2,
    0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9,
    0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
    0xD8, 0xD9, 0xDA, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3,
    0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFF, 0xC4, 0x00, 0x1F, 0x01, 0x00, 0x03, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0xFF, 0xC4, 0x00, 0xB5, 0x11, 0x00, 0x02, 0x01,
    0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02,
    0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32,
    0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0, 0x15, 0x62, 0x72,
    0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26, 0x27, 0x28, 0x29,
    0X2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x53,
    0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x73,
    0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A,
    0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8,
    0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6,
    0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE2, 0xE3, 0xE4,
    0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFF,
    0xDA, 0x00, 0x0C, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3F, 0x00
};

static mjpeg_device_t mjpeg_cfg = {
    .quality = 50,
    .yuv_format = MJPEG_YUV_FORMAT_YUV422_INTERLEAVE,
    .write_buffer_addr = MJPEG_WRITE_ADDR,
    .write_buffer_size = MJPEG_WRITE_SIZE,
    .read_buffer_addr = MJPEG_READ_ADDR,
    .read_buffer_size = MJPEG_READ_SIZE,
    .resolution_x = CAMERA_RESOLUTION_X,
    .resolution_y = CAMERA_RESOLUTION_Y,

    .packet_cut_mode = 2,
    .frame_head_length = JPG_HEAD_LENGTH_422,
    .packet_head_length = PACKET_HEAD_LENGTH,
    .packet_body_length = PACKET_BODY_LENGTH,
    .packet_tail_length = PACKET_TAIL_LENGTH,
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

static usbd_class_t video_class;
static usbd_interface_t video_control_intf;
static usbd_interface_t video_stream_intf;

static usbd_endpoint_t video_in_ep = {
    .ep_cb = NULL,
    .ep_addr = 0x81
};

extern struct device *usb_dc_init(void);

int main(void)
{
    uint32_t jpeg_frame_size;
    uint8_t *picture;
    uint8_t q;
    uint32_t double_buffer_flag = 0;
#ifdef TIM_TEST
    uint32_t timer_start = 0;
    uint32_t timer_end = 0;
#endif
    bflb_platform_init(0);

    bsp_sf_psram_init(1);

#ifdef USING_CAMERA
    cam_clk_out();

    // cam_hsync_crop(0, 2 * CAMERA_RESOLUTION_X);
    // cam_vsync_crop(0, CAMERA_RESOLUTION_Y);

    if (SUCCESS != image_sensor_init(ENABLE, &camera_cfg, &mjpeg_cfg)) {
        MSG("Camera Init error!\r\n");
        BL_CASE_FAIL;
        while (1) {
        }
    }

    MSG("Camera Init success!\r\n");

#ifdef PSRAM_TEST
    test32();
    test8();
#endif

#endif

    usbd_desc_register(video_descriptor);
    usbd_video_add_interface(&video_class, &video_control_intf);
    usbd_video_add_interface(&video_class, &video_stream_intf);
    usbd_interface_add_endpoint(&video_stream_intf, &video_in_ep);

    usb_fs = usb_dc_init();

    if (usb_fs) {
        device_control(usb_fs, DEVICE_CTRL_SET_INT, (void *)(USB_SOF_IT));
    }

    dma_register(DMA0_CH2_INDEX, "ch2");
    dma_ch2 = device_find("ch2");

    if (dma_ch2) {
        DMA_DEV(dma_ch2)->direction = DMA_MEMORY_TO_PERIPH;
        DMA_DEV(dma_ch2)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch2)->src_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch2)->dst_req = DMA_REQUEST_USB_EP1;
        DMA_DEV(dma_ch2)->src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch2)->dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch2)->src_burst_size = DMA_BURST_16BYTE;
        DMA_DEV(dma_ch2)->dst_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch2)->src_width = DMA_TRANSFER_WIDTH_8BIT;
        DMA_DEV(dma_ch2)->dst_width = DMA_TRANSFER_WIDTH_8BIT;
        device_open(dma_ch2, 0);
        // device_set_callback(dma_ch2, dma2_irq_callback);
        // device_control(dma_ch2, DEVICE_CTRL_SET_INT, NULL);
        device_control(usb_fs, DEVICE_CTRL_ATTACH_TX_DMA, dma_ch2);
        device_control(usb_fs, DEVICE_CTRL_USB_DC_SET_TX_DMA, (void *)0x81);
    }

    // header[1] ^= 0x01;//toggle bit0 every new frame

    while (1) {
        if (play_status == 2) {
            if (tx_enable_flag == 0) {
#ifdef USING_CAMERA
#ifdef TIM_TEST
                timer_start = bflb_platform_get_time_ms();
#endif

                while (SUCCESS != mjpeg_get_one_frame(&picture, &jpeg_frame_size, &q)) {
                }

                //cam_stop();
                mjpeg_stop();

                arch_memcpy_fast(picture, jpgHeadBuf, JPG_HEAD_LENGTH_422);
#else

                if (jpeg_count == 0) {
                    jpeg_frame_size = sizeof(jpeg_data);
                } else if (jpeg_count == 1) {
                    jpeg_frame_size = sizeof(jpeg_data1);
                } else if (jpeg_count == 2) {
                    jpeg_frame_size = sizeof(jpeg_data2);
                }

#endif
                tx_enable_flag = 1;
                double_buffer_flag = 0;
                packets_cnt = 0;
                picture_pos = 0;
                packet_buffer1[1] ^= 0x01; //toggle bit0 every new frame
                packet_buffer2[1] ^= 0x01; //toggle bit0 every new frame
                packets_in_frame = jpeg_frame_size / (VIDEO_PACKET_SIZE - 2) + 1;
                last_packet_size = jpeg_frame_size - ((packets_in_frame - 1) * (VIDEO_PACKET_SIZE - 2)) + 2;
                //MSG("packets_in_frame:%d,last_packet_size:%d,total_send_size:%d\r\n",packets_in_frame,last_packet_size,((packets_in_frame-1)*VIDEO_PACKET_SIZE+last_packet_size));

                arch_memcpy_fast(&packet_buffer1[2], &picture[picture_pos], VIDEO_PACKET_SIZE - 2);
                mjpeg_start();
                device_write(usb_fs, 0x81, packet_buffer1, VIDEO_PACKET_SIZE);
                double_buffer_flag = 1;
                picture_pos += VIDEO_PACKET_SIZE - 2;
                packets_cnt++;

                if (packets_cnt == (packets_in_frame - 1)) {
                    arch_memcpy_fast(&packet_buffer2[2], &picture[picture_pos], last_packet_size - 2);
                } else {
                    arch_memcpy_fast(&packet_buffer2[2], &picture[picture_pos], VIDEO_PACKET_SIZE - 2);
                }
            }

            if (packets_cnt < (packets_in_frame - 1)) {
                if (!dma_channel_check_busy(dma_ch2)) {
#ifdef USING_CAMERA
#else

                    if (jpeg_count == 0) {
                        memcpy(&packet_buffer[2], &jpeg_data[picture_pos], VIDEO_PACKET_SIZE - 2);
                    } else if (jpeg_count == 1) {
                        memcpy(&packet_buffer[2], &jpeg_data1[picture_pos], VIDEO_PACKET_SIZE - 2);
                    } else if (jpeg_count == 2) {
                        memcpy(&packet_buffer[2], &jpeg_data2[picture_pos], VIDEO_PACKET_SIZE - 2);
                    }

#endif

                    if (double_buffer_flag == 0) {
                        device_write(usb_fs, 0x81, packet_buffer1, VIDEO_PACKET_SIZE);
                        double_buffer_flag = 1;
                        picture_pos += VIDEO_PACKET_SIZE - 2;
                        packets_cnt++;

                        if (packets_cnt == (packets_in_frame - 1)) {
                            arch_memcpy_fast(&packet_buffer2[2], &picture[picture_pos], last_packet_size - 2);
                        } else {
                            arch_memcpy_fast(&packet_buffer2[2], &picture[picture_pos], VIDEO_PACKET_SIZE - 2);
                        }
                    } else {
                        device_write(usb_fs, 0x81, packet_buffer2, VIDEO_PACKET_SIZE);
                        double_buffer_flag = 0;
                        picture_pos += VIDEO_PACKET_SIZE - 2;
                        packets_cnt++;

                        if (packets_cnt == (packets_in_frame - 1)) {
                            arch_memcpy_fast(&packet_buffer1[2], &picture[picture_pos], last_packet_size - 2);
                        } else {
                            arch_memcpy_fast(&packet_buffer1[2], &picture[picture_pos], VIDEO_PACKET_SIZE - 2);
                        }
                    }
                }
            } else {
                if (!dma_channel_check_busy(dma_ch2)) {
#ifdef USING_CAMERA
#else

                    if (jpeg_count == 0) {
                        memcpy(&packet_buffer[2], &jpeg_data[picture_pos], last_packet_size - 2);
                    } else if (jpeg_count == 1) {
                        memcpy(&packet_buffer[2], &jpeg_data1[picture_pos], last_packet_size - 2);
                    } else if (jpeg_count == 2) {
                        memcpy(&packet_buffer[2], &jpeg_data2[picture_pos], last_packet_size - 2);
                    }

#endif

                    if (double_buffer_flag == 0) {
                        device_write(usb_fs, 0x81, packet_buffer1, last_packet_size);
                    } else {
                        device_write(usb_fs, 0x81, packet_buffer2, last_packet_size);
                    }

                    tx_enable_flag = 0;
#ifdef TIM_TEST
                    timer_end = bflb_platform_get_time_ms();
                    MSG("time:%d\r\n", (timer_end - timer_start));
#endif

#ifdef USING_CAMERA
#else

                    if (jpeg_count == 2) {
                        jpeg_count = 0;
                    } else {
                        jpeg_count++;
                    }

                    bflb_platform_delay_ms(500);
#endif
                }
            }
        }

        __asm volatile("nop");
    }
}
