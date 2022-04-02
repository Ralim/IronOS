/**
 * @file ili9341.c
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

#include "ili9341.h"
#include "mcu_lcd.h"

#include "hal_spi.h"
#include "hal_dma.h"
#include "hal_gpio.h"

static struct device *spi0;
static struct device *dma_ch3;

const ili9341_init_cmd_t ili9341_init_cmds[] = {
    { 0x11, "\x00", 0x80 }, /* Exit sleep */
    { 0xCF, "\x00\xd9\x30", 3 },
    { 0xED, "\x64\x03\x12\x81", 4 },
    { 0xE8, "\x85\x01\x78", 3 },
    { 0xCB, "\x39\x2C\x00\x34\x02", 5 },
    { 0xF7, "\x20", 1 },
    { 0xEA, "\x00\x00", 2 },

    { 0xC0, "\x23", 1 }, /*Power control*/
    { 0xC1, "\x12", 1 }, /*Power control */
    { 0xC2, "\x11", 1 },
    { 0xC5, "\x40\x30", 2 }, /*VCOM control 1*/
    { 0xC7, "\xa9", 1 },     /*VCOM control 2*/
    { 0x36, "\x08", 1 },     /*Memory Access Control*/
    { 0x3A, "\x55", 1 },     /*Pixel Format Set*/
    { 0xB1, "\x00\x18", 2 }, /* Frame Rate Control */
    { 0xB6, "\x0a\xa2", 2 }, /* Display Function Control */
    { 0x0C, "\xd5", 1 },     /* display pixel format,RGB 16bits,MCU 16bits  */
    { 0xF2, "\x00", 1 },     /* 3Gamma Function Disable */
    { 0xF7, "\x20", 1 },

    { 0x26, "\x01", 1 },                                                          /* Gamma curve selected */
    { 0xE0, "\x1F\x1A\x18\x0A\x0F\x06\x45\x87\x32\x0A\x07\x02\x07\x05\x00", 15 }, /* Set Gamma */
    { 0XE1, "\x00\x25\x27\x05\x10\x09\x3A\x78\x4D\x05\x18\x0D\x38\x3A\x1F", 15 }, /* Set Gamma */
    { 0xB7, "\x07", 1 },

    { 0x29, "\x00", 0x80 }, /* Display on */
};

/**
 * @brief ili9341_spi_init
 *
 * @return int  0:succes  1:error
 */
static int ili9341_spi_init(void)
{
    gpio_set_mode(ILI9341_CS_PIN, GPIO_OUTPUT_MODE);
    gpio_set_mode(ILI9341_DC_PIN, GPIO_OUTPUT_MODE);
    ILI9341_CS_HIGH;
    ILI9341_DC_HIGH;

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
        SPI_DEV(spi0)->fifo_threshold = 4;

        device_open(spi0, DEVICE_OFLAG_STREAM_TX);
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
        device_control(dma_ch3, DEVICE_CTRL_SET_INT, NULL);
    } else {
        return 1;
    }

    device_control(spi0, DEVICE_CTRL_TX_DMA_SUSPEND, NULL);
    return 0;
}

/**
 * @brief ili9341_write_cmd
 *
 * @param cmd
 * @return int 0:succes  1:error
 */
static int ili9341_write_cmd(uint8_t cmd)
{
    ILI9341_DC_LOW;
    ILI9341_CS_LOW;
    int res = spi_transmit(spi0, &cmd, 1, SPI_TRANSFER_TYPE_8BIT);
    ILI9341_CS_HIGH;
    return res;
}

/**
 * @brief ili9341_write_data_1byte
 *
 * @param data
 * @return int 0:succes  1:error
 */
static int ili9341_write_data_1byte(uint8_t data)
{
    ILI9341_DC_HIGH;
    ILI9341_CS_LOW;
    int res = spi_transmit(spi0, &data, 1, SPI_TRANSFER_TYPE_8BIT);
    ILI9341_CS_HIGH;
    return res;
}

/**
 * @brief ili9341_write_data_2byte
 *
 * @param data
 * @return int 0:succes  1:error
 */
// static int ili9341_write_data_2byte(uint16_t data)
// {
//     data = ((data >> 8) & 0xFF) | data << 8;
//     ILI9341_DC_HIGH;
//     ILI9341_CS_LOW;
//     int res = spi_transmit(spi0, &data, 1, SPI_TRANSFER_TYPE_16BIT);
//     ILI9341_CS_HIGH;
//     return res;
// }

/**
 * @brief ili9341_write_data_4byte
 *
 * @param data
 * @return int 0:succes  1:error
 */
static int ili9341_write_data_4byte(uint32_t data)
{
    data = ((data >> 24) & 0xFF) | ((data >> 8) & 0xFF00) | ((data << 8) & 0xFF0000) | ((data << 24));
    ILI9341_DC_HIGH;
    ILI9341_CS_LOW;
    int res = spi_transmit(spi0, &data, 1, SPI_TRANSFER_TYPE_32BIT);
    ILI9341_CS_HIGH;
    return res;
}

/**
 * @brief ili9341_draw_is_busy, After the call ili9341_draw_picture_dma must check this,
 *         if ili9341_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int ili9341_draw_is_busy(void)
{
    if (dma_channel_check_busy(SPI_DEV(spi0)->tx_dma)) {
        return 1;
    } else {
        device_control(spi0, DEVICE_CTRL_TX_DMA_SUSPEND, NULL);
        ILI9341_CS_HIGH;
        return 0;
    }
}

/**
 * @brief ili9341_init
 *
 * @return int
 */
int ili9341_init()
{
    int res = ili9341_spi_init();
    if (res) {
        return res;
    }

    for (uint16_t i = 0; i < (sizeof(ili9341_init_cmds) / sizeof(ili9341_init_cmd_t)); i++) {
        /* send register address */
        res |= ili9341_write_cmd(ili9341_init_cmds[i].cmd);

        /* send register data */
        for (uint8_t j = 0; j < (ili9341_init_cmds[i].databytes & 0x7F); j++) {
            res |= ili9341_write_data_1byte(ili9341_init_cmds[i].data[j]);
        }

        if (res) {
            return res;
        }

        /* delay */
        if (ili9341_init_cmds[i].databytes & 0x80) {
            bflb_platform_delay_ms(100);
        }
    }
    ili9341_set_draw_window(0, 0, ILI9341_H, ILI9341_W);
    return res;
}

/**
 * @brief
 *
 * @param dir
 * @param mir_flag
 */
int ili9341_set_dir(uint8_t dir, uint8_t mir_flag)
{
    ili9341_write_cmd(0x36);
    switch (dir) {
        case 0:
            if (!mir_flag)
                ili9341_write_data_1byte(0x08);
            else
                ili9341_write_data_1byte(0x48);
            break;
        case 1:
            if (!mir_flag)
                ili9341_write_data_1byte(0xA8);
            else
                ili9341_write_data_1byte(0x28);
            break;
        case 2:
            if (!mir_flag)
                ili9341_write_data_1byte(0xC8);
            else
                ili9341_write_data_1byte(0x88);
            break;
        case 3:
            if (!mir_flag)
                ili9341_write_data_1byte(0x68);
            else
                ili9341_write_data_1byte(0xE8);

            break;
        default:
            return -1;
            break;
    }
    return dir;
}

/**
 * @brief ili9341_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void ili9341_set_draw_window(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
#if ILI9341_OFFSET_X
    x1 += ILI9341_OFFSET_X;
    x2 += ILI9341_OFFSET_X;
#endif
#if ILI9341_OFFSET_Y
    y1 += ILI9341_OFFSET_Y;
    y2 += ILI9341_OFFSET_Y;
#endif
    ili9341_write_cmd(0x2a);
    ili9341_write_data_4byte(x1 << 16 | (x2 & 0xFFFF));
    ili9341_write_cmd(0x2b);
    ili9341_write_data_4byte(y1 << 16 | (y2 & 0xFFFF));
    ili9341_write_cmd(0x2c);
}

/**
 * @brief ili9341_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void ili9341_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    if (lcd_auto_swap_flag) {
        color = ((color >> 8) & 0xFF) | color << 8;
    }
    ili9341_set_draw_window(x, y, x, y);
    ILI9341_DC_HIGH;
    ILI9341_CS_LOW;
    spi_transmit(spi0, &color, 1, SPI_TRANSFER_TYPE_16BIT);
    ILI9341_CS_HIGH;
}

/**
 * @brief ili9341_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void ili9341_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    if (lcd_auto_swap_flag) {
        color = ((color >> 8) & 0xFF) | color << 8;
    }
    ili9341_set_draw_window(x1, y1, x2, y2);
    ILI9341_DC_HIGH;
    ILI9341_CS_LOW;
    for (uint16_t i = y1; i <= y2; i++) {
        for (uint16_t j = x1; j <= x2; j++)
            spi_transmit(spi0, &color, 1, SPI_TRANSFER_TYPE_16BIT);
    }
    ILI9341_CS_HIGH;
}

/**
 * @brief ili9341_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (ili9341_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void ili9341_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *picture)
{
    uint8_t ret = 0;
    size_t picture_size = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* 数据高低位切换 */
    if (lcd_auto_swap_flag) {
        lcd_swap_color_data16(picture, picture, picture_size);
    }

    // DMA_DEV(dma_ch3)->src_width = DMA_TRANSFER_WIDTH_32BIT;
    // DMA_DEV(dma_ch3)->src_burst_size = DMA_BURST_1BYTE;
    // DMA_DEV(dma_ch3)->dst_burst_size = DMA_BURST_4BYTE;

    ili9341_set_draw_window(x1, y1, x2, y2);
    ILI9341_DC_HIGH;
    ILI9341_CS_LOW;
    device_control(spi0, DEVICE_CTRL_TX_DMA_RESUME, NULL);
    ret = device_write(spi0, 0, picture, picture_size * 2);
    if (ret != 0) {
        MSG("device write fail!\r\n");
    }
}

/**
 * @brief ili9341_draw_picture,Blocking，Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void ili9341_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *picture)
{
    ili9341_draw_picture_nonblocking(x1, y1, x2, y2, picture);
    while (ili9341_draw_is_busy()) {
    };
}
