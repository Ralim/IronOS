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

void dht11_io_in(void)
{
    gpio_set_mode(GPIO_PIN_12, GPIO_INPUT_PP_MODE);
}

void dht11_io_out(void)
{
    gpio_set_mode(GPIO_PIN_12, GPIO_OUTPUT_PP_MODE);
}

void dht11_io_set(void)
{
    gpio_write(GPIO_PIN_12, 1);
}

void dht11_io_rst(void)
{
    gpio_write(GPIO_PIN_12, 0);
}

uint8_t dht11_data_in(void)
{
    uint8_t tmp = 0;
    tmp = gpio_read(GPIO_PIN_12);
    // MSG("ping 12:%d \n", tmp);
    // return gpio_read(GPIO_PIN_12);
    return tmp;
}

void dht11_rst(void)
{
    dht11_io_out();
    dht11_io_rst();
    bflb_platform_delay_ms(20);
    dht11_io_set();
    bflb_platform_delay_us(30);
}

uint8_t dht11_check(void)
{
    uint8_t retry = 0;
    dht11_io_in();

    while (!dht11_data_in() && retry < 100) {
        retry++;
        bflb_platform_delay_us(1);
    };
    if (retry >= 100) {
        return 1;
    }

    retry = 0;
    while (dht11_data_in() && retry < 100) {
        retry++;
        bflb_platform_delay_us(1);
    };
    if (retry >= 100) {
        return 1;
    } else {
        return 0;
    }

    while (!dht11_data_in() && retry < 100) {
        retry++;
        bflb_platform_delay_us(1);
    }

    if (retry >= 100) {
        return 1;
    } else {
        return 0;
    }
    return 0;
}

uint8_t dht11_read_bit(void)
{
    uint8_t retry = 0;

    while (dht11_data_in() && retry < 100) {
        retry++;
        bflb_platform_delay_us(1);
    }

    retry = 0;

    while (!dht11_data_in() && retry < 100) {
        retry++;
        bflb_platform_delay_us(1);
    }

    bflb_platform_delay_us(40);

    if (dht11_data_in()) {
        return 1;
    } else {
        return 0;
    }
    return 0;
}

uint8_t dht11_read_byte(void)
{
    uint8_t i, data;
    data = 0;

    for (i = 0; i < 8; i++) {
        data <<= 1;
        data |= dht11_read_bit();
    }

    return data;
}

uint8_t dht11_read_data(uint8_t *temp, uint8_t *humi)
{
    uint8_t buf[5];
    uint8_t i;

    dht11_rst();

    if (dht11_check() == 0) {
        for (i = 0; i < 5; i++) {
            buf[i] = dht11_read_byte();
        }

        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4]) {
            *humi = buf[0];
            *temp = buf[2];
        } else {
            return 1;
        }
    } else {
        return 0;
    }
    return 0;
}

uint8_t dht11_init(void)
{
    dht11_rst();
    return dht11_check();
}

int main(void)
{
    uint8_t temperature = 0;
    uint8_t humidity = 0;

    bflb_platform_init(0);

    bflb_platform_delay_ms(1000);

    if (!dht11_init()) {
        MSG("dht11 init error!\n");
    }

    MSG("dht11 demo !\r\n");

    while (1) {
        dht11_read_data(&temperature, &humidity);
        MSG("temperature:%2d  humidity:%2d \n", temperature, humidity);

        bflb_platform_delay_ms(1500);
    }
}
