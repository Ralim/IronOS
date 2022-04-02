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
#include "usbd_audio.h"
#include "usbd_hid.h"
#include "hal_dma.h"
#include "music.h"

#define USBD_VID           0xffff
#define USBD_PID           0xffff
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define AUDIO_IN_EP         0x81
#define HID_INT_EP          0x82
#define HID_INT_EP_SIZE     8
#define HID_INT_EP_INTERVAL 10

#define USB_HID_CONFIG_DESC_SIZ    34
#define HID_MOUSE_REPORT_DESC_SIZE 74

/* AUDIO Class Config */
#define USBD_AUDIO_FREQ 16000U

#define AUDIO_SAMPLE_FREQ(frq) (uint8_t)(frq), (uint8_t)((frq >> 8)), (uint8_t)((frq >> 16))

/* AudioFreq * DataSize (2 bytes) * NumChannels (Stereo: 2) */
#define AUDIO_OUT_PACKET ((uint32_t)((USBD_AUDIO_FREQ * 2 * 2) / 1000))
/* 16bit(2 Bytes) 单声道(Mono:1) */
#define AUDIO_IN_PACKET ((uint32_t)((USBD_AUDIO_FREQ * 2 * 1) / 1000))

#define MIC_IT_ID 0x01
#define MIC_FU_ID 0x02
#define MIC_OT_ID 0x03

#if 0
#define USB_AUDIO_CONFIG_DESC_SIZ (unsigned long)(9 +  \
                                                  9 +  \
                                                  9 +  \
                                                  12 + \
                                                  11 + \
                                                  9 +  \
                                                  9 +  \
                                                  9 +  \
                                                  7 +  \
                                                  11 + \
                                                  9 +  \
                                                  7 +  \
                                                  25)

USB_DESC_SECTION const uint8_t audio_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0001, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x03, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),

    /* ------------------ AudioControl Interface ------------------ */
    /* USB Microphone Standard AC Interface Descriptor */
    0x09,                          /* bLength */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType */
    0x00,                          /* bInterfaceNumber */
    0x00,                          /* bAlternateSetting */
    0x00,                          /* bNumEndpoints */
    USB_DEVICE_CLASS_AUDIO,        /* bInterfaceClass */
    AUDIO_SUBCLASS_AUDIOCONTROL,   /* bInterfaceSubClass */
    AUDIO_PROTOCOL_UNDEFINED,      /* bInterfaceProtocol */
    0x00,                          /* iInterface */
    /* 09 byte*/

    /* USB Microphone Class-specific AC Interface Descriptor */
    0x09,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_HEADER,            /* bDescriptorSubtype */
    0x00, /* 1.00 */                 /* bcdADC */
    0x01,
    0x29, /* wTotalLength */
    0x00,
    0x01, /* bInCollection */
    0x01, /* baInterfaceNr */
    /* 09 byte*/

    /* USB Microphone Input Terminal Descriptor */
    0x0C,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_INPUT_TERMINAL,    /* bDescriptorSubtype */
    MIC_IT_ID,                       /* bTerminalID */
    0x01,                            /* wTerminalType : Microphone 0x0201 */
    0x02,
    0x00, /* bAssocTerminal */
    0x01, /* bNrChannels */
    0x01, /* wChannelConfig : Mono sets no position bits */
    0x00,
    0x00, /* iChannelNames */
    0x00, /* iTerminal */
    /* 12 byte*/

    /* USB Microphone Audio Feature Unit Descriptor */
    0x0B,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_FEATURE_UNIT,      /* bDescriptorSubtype */
    MIC_FU_ID,                       /* bUnitID */
    MIC_IT_ID,                       /* bSourceID */
    0x02,                            /* bControlSize */
    0x01,                            /* bmaControls(0) Mute */
    0x00,
    0x02, /* bmaControls(1) Volume */
    0x00,
    0x00, /* iTerminal */
    /* 11 byte*/

    /*USB Microphone Output Terminal Descriptor */
    0x09,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_OUTPUT_TERMINAL,   /* bDescriptorSubtype */
    MIC_OT_ID,                       /* bTerminalID */
    0x01,                            /* wTerminalType : USB Streaming */
    0x01,
    0x00,      /* bAssocTerminal */
    MIC_FU_ID, /* bSourceID */
    0x00,      /* iTerminal */
    /* 09 byte*/

    /* --------------- AudioStreaming Interface --------------- */
    /* USB Microphone Standard AS Interface Descriptor - Audio Streaming Zero Bandwith */
    /* Interface 1, Alternate Setting 0                                             */
    0x09,                          /* bLength */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType */
    0x01,                          /* bInterfaceNumber */
    0x00,                          /* bAlternateSetting */
    0x00,                          /* bNumEndpoints */
    USB_DEVICE_CLASS_AUDIO,        /* bInterfaceClass */
    AUDIO_SUBCLASS_AUDIOSTREAMING, /* bInterfaceSubClass */
    AUDIO_PROTOCOL_UNDEFINED,      /* bInterfaceProtocol */
    0x00,                          /* iInterface */
    /* 09 byte*/

    /* USB Microphone Standard AS Interface Descriptor - Audio Streaming Operational */
    /* Interface 1, Alternate Setting 1                                           */
    0x09,                          /* bLength */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType */
    0x01,                          /* bInterfaceNumber */
    0x01,                          /* bAlternateSetting */
    0x01,                          /* bNumEndpoints */
    USB_DEVICE_CLASS_AUDIO,        /* bInterfaceClass */
    AUDIO_SUBCLASS_AUDIOSTREAMING, /* bInterfaceSubClass */
    AUDIO_PROTOCOL_UNDEFINED,      /* bInterfaceProtocol */
    0x00,                          /* iInterface */
    /* 09 byte*/

    /* USB Microphone Class-specific AS General Interface Descriptor */
    0X07,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_STREAMING_GENERAL,         /* bDescriptorSubtype */
    MIC_OT_ID,                       /* bTerminalLink : Unit ID of the Output Terminal*/
    0x01,                            /* bDelay */
    0x01,                            /* wFormatTag : AUDIO_FORMAT_PCM */
    0x00,
    /* 07 byte*/

    /* USB Microphone Audio Type I Format Type Descriptor */
    0x0B,                               /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,    /* bDescriptorType */
    AUDIO_STREAMING_FORMAT_TYPE,        /* bDescriptorSubtype */
    AUDIO_FORMAT_TYPE_I,                /* bFormatType */
    0x01,                               /* bNrChannels */
    0x02,                               /* bSubFrameSize : 2 Bytes per audio subframe */
    0x10,                               /* bBitResolution : 16 bits per sample */
    0x01,                               /* bSamFreqType : only one frequency supported */
    AUDIO_SAMPLE_FREQ(USBD_AUDIO_FREQ), /* tSamFreq : Audio sampling frequency coded on 3 bytes */
    /* 11 byte*/

    /* USB Microphone Standard AS Audio Data Endpoint Descriptor */
    0x09,                         /* bLength */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType */
    AUDIO_IN_EP,                  /* bEndpointAddress : IN endpoint 1 */
    0x01,                         /* bmAttributes */
    LO_BYTE(AUDIO_IN_PACKET),     /* wMaxPacketSize */
    HI_BYTE(AUDIO_IN_PACKET),
    0x01, /* bInterval : one packet per frame */
    0x00, /* bRefresh */
    0x00, /* bSynchAddress */
    /* 09 byte*/

    /* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor */
    0x07,                           /* bLength */
    AUDIO_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_ENDPOINT_GENERAL,         /* bDescriptor */
    0x00,                           /* bmAttributes AUDIO_SAMPLING_FREQ_CONTROL */
    0x00,                           /* bLockDelayUnits */
    0x00,                           /* wLockDelay */
    0x00,
    /* 07 byte*/
    /************** Descriptor of Joystick Mouse interface ****************/
    /* 09 */
    0x09,                          /* bLength: Interface Descriptor size */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType: Interface descriptor type */
    0x02,                          /* bInterfaceNumber: Number of Interface */
    0x00,                          /* bAlternateSetting: Alternate setting */
    0x01,                          /* bNumEndpoints */
    0x03,                          /* bInterfaceClass: HID */
    0x01,                          /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x02,                          /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,                             /* iInterface: Index of string descriptor */
    /******************** Descriptor of Joystick Mouse HID ********************/
    /* 18 */
    0x09,                    /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE_HID, /* bDescriptorType: HID */
    0x11,                    /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,                       /* bCountryCode: Hardware target country */
    0x01,                       /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,                       /* bDescriptorType */
    HID_MOUSE_REPORT_DESC_SIZE, /* wItemLength: Total length of Report descriptor */
    0x00,
    /******************** Descriptor of Mouse endpoint ********************/
    /* 27 */
    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
    HID_INT_EP,                   /* bEndpointAddress: Endpoint Address (IN) */
    0x03,                         /* bmAttributes: Interrupt endpoint */
    HID_INT_EP_SIZE,              /* wMaxPacketSize: 4 Byte max */
    0x00,
    HID_INT_EP_INTERVAL, /* bInterval: Polling Interval */

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
    'A', 0x00,                  /* wcChar9 */
    'U', 0x00,                  /* wcChar10 */
    'D', 0x00,                  /* wcChar11 */
    'I', 0x00,                  /* wcChar12 */
    'O', 0x00,                  /* wcChar13 */
    ' ', 0x00,                  /* wcChar14 */
    'D', 0x00,                  /* wcChar15 */
    'E', 0x00,                  /* wcChar16 */
    'M', 0x00,                  /* wcChar17 */
    'm', 0x00,                  /* wcChar18 */
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

#else

#define USB_AUDIO_CONFIG_DESC_SIZ (unsigned long)(9 +  \
                                                  9 +  \
                                                  9 +  \
                                                  12 + \
                                                  9 +  \
                                                  7 +  \
                                                  10 + \
                                                  9 +  \
                                                  9 +  \
                                                  7 +  \
                                                  11 + \
                                                  9 +  \
                                                  7 +  \
                                                  25)

USB_DESC_SECTION const uint8_t audio_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0001, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_AUDIO_CONFIG_DESC_SIZ, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),

    /* ------------------ AudioControl Interface ------------------ */
    /* USB Microphone Standard AC Interface Descriptor */
    0x09,                          /* bLength */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType */
    0x00,                          /* bInterfaceNumber */
    0x00,                          /* bAlternateSetting */
    0x00,                          /* bNumEndpoints */
    USB_DEVICE_CLASS_AUDIO,        /* bInterfaceClass */
    AUDIO_SUBCLASS_AUDIOCONTROL,   /* bInterfaceSubClass */
    AUDIO_PROTOCOL_UNDEFINED,      /* bInterfaceProtocol */
    0x00,                          /* iInterface */
    /* 09 byte*/

    /* USB Microphone Class-specific AC Interface Descriptor */
    0x09,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_HEADER,            /* bDescriptorSubtype */
    0x00, /* 1.00 */                 /* bcdADC */
    0x01,
    0x2f, /* wTotalLength */
    0x00,
    0x01, /* bInCollection */
    0x01, /* baInterfaceNr */
    /* 09 byte*/

    /* USB Microphone Input Terminal Descriptor */
    0x0C,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_INPUT_TERMINAL,    /* bDescriptorSubtype */
    0x02,                            /* bTerminalID */
    0x05,                            /* wTerminalType : Microphone 0x0201 */
    0x02,
    0x00, /* bAssocTerminal */
    0x02, /* bNrChannels */
    0x03, /* wChannelConfig : Mono sets no position bits */
    0x00,
    0x00, /* iChannelNames */
    0x00, /* iTerminal */
    /* 12 byte*/

    /*USB Microphone Output Terminal Descriptor */
    0x09,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_OUTPUT_TERMINAL,   /* bDescriptorSubtype */
    0x12,                            /* bTerminalID */
    0x01,                            /* wTerminalType : speaker */
    0x01,
    0x02, /* bAssocTerminal */
    0x21, /* bSourceID */
    0x00, /* iTerminal */
    /* 09 byte*/

    0x07,
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,
    AUDIO_CONTROL_SELECTOR_UNIT,
    0x21, // bUnitID
    0x01, //bNrInPins
    0x32, //baSourceID0
    0x00, //iSelector

    /* USB Microphone Audio Feature Unit Descriptor */
    0x0a,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_FEATURE_UNIT,      /* bDescriptorSubtype */
    0x32,                            /* bUnitID */
    0x02,                            /* bSourceID */
    0x01,                            /* bControlSize */
    0x03,                            /* bmaControls(0) Mute */
    0x00,                            /* bmaControls(1) Volume */
    0x00,                            /* bmaControls(2) Volume */
    0x00,                            /* iFeature */
    /* 10 byte*/

    /* --------------- AudioStreaming Interface --------------- */
    /* USB Microphone Standard AS Interface Descriptor - Audio Streaming Zero Bandwith */
    /* Interface 1, Alternate Setting 0                                             */
    0x09,                          /* bLength */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType */
    0x01,                          /* bInterfaceNumber */
    0x00,                          /* bAlternateSetting */
    0x00,                          /* bNumEndpoints */
    USB_DEVICE_CLASS_AUDIO,        /* bInterfaceClass */
    AUDIO_SUBCLASS_AUDIOSTREAMING, /* bInterfaceSubClass */
    AUDIO_PROTOCOL_UNDEFINED,      /* bInterfaceProtocol */
    0x00,                          /* iInterface */
    /* 09 byte*/

    /* USB Microphone Standard AS Interface Descriptor - Audio Streaming Operational */
    /* Interface 1, Alternate Setting 1                                           */
    0x09,                          /* bLength */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType */
    0x01,                          /* bInterfaceNumber */
    0x01,                          /* bAlternateSetting */
    0x01,                          /* bNumEndpoints */
    USB_DEVICE_CLASS_AUDIO,        /* bInterfaceClass */
    AUDIO_SUBCLASS_AUDIOSTREAMING, /* bInterfaceSubClass */
    AUDIO_PROTOCOL_UNDEFINED,      /* bInterfaceProtocol */
    0x00,                          /* iInterface */
    /* 09 byte*/

    /* USB Microphone Class-specific AS General Interface Descriptor */
    0X07,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_STREAMING_GENERAL,         /* bDescriptorSubtype */
    0x12,                            /* bTerminalLink : Unit ID of the Output Terminal*/
    0x01,                            /* bDelay */
    0x01,                            /* wFormatTag : AUDIO_FORMAT_PCM */
    0x00,
    /* 07 byte*/

    /* USB Microphone Audio Type I Format Type Descriptor */
    0x0b,                               /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE,    /* bDescriptorType */
    AUDIO_STREAMING_FORMAT_TYPE,        /* bDescriptorSubtype */
    AUDIO_FORMAT_TYPE_I,                /* bFormatType */
    0x02,                               /* bNrChannels */
    0x02,                               /* bSubFrameSize : 2 Bytes per audio subframe */
    0x10,                               /* bBitResolution : 16 bits per sample */
    0x01,                               /* bSamFreqType : only one frequency supported */
    AUDIO_SAMPLE_FREQ(USBD_AUDIO_FREQ), /* tSamFreq : Audio sampling frequency coded on 3 bytes */
    /* 11 byte*/

    /* USB Microphone Standard AS Audio Data Endpoint Descriptor */
    0x09,                         /* bLength */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType */
    AUDIO_IN_EP,                  /* bEndpointAddress : IN endpoint 1 */
    0x01,                         /* bmAttributes */
    LO_BYTE(AUDIO_IN_PACKET),     /* wMaxPacketSize */
    HI_BYTE(AUDIO_IN_PACKET),
    0x01, /* bInterval : one packet per frame */
    0x00, /* bRefresh */
    0x00, /* bSynchAddress */
    /* 09 byte*/

    /* USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor */
    0x07,                           /* bLength */
    AUDIO_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_ENDPOINT_GENERAL,         /* bDescriptor */
    0x00,                           /* bmAttributes AUDIO_SAMPLING_FREQ_CONTROL */
    0x00,                           /* bLockDelayUnits */
    0x00,                           /* wLockDelay */
    0x00,
    /* 07 byte*/
    /************** Descriptor of Joystick Mouse interface ****************/
    /* 09 */
    0x09,                          /* bLength: Interface Descriptor size */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType: Interface descriptor type */
    0x02,                          /* bInterfaceNumber: Number of Interface */
    0x00,                          /* bAlternateSetting: Alternate setting */
    0x01,                          /* bNumEndpoints */
    0x03,                          /* bInterfaceClass: HID */
    0x01,                          /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x02,                          /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,                             /* iInterface: Index of string descriptor */
    /******************** Descriptor of Joystick Mouse HID ********************/
    /* 18 */
    0x09,                    /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE_HID, /* bDescriptorType: HID */
    0x11,                    /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,                       /* bCountryCode: Hardware target country */
    0x01,                       /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,                       /* bDescriptorType */
    HID_MOUSE_REPORT_DESC_SIZE, /* wItemLength: Total length of Report descriptor */
    0x00,
    /******************** Descriptor of Mouse endpoint ********************/
    /* 27 */
    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
    HID_INT_EP,                   /* bEndpointAddress: Endpoint Address (IN) */
    0x03,                         /* bmAttributes: Interrupt endpoint */
    HID_INT_EP_SIZE,              /* wMaxPacketSize: 4 Byte max */
    0x00,
    HID_INT_EP_INTERVAL, /* bInterval: Polling Interval */

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
    'A', 0x00,                  /* wcChar9 */
    'U', 0x00,                  /* wcChar10 */
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

    0x00
};
#endif
static const uint8_t hid_mouse_report_desc[HID_MOUSE_REPORT_DESC_SIZE] = {
    0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    0x09, 0x02, // USAGE (Mouse)
    0xA1, 0x01, // COLLECTION (Application)
    0x09, 0x01, //   USAGE (Pointer)

    0xA1, 0x00, //   COLLECTION (Physical)
    0x05, 0x09, //     USAGE_PAGE (Button)
    0x19, 0x01, //     USAGE_MINIMUM (Button 1)
    0x29, 0x03, //     USAGE_MAXIMUM (Button 3)

    0x15, 0x00, //     LOGICAL_MINIMUM (0)
    0x25, 0x01, //     LOGICAL_MAXIMUM (1)
    0x95, 0x03, //     REPORT_COUNT (3)
    0x75, 0x01, //     REPORT_SIZE (1)

    0x81, 0x02, //     INPUT (Data,Var,Abs)
    0x95, 0x01, //     REPORT_COUNT (1)
    0x75, 0x05, //     REPORT_SIZE (5)
    0x81, 0x01, //     INPUT (Cnst,Var,Abs)

    0x05, 0x01, //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30, //     USAGE (X)
    0x09, 0x31, //     USAGE (Y)
    0x09, 0x38,

    0x15, 0x81, //     LOGICAL_MINIMUM (-127)
    0x25, 0x7F, //     LOGICAL_MAXIMUM (127)
    0x75, 0x08, //     REPORT_SIZE (8)
    0x95, 0x03, //     REPORT_COUNT (2)

    0x81, 0x06, //     INPUT (Data,Var,Rel)
    0xC0, 0x09,
    0x3c, 0x05,
    0xff, 0x09,

    0x01, 0x15,
    0x00, 0x25,
    0x01, 0x75,
    0x01, 0x95,

    0x02, 0xb1,
    0x22, 0x75,
    0x06, 0x95,
    0x01, 0xb1,

    0x01, 0xc0 //   END_COLLECTION
};

/* USB HID device Configuration Descriptor */
static uint8_t hid_desc[9] __ALIGN_END = {
    /* 18 */
    0x09,                    /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE_HID, /* bDescriptorType: HID */
    0x11,                    /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,                       /* bCountryCode: Hardware target country */
    0x01,                       /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,                       /* bDescriptorType */
    HID_MOUSE_REPORT_DESC_SIZE, /* wItemLength: Total length of Report descriptor */
    0x00,
};

struct device *usb_fs;
static uint8_t play_status = 0;
static uint32_t music_tx_flag = 0;
/* 鼠标报文结构体 */
struct hid_mouse {
    uint8_t buttons;
    int8_t x;
    int8_t y;
    int8_t wheel;
};
struct hid_mouse mouse_cfg;

//static uint8_t pakcet_buffer[AUDIO_IN_PACKET] __attribute__((section(".tcm_code")));

void usbd_audio_set_interface_callback(uint8_t value)
{
    if (value) {
        play_status = 1;
        MSG("OPEN\r\n");
        device_control(usb_fs, DEVICE_CTRL_USB_DC_SET_ACK, (void *)0x81);
    } else {
        play_status = 0;
        MSG("CLOSE\r\n");
    }
}

static usbd_class_t audio_class;
static usbd_interface_t audio_control_intf;
static usbd_interface_t audio_stream_intf;

static usbd_endpoint_t audio_in_ep = {
    .ep_cb = NULL,
    .ep_addr = AUDIO_IN_EP
};

static usbd_class_t hid_class;
static usbd_interface_t hid_intf;

void usbd_hid_int_callback(uint8_t ep)
{
    mouse_cfg.x += 10;
    mouse_cfg.y = 0;
    usbd_ep_write(HID_INT_EP, (uint8_t *)&mouse_cfg, 4, NULL);
}

static usbd_endpoint_t hid_in_ep = {
    .ep_cb = usbd_hid_int_callback,
    .ep_addr = HID_INT_EP
};
extern struct device *usb_dc_init(void);
struct device *dma_ch2;

uint32_t audio_pos = 0;
uint32_t musci_size = 0;
uint32_t frame_count = 0;
uint32_t frame_cnt = 0;
uint32_t last_frame_size = 0;

int main(void)
{
    bflb_platform_init(0);

    usbd_desc_register(audio_descriptor);
    usbd_audio_add_interface(&audio_class, &audio_control_intf);
    usbd_audio_add_interface(&audio_class, &audio_stream_intf);
    usbd_interface_add_endpoint(&audio_stream_intf, &audio_in_ep);
    usbd_hid_add_interface(&hid_class, &hid_intf);
    usbd_interface_add_endpoint(&hid_intf, &hid_in_ep);
    usbd_hid_report_descriptor_register(2, hid_mouse_report_desc, HID_MOUSE_REPORT_DESC_SIZE);
    usb_fs = usb_dc_init();

    if (usb_fs) {
        //device_control(usb_fs, DEVICE_CTRL_SET_INT, (void *)(USB_EP2_DATA_IN_IT));
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
        device_control(usb_fs, DEVICE_CTRL_USB_DC_SET_TX_DMA, (void *)AUDIO_IN_EP);
    }

    while (1) {
        if (play_status == 1) {
            if (music_tx_flag == 0) {
                musci_size = sizeof(music);
                frame_count = musci_size / 4064;
                last_frame_size = musci_size % 4064;
                frame_cnt = 0;
                audio_pos = 0;

                if (last_frame_size % AUDIO_IN_PACKET) {
                    MSG("data len is not AUDIO_IN_PACKET multiple\r\n");
                    while (1) {
                    }
                }

                MSG("frame_count:%d,last_frame_size:%d\r\n", frame_count, last_frame_size);
                music_tx_flag = 1;
            }
            if (!dma_channel_check_busy(dma_ch2)) {
                if (frame_cnt < frame_count) {
                    device_write(usb_fs, AUDIO_IN_EP, &music[audio_pos], 4064);
                    frame_cnt++;
                    audio_pos += 4064;
                } else {
                    device_write(usb_fs, AUDIO_IN_EP, &music[audio_pos], last_frame_size);
                    music_tx_flag = 0;
                }
            }
        }

        __asm volatile("nop");
    }
}
