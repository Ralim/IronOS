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
#include "hal_qdec.h"
#include "hal_gpio.h"

static volatile uint32_t flag = 0;

static void qdec_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    if (state == QDEC_REPORT_EVENT) {
        uint16_t num = 0;

        flag++;
        num = device_control(dev, DEVICE_CTRL_GET_SAMPLE_VAL, 0);
        if (num & (1 << 15)) {
            MSG("QDEC1 :%d,d=%d,v=-%5d\r\n", flag, device_control(dev, DEVICE_CTRL_GET_SAMPLE_DIR, 0), 32768 - (num & 0x7FFF));
        } else {
            MSG("QDEC1 :%d,d=%d,v= %5d\r\n", flag, device_control(dev, DEVICE_CTRL_GET_SAMPLE_DIR, 0), num);
        }
    }
}

int main(void)
{
    bflb_platform_init(0);

    qdec_register(QDEC1_INDEX, "qdec_it");

    struct device *qdec_it = device_find("qdec_it");

    if (qdec_it) {
        device_open(qdec_it, DEVICE_OFLAG_INT_RX);
        device_set_callback(qdec_it, qdec_irq_callback);
    } else {
        MSG("device find fail \r\n");
    }

    device_control(qdec_it, DEVICE_CTRL_SET_INT, (void *)QDEC_REPORT_EVENT);
    device_control(qdec_it, DEVICE_CTRL_RESUME, 0);

    while (flag < 100) {
        bflb_platform_delay_ms(100);
    }
    device_control(qdec_it, DEVICE_CTRL_CLR_INT, 0);
    device_control(qdec_it, DEVICE_CTRL_SUSPEND, 0);

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
