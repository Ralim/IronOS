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
#include "hal_spi.h"
#include "hal_sec_ecdsa.h"
#include "bsp_il9341.h"
#include "touch.h"

#define TRNG_TEST_NUM  4
#define TRNG_DATA_SIZE 32

uint8_t trng_buf[256] = { 0 };
uint8_t display_buf[128] = { 0 };

static void trng_dump_data(uint8_t data[TRNG_DATA_SIZE])
{
    int i;
    MSG("TRNG data \r\n");
    for (i = 0; i < TRNG_DATA_SIZE; i++) {
        MSG("%02d ", data[i]);
    }
    MSG("\r\n");
}

int main(void)
{
    int16_t read_x, read_y;
    uint32_t num_sum[10] = { 0 };
    uint8_t tmp_digit = 0;
    uint8_t trng_data[TRNG_TEST_NUM][TRNG_DATA_SIZE];
    bflb_platform_init(0);
    LCD_Init();
    LCD_Clear(0x0000);

    touch_init();
    // LCD_DrawCircle(100, 100, 20, 0x00ff);
    // LCD_DrawFont(10, 10, 0xFFFF, 0x0000, (uint8_t *)"123ABC", 6);

    MSG("trng case! \r\n");

    if (sec_eng_trng_enable()) {
        MSG("sec eng trng enable fail! \r\n");
    }

    // for (int i = 0; i < TRNG_TEST_NUM; i++) {
    //     MSG("Reading data from TRNG module\r\n");
    //     sec_eng_trng_read(trng_data[i]);
    //     trng_dump_data(trng_data[i]);
    // }

    // sec_eng_trng_disable();

    MSG("trng case success! \r\n");
    uint8_t read_flag = 0;
    while (1) {
        device_control(touch_spi, DEVICE_CTRL_SPI_CONFIG_CLOCK, (void *)3600000);
        while (touch_read(&read_x, &read_y)) {
        }
        device_control(touch_spi, DEVICE_CTRL_SPI_CONFIG_CLOCK, (void *)36000000);

        for (int i = 0; i < TRNG_TEST_NUM; i++) {
            // MSG("Reading data from TRNG module\r\n");
            sec_eng_trng_read(trng_data[i]);
            // trng_dump_data(trng_data[i]);
        }

        display_buf[0] = 0;
        // for (int i = 0; i < 8; i++) {
        tmp_digit = trng_data[0][0] % 10;
        sprintf(trng_buf, "%d  ", tmp_digit);
        strcat(display_buf, trng_buf);
        num_sum[tmp_digit]++;
        // }
        // LCD_DrawFont(100, 100, 0xFFFF, 0x0000, display_buf, 0x1);
        // LCD_DrawFont_64x32(10, 10, 0xFFFF, 0x0000, (uint8_t *)"0", 0x1);
        LCD_DrawFont_64x32(104, 124, 0xFFFF, 0x0000, display_buf, 0x1);
        for (int j = 0; j < 10; j++) {
            sprintf(trng_buf, "%d:%d ", j, num_sum[j]);
            LCD_DrawFont(10, 10 + 25 * j, 0xFFFF, 0x0000, trng_buf, 0xFF);
        }
        bflb_platform_delay_ms(10);
    }

    sec_eng_trng_disable();
}
