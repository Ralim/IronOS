/**
 * @file st7735s.c
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
 *
 */

#include "st7735s.h"
#include "mcu_lcd.h"

#include "hal_spi.h"
#include "hal_dma.h"
#include "hal_gpio.h"

static struct device *spi0;
static struct device *dma_ch3;

const st7735s_init_cmd_t st7735s_init_cmds[] = {
    { 0x01, NULL, 0x80 }, /* Software reset */
    { 0x11, NULL, 0x80 }, /* Exit sleep */

    { 0xB1, "\x01\x2C\x2D", 3 },             /* Frame rate ctrl - normal mode,  Rate = fosc/(1x2+40) * (LINE+2C+2D) */
    { 0xB2, "\x01\x2C\x2D", 3 },             /* Frame rate control - idle mode, Rate = fosc/(1x2+40) * (LINE+2C+2D) */
    { 0xB3, "\x01\x2C\x2D\x01\x2C\x2D", 6 }, /* Frame rate ctrl - partial mode, Dot inversion mode. Line inversion mode */
    { 0xB4, "\x07", 1 },                     /* Display inversion ctrl, No inversion */

    { 0xC0, "\xa2\x02\x84", 3 }, /* Power control, -4.6V AUTO mode */
    { 0xC1, "\xc5", 1 },         /* Power control, VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD */
    { 0xC2, "\x0a\x00", 2 },     /* Power control, Opamp current small, Boost frequency */
    { 0xC3, "\x8a\x2a", 2 },     /* Power control, BCLK/2, Opamp current small & Medium low */
    { 0xC4, "\x8a\xee", 2 },     /* Power control */
    { 0xC5, "\x0e", 1 },         /* Power control */

    { 0x20, "\x00", 1 }, /* set non-inverted mode, 16-bit color */
    //{ 0x21, "\x00", 1 },  /* set inverted mode */
    { 0x3A, "\x05", 1 }, /* set color mode */
    { 0x36, "\xC0", 1 }, /* set MX, MY, RGB mode */

    { 0xE0, "\x02\x1c\x07\x12\x37\x32\x29\x2d\x29\x25\x2B\x39\x00\x01\x03\x10", 16 }, /* set Gamma Sequence */
    { 0xE1, "\x03\x1d\x07\x06\x2E\x2C\x29\x2D\x2E\x2E\x37\x3F\x00\x00\x02\x10", 16 }, /* set Gamma Sequence */

    { 0x13, NULL, 0x80 }, /* Normal display on */
    { 0x20, NULL, 0x80 }, /* Display on */

};

/**
 * @brief st7735s_spi_init
 *
 * @return int  0:succes  1:error
 */
static int st7735s_spi_init(void)
{
    gpio_set_mode(ST7735S_CS_PIN, GPIO_OUTPUT_MODE);
    gpio_set_mode(ST7735S_DC_PIN, GPIO_OUTPUT_MODE);
    ST7735S_CS_HIGH;
    ST7735S_DC_HIGH;

    spi0 = device_find("lcd_dev_ifs");
    if (spi0) {
        device_close(spi0);
    } else {
        spi_register(SPI0_INDEX, "lcd_dev_ifs");
        spi0 = device_find("lcd_dev_ifs");
    }
    if (spi0) {
        SPI_DEV(spi0)->mode = SPI_MASTER_MODE;
        SPI_DEV(spi0)->clk = (36 * 1000 * 1000);
        SPI_DEV(spi0)->direction = SPI_MSB_BYTE0_DIRECTION_FIRST;
        SPI_DEV(spi0)->clk_polaraity = SPI_POLARITY_LOW;
        SPI_DEV(spi0)->datasize = SPI_DATASIZE_8BIT;
        SPI_DEV(spi0)->clk_phase = SPI_PHASE_1EDGE;
        SPI_DEV(spi0)->fifo_threshold = 0;

        device_open(spi0, DEVICE_OFLAG_STREAM_TX | DEVICE_OFLAG_STREAM_RX);
    } else {
        return 1;
    }

    dma_ch3 = device_find("lcd_dev_ifs_dma");
    if (dma_ch3) {
        device_close(dma_ch3);
    } else {
        dma_register(DMA0_CH3_INDEX, "lcd_dev_ifs_dma");
        dma_ch3 = device_find("lcd_dev_ifs_dma");
    }
    if (dma_ch3) {
        DMA_DEV(dma_ch3)->direction = DMA_MEMORY_TO_PERIPH;
        DMA_DEV(dma_ch3)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch3)->src_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch3)->dst_req = DMA_REQUEST_SPI0_TX;
        DMA_DEV(dma_ch3)->src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch3)->dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch3)->src_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch3)->dst_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch3)->src_width = DMA_TRANSFER_WIDTH_8BIT;
        DMA_DEV(dma_ch3)->dst_width = DMA_TRANSFER_WIDTH_8BIT;
        device_open(dma_ch3, 0);
        device_set_callback(dma_ch3, NULL);
        device_control(spi0, DEVICE_CTRL_ATTACH_TX_DMA, dma_ch3);
        device_control(dma_ch3, DEVICE_CTRL_CLR_INT, NULL);
    } else {
        return 1;
    }

    device_control(spi0, DEVICE_CTRL_TX_DMA_SUSPEND, NULL);
    return 0;
}

/**
 * @brief st7735s_write_cmd
 *
 * @param cmd
 * @return int 0:succes  1:error
 */
static int st7735s_write_cmd(uint8_t cmd)
{
    ST7735S_DC_LOW;
    ST7735S_CS_LOW;
    int res = spi_transmit(spi0, &cmd, 1, SPI_TRANSFER_TYPE_8BIT);
    ST7735S_CS_HIGH;
    return res;
}

/**
 * @brief st7735s_write_data_1byte
 *
 * @param data
 * @return int 0:succes  1:error
 */
static int st7735s_write_data_1byte(uint8_t data)
{
    ST7735S_DC_HIGH;
    ST7735S_CS_LOW;
    int res = spi_transmit(spi0, &data, 1, SPI_TRANSFER_TYPE_8BIT);
    ST7735S_CS_HIGH;
    return res;
}

/**
 * @brief st7735s_write_data_2byte
 *
 * @param data
 * @return int 0:succes  1:error
 */
// static int st7735s_write_data_2byte(uint16_t data)
// {
//     data = ((data >> 8) & 0xFF) | data << 8;
//     ST7735S_DC_HIGH;
//     ST7735S_CS_LOW;
//     int res = spi_transmit(spi0, &data, 1, SPI_TRANSFER_TYPE_16BIT);
//     ST7735S_CS_HIGH;
//     return res;
// }

/**
 * @brief st7735s_write_data_4byte
 *
 * @param data
 * @return int 0:succes  1:error
 */
static int st7735s_write_data_4byte(uint32_t data)
{
    data = ((data >> 24) & 0xFF) | ((data >> 8) & 0xFF00) | ((data << 8) & 0xFF0000) | ((data << 24));
    ST7735S_DC_HIGH;
    ST7735S_CS_LOW;
    int res = spi_transmit(spi0, &data, 1, SPI_TRANSFER_TYPE_32BIT);
    ST7735S_CS_HIGH;
    return res;
}

/**
 * @brief st7735s_draw_is_busy, After the call st7735s_draw_picture_dma must check this,
 *         if st7735s_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int st7735s_draw_is_busy(void)
{
    if (dma_channel_check_busy(SPI_DEV(spi0)->tx_dma)) {
        return 1;
    } else {
        device_control(spi0, DEVICE_CTRL_TX_DMA_SUSPEND, NULL);
        ST7735S_CS_HIGH;
        return 0;
    }
}

/**
 * @brief st7735s_init
 *
 * @return int
 */
int st7735s_init()
{
    int res = st7735s_spi_init();
    if (res) {
        return res;
    }

    for (uint16_t i = 0; i < (sizeof(st7735s_init_cmds) / sizeof(st7735s_init_cmd_t)); i++) {
        /* send register address */
        res |= st7735s_write_cmd(st7735s_init_cmds[i].cmd);

        /* send register data */
        for (uint8_t j = 0; j < (st7735s_init_cmds[i].databytes & 0x7F); j++) {
            res |= st7735s_write_data_1byte(st7735s_init_cmds[i].data[j]);
        }

        if (res) {
            return res;
        }

        /* delay */
        if (st7735s_init_cmds[i].databytes & 0x80) {
            bflb_platform_delay_ms(100);
        }
    }
    st7735s_set_draw_window(0, 0, ST7735S_W, ST7735S_H);
    return res;
}

/**
 * @brief  st7735s_set_dir
 *
 * @param dir
 */
void st7735s_set_dir(uint8_t dir)
{
    st7735s_write_cmd(0x36);
    switch (dir) {
        case 0:
            st7735s_write_data_1byte(0x08);
            break;
        case 1:
            st7735s_write_data_1byte(0xA8);
            break;
        case 2:
            st7735s_write_data_1byte(0xC8);
            break;
        case 3:
            st7735s_write_data_1byte(0x68);
            break;
        default:
            break;
    }
}

void st7735s_set_draw_window(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
#if ST7735S_OFFSET_X
    x1 += ST7735S_OFFSET_X;
    x2 += ST7735S_OFFSET_X;
#endif
#if ST7735S_OFFSET_Y
    y1 += ST7735S_OFFSET_Y;
    y2 += ST7735S_OFFSET_Y;
#endif
    st7735s_write_cmd(0x2a);
    st7735s_write_data_4byte(x1 << 16 | (x2 & 0xFFFF));
    st7735s_write_cmd(0x2b);
    st7735s_write_data_4byte(y1 << 16 | (y2 & 0xFFFF));
    st7735s_write_cmd(0x2c);
}

/**
 * @brief st7735s_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void st7735s_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    if (lcd_auto_swap_flag) {
        color = ((color >> 8) & 0xFF) | color << 8;
    }
    st7735s_set_draw_window(x, y, x, y);
    ST7735S_DC_HIGH;
    ST7735S_CS_LOW;
    spi_transmit(spi0, &color, 1, SPI_TRANSFER_TYPE_16BIT);
    ST7735S_CS_HIGH;
}

/**
 * @brief st7735s_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void st7735s_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    if (lcd_auto_swap_flag) {
        color = ((color >> 8) & 0xFF) | color << 8;
    }
    st7735s_set_draw_window(x1, y1, x2, y2);
    ST7735S_DC_HIGH;
    ST7735S_CS_LOW;
    for (uint16_t i = y1; i <= y2; i++) {
        for (uint16_t j = x1; j <= x2; j++)
            spi_transmit(spi0, &color, 1, SPI_TRANSFER_TYPE_16BIT);
    }
    ST7735S_CS_HIGH;
}

/**
 * @brief st7735s_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (st7735s_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void st7735s_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *picture)
{
    size_t picture_size = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* 数据高低位切换 */
    if (lcd_auto_swap_flag) {
        lcd_swap_color_data16(picture, picture, picture_size);
    }

    st7735s_set_draw_window(x1, y1, x2, y2);
    ST7735S_DC_HIGH;
    ST7735S_CS_LOW;
    device_control(spi0, DEVICE_CTRL_TX_DMA_RESUME, NULL);
    device_write(spi0, 0, picture, picture_size * 2);
}

/**
 * @brief st7735s_draw_picture,Blocking，Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void st7735s_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *picture)
{
    st7735s_draw_picture_nonblocking(x1, y1, x2, y2, picture);
    while (st7735s_draw_is_busy()) {
        BL_DRV_DUMMY;
    };
}
