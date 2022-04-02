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
#include "bl702_glb.h"
#include "bl702_spi.h"
#include "hal_spi.h"
#include "hal_i2c.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "bsp_es8388.h"

#define SPI0_CS GPIO_PIN_10

//播放控制块
typedef struct audio_dev {
    uint8_t *buff[2];
    uint32_t buff_data_size[2]; //buff内数据长度
    uint32_t buff_size_max;     //buff的大小
    uint8_t buff_using;         //正在使用的buff

    uint8_t audio_state; //状态
    uint8_t audio_type;  //类型

    int (*audio_init)(struct audio_dev *audio_dev); //初始化函数
    int (*audio_control)(struct audio_dev *audio_dev, int cmd, void *args);

    int (*audio_callback)(struct audio_dev *audio_dev); //中断回调函数，用来重新装载buff，

    struct device *device; //i2s的device，可以考虑换成指向其他外设
    //wav_information_t *wav_information; //wav信息结构体，可以考虑换成一个共用体指向其他音乐格式数据
} audio_dev_t;

int record_callback(audio_dev_t *audio_dev);

audio_dev_t Audio_Dev = { 0 };

struct device *spi0;
struct device *dma_ch2; /* spi tx */
struct device *dma_ch3; /* spi rx */
static ES8388_Cfg_Type ES8388Cfg = {
    .work_mode = ES8388_CODEC_MDOE,          /*!< ES8388 work mode */
    .role = ES8388_MASTER,                   /*!< ES8388 role */
    .mic_input_mode = ES8388_DIFF_ENDED_MIC, /*!< ES8388 mic input mode */
    .mic_pga = ES8388_MIC_PGA_3DB,           /*!< ES8388 mic PGA */
    .i2s_frame = ES8388_LEFT_JUSTIFY_FRAME,  /*!< ES8388 I2S frame */
    .data_width = ES8388_DATA_LEN_16,        /*!< ES8388 I2S dataWitdh */
};

uint8_t buff[2][2 * 1024] __attribute__((section(".system_ram"), aligned(4)));

int record_callback(audio_dev_t *audio_dev);

void dma_ch2_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    Audio_Dev.audio_callback(&Audio_Dev);
}

uint8_t spi_init(void)
{
    spi_register(SPI0_INDEX, "spi0");

    spi0 = device_find("spi0");

    if (spi0) {
        ((spi_device_t *)spi0)->mode = SPI_SLVAE_MODE;
        ((spi_device_t *)spi0)->direction = SPI_MSB_BYTE0_DIRECTION_FIRST;
        ((spi_device_t *)spi0)->clk_polaraity = SPI_POLARITY_LOW;
        ((spi_device_t *)spi0)->datasize = SPI_DATASIZE_16BIT;
        ((spi_device_t *)spi0)->clk_polaraity = SPI_POLARITY_LOW;
        ((spi_device_t *)spi0)->clk_phase = SPI_PHASE_1EDGE;
        ((spi_device_t *)spi0)->fifo_threshold = 2;

        device_open(spi0, DEVICE_OFLAG_DMA_RX | DEVICE_OFLAG_DMA_TX);
    }

    dma_register(DMA0_CH2_INDEX, "ch2");
    dma_ch2 = device_find("ch2");

    if (dma_ch2) {
        DMA_DEV(dma_ch2)->direction = DMA_MEMORY_TO_PERIPH;
        DMA_DEV(dma_ch2)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch2)->src_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch2)->dst_req = DMA_REQUEST_SPI0_TX;
        DMA_DEV(dma_ch2)->src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch2)->dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch2)->src_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch2)->dst_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch2)->src_width = DMA_TRANSFER_WIDTH_16BIT;
        DMA_DEV(dma_ch2)->dst_width = DMA_TRANSFER_WIDTH_16BIT;
        device_open(dma_ch2, 0);
        device_set_callback(dma_ch2, dma_ch2_irq_callback);
        device_control(dma_ch2, DEVICE_CTRL_SET_INT, NULL);
        device_control(spi0, DEVICE_CTRL_ATTACH_TX_DMA, dma_ch2);
    }

    dma_register(DMA0_CH3_INDEX, "ch3");
    dma_ch3 = device_find("ch3");
    if (dma_ch3) {
        DMA_DEV(dma_ch3)->direction = DMA_PERIPH_TO_MEMORY;
        DMA_DEV(dma_ch3)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch3)->src_req = DMA_REQUEST_SPI0_RX;
        DMA_DEV(dma_ch3)->dst_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch3)->src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch3)->dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch3)->src_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch3)->dst_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch3)->src_width = DMA_TRANSFER_WIDTH_16BIT;
        DMA_DEV(dma_ch3)->dst_width = DMA_TRANSFER_WIDTH_16BIT;
        device_open(dma_ch3, 0);
        device_set_callback(dma_ch3, NULL);
        device_control(dma_ch3, DEVICE_CTRL_SET_INT, NULL);
        device_control(spi0, DEVICE_CTRL_ATTACH_RX_DMA, dma_ch3);
    }

    return SUCCESS;
}

int record_init(audio_dev_t *audio_dev)
{
    GLB_GPIO_Cfg_Type gpio_cfg = {
        .drive = 0,
        .smtCtrl = 1,
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_UP,
        .gpioPin = SPI0_CS,
        .gpioFun = GPIO_FUN_UNUSED,
    };

    /* audio结构体填充*/
    Audio_Dev.audio_callback = record_callback;
    audio_dev->buff[0] = buff[0];
    audio_dev->buff[1] = buff[1];
    audio_dev->buff_size_max = sizeof(buff) / 2;

    memset(audio_dev->buff[0], 0xF0, sizeof(buff) / 2);
    memset(audio_dev->buff[1], 0xF0, sizeof(buff) / 2);

    audio_dev->audio_state = 0;
    audio_dev->buff_using = 0;

    /* 初始化SPI和DMA */
    spi_init();

    // /* 取消DMA方式发送 */
    // device_control(spi0, DEVICE_CTRL_TX_DMA_SUSPEND, NULL);
    // /* 向FIFO里填几个数据，以下两种方式都行，第一种会因为发送超时结束，第二种直接填FIFO。不这样后面就会出错，原因不明 */
    // //spi_transmit(spi0, test_buf, 4, 1);

    // for (uint8_t i = 0; i < 4; i++) {
    //     BL_WR_REG(SPI_BASE, SPI_FIFO_WDATA, 0xFF00);
    // }
    // /* 恢复DMA方式发送 */
    // device_control(spi0, DEVICE_CTRL_TX_DMA_RESUME, NULL);

    /* output MCLK */
    GLB_Set_Chip_Out_0_CLK_Sel(GLB_CHIP_CLK_OUT_I2S_REF_CLK);
    GLB_Set_I2S_CLK(ENABLE, GLB_I2S_OUT_REF_CLK_NONE);

    /* init ES8388 Codec */
    ES8388_Init(&ES8388Cfg);
    ES8388_Set_Voice_Volume(10);

    // /* FS复用成普通GPIO的 输入模式 */
    // gpio_cfg.gpioFun = GPIO_FUN_UNUSED;
    // GLB_GPIO_Init(&gpio_cfg);
    // gpio_set_mode(SPI0_CS, GPIO_INPUT_PP_MODE);

    // // /* 先关SPI*/
    // device_control(spi0, DEVICE_CTRL_SUSPEND, NULL);
    /* 让SPI-DMA数据发送准备就绪 */
    device_write(spi0, 0, audio_dev->buff[1], audio_dev->buff_size_max);

    // /* 等待FS(CS)低电平 */
    // while (gpio_read(SPI0_CS)) {
    // };
    // /* 等待FS(CS)上升沿 */
    // while (!gpio_read(SPI0_CS)) {
    // };

    // /* 重新复用位SPI-CS功能 */
    // gpio_cfg.gpioFun = GPIO_FUN_SPI;
    // GLB_GPIO_Init(&gpio_cfg);

    // /* 确认同步后再启动SPI */
    // MSG(" SPI");
    // device_control(spi0, DEVICE_CTRL_RESUME, NULL);
    // MSG(" OK\r\n");

    return 0;
}

int record_callback(audio_dev_t *audio_dev)
{
    MSG("init %d\r\n", audio_dev->buff_using);

    device_write(spi0, 0, audio_dev->buff[audio_dev->buff_using], audio_dev->buff_size_max);

    audio_dev->buff_using = !audio_dev->buff_using;

    device_read(spi0, 0, audio_dev->buff[audio_dev->buff_using], audio_dev->buff_size_max);

    for (uint8_t i = 0; i < 20; i++)
        MSG("%x ", audio_dev->buff[audio_dev->buff_using][i + 100]);
    MSG("\r\n");

    return 0;
}

int main(void)
{
    bflb_platform_init(0);

    MSG("spi_i2s_record case \r\n");

    record_init(&Audio_Dev);

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
