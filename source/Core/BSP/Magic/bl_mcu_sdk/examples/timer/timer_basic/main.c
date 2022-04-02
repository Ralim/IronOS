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
#include "hal_timer.h"

int main(void)
{
    bflb_platform_init(0);

    timer_register(TIMER1_INDEX, "timer1");

    struct device *timer1 = device_find("timer1");

    if (timer1) {
        device_open(timer1, DEVICE_OFLAG_STREAM_TX); // 1s timing
    } else {
        MSG("timer device open failed! \n");
    }
    while (1) {
        if (device_control(timer1, DEVICE_CTRL_GET_INT, (void *)TIMER_COMP0_IT)) {
            BL_CASE_SUCCESS;
        }
        bflb_platform_delay_ms(100);
    }
}
