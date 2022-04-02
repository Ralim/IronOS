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
#include "hal_dac.h"
#include "hal_dma.h"
#include "zfb_16.h"

int main(void)
{
    bflb_platform_init(0);

    /* register & open dac device */
    dac_register(DAC0_INDEX, "dac");
    struct device *dac = device_find("dac");

    if (dac) {
        ((dac_device_t *)dac)->sample_freq = DAC_SAMPLE_FREQ_16KHZ;
        device_open(dac, DEVICE_OFLAG_DMA_TX);
        MSG("device open success\r\n");
    }

    /* register & open dma device */
    if (dma_allocate_register("dac_dma") == SUCCESS) {
        MSG("dma allocate success\r\n");
    }

    struct device *dac_dma = device_find("dac_dma");

    if (dac_dma) {
        DMA_DEV(dac_dma)->direction = DMA_MEMORY_TO_PERIPH;
        DMA_DEV(dac_dma)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dac_dma)->src_req = DMA_REQUEST_NONE;
        DMA_DEV(dac_dma)->dst_req = DMA_REQUEST_DAC0;
        DMA_DEV(dac_dma)->src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dac_dma)->dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dac_dma)->src_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dac_dma)->dst_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dac_dma)->src_width = DMA_TRANSFER_WIDTH_16BIT;
        DMA_DEV(dac_dma)->dst_width = DMA_TRANSFER_WIDTH_16BIT;
        device_open(dac_dma, 0);
    }

    /* connect dac device and dma device */
    device_control(dac, DEVICE_CTRL_ATTACH_TX_DMA, dac_dma);

    device_write(dac, DAC_CHANNEL_0, zfb_source, sizeof(zfb_source));

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
