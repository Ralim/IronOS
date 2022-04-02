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

static void gpio_int_callback(uint32_t pin)
{
    MSG("gpio :%d rising trigger !\r\n", pin);
}

int main(void)
{
    bflb_platform_init(0);

    gpio_set_mode(GPIO_PIN_18, GPIO_SYNC_FALLING_TRIGER_INT_MODE);
    gpio_attach_irq(GPIO_PIN_18, gpio_int_callback);
    gpio_irq_enable(GPIO_PIN_18, ENABLE);
    gpio_set_mode(GPIO_PIN_19, GPIO_SYNC_FALLING_TRIGER_INT_MODE);
    gpio_attach_irq(GPIO_PIN_19, gpio_int_callback);
    gpio_irq_enable(GPIO_PIN_19, ENABLE);
    gpio_set_mode(GPIO_PIN_20, GPIO_SYNC_RISING_TRIGER_INT_MODE);
    gpio_attach_irq(GPIO_PIN_20, gpio_int_callback);
    gpio_irq_enable(GPIO_PIN_20, ENABLE);
    gpio_set_mode(GPIO_PIN_21, GPIO_SYNC_RISING_TRIGER_INT_MODE);
    gpio_attach_irq(GPIO_PIN_21, gpio_int_callback);
    gpio_irq_enable(GPIO_PIN_21, ENABLE);

    MSG("gpio int test !\r\n");

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
