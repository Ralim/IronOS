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
#include "GSL61xx.h"

uint8_t image[120 * 104];

void main(void)
{
    uint32_t id = 0;
    uint8_t i = 0;

    bflb_platform_init(0);

    if (SUCCESS == GSL61xx_init()) {
        //GSL_FP_ReadChipID(&id);

        //MSG("SPI communicate test success fingerprint id = %d\r\n",id);
    } else {
        //MSG("SPI communicate test failed\r\n");
    }

    struct device *uart = device_find("debug_log");

    CaptureGSL61xx(image);

    for (i = 0; i < 120; i++) {
        device_write(uart, 0, (uint8_t *)(&image[i * 104]), 104);
    }

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}