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
#include "hal_gpio.h"

adc_channel_t posChList[] = { ADC_CHANNEL_VABT_HALF }; // ADC Positive channel list setting
adc_channel_t negChList[] = { ADC_CHANNEL_GND };       // ADC Negative channel list setting

struct device *vbat_test;

adc_channel_val_t result;

int main(void)
{
    bflb_platform_init(0);

    adc_channel_cfg_t adc_channel_cfg;

    adc_channel_cfg.pos_channel = posChList;
    adc_channel_cfg.neg_channel = negChList;
    adc_channel_cfg.num = 1;

    adc_register(ADC0_INDEX, "adc");

    vbat_test = device_find("adc");

    if (vbat_test) {
        MSG("adc vbat test device find success\r\n");
        ADC_DEV(vbat_test)->continuous_conv_mode = DISABLE;
        device_open(vbat_test, DEVICE_OFLAG_STREAM_RX);
        device_control(vbat_test, DEVICE_CTRL_ADC_VBAT_ON, NULL);
        device_control(vbat_test, DEVICE_CTRL_ADC_CHANNEL_CONFIG, &adc_channel_cfg);
    }

    while (1) {
        bflb_platform_delay_ms(1000);
        MSG("adc vbat test case\r\n");

        for (uint8_t i = 0; i < 10; i++) {
            adc_channel_start(vbat_test);
            device_read(vbat_test, 0, &result, 1); /*max size is 32*/
            MSG("vBat = %d mV\n", (uint32_t)(result.volt * 2000));

            bflb_platform_delay_ms(500);
        }
    }
}