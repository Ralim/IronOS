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
#include "hal_gpio.h"

uint32_t count = 0;

void gpio_blink_init(void)
{
    gpio_set_mode(GPIO_PIN_22, GPIO_OUTPUT_PP_MODE);
    gpio_set_mode(GPIO_PIN_29, GPIO_OUTPUT_PP_MODE);
    gpio_set_mode(GPIO_PIN_30, GPIO_OUTPUT_PP_MODE);
    gpio_set_mode(GPIO_PIN_31, GPIO_OUTPUT_PP_MODE);

    gpio_write(GPIO_PIN_22, 0);
    gpio_write(GPIO_PIN_29, 0);
    gpio_write(GPIO_PIN_30, 0);
    gpio_write(GPIO_PIN_31, 0);
}

void led_blink_all(void)
{
    gpio_write(GPIO_PIN_22, 0);
    gpio_write(GPIO_PIN_29, 0);
    gpio_write(GPIO_PIN_30, 0);
    gpio_write(GPIO_PIN_31, 0);

    bflb_platform_delay_ms(100);

    gpio_write(GPIO_PIN_22, 1);
    gpio_write(GPIO_PIN_29, 1);
    gpio_write(GPIO_PIN_30, 1);
    gpio_write(GPIO_PIN_31, 1);

    bflb_platform_delay_ms(100);
}

void led_running(void)
{
    gpio_write(GPIO_PIN_22, 1);
    bflb_platform_delay_ms(200);
    gpio_write(GPIO_PIN_22, 0);

    gpio_write(GPIO_PIN_29, 1);
    bflb_platform_delay_ms(200);
    gpio_write(GPIO_PIN_29, 0);

    gpio_write(GPIO_PIN_30, 1);
    bflb_platform_delay_ms(200);
    gpio_write(GPIO_PIN_30, 0);

    gpio_write(GPIO_PIN_31, 1);
    bflb_platform_delay_ms(200);
    gpio_write(GPIO_PIN_31, 0);

    count++;
}

int main(void)
{
    bflb_platform_init(0);

    gpio_blink_init();

    MSG("gpio test !\r\n");

    while (1) {
        led_running();
    }
}
