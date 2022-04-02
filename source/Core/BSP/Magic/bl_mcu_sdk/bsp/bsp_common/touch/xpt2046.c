/**
 * @file xip2046.c
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

#include "hal_spi.h"
#include "hal_gpio.h"
#include "bl702_spi.h"
#include "xpt2046.h"
// #include "lvgl.h"

#define CMD_Y_READ  0b10010000 // NOTE: XPT2046 data sheet says this is actually Y
#define CMD_X_READ  0b11010000 // NOTE: XPT2046 data sheet says this is actually X
#define CMD_Z1_READ 0b10110000
#define CMD_Z2_READ 0b11000000

struct device *touch_spi;

uint16_t avg_buf_x[XPT2046_AVG_NUM];
uint16_t avg_buf_y[XPT2046_AVG_NUM];

static void xpt2046_spi_read_reg(uint8_t reg, uint8_t *data, uint8_t byte_count)
{
    gpio_write(TOUCH_PIN_CS, 0);
    spi_transmit(touch_spi, &reg, 1, 0);
    spi_receive(touch_spi, data, byte_count, 0);
    gpio_write(TOUCH_PIN_CS, 1);
}

// static void xpt2046_spi_write_reg(uint8_t reg, uint8_t* data, uint8_t byte_count)
// {
//     gpio_write(TOUCH_PIN_CS,0);
//     spi_transmit(touch_spi, &reg,  1, 0);
//     spi_transmit(touch_spi, data, byte_count,0);
//     gpio_write(TOUCH_PIN_CS,1);
// }

static uint16_t xpt2046_cmd(uint8_t cmd)
{
    uint8_t data[2];
    xpt2046_spi_read_reg(cmd, data, 2);
    uint16_t val = ((data[0] << 8) | data[1]) >> 3;
    return val;
}
/******************************************************************************
 * @brief  xpt2046 touchpad init
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void xpt2046_init(void)
{
    gpio_set_mode(TOUCH_PIN_CS, GPIO_OUTPUT_MODE);
    gpio_write(TOUCH_PIN_CS, 1);
    touch_spi = device_find("spi0");

    if (touch_spi) {
        device_close(touch_spi);
    } else {
        spi_register(SPI0_INDEX, "spi0");
        touch_spi = device_find("spi0");
    }

    if (touch_spi) {
        device_open(touch_spi, DEVICE_OFLAG_STREAM_TX | DEVICE_OFLAG_STREAM_RX);
    }
}

/******************************************************************************
 * @brief  xpt2046_touch_detect_t
 *
 * @param
 *
 * @return
 *
*******************************************************************************/
static uint8_t xpt2048_is_touch_detected()
{
    // check pressure if we are pressure or IRQ and pressure
    uint16_t z1 = xpt2046_cmd(CMD_Z1_READ);
    uint16_t z2 = xpt2046_cmd(CMD_Z2_READ);

    // this is not what the confusing datasheet says but it seems to
    // be enough to detect real touches on the panel
    uint16_t z = z1 + 4096 - z2;

    if (z > XPT2046_TOUCH_THRESHOLD) {
        return 1;
    } else {
        return 0;
    }
}

/******************************************************************************
 * @brief  xpt2046_ads_get
 *
 * @param
 *
 * @return
 *
*******************************************************************************/
static uint8_t xpt2046_ads_get(int16_t *x, int16_t *y)
{
    for (uint8_t i = 0; i < XPT2046_AVG_NUM; i++) {
        if (xpt2048_is_touch_detected() == 0) {
            return 0;
        }

        avg_buf_x[i] = xpt2046_cmd(CMD_X_READ);
        avg_buf_y[i] = xpt2046_cmd(CMD_Y_READ);
    }

    int16_t x_min = avg_buf_x[0], x_max = avg_buf_x[0];
    int16_t y_min = avg_buf_y[0], y_max = avg_buf_y[0];
    int32_t x_sum = avg_buf_x[0], y_sum = avg_buf_y[0];

    for (uint8_t i = 1; i < XPT2046_AVG_NUM; i++) {
        if (x_min < avg_buf_x[i]) {
            x_min = avg_buf_x[i];
        }

        if (x_max > avg_buf_x[i]) {
            x_max = avg_buf_x[i];
        }

        if (y_min < avg_buf_y[i]) {
            y_min = avg_buf_y[i];
        }

        if (y_max > avg_buf_y[i]) {
            y_max = avg_buf_y[i];
        }

        x_sum += avg_buf_x[i];
        y_sum += avg_buf_y[i];
    }

    if (x_max - x_min > 40 || y_max - y_min > 50) {
        return 0;
    }

    *x = (x_sum - x_min - x_max) / (XPT2046_AVG_NUM - 2);
    *y = (y_sum - y_min - y_max) / (XPT2046_AVG_NUM - 2);
    return 2;
}

/******************************************************************************
 * @brief  xpt2046_corr
 *
 * @param
 *
 * @return
 *
*******************************************************************************/
static uint8_t xpt2046_adc2xy(int16_t *x, int16_t *y)
{
    if ((*x) > XPT2046_X_MIN) {
        (*x) -= XPT2046_X_MIN;
    } else {
        (*x) = 0;
    }

    if ((*y) > XPT2046_Y_MIN) {
        (*y) -= XPT2046_Y_MIN;
    } else {
        (*y) = 0;
    }

    (*x) = (uint32_t)((uint32_t)(*x) * 240) /
           (XPT2046_X_MAX - XPT2046_X_MIN);

    (*y) = (uint32_t)((uint32_t)(*y) * 320) /
           (XPT2046_Y_MAX - XPT2046_Y_MIN);

    return 1;
}

/******************************************************************************
 * @brief  xpt2046_read
 *
 * @param
 *
 * @return
 *
*******************************************************************************/
uint8_t xpt2046_read(int16_t *x, int16_t *y)
{
    static int16_t xt = 0, yt = 0;
    static uint8_t avg_last = 0;
    int16_t x1, y1;

    if (xpt2046_ads_get(&x1, &y1) == 0) {
        goto end;
    }

    if (xpt2046_adc2xy(&x1, &y1) == 0) {
        goto end;
    }

    if (avg_last == 0) {
        avg_last = 1;
        return 0;
    } else {
        avg_last = 2;
        xt = x1;
        *x = x1;
        yt = y1;
        *y = y1;
        return 1;
    }

end:

    if (avg_last == 2) {
        avg_last = 1;
        *x = xt;
        *y = yt;
        return 1;
    } else {
        avg_last = 0;
        return 0;
    }
}
