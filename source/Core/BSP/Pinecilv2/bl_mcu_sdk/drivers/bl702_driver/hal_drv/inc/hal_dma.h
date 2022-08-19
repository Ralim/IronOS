/**
 * @file hal_dma.h
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
#ifndef __HAL_DMA__H__
#define __HAL_DMA__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_common.h"
#include "drv_device.h"
#include "bl702_config.h"

#define DEVICE_CTRL_DMA_CHANNEL_GET_STATUS 0x10
#define DEVICE_CTRL_DMA_CHANNEL_START      0x11
#define DEVICE_CTRL_DMA_CHANNEL_STOP       0x12
#define DEVICE_CTRL_DMA_CHANNEL_UPDATE     0x13
#define DEVICE_CTRL_DMA_CONFIG_SI          0x14
#define DEVICE_CTRL_DMA_CONFIG_DI          0x15

enum dma_index_type {
#ifdef BSP_USING_DMA0_CH0
    DMA0_CH0_INDEX,
#endif
#ifdef BSP_USING_DMA0_CH1
    DMA0_CH1_INDEX,
#endif
#ifdef BSP_USING_DMA0_CH2
    DMA0_CH2_INDEX,
#endif
#ifdef BSP_USING_DMA0_CH3
    DMA0_CH3_INDEX,
#endif
#ifdef BSP_USING_DMA0_CH4
    DMA0_CH4_INDEX,
#endif
#ifdef BSP_USING_DMA0_CH5
    DMA0_CH5_INDEX,
#endif
#ifdef BSP_USING_DMA0_CH6
    DMA0_CH6_INDEX,
#endif
#ifdef BSP_USING_DMA0_CH7
    DMA0_CH7_INDEX,
#endif
    DMA_MAX_INDEX
};

#define dma_channel_start(dev)        device_control(dev, DEVICE_CTRL_DMA_CHANNEL_START, NULL)
#define dma_channel_stop(dev)         device_control(dev, DEVICE_CTRL_DMA_CHANNEL_STOP, NULL)
#define dma_channel_update(dev, list) device_control(dev, DEVICE_CTRL_DMA_CHANNEL_UPDATE, list)
#define dma_channel_check_busy(dev)   device_control(dev, DEVICE_CTRL_DMA_CHANNEL_GET_STATUS, NULL)

#define DMA_LLI_ONCE_MODE  0
#define DMA_LLI_CYCLE_MODE 1

#define DMA_ADDR_INCREMENT_DISABLE 0 /*!< Addr increment mode disable */
#define DMA_ADDR_INCREMENT_ENABLE  1 /*!< Addr increment mode enable  */

#define DMA_TRANSFER_WIDTH_8BIT  0
#define DMA_TRANSFER_WIDTH_16BIT 1
#define DMA_TRANSFER_WIDTH_32BIT 2

#define DMA_BURST_1BYTE  0
#define DMA_BURST_4BYTE  1
#define DMA_BURST_8BYTE  2
#define DMA_BURST_16BYTE 3

#define DMA_ADDR_UART0_TDR (0x4000A000 + 0x88)
#define DMA_ADDR_UART0_RDR (0x4000A000 + 0x8C)
#define DMA_ADDR_UART1_TDR (0x4000A100 + 0x88)
#define DMA_ADDR_UART1_RDR (0x4000A100 + 0x8C)
#define DMA_ADDR_I2C_TDR   (0x4000A300 + 0x88)
#define DMA_ADDR_I2C_RDR   (0x4000A300 + 0x8C)
#define DMA_ADDR_SPI_TDR   (0x4000A200 + 0x88)
#define DMA_ADDR_SPI_RDR   (0x4000A200 + 0x8C)
#define DMA_ADDR_I2S_TDR   (0x4000AA00 + 0x88)
#define DMA_ADDR_I2S_RDR   (0x4000AA00 + 0x8C)
#define DMA_ADDR_ADC_RDR   (0x40002000 + 0x04)
#define DMA_ADDR_DAC_TDR   (0x40002000 + 0X48)

#define DMA_REQUEST_NONE     0x00000000 /*!< DMA request peripheral:None */
#define DMA_REQUEST_UART0_RX 0x00000000 /*!< DMA request peripheral:UART0 RX */
#define DMA_REQUEST_UART0_TX 0x00000001 /*!< DMA request peripheral:UART0 TX */
#define DMA_REQUEST_UART1_RX 0x00000002 /*!< DMA request peripheral:UART1 RX */
#define DMA_REQUEST_UART1_TX 0x00000003 /*!< DMA request peripheral:UART1 TX */
#define DMA_REQUEST_I2C0_RX  0x00000006 /*!< DMA request peripheral:I2C RX */
#define DMA_REQUEST_I2C0_TX  0x00000007 /*!< DMA request peripheral:I2C TX */
#define DMA_REQUEST_SPI0_RX  0x0000000A /*!< DMA request peripheral:SPI RX */
#define DMA_REQUEST_SPI0_TX  0x0000000B /*!< DMA request peripheral:SPI TX */
#define DMA_REQUEST_I2S_RX   0x00000014 /*!< DMA request peripheral:I2S RX */
#define DMA_REQUEST_I2S_TX   0x00000015 /*!< DMA request peripheral:I2S TX */
#define DMA_REQUEST_ADC0     0x00000016 /*!< DMA request peripheral:ADC0 */
#define DMA_REQUEST_DAC0     0x00000017 /*!< DMA request peripheral:DAC0 */
#define DMA_REQUEST_USB_EP0  0x00000018 /*!< DMA request peripheral:USB EP0*/
#define DMA_REQUEST_USB_EP1  0x00000019 /*!< DMA request peripheral:USB EP1*/
#define DMA_REQUEST_USB_EP2  0x0000001A /*!< DMA request peripheral:USB EP2*/
#define DMA_REQUEST_USB_EP3  0x0000001B /*!< DMA request peripheral:USB EP3*/
#define DMA_REQUEST_USB_EP4  0x0000001C /*!< DMA request peripheral:USB EP4*/
#define DMA_REQUEST_USB_EP5  0x0000001D /*!< DMA request peripheral:USB EP5*/
#define DMA_REQUEST_USB_EP6  0x0000001E /*!< DMA request peripheral:USB EP6*/
#define DMA_REQUEST_USB_EP7  0x0000001F /*!< DMA request peripheral:USB EP7 */

/**
 *  @brief DMA transfer direction type definition
 */
typedef enum {
    DMA_MEMORY_TO_MEMORY = 0, /*!< DMA transfer type:memory to memory */
    DMA_MEMORY_TO_PERIPH,     /*!< DMA transfer type:memory to peripheral */
    DMA_PERIPH_TO_MEMORY,     /*!< DMA transfer type:peripheral to memory */
    DMA_PERIPH_TO_PERIPH,     /*!< DMA transfer type:peripheral to peripheral */
} dma_transfer_dir_type;

typedef union {
    struct
    {
        uint32_t TransferSize : 12; /* [11: 0],        r/w,        0x0 */
        uint32_t SBSize       : 2;  /* [13:12],        r/w,        0x1 */
        uint32_t dst_min_mode : 1;  /* [   14],        r/w,        0x0 */
        uint32_t DBSize       : 2;  /* [16:15],        r/w,        0x1 */
        uint32_t dst_add_mode : 1;  /* [   17],        r/w,        0x0 */
        uint32_t SWidth       : 2;  /* [19:18],        r/w,        0x2 */
        uint32_t reserved_20  : 1;  /* [   20],       rsvd,        0x0 */
        uint32_t DWidth       : 2;  /* [22:21],        r/w,        0x2 */
        uint32_t fix_cnt      : 2;  /* [24:23],        r/w,        0x0 */
        uint32_t SLargerD     : 1;  /* [   25],        r/w,        0x0 */
        uint32_t SI           : 1;  /* [   26],        r/w,        0x1 */
        uint32_t DI           : 1;  /* [   27],        r/w,        0x1 */
        uint32_t Prot         : 3;  /* [30:28],        r/w,        0x0 */
        uint32_t I            : 1;  /* [   31],        r/w,        0x0 */
    } bits;
    uint32_t WORD;
} dma_control_data_t;

typedef struct
{
    uint32_t src_addr;
    uint32_t dst_addr;
    uint32_t nextlli;
    dma_control_data_t cfg;
} dma_lli_ctrl_t;

typedef struct dma_device {
    struct device parent;
    uint8_t id;
    uint8_t ch;
    uint8_t transfer_mode;
    uint8_t direction;
    uint32_t src_req;
    uint32_t dst_req;
    uint8_t src_addr_inc;
    uint8_t dst_addr_inc;
    uint8_t src_burst_size;
    uint8_t dst_burst_size;
    uint8_t src_width;
    uint8_t dst_width;
    dma_lli_ctrl_t *lli_cfg;
} dma_device_t;

#define DMA_DEV(dev) ((dma_device_t *)dev)

int dma_register(enum dma_index_type index, const char *name);
int dma_allocate_register(const char *name);
int dma_reload(struct device *dev, uint32_t src_addr, uint32_t dst_addr, uint32_t transfer_size);

#ifdef __cplusplus
}
#endif

#endif
