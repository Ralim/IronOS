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

struct device *timer0;

volatile uint32_t cnt = 0;

void timer0_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    if (state == TIMER_EVENT_COMP0) {
        MSG("timer event comp0! \r\n");
    } else if (state == TIMER_EVENT_COMP1) {
        MSG("timer event comp1! \r\n");
    } else if (state == TIMER_EVENT_COMP2) {
        BL_CASE_SUCCESS;
        timer_timeout_cfg_t cfg = { 2, 12000000 }; /*modify compare id 2 timeout 12s*/
        device_write(dev, 0, &cfg, sizeof(timer_timeout_cfg_t));
        MSG("timer event comp2! \r\n");
    }
}

int main(void)
{
    bflb_platform_init(0);

    timer_register(TIMER0_INDEX, "timer0");

    timer0 = device_find("timer0");

    if (timer0) {
        device_open(timer0, DEVICE_OFLAG_INT_TX); /* 1s,2s,3s timing*/
        device_set_callback(timer0, timer0_irq_callback);
        device_control(timer0, DEVICE_CTRL_SET_INT, (void *)(TIMER_COMP0_IT | TIMER_COMP1_IT | TIMER_COMP2_IT));
    } else {
        MSG("timer device open failed! \n");
    }

    while (1) {
    }
}
