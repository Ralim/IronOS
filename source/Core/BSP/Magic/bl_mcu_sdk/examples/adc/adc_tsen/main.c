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

adc_channel_t posChList[] = { ADC_CHANNEL_TSEN_P };
adc_channel_t negChList[] = { ADC_CHANNEL_GND };

uint16_t tsen_offset = 0;

struct device *adc_tsen;

int main(void)
{
    float sum_val = 0.0;
    bflb_platform_init(0);

    adc_channel_cfg_t adc_channel_cfg;

    adc_channel_cfg.pos_channel = posChList;
    adc_channel_cfg.neg_channel = negChList;
    adc_channel_cfg.num = 1;

    MSG("adc tsen test case \r\n");
    adc_register(ADC0_INDEX, "adc_tsen");

    adc_tsen = device_find("adc_tsen");

    if (adc_tsen) {
        ADC_DEV(adc_tsen)->continuous_conv_mode = DISABLE;
        device_open(adc_tsen, DEVICE_OFLAG_STREAM_RX);
        device_control(adc_tsen, DEVICE_CTRL_ADC_CHANNEL_CONFIG, &adc_channel_cfg);
        device_control(adc_tsen, DEVICE_CTRL_ADC_TSEN_ON, NULL);
    } else {
        MSG("adc device find fail \n");
    }

    if (adc_trim_tsen(&tsen_offset) == ERROR) {
        MSG("read efuse data fail \n");
        return ERROR;
    } else {
        MSG("offset = %d \n", tsen_offset);
    }

    while (1) {
        for (int i = 0; i < 50; i++) {
            sum_val += adc_get_tsen(tsen_offset);
            bflb_platform_delay_ms(10);
        }

        MSG("Tsen temp:%0.2f \n", (sum_val / 50.0));
        sum_val = 0;
    }
}
