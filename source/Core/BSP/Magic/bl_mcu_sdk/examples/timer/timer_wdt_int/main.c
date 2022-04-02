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
#include "hal_wdt.h"

struct device *wdt_int;

static uint8_t watchdog_flag = 0;

void wdt_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    if (state == WDT_EVENT) {
        watchdog_flag = 1;
        device_control(wdt_int, DEVICE_CTRL_CLR_INT, 0);
    }
}

int main(void)
{
    bflb_platform_init(0);

    uint32_t wdt_cnt = 0;

    wdt_register(WDT_INDEX, "wdt_int");
    wdt_int = device_find("wdt_int");

    if (wdt_int) {
        device_set_callback(wdt_int, wdt_irq_callback);
        device_control(wdt_int, DEVICE_CTRL_SET_INT, NULL);
        device_open(wdt_int, DEVICE_OFLAG_INT_TX); // 6000 counter
        // device_write(wdt_int, 0, (void *)6000, 0);
    } else {
        MSG("wdt device open failed! \r\n");
    }

    while (1) {
        wdt_cnt = device_control(wdt_int, DEVICE_CTRL_GET_WDT_COUNTER, 0);
        MSG("Watchdog counter value=%d \r\n", wdt_cnt);
        if (wdt_cnt > 1000) {
            MSG("Watchdog reset counter \r\n");
            device_control(wdt_int, DEVICE_CTRL_RST_WDT_COUNTER, 0);
            break;
        }
    }

    MSG("Waiting for watchdog interrupt or reset \r\n");

    while (watchdog_flag == 0) {
        MSG("Watchdog counter value=%d \r\n", device_control(wdt_int, DEVICE_CTRL_GET_WDT_COUNTER, 0));
    }
    MSG("Watchdog interrupt arrived \r\n");

    device_close(wdt_int);

    BL_CASE_SUCCESS;
    while (1) {
    }
}
