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
#include "hal_flash.h"

int main(void)
{
    uint8_t writeTestData[256];
    uint8_t readTestData[256];
    uint16_t i = 0;

    bflb_platform_init(0);

    for (i = 0; i < 256; i++) {
        writeTestData[i] = i;
    }

    /* erase 0x00010000 16k flash */
    flash_erase(0x00010000, 16 * 1024);

    /* write 0x00010000 flash data */
    flash_write(0x00010000, writeTestData, sizeof(writeTestData));

    memset(readTestData, 0, 256);

    /* read 0x00010000 flash data */
    flash_read(0x00010000, readTestData, sizeof(readTestData));

    for (i = 0; i < 256; i++) {
        if (readTestData[i] != i) {
            BL_CASE_FAIL;
            while (1) {
            }
        }
    }

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
