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
#include "hal_gpio.h"
#include "hal_dma.h"

#define TRANSFER_LEN   1
#define RGB24(r, g, b) ((r << 16) | (g << 8) | (b))

#define LED_NUM (60)

// static uint32_t txBuff[TRANSFER_LEN] = { 0 };
struct device *spi_ws2812b;

static uint8_t color_green[24] = { 0xFC, 0xFC, 0xFC, 0xFC,
                                   0xFC, 0xFC, 0xFC, 0xFC,
                                   0xE0, 0xE0, 0xE0, 0xE0,
                                   0xE0, 0xE0, 0xE0, 0xE0,
                                   0xE0, 0xE0, 0xE0, 0xE0,
                                   0xE0, 0xE0, 0xE0, 0xE0 };

static uint8_t code01[2] = { 0xE0, 0xFC };

static uint32_t color_blue_24[6] = {
    0xE0E0E0E0,
    0xE0E0E0E0,
    0xE0E0E0E0,
    0xE0E0E0E0,
    0xFCFCFCFC,
    0xFCFCFCFC,
};

static uint8_t tx_buf[24] = { 0 };

/**
 * @brief Get the rgb object
 *
 * @param x  // rgb color val, must less 1530
 * @param a  // led luminance, must less 8; so this is 8 position adjustment
 * @return uint32_t // rgb 24
 */
uint32_t get_rgb(uint32_t x, uint8_t a)
{
    uint8_t r, g, b;
    uint16_t i = 0, j = 0;
    x %= 1530;
    i = x / 256;

    j = x % 256;

    switch (i) {
        case 0: {
            r = 255;
            g = 0;
            b = j;
        } break;

        case 1: {
            r = 255 - j;
            g = 0;
            b = 255;
        } break;
        case 2: {
            r = 0;
            g = j;
            b = 255;
        } break;
        case 3: {
            r = 0;
            g = 255;
            b = 255 - j;
        } break;
        case 4: {
            r = j;
            g = 255;
            b = 0;
        } break;
        case 5: {
            r = 255;
            g = 255 - j;
            b = 0;
        } break;

        default:
            return 0;
            break;
    }

    return RGB24((r >> a), (g >> a), (b >> a));
}

/**
 * @brief color to spi send byte
 *
 * @param color
 * @param pbyte
 */
void color24_to_8byte(uint32_t color, uint8_t *pbyte)
{
    uint8_t i = 0;
    uint32_t n = 0;

    uint32_t j = 0x8000; // 24bit, G
    for (i = 0; i < 8; i++) {
        if ((color & j) == j) {
            pbyte[n] = 0xFC;
        } else {
            pbyte[n] = 0xE0;
        }
        n++;
        j >>= 1;
    }
    j = 0x800000; // 16bit, R
    for (i = 0; i < 8; i++) {
        if ((color & j) == j) {
            pbyte[n] = 0xFC;
        } else {
            pbyte[n] = 0xE0;
        }
        n++;
        j >>= 1;
    }
    j = 0x80; // 8bit, B
    for (i = 0; i < 8; i++) {
        if ((color & j) == j) {
            pbyte[n] = 0xFC;
        } else {
            pbyte[n] = 0xE0;
        }
        n++;
        j >>= 1;
    }
}

/**
 * @brief spi send one led color
 *
 * @param color
 */
void spi_send_val(uint32_t color)
{
    color24_to_8byte(color, tx_buf);

    spi_transmit(spi_ws2812b, &tx_buf, 24, 0);
    // spi_transmit(spi_ws2812b, &color_green, 24, 0);
}

/**
 * @brief spi send ws2812B 0 code
 *
 */
void spi_send_code0(void)
{
    // MSG("spi send 0 code 0x%x \r\n", code01[0]);
    spi_transmit(spi_ws2812b, &code01[0], 1, 0);
}

/**
 * @brief spi send ws2812B 1 code
 *
 */
void spi_send_code1(void)
{
    spi_transmit(spi_ws2812b, &code01[1], 1, 0);
}

/**
 * @brief
 *
 * @param data
 */
void ws2812_write_byte(uint8_t data)
{
    uint8_t i = 0;

    for (i = 0; i < 8; i++) {
        if ((data & 0x80) == 0x80) {
            spi_send_code0();
        } else {
            spi_send_code1();
        }
        data = data << 1;
    }
}

/**
 * @brief
 *
 * @param rgb_val
 */
void ws2812_write_val(uint32_t rgb_val)
{
    ws2812_write_byte((uint8_t)(rgb_val >> 8));  // G
    ws2812_write_byte((uint8_t)(rgb_val >> 16)); // R
    ws2812_write_byte((uint8_t)(rgb_val >> 0));  // B
}

int main(void)
{
    uint16_t rgb_base = 0;
    uint16_t k = 51;
    uint32_t rgb_buff[LED_NUM];

    bflb_platform_init(0);
    spi_register(SPI0_INDEX, "spi_ws2812b");
    spi_ws2812b = device_find("spi_ws2812b");

    if (spi_ws2812b) {
        device_open(spi_ws2812b, DEVICE_OFLAG_STREAM_TX);
    }

    while (1) {
        uint32_t rgb_tmp = rgb_base;
        for (uint16_t i = 0; i < LED_NUM; i++) {
            rgb_buff[i] = get_rgb((rgb_tmp += k), 4);
        }

        rgb_base += 5;
        if (rgb_base >= 1530) {
            rgb_base = 0;
        }

        for (uint16_t i = 0; i < LED_NUM; i++) {
            spi_send_val(rgb_buff[i]);
        }

        bflb_platform_delay_ms(1);
    }

    BL_CASE_SUCCESS;
    while (1)
        ;
}
