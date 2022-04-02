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
#include "bsp_sf_psram.h"

uint8_t psram_id[8] = { 0 };

void test32(void)
{
    uint32_t i, val;

    for (i = 0; i < 256; i += 4) {
        *((volatile uint32_t *)(BSP_PSRAM_BASE + i)) = i / 4;
    }

    for (i = 0; i < 256; i += 4) {
        val = *((volatile uint32_t *)(BSP_PSRAM_BASE + i));
        MSG("addr = 0x%08X, val = 0x%08X\r\n", (BSP_PSRAM_BASE + i), *((volatile uint32_t *)(BSP_PSRAM_BASE + i)));

        if (i / 4 != val) {
            BL_CASE_FAIL;
            while (1) {
            }
        }
    }
}

void test8(void)
{
    uint32_t i;
    uint8_t val;

    for (i = 0; i < 256; i++) {
        *((volatile uint8_t *)(BSP_PSRAM_BASE + i)) = i;
    }

    for (i = 0; i < 256; i++) {
        val = *((volatile uint8_t *)(BSP_PSRAM_BASE + i));
        MSG("addr = 0x%08X, val = 0x%08X\r\n", (BSP_PSRAM_BASE + i), *((volatile uint8_t *)(BSP_PSRAM_BASE + i)));

        if ((uint8_t)i != val) {
            BL_CASE_FAIL;
            while (1) {
            }
        }
    }
}

int main(void)
{
    bflb_platform_init(0);

    MSG("PSRAM write read case \n");

    bsp_sf_psram_init(1);
    bsp_sf_psram_read_id(psram_id);
    MSG("BSP SF PSRAM ID: %02X %02X %02X %02X %02X %02X %02X %02X. \n",
        psram_id[0], psram_id[1], psram_id[2], psram_id[3], psram_id[4], psram_id[5], psram_id[6], psram_id[7]);

    test32();
    test8();

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
