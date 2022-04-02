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
#include "hal_adc.h"
#include "hal_dma.h"

adc_channel_t posChList[] = { ADC_CHANNEL4 };
adc_channel_t negChList[] = { ADC_CHANNEL_GND };

adc_channel_val_t result_val[16];

struct device *adc_test;

uint32_t adc_buffer[16];

adc_data_parse_t data_parse;

void dma_ch0_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    device_control(adc_test, DEVICE_CTRL_ADC_DATA_PARSE, (void*)&data_parse);
    for (uint8_t i = 0; i < 16; i++) {
        MSG("PosId = %d NegId = %d V= %d mV \r\n", result_val[i].posChan, result_val[i].negChan, (uint32_t)(result_val[i].volt * 1000));
    }
}

int main(void)
{
    bflb_platform_init(0);

    adc_channel_cfg_t adc_channel_cfg;

    adc_channel_cfg.pos_channel = posChList;
    adc_channel_cfg.neg_channel = negChList;
    adc_channel_cfg.num = 1;

    adc_register(ADC0_INDEX, "adc");

    adc_test = device_find("adc");

    if (adc_test) {
        ADC_DEV(adc_test)->continuous_conv_mode = ENABLE;
        ADC_DEV(adc_test)->fifo_threshold = ADC_FIFO_THRESHOLD_16BYTE;
        device_open(adc_test, DEVICE_OFLAG_DMA_RX);
        if (device_control(adc_test, DEVICE_CTRL_ADC_CHANNEL_CONFIG, &adc_channel_cfg) == ERROR) {
            MSG("ADC channel config error , Please check the channel corresponding to IO is initial success by board system or Channel is invaild \r\n");
            BL_CASE_FAIL;
            while (1)
                ;
        }
        MSG("adc device find success\r\n");
    }
    dma_register(DMA0_CH0_INDEX, "dma_ch0");
    struct device *dma_ch0 = device_find("dma_ch0");

    if (dma_ch0) {
        DMA_DEV(dma_ch0)->direction = DMA_PERIPH_TO_MEMORY;
        DMA_DEV(dma_ch0)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch0)->src_req = DMA_REQUEST_ADC0;
        DMA_DEV(dma_ch0)->dst_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch0)->src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch0)->dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch0)->src_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch0)->dst_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch0)->src_width = DMA_TRANSFER_WIDTH_32BIT;
        DMA_DEV(dma_ch0)->dst_width = DMA_TRANSFER_WIDTH_32BIT;
        device_open(dma_ch0, 0);
        device_set_callback(dma_ch0, dma_ch0_irq_callback);
        device_control(dma_ch0, DEVICE_CTRL_SET_INT, NULL);
    }

    /* connect dac device and dma device */
    device_control(adc_test, DEVICE_CTRL_ATTACH_RX_DMA, dma_ch0);

    data_parse.input = adc_buffer;
    data_parse.output = result_val;
    data_parse.num = 16;

    adc_channel_start(adc_test);
    while (1) {
        if (!dma_channel_check_busy(dma_ch0)) {
            device_read(adc_test, 0, adc_buffer, sizeof(adc_buffer) / sizeof(uint8_t));/* size need convert to uint8_t*/
        }

        bflb_platform_delay_ms(500);
    }
}