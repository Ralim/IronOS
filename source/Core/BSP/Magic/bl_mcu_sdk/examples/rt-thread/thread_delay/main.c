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
#include <rtthread.h>
#include "hal_gpio.h"

int main(void)
{
    gpio_set_mode(GPIO_PIN_22, GPIO_OUTPUT_PP_MODE);

    while (1) {
        gpio_write(GPIO_PIN_22, 0);
        rt_thread_mdelay(1000);
        gpio_write(GPIO_PIN_22, 1);
        rt_thread_mdelay(1000);
    }
}
