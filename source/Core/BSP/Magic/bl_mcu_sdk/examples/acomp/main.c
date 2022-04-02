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
#include "hal_pm.h"
#include "hal_acomp.h"
#include "hal_uart.h"

void pm_irq_callback(enum pm_event_type event)
{
    if (event == PM_HBN_ACOMP0_WAKEUP_EVENT) {
        MSG("acomp0 int\r\n");
    } else if (event == PM_HBN_ACOMP1_WAKEUP_EVENT) {
        MSG("acomp1 int\r\n");
    }
}

int main(void)
{
    bflb_platform_init(0);

    pm_hbn_out1_irq_register();
    acomp_device_t acomp_device;

    acomp_device.pos_ch = ACOMP_CHANNEL_ADC_CHANNEL6; //GPIO7
    acomp_device.neg_ch = ACOMP_CHANNEL_0P375VBAT;
    acomp_device.pos_hysteresis_vol = ACOMP_HYSTERESIS_VOLT_50MV;
    acomp_device.neg_hysteresis_vol = ACOMP_HYSTERESIS_VOLT_50MV;
    acomp_init(0, &acomp_device);

    acomp_device.pos_ch = ACOMP_CHANNEL_ADC_CHANNEL2; //GPIO17
    acomp_device.neg_ch = ACOMP_CHANNEL_0P375VBAT;
    acomp_device.pos_hysteresis_vol = ACOMP_HYSTERESIS_VOLT_50MV;
    acomp_device.neg_hysteresis_vol = ACOMP_HYSTERESIS_VOLT_50MV;
    acomp_init(1, &acomp_device);

    acomp_enable(0);
    acomp_interrupt_unmask(0, ACOMP_POSITIVE_IT | ACOMP_NEGATIVE_IT);
    acomp_enable(1);
    acomp_interrupt_unmask(1, ACOMP_POSITIVE_IT | ACOMP_NEGATIVE_IT);
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
