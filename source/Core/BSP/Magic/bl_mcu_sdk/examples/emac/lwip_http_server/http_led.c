/**
 * @file http_led.c
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

#include "lwip/api.h"
#include "lwip/arch.h"
#include "lwip/opt.h"

#include "http_server.h"
#include "http_led.h"
#include "hal_gpio.h"

#define LED_OFF  0
#define LED_ON   1
#define LED_GPIO GPIO_PIN_6

void bsp_led_config(void)
{
    gpio_write(LED_GPIO, LED_OFF);
    gpio_set_mode(LED_GPIO, GPIO_OUTPUT_PP_MODE);
}

void led_on(void)
{
    gpio_write(LED_GPIO, LED_ON);
}

void led_off(void)
{
    gpio_write(LED_GPIO, LED_OFF);
}

void led_task(void *pvParameters)
{
    const TickType_t x_delay = pdMS_TO_TICKS(50);

    bsp_led_config();

    s_x_btn_semaphore = xSemaphoreCreateBinary();

    while (1) {
        if (xSemaphoreTake(s_x_btn_semaphore, x_delay) == pdTRUE) {
            if (button_clicked_flag) {
                led_on();
            } else {
                led_off();
            }
        }
    }
}
