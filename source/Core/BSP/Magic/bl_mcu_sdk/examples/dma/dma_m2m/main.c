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
#include "hal_dma.h"

uint8_t dma_src_buffer[8000] __attribute__((section(".system_ram")));
uint8_t dma_dst_buffer[8000] __attribute__((section(".system_ram")));

void dma_transfer_done(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    uint32_t index = 0;

    if (!state) {
        for (index = 0; index < 8000; index++) {
            if (dma_dst_buffer[index] != 0xff) {
                BL_CASE_FAIL;
                return;
            }
        }

        BL_CASE_SUCCESS;
    }
}

int main(void)
{
    bflb_platform_init(0);

    dma_register(DMA0_CH0_INDEX, "dma_ch0");

    struct device *dma_ch0 = device_find("dma_ch0");

    if (dma_ch0) {
        DMA_DEV(dma_ch0)->direction = DMA_MEMORY_TO_MEMORY;
        DMA_DEV(dma_ch0)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch0)->src_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch0)->dst_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch0)->src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch0)->dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch0)->src_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch0)->dst_burst_size = DMA_BURST_4BYTE;
        DMA_DEV(dma_ch0)->src_width = DMA_TRANSFER_WIDTH_32BIT;
        DMA_DEV(dma_ch0)->dst_width = DMA_TRANSFER_WIDTH_32BIT;
        device_open(dma_ch0, 0);
        device_set_callback(dma_ch0, dma_transfer_done);
        device_control(dma_ch0, DEVICE_CTRL_SET_INT, NULL);
    }

    /* prepare test memory src as 0xff but dest as 0x0 */

    memset(dma_src_buffer, 0xff, 8000);
    memset(dma_dst_buffer, 0, 8000);

    dma_reload(dma_ch0, (uint32_t)dma_src_buffer, (uint32_t)dma_dst_buffer, 8000);

    dma_channel_start(dma_ch0);

    while (1) {
        bflb_platform_delay_ms(100);
    }
}
