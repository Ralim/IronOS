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
#include "hal_spi.h"
#include "hal_i2c.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "bl702_glb.h"
#include "bl702_spi.h"
#include "fhm_onechannel_16k_20.h"

/***************************************************************
					MARCO Definition
****************************************************************/
#define AUTIO_ES8388      0
#define AUTIO_WM8978      1
#define AUTIO_MODULE_TYPE AUTIO_ES8388

#if (AUTIO_MODULE_TYPE == AUTIO_ES8388)
#include "bsp_es8388.h"
#elif (AUTIO_MODULE_TYPE == AUTIO_WM8978)
#include "bsp_wm8978.h"
#else
#endif

/***************************************************************
					Struct Declaration
****************************************************************/
typedef struct audio_dev {
    uint8_t *buff1;
    uint8_t *buff2;
    uint32_t buff1_data_size;
    uint32_t buff2_data_size;
    uint32_t buff_size_max;
    uint8_t buff_using;

    uint8_t audio_state;
    uint8_t audio_type;

    int (*audio_init)(struct audio_dev *audio_dev);
    int (*audio_control)(struct audio_dev *audio_dev, int cmd, void *args);

    int (*audio_callback)(struct audio_dev *audio_dev);
    struct device *device;
} audio_dev_t;

#if (AUTIO_MODULE_TYPE == AUTIO_ES8388)
static ES8388_Cfg_Type ES8388Cfg = {
    .work_mode = ES8388_CODEC_MDOE,          /*!< ES8388 work mode */
    .role = ES8388_MASTER,                   /*!< ES8388 role */
    .mic_input_mode = ES8388_DIFF_ENDED_MIC, /*!< ES8388 mic input mode */
    .mic_pga = ES8388_MIC_PGA_9DB,           /*!< ES8388 mic PGA */
    .i2s_frame = ES8388_LEFT_JUSTIFY_FRAME,  //ES8388_LEFT_JUSTIFY_FRAME,  /*!< ES8388 I2S frame */
    .data_width = ES8388_DATA_LEN_16,        //ES8388_DATA_LEN_16,        /*!< ES8388 I2S dataWitdh */
};
#endif
/***************************************************************
					Variable Definition
****************************************************************/
audio_dev_t audio_dev0 = { 0 };
struct device *spi0;
struct device *dma_ch2;

/***************************************************************
					Function Definition
****************************************************************/
uint8_t spi_init(void)
{
    spi_register(SPI0_INDEX, "spi0");
    dma_register(DMA0_CH3_INDEX, "ch2");
    spi0 = device_find("spi0");

    if (spi0) {
        ((spi_device_t *)spi0)->mode = SPI_SLVAE_MODE;
        ((spi_device_t *)spi0)->direction = SPI_MSB_BYTE3_DIRECTION_FIRST;
        ((spi_device_t *)spi0)->clk_polaraity = SPI_POLARITY_HIGH;
        ((spi_device_t *)spi0)->datasize = SPI_DATASIZE_16BIT;
        ((spi_device_t *)spi0)->clk_phase = SPI_PHASE_1EDGE;
        ((spi_device_t *)spi0)->fifo_threshold = 3;

        //dma send spi
        device_open(spi0, DEVICE_OFLAG_DMA_TX);
    }

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
        device_set_callback(dma_ch2, NULL);
        device_control(dma_ch2, DEVICE_CTRL_SET_INT, NULL);
        device_control(spi0, DEVICE_CTRL_ATTACH_TX_DMA, dma_ch2);
    }

    return SUCCESS;
}

ATTR_TCM_SECTION int main(void)
{
    uint8_t flag = 0;
    GLB_GPIO_Cfg_Type gpio_cfg;

    audio_dev0.buff1 = (void *)fhm_onechannel_16k_20;
    audio_dev0.buff_size_max = sizeof(fhm_onechannel_16k_20);
    audio_dev0.buff1_data_size = sizeof(fhm_onechannel_16k_20);
    audio_dev0.audio_state = 1;

    bflb_platform_init(0);

    MSG("spi i2s mono play start...\r\n");
    flag = spi_init();
    if (flag) {
        MSG("[E]spi init fail!\r\n");
    } else {
    }

    /* Disable spi dev */
    device_control(spi0, DEVICE_CTRL_SUSPEND, NULL);

    /* Spi CS pin config input mode*/
    gpio_cfg.gpioMode = GPIO_MODE_AF;
    gpio_cfg.pullType = GPIO_PULL_UP;
    gpio_cfg.gpioPin = GPIO_PIN_10;
    gpio_cfg.gpioFun = GPIO_FUN_UNUSED;
    GLB_GPIO_Init(&gpio_cfg);
    gpio_set_mode(GPIO_PIN_10, GPIO_INPUT_PP_MODE);

    /*GPIO6 output clk for I2S MCLK*/
    GLB_Set_Chip_Out_0_CLK_Sel(GLB_CHIP_CLK_OUT_I2S_REF_CLK);
    GLB_Set_I2S_CLK(ENABLE, GLB_I2S_OUT_REF_CLK_NONE);

#if (AUTIO_MODULE_TYPE == AUTIO_WM8978)

    /*init wm8978*/
    if (0 != WM8978_Init()) {
        MSG("[E]WM8978 Init fail\r\n");
    }

    if (false == wm8978_master_cfg()) {
        MSG("[E]wm8978 master cfg fail! \r\n");
    }

    WM8978_PlayMode();
#elif (AUTIO_MODULE_TYPE == AUTIO_ES8388)
    /* init ES8388 Codec */
    ES8388_Init(&ES8388Cfg);
    //ES8388_Reg_Dump();
    flag = ES8388_Set_Voice_Volume(30);
    if (flag) {
        MSG("[E]ES8388 set volume fail!\r\n");
    } else {
    }
    MSG("ES8388 config ok!\r\n");
#else
    MSG("[E]Please Check AUTIO_MODULE_TYPE!\r\n");
#endif

    /* Config DMA Tx Buff */
    device_write(spi0, 0, audio_dev0.buff1, audio_dev0.buff1_data_size);

    gpio_cfg.gpioMode = GPIO_MODE_AF;
    gpio_cfg.pullType = GPIO_PULL_UP;
    gpio_cfg.gpioPin = GPIO_PIN_10;
    gpio_cfg.gpioFun = GPIO_FUN_SPI;

    /* Wait SPI CS Rise edge */
    while (gpio_read(GPIO_PIN_10))
        ;

    while (!gpio_read(GPIO_PIN_10))
        ;

    /* Config GPIO10 as SPI CS*/
    GLB_GPIO_Init(&gpio_cfg);

    /* Enable spi dev */
    device_control(spi0, DEVICE_CTRL_RESUME, NULL);

    while (1) {
        bflb_platform_delay_ms(10);
    }
}