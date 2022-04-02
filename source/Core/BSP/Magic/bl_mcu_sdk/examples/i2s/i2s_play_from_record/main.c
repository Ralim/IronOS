/**
 * @file fatfs_posix_api.c
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
#include "hal_i2s.h"
#include "hal_dma.h"
#include "bsp_es8388.h"

#define BUFF_SIZE (1 * 1024)

struct device *i2s;
struct device *dma_ch2, *dma_ch3;
struct device *uart;

volatile static uint8_t buff_using_num = 0;
static uint8_t Data_Buff[2][BUFF_SIZE] __attribute__((section(".system_ram"), aligned(4)));

static ES8388_Cfg_Type ES8388Cfg = {
    .work_mode = ES8388_CODEC_MDOE,          /*!< ES8388 work mode */
    .role = ES8388_SLAVE,                    /*!< ES8388 role */
    .mic_input_mode = ES8388_DIFF_ENDED_MIC, /*!< ES8388 mic input mode */
    .mic_pga = ES8388_MIC_PGA_9DB,           /*!< ES8388 mic PGA */
    .i2s_frame = ES8388_LEFT_JUSTIFY_FRAME,  /*!< ES8388 I2S frame */
    .data_width = ES8388_DATA_LEN_16,        /*!< ES8388 I2S dataWitdh */
};

static void dma_ch3_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    device_read(i2s, 0, Data_Buff[!buff_using_num], BUFF_SIZE);

    device_write(i2s, 0, Data_Buff[buff_using_num], BUFF_SIZE);

    buff_using_num = !buff_using_num;

    return;
}

int main(void)
{
    bflb_platform_init(0);

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
    dma_register(DMA0_CH3_INDEX, "dma_ch3_i2s_rx");
    dma_ch3 = device_find("dma_ch3_i2s_rx");
    if (dma_ch3) {
        DMA_DEV(dma_ch3)->direction = DMA_PERIPH_TO_MEMORY;
        DMA_DEV(dma_ch3)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch3)->src_req = DMA_REQUEST_I2S_RX;
        DMA_DEV(dma_ch3)->dst_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch3)->src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch3)->dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch3)->src_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch3)->dst_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch3)->src_width = DMA_TRANSFER_WIDTH_32BIT;
        DMA_DEV(dma_ch3)->dst_width = DMA_TRANSFER_WIDTH_32BIT;
        device_open(dma_ch3, 0);

        /* connect i2s device and dma device */
        device_control(i2s, DEVICE_CTRL_ATTACH_RX_DMA, (void *)dma_ch3);

        /* Set the interrupt function, for double buffering*/
        device_set_callback(I2S_DEV(i2s)->rx_dma, dma_ch3_irq_callback);
        device_control(I2S_DEV(i2s)->rx_dma, DEVICE_CTRL_SET_INT, NULL);
    }

    dma_register(DMA0_CH2_INDEX, "dma_ch2_i2s_tx");
    dma_ch2 = device_find("dma_ch2_i2s_tx");
    if (dma_ch2) {
        DMA_DEV(dma_ch2)->direction = DMA_MEMORY_TO_PERIPH;
        DMA_DEV(dma_ch2)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch2)->src_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch2)->dst_req = DMA_REQUEST_I2S_TX;
        DMA_DEV(dma_ch2)->src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch2)->dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch2)->src_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch2)->dst_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch2)->src_width = DMA_TRANSFER_WIDTH_32BIT;
        DMA_DEV(dma_ch2)->dst_width = DMA_TRANSFER_WIDTH_32BIT;
        device_open(dma_ch2, 0);

        /* connect i2s device and dma device */
        device_control(i2s, DEVICE_CTRL_ATTACH_TX_DMA, (void *)dma_ch2);

        /* Set the interrupt function, for double buffering*/
        device_set_callback(I2S_DEV(i2s)->tx_dma, NULL);
        device_control(I2S_DEV(i2s)->tx_dma, DEVICE_CTRL_SET_INT, NULL);
    }

    uart = device_find("debug_log");

    device_read(i2s, 0, Data_Buff[0], BUFF_SIZE);
    device_write(i2s, 0, Data_Buff[1], BUFF_SIZE);

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
