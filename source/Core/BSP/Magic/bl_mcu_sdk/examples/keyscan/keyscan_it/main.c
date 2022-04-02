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
#include "hal_keyscan.h"

static void keyscan_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    uint32_t key_code = (uint32_t)args;
    MSG("Keycode0 value: %d\r\n", key_code & 0xff);
    MSG("Keycode1 value: %d\r\n", (key_code & 0xff00) >> 8);
    MSG("Keycode2 value: %d\n", (key_code & 0xff0000) >> 16);
    MSG("Keycode3 value: %d\r\n", (key_code & 0xff000000) >> 24);
}

int main(void)
{
    bflb_platform_init(0);
    keyscan_register(KEYSCAN_INDEX, "4x4_keyscan");
    struct device *keyscan = device_find("4x4_keyscan");

    if (keyscan) {
        KEYSCAN_DEV(keyscan)->col_num = COL_NUM_4;
        KEYSCAN_DEV(keyscan)->row_num = ROW_NUM_4;
        device_open(keyscan, 0); //current scan latency is 32K/1/8 = 4Khz
        device_set_callback(keyscan, keyscan_irq_callback);
        device_control(keyscan, DEVICE_CTRL_SET_INT, NULL);
        device_control(keyscan, DEVICE_CTRL_RESUME, NULL);
        MSG("keyscan found\n");
    }

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
