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
#include "hal_dma.h"
#include "hal_i2s.h"
#include "hal_dma.h"
#include "bsp_es8388.h"

#define USBD_VID           0xffff
#define USBD_PID           0xffff
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define AUDIO_IN_EP  0x81
#define AUDIO_OUT_EP 0x02

#define USB_HID_CONFIG_DESC_SIZ    34
#define HID_MOUSE_REPORT_DESC_SIZE 74

/* AUDIO Class Config */
#define USBD_AUDIO_FREQ 16000U

#define AUDIO_SAMPLE_FREQ(frq) (uint8_t)(frq), (uint8_t)((frq >> 8)), (uint8_t)((frq >> 16))

/* AudioFreq * DataSize (2 bytes) * NumChannels (Stereo: 2) */
#define AUDIO_OUT_PACKET ((uint32_t)((USBD_AUDIO_FREQ * 2 * 2) / 1000))
/* 16bit(2 Bytes) 双声道(Mono:2) */
#define AUDIO_IN_PACKET ((uint32_t)((USBD_AUDIO_FREQ * 2 * 2) / 1000))

#define MIC_IT_ID 0x01
#define MIC_FU_ID 0x02
#define MIC_OT_ID 0x03

#define USB_AUDIO_CONFIG_DESC_SIZ (unsigned long)(9 +  \
                                                  9 +  \
                                                  10 + \
                                                  12 + \
                                                  12 + \
                                                  9 +  \
                                                  9 +  \
                                                  10 + \
                                                  9 +  \
                                                  9 +  \
                                                  9 +  \
                                                  7 +  \
                                                  11 + \
                                                  9 +  \
                                                  7 +  \
                                                  9 +  \
                                                  9 +  \
                                                  7 +  \
                                                  11 + \
                                                  9 +  \
                                                  7)

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
    0x0a,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_HEADER,            /* bDescriptorSubtype */
    0x00, /* 1.00 */                 /* bcdADC */
    0x01,
    0x47, /* wTotalLength */
    0x00,
    0x02, /* bInCollection */
    0x01, /* baInterfaceNr */
    0x02, /* baInterfaceNr */
    /* 09 byte*/

    /* USB Microphone Input Terminal Descriptor */
    0x0C,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_INPUT_TERMINAL,    /* bDescriptorSubtype */
    0x02,                            /* bTerminalID */
    0x01,                            /* wTerminalType : Microphone 0x0201 */
    0x02,
    0x00, /* bAssocTerminal */
    0x02, /* bNrChannels */
    0x01, /* wChannelConfig : Mono sets no position bits */
    0x00,
    0x00, /* iChannelNames */
    0x00, /* iTerminal */
    /* 12 byte*/

    /* USB Microphone Input Terminal Descriptor */
    0x0C,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_INPUT_TERMINAL,    /* bDescriptorSubtype */
    0x01,                            /* bTerminalID */
    0x01,                            /* wTerminalType : Microphone 0x0101 */
    0x01,
    0x00, /* bAssocTerminal */
    0x02, /* bNrChannels */
    0x01, /* wChannelConfig : Mono sets no position bits */
    0x00,
    0x00, /* iChannelNames */
    0x00, /* iTerminal */
    /* 12 byte*/

    /*USB Microphone Output Terminal Descriptor */
    0x09,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_OUTPUT_TERMINAL,   /* bDescriptorSubtype */
    0x06,                            /* bTerminalID */
    0x01,                            /* wTerminalType : speaker */
    0x03,
    0x00, /* bAssocTerminal */
    0x09, /* bSourceID */
    0x00, /* iTerminal */
    /* 09 byte*/
    /*USB Microphone Output Terminal Descriptor */
    0x09,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_OUTPUT_TERMINAL,   /* bDescriptorSubtype */
    0x07,                            /* bTerminalID */
    0x01,                            /* wTerminalType : speaker */
    0x01,
    0x00, /* bAssocTerminal */
    0x0a, /* bSourceID */
    0x00, /* iTerminal */
    /* 09 byte*/

    /* USB Microphone Audio Feature Unit Descriptor */
    0x0a,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_FEATURE_UNIT,      /* bDescriptorSubtype */
    0x09,                            /* bUnitID */
    0x01,                            /* bSourceID */
    0x01,                            /* bControlSize */
    0x01,                            /* bmaControls(0) Mute */
    0x02,                            /* bmaControls(1) Volume */
    0x02,                            /* bmaControls(2) Volume */
    0x00,                            /* iFeature */
    /* 10 byte*/
    /* USB Microphone Audio Feature Unit Descriptor */
    0x09,                            /* bLength */
    AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType */
    AUDIO_CONTROL_FEATURE_UNIT,      /* bDescriptorSubtype */
    0x0a,                            /* bUnitID */
    0x02,                            /* bSourceID */
    0x01,                            /* bControlSize */
    0x43,                            /* bmaControls(0) Mute */
    0x00,                            /* bmaControls(1) Volume */
    0x00,                            /* iFeature */
    /* 9 byte*/

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
    0x01,                            /* bTerminalLink : Unit ID of the Output Terminal*/
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
    AUDIO_OUT_EP,                 /* bEndpointAddress : IN endpoint 1 */
    0x01,                         /* bmAttributes */
    LO_BYTE(AUDIO_OUT_PACKET),    /* wMaxPacketSize */
    HI_BYTE(AUDIO_OUT_PACKET),
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
    /* --------------- AudioStreaming Interface --------------- */
    /* USB Microphone Standard AS Interface Descriptor - Audio Streaming Zero Bandwith */
    /* Interface 1, Alternate Setting 0                                             */
    0x09,                          /* bLength */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType */
    0x02,                          /* bInterfaceNumber */
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
    0x02,                          /* bInterfaceNumber */
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
    0x07,                            /* bTerminalLink : Unit ID of the Output Terminal*/
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

struct device *usb_fs;
static uint8_t play_status = 0;

static usbd_class_t audio_class;
static usbd_interface_t audio_control_intf;
static usbd_interface_t audio_stream_intf;
static usbd_interface_t audio_stream_intf2;

struct device *i2s;
struct device *dma_ch2_i2s_tx, *dma_ch3_i2s_rx;

#define BUFF_SIZE (512)

volatile static uint8_t record_buff_using_num = 0, record_updata_flag = 0;
static uint8_t record_data_buff[2][BUFF_SIZE + 128] __attribute__((section(".system_ram"), aligned(4)));

volatile static uint8_t play_buff_using_num = 0, play_updata_flag = 0;
static uint8_t play_data_buff[2][BUFF_SIZE + 128] __attribute__((section(".system_ram"), aligned(4)));

static uint8_t vol_change_flag = 0;

static ES8388_Cfg_Type ES8388Cfg = {
    .work_mode = ES8388_CODEC_MDOE,          /*!< ES8388 work mode */
    .role = ES8388_SLAVE,                    /*!< ES8388 role */
    .mic_input_mode = ES8388_DIFF_ENDED_MIC, /*!< ES8388 mic input mode */
    .mic_pga = ES8388_MIC_PGA_9DB,           /*!< ES8388 mic PGA */
    .i2s_frame = ES8388_LEFT_JUSTIFY_FRAME,  /*!< ES8388 I2S frame */
    .data_width = ES8388_DATA_LEN_16,        /*!< ES8388 I2S dataWitdh */
};

static uint32_t usb_data_offset = 0;
static uint8_t out_buffer[64];

void usbd_audio_out_callback(uint8_t ep)
{
    uint32_t actual_read_length = 0;
    int n;

    if (!play_updata_flag) {
        n = usbd_ep_read(ep, &play_data_buff[!play_buff_using_num][usb_data_offset], 64, &actual_read_length);
    } else {
        n = usbd_ep_read(ep, out_buffer, 64, &actual_read_length);
    }

    if (n < 0) {
        usbd_ep_set_stall(ep);
        return;
    } else {
        if (!play_updata_flag) {
            usb_data_offset += actual_read_length;
            /* The data is enough */
            if (usb_data_offset >= BUFF_SIZE) {
                play_updata_flag = 1;
                usb_data_offset = 0;
            }
        } else {
            /* too much data */
            MSG("wait_flag\r\n");
        }
    }
}

void usbd_audio_set_interface_callback(uint8_t value)
{
    if (value) {
        play_status = 1;
        device_control(usb_fs, DEVICE_CTRL_USB_DC_SET_ACK, (void *)0x81);
        device_control(usb_fs, DEVICE_CTRL_USB_DC_SET_ACK, (void *)0x2);
        MSG("OPEN\r\n");
    } else {
        play_status = 0;
        usb_data_offset = 0;
        MSG("CLOSE\r\n");
    }
}

static usbd_endpoint_t audio_out_ep = {
    .ep_cb = usbd_audio_out_callback,
    .ep_addr = AUDIO_OUT_EP
};

void usbd_audio_set_volume(uint8_t vol)
{
    vol_change_flag = vol;
}

extern struct device *usb_dc_init(void);
struct device *dma_ch4_usb_tx;

static void dma_ch2_i2s_tx_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    // if (play_updata_flag) {
    //     MSG("data ok\r\n");
    //     play_buff_using_num = !play_buff_using_num;
    //     device_write(i2s, 0, play_data_buff[play_buff_using_num], BUFF_SIZE);
    //     play_updata_flag = 0;
    //     play_wait_data_flag = 0;

    // } else {
    //     play_wait_data_flag = 1;
    //     MSG("data err\r\n");
    // }

    return;
}

static void dma_ch3_i2s_rx_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    record_updata_flag = 1;

    // record_buff_using_num = !record_buff_using_num;

    // device_read(i2s, 0, record_data_buff[record_buff_using_num], BUFF_SIZE);

    return;
}

void audio_init()
{
    /* init ES8388 Codec */
    ES8388_Init(&ES8388Cfg);
    //ES8388_Reg_Dump();
    ES8388_Set_Voice_Volume(60);

    /* register & open i2s device */
    i2s_register(I2S0_INDEX, "I2S");
    i2s = device_find("I2S");
    if (i2s) {
        I2S_DEV(i2s)->iis_mode = I2S_MODE_MASTER;
        I2S_DEV(i2s)->interface_mode = I2S_MODE_LEFT;
        I2S_DEV(i2s)->sampl_freq_hz = 16 * 1000;
        I2S_DEV(i2s)->channel_num = I2S_FS_CHANNELS_NUM_2;
        I2S_DEV(i2s)->frame_size = I2S_FRAME_LEN_16;
        I2S_DEV(i2s)->data_size = I2S_DATA_LEN_16;
        I2S_DEV(i2s)->fifo_threshold = 3;
        device_open(i2s, DEVICE_OFLAG_DMA_TX | DEVICE_OFLAG_DMA_RX);
    }

    /* register & open dma device */
    dma_register(DMA0_CH2_INDEX, "dma_ch2_i2s_tx");
    dma_ch2_i2s_tx = device_find("dma_ch2_i2s_tx");
    if (dma_ch2_i2s_tx) {
        DMA_DEV(dma_ch2_i2s_tx)->direction = DMA_MEMORY_TO_PERIPH;
        DMA_DEV(dma_ch2_i2s_tx)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch2_i2s_tx)->src_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch2_i2s_tx)->dst_req = DMA_REQUEST_I2S_TX;
        DMA_DEV(dma_ch2_i2s_tx)->src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch2_i2s_tx)->dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch2_i2s_tx)->dst_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch2_i2s_tx)->src_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch2_i2s_tx)->src_width = DMA_TRANSFER_WIDTH_32BIT;
        DMA_DEV(dma_ch2_i2s_tx)->dst_width = DMA_TRANSFER_WIDTH_32BIT;
        device_open(dma_ch2_i2s_tx, 0);

        /* connect i2s device and dma device */
        device_control(i2s, DEVICE_CTRL_ATTACH_TX_DMA, (void *)dma_ch2_i2s_tx);

        /* Set the interrupt function, for double buffering*/
        device_set_callback(I2S_DEV(i2s)->tx_dma, dma_ch2_i2s_tx_irq_callback);
        device_control(I2S_DEV(i2s)->tx_dma, DEVICE_CTRL_SET_INT, NULL);
    }

    dma_register(DMA0_CH3_INDEX, "dma_ch3_i2s_rx");
    dma_ch3_i2s_rx = device_find("dma_ch3_i2s_rx");
    if (dma_ch3_i2s_rx) {
        DMA_DEV(dma_ch3_i2s_rx)->direction = DMA_PERIPH_TO_MEMORY;
        DMA_DEV(dma_ch3_i2s_rx)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch3_i2s_rx)->src_req = DMA_REQUEST_I2S_RX;
        DMA_DEV(dma_ch3_i2s_rx)->dst_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch3_i2s_rx)->src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch3_i2s_rx)->dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch3_i2s_rx)->dst_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch3_i2s_rx)->src_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch3_i2s_rx)->src_width = DMA_TRANSFER_WIDTH_32BIT;
        DMA_DEV(dma_ch3_i2s_rx)->dst_width = DMA_TRANSFER_WIDTH_32BIT;
        device_open(dma_ch3_i2s_rx, 0);

        /* connect i2s device and dma device */
        device_control(i2s, DEVICE_CTRL_ATTACH_RX_DMA, (void *)dma_ch3_i2s_rx);

        /* Set the interrupt function, for double buffering*/
        device_set_callback(I2S_DEV(i2s)->rx_dma, dma_ch3_i2s_rx_irq_callback);
        device_control(I2S_DEV(i2s)->rx_dma, DEVICE_CTRL_SET_INT, NULL);
    }
}

int main(void)
{
    bflb_platform_init(0);

    audio_init();

    usbd_desc_register(audio_descriptor);
    usbd_audio_add_interface(&audio_class, &audio_control_intf);
    usbd_audio_add_interface(&audio_class, &audio_stream_intf);
    usbd_audio_add_interface(&audio_class, &audio_stream_intf2);
    usbd_interface_add_endpoint(&audio_stream_intf, &audio_out_ep);

    usb_fs = usb_dc_init();

    if (usb_fs) {
        device_control(usb_fs, DEVICE_CTRL_SET_INT, (void *)(USB_EP2_DATA_OUT_IT));
    }
    dma_register(DMA0_CH4_INDEX, "dma_ch4_usb_tx");
    dma_ch4_usb_tx = device_find("dma_ch4_usb_tx");

    if (dma_ch4_usb_tx) {
        DMA_DEV(dma_ch4_usb_tx)->direction = DMA_MEMORY_TO_PERIPH;
        DMA_DEV(dma_ch4_usb_tx)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch4_usb_tx)->src_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch4_usb_tx)->dst_req = DMA_REQUEST_USB_EP1;
        DMA_DEV(dma_ch4_usb_tx)->src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch4_usb_tx)->dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch4_usb_tx)->src_burst_size = DMA_BURST_16BYTE;
        DMA_DEV(dma_ch4_usb_tx)->dst_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch4_usb_tx)->src_width = DMA_TRANSFER_WIDTH_8BIT;
        DMA_DEV(dma_ch4_usb_tx)->dst_width = DMA_TRANSFER_WIDTH_8BIT;
        device_open(dma_ch4_usb_tx, 0);
        // device_set_callback(dma_ch4_usb_tx, dma2_irq_callback);
        // device_control(dma_ch4_usb_tx, DEVICE_CTRL_SET_INT, NULL);
        device_control(usb_fs, DEVICE_CTRL_ATTACH_TX_DMA, dma_ch4_usb_tx);
        device_control(usb_fs, DEVICE_CTRL_USB_DC_SET_TX_DMA, (void *)AUDIO_IN_EP);
    }

    /* i2s record start */
    device_read(i2s, 0, record_data_buff[0], BUFF_SIZE);

    while (1) {
        /* Waiting for record data update */
        if (record_updata_flag) {
            if (!dma_channel_check_busy(dma_ch4_usb_tx)) {
                device_write(usb_fs, AUDIO_IN_EP, record_data_buff[!record_buff_using_num], BUFF_SIZE);
                record_updata_flag = 0;
                record_buff_using_num = !record_buff_using_num;
                device_read(i2s, 0, record_data_buff[record_buff_using_num], BUFF_SIZE);
            }
        }

        if (play_updata_flag) {
            dma_channel_stop(dma_ch2_i2s_tx);
            play_buff_using_num = !play_buff_using_num;
            device_write(i2s, 0, play_data_buff[play_buff_using_num], BUFF_SIZE);
            play_updata_flag = 0;
            usb_data_offset = 0;
        }

        if (vol_change_flag) {
            ES8388_Set_Voice_Volume(vol_change_flag);
            vol_change_flag = 0;
        }

        __asm volatile("nop");
        __asm volatile("nop");
    }
}
