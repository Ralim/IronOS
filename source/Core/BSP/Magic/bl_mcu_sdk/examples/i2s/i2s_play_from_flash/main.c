
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
 * Note:
 *
 * This Case is currently based on BL706_AVB development board with ili9341 controller screen.
 * If you use other hardware platforms, please modify the pin Settings by yourself.
 *
 * This routine audio playback requires transcoding the audio into PCM encoding and exporting it as an audio array.
 * See fhm_onechannel_16k_20.h for audio array examples , and i2s/script for transcoding scripts.
 *
 * Your should Short connect func1 and func2 jump cap on 706_AVB Board,and Plug the Audio Codec Board(ES8388) whit speaker into the HD11 interface.
 * Audio default configuration is
 *
 *                   bitwith= 16
 *                   datarate = 16khz ,
 *                   channel number= 1 ,
 *
 * if you want to play different wav format source , pleases modify this case .
 *
 */
#include "bflb_platform.h"
#include "hal_i2s.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "fhm_onechannel_16k_20.h"
#include "bsp_es8388.h"

struct device *i2s;
struct device *dma_ch2;

static ES8388_Cfg_Type ES8388Cfg = {
    .work_mode = ES8388_CODEC_MDOE,          /*!< ES8388 work mode */
    .role = ES8388_SLAVE,                    /*!< ES8388 role */
    .mic_input_mode = ES8388_DIFF_ENDED_MIC, /*!< ES8388 mic input mode */
    .mic_pga = ES8388_MIC_PGA_3DB,           /*!< ES8388 mic PGA */
    .i2s_frame = ES8388_LEFT_JUSTIFY_FRAME,  /*!< ES8388 I2S frame */
    .data_width = ES8388_DATA_LEN_16,        /*!< ES8388 I2S dataWitdh */
};

int main(void)
{
    bflb_platform_init(0);

    /* init ES8388 Codec */
    ES8388_Init(&ES8388Cfg);
    ES8388_Set_Voice_Volume(20);

    MSG("Play Music Form flash\r\n");

    /* register & open i2s device */
    i2s_register(I2S0_INDEX, "I2S");
    i2s = device_find("I2S");
    if (i2s) {
        I2S_DEV(i2s)->iis_mode = I2S_MODE_MASTER;
        I2S_DEV(i2s)->interface_mode = I2S_MODE_LEFT;
        I2S_DEV(i2s)->sampl_freq_hz = 16 * 1000;
        I2S_DEV(i2s)->channel_num = I2S_FS_CHANNELS_NUM_MONO;
        I2S_DEV(i2s)->frame_size = I2S_FRAME_LEN_16;
        I2S_DEV(i2s)->data_size = I2S_DATA_LEN_16;
        I2S_DEV(i2s)->fifo_threshold = 3;
        device_open(i2s, DEVICE_OFLAG_DMA_TX | DEVICE_OFLAG_DMA_RX);
    }

    /* register & open dma device */
    dma_register(DMA0_CH2_INDEX, "dma_ch2_i2s_tx");
    dma_ch2 = device_find("dma_ch2_i2s_tx");
    if (dma_ch2) {
        DMA_DEV(dma_ch2)->direction = DMA_MEMORY_TO_PERIPH;
        DMA_DEV(dma_ch2)->transfer_mode = DMA_LLI_CYCLE_MODE;
        DMA_DEV(dma_ch2)->src_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch2)->dst_req = DMA_REQUEST_I2S_TX;
        DMA_DEV(dma_ch2)->src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch2)->dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch2)->src_width = DMA_TRANSFER_WIDTH_16BIT;
        DMA_DEV(dma_ch2)->dst_width = DMA_TRANSFER_WIDTH_16BIT;
        DMA_DEV(dma_ch2)->src_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch2)->dst_burst_size = DMA_BURST_4BYTE;
        device_open(dma_ch2, 0);

        /* connect i2s device and dma device */
        device_control(i2s, DEVICE_CTRL_ATTACH_TX_DMA, (void *)dma_ch2);
    }

    /* start send */
    device_write(i2s, 0, fhm_onechannel_16k_20, sizeof(fhm_onechannel_16k_20));

    MSG("Play Music start \r\n");

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
