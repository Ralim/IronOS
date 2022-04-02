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
#include "hal_uart.h"
#include "hal_dma.h"

static uint8_t src_buffer[4100] __attribute__((section(".system_ram")));
static uint8_t dma_tc_flag0 = 0;

void dma_ch2_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    if (!state) {
        dma_tc_flag0 = 1;
        MSG("dma transfer success\r\n");
    }
}

void sram_init()
{
    memset(src_buffer, 'a', 4100);
    src_buffer[3999] = 'B';
    src_buffer[4095] = 'A';
    src_buffer[4096] = 'B';
    src_buffer[4097] = 'C';
    src_buffer[4098] = 'D';
    src_buffer[4099] = 'E';
}
int main(void)
{
    bflb_platform_init(0);
    sram_init();

    uart_register(UART1_INDEX, "uart1");
    struct device *uart = device_find("uart1");

    if (uart) {
        device_open(uart, DEVICE_OFLAG_DMA_TX);
    }

    dma_register(DMA0_CH2_INDEX, "ch2");
    struct device *dma_ch2 = device_find("ch2");

    if (dma_ch2) {
        DMA_DEV(dma_ch2)->direction = DMA_MEMORY_TO_PERIPH;
        DMA_DEV(dma_ch2)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch2)->src_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch2)->dst_req = DMA_REQUEST_UART1_TX;
        DMA_DEV(dma_ch2)->src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch2)->dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch2)->src_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch2)->dst_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch2)->src_width = DMA_TRANSFER_WIDTH_8BIT;
        DMA_DEV(dma_ch2)->dst_width = DMA_TRANSFER_WIDTH_8BIT;
        device_open(dma_ch2, 0);
        device_set_callback(dma_ch2, dma_ch2_irq_callback);
        device_control(dma_ch2, DEVICE_CTRL_SET_INT, NULL);
    }

    device_control(uart, DEVICE_CTRL_ATTACH_TX_DMA, dma_ch2);

    while (1) {
        device_write(uart, 0, src_buffer, 4100);

        while (dma_tc_flag0 == 0) {
            bflb_platform_delay_ms(1);
        }

        bflb_platform_delay_ms(1000);
    }
}
