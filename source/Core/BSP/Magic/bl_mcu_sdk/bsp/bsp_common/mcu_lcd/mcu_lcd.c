/**
 * @file mcu_lcd.c
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

#include "mcu_lcd.h"
#include "font.h"
#include "bflb_platform.h"
#include "drv_device.h"

struct device *lcd_dev_ifs = NULL;

uint8_t lcd_dir = 0;
uint8_t lcd_auto_swap_flag = 1;
uint16_t lcd_max_x = LCD_W, lcd_max_y = LCD_H;

/**
 * @brief 设置是否需要自动交换 像素数据大小端, 默认需要的屏幕会开启
 *
 * @param flag 0:off  1:on
 * @return int
 */
int lcd_auto_swap_set(uint8_t flag)
{
    lcd_auto_swap_flag = flag ? 1 : 0;
    return lcd_auto_swap_flag;
}

/**
 * @brief 批量交换 像素数据大小端，在 lcd_auto_swap_flag == 1 时，绘制时自动调用
 *
 * @param dst destination
 * @param src source
 * @param color_num color num
 * @return int
 */
int lcd_swap_color_data16(uint16_t *dst, uint16_t *src, uint32_t color_num)
{
    for (size_t i = 0; i < color_num; i++) {
        dst[i] = (src[i] << 8) | (src[i] >> 8);
    }
    return 0;
}

/**
 * @brief LCD init
 *
 * @return int
 */
int lcd_init(void)
{
    int res;
#if defined(MCU_LCD_ILI9341)
    res = ili9341_init();
#elif defined(MCU_LCD_ST7735S)
    res = st7735s_init();
#elif defined(MCU_LCD_ST7789V)
    res = st7789v_init();
#endif
    lcd_dev_ifs = device_find("lcd_dev_ifs");
    return res;
}

/**
 * @brief Set display direction and mir
 *
 * @param dir 0~3 : 0~270 Angle
 * @param mir_flag 0:normal  1:Horizontal Mirroring(if support)
 * @return int
 */
int lcd_set_dir(uint8_t dir, uint8_t mir_flag)
{
    dir %= 4;
    lcd_dir = dir;
    if (dir == 0 || dir == 2) {
        lcd_max_x = LCD_W;
        lcd_max_y = LCD_H;
    } else {
        lcd_max_x = LCD_H;
        lcd_max_y = LCD_W;
    }
#if defined(MCU_LCD_ILI9341)
    ili9341_set_dir(dir, mir_flag);
    return 0;
#elif defined(MCU_LCD_ST7735S)
    st7735s_set_dir(dir);
    return 0;
#elif defined(MCU_LCD_ST7789V)
    st7789v_set_dir(dir);
    return 0;
#endif
}

/**
 * @brief Draws a point at the specified position
 *
 * @param x  X coordinate
 * @param y  Y coordinate
 * @param color
 * @return int
 */
int lcd_draw_point(uint16_t x, uint16_t y, lcd_color_t color)
{
#if defined(MCU_LCD_ILI9341)
    ili9341_draw_point(x, y, color);
    return 0;
#elif defined(MCU_LCD_ST7735S)
    st7735s_draw_point(x, y, color);
    return 0;
#elif defined(MCU_LCD_ST7789V)
    st7789v_draw_point(x, y, color);
    return 0;
#endif
}

/**
 * @brief Draw a monochrome rectangle (May be less efficient)
 *
 * @param x1 start coordinate
 * @param y1 start coordinate
 * @param x2 end coordinate
 * @param y2 end coordinate
 * @param color
 * @return int
 */
int lcd_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color)
{
#if defined(MCU_LCD_ILI9341)
    ili9341_draw_area(x1, y1, x2, y2, color);
    return 0;
#elif defined(MCU_LCD_ST7735S)
    st7735s_draw_area(x1, y1, x2, y2, color);
    return 0;
#elif defined(MCU_LCD_ST7789V)
    st7789v_draw_area(x1, y1, x2, y2, color);
    return 0;
#endif
}

/**
 * @brief Draw a picture in the designated area(blocking),Will wait for the drawing to finish
 *
 * @param x1 start coordinate
 * @param y1 start coordinate
 * @param x2 end coordinate
 * @param y2 end coordinate
 * @param picture
 * @return int
 */
int lcd_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t *picture)
{
#if defined(MCU_LCD_ILI9341)
    ili9341_draw_picture_blocking(x1, y1, x2, y2, picture);
    return 0;
#elif defined(MCU_LCD_ST7735S)
    st7735s_draw_picture_blocking(x1, y1, x2, y2, picture);
    return 0;
#elif defined(MCU_LCD_ST7789V)
    st7789v_draw_picture_blocking(x1, y1, x2, y2, picture);
    return 0;
#endif
}

/**
 * @brief  Draw a picture in the designated area(nonblocking,if it supports),
 *  Must be calle lcd_draw_is_busy! and (lcd_draw_is_busyd()==1) before performing other drawing and changing  picture data!
 *
 * @param x1 start coordinate
 * @param y1 start coordinate
 * @param x2 end coordinate
 * @param y2 end coordinate
 * @param picture
 * @return int
 */
int lcd_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t *picture)
{
#if defined(MCU_LCD_ILI9341)
    ili9341_draw_picture_nonblocking(x1, y1, x2, y2, picture);
    return 0;
#elif defined(MCU_LCD_ST7735S)
    st7735s_draw_picture_nonblocking(x1, y1, x2, y2, picture);
    return 0;
#elif defined(MCU_LCD_ST7789V)
    st7789v_draw_picture_nonblocking(x1, y1, x2, y2, picture);
    return 0;
#endif
}

/**
 * @brief Check if it is drawing, must call it After call lcd_draw_picture_nonblocking
 *
 * @return int 1:lcd Drawing,Prohibit other operations! 0:Drawing is over
 */
int lcd_draw_is_busy(void)
{
#if defined(MCU_LCD_ILI9341)
    return ili9341_draw_is_busy();
#elif defined(MCU_LCD_ST7735S)
    return st7735s_draw_is_busy();
#elif defined(MCU_LCD_ST7789V)
    return st7789v_draw_is_busy();
#endif
}

/**
 * @brief clear lcd
 *
 * @param color
 * @return int
 */
int lcd_clear(uint16_t color)
{
    if (lcd_dir == 0 || lcd_dir == 2) {
        lcd_draw_area(0, 0, LCD_W, LCD_H, color);
    } else {
        lcd_draw_area(0, 0, LCD_H, LCD_W, color);
    }
    return 0;
}

/**
 * @brief
 *
 * @param x1 start coordinate
 * @param y1 start coordinate
 * @param x2 end coordinate
 * @param y2 end coordinate
 * @param color
 * @return int
 */
int lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color)
{
    int xVariation, yVariation, temp;
    int absX, absY, i;
    xVariation = x2 - x1;
    yVariation = y2 - y1;
    absX = ABS(xVariation);
    absY = ABS(yVariation);

    if (absX > absY) {
        for (i = 0; i < absX + 1; i++) {
            temp = yVariation * 100 / absX * i / 100;

            if (xVariation > 0) {
                lcd_draw_point(x1 + i, y1 + temp, color);
            } else {
                lcd_draw_point(x1 - i, y1 + temp, color);
            }
        }
    } else {
        for (i = 0; i < absY + 1; i++) {
            temp = xVariation * 100 / absY * i / 100;

            if (yVariation > 0) {
                lcd_draw_point(x1 + temp, y1 + i, color);
            } else {
                lcd_draw_point(x1 + temp, y1 - i, color);
            }
        }
    }
    return 0;
}

/**
 * @brief
 *
 * @param x1 start coordinate
 * @param y1 start coordinate
 * @param x2 end coordinate
 * @param y2 end coordinate
 * @param color
 * @return int
 */
int lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color)
{
    lcd_draw_line(x1, y1, x2, y1, color);
    lcd_draw_line(x2, y1, x2, y2, color);
    lcd_draw_line(x2, y2, x1, y2, color);
    lcd_draw_line(x1, y2, x1, y1, color);
    return 0;
}

/**
 * @brief draw a circle
 *
 * @param x coordinate
 * @param y coordinate
 * @param r
 * @param color
 * @return int
 */
int lcd_draw_circle(uint16_t x, uint16_t y, uint16_t r, lcd_color_t color)
{
    int a = 0, b;
    int di;
    b = r;
    di = 3 - (r << 1);

    while (a <= b) {
        lcd_draw_point(x - b, y - a, color);
        lcd_draw_point(x + b, y - a, color);
        lcd_draw_point(x - a, y + b, color);
        lcd_draw_point(x - b, y - a, color);
        lcd_draw_point(x - a, y - b, color);
        lcd_draw_point(x + b, y + a, color);
        lcd_draw_point(x + a, y - b, color);
        lcd_draw_point(x + a, y + b, color);
        lcd_draw_point(x - b, y + a, color);
        a++;

        if (di < 0) {
            di += 4 * a + 6;
        } else {
            di += 10 + 4 * (a - b);
            b--;
        }

        lcd_draw_point(x + a, y + b, color);
    }
    return 0;
}

#if FONT_ASCII_16X8

/**
 * @brief Draw font(16*8) ,Use double buffer to speed up drawing
 *
 * @param x start coordinate
 * @param y start coordinate
 * @param color font color
 * @param bk_color Background color
 * @param str The string to be displayed
 * @param num  number of characters displayed
 * @return int
 */
int lcd_draw_str_ascii16(uint16_t x, uint16_t y, lcd_color_t color, lcd_color_t bk_color, uint8_t *str, uint8_t num)
{
    lcd_color_t draw_buff[2][16 * 8];
    uint16_t buff_color_num;
    uint8_t buff_using_num = 0;
    uint8_t ch, temp;
    uint16_t x0 = x;

    for (uint16_t i = 0; i < num && str[i]; i++) {
        if (str[i] < 128) {
            if (x > LCD_W - 8) {
                x = x0;
                y += 16;
            }
            if (x > LCD_W - 8 || y > LCD_H - 16)
                break;

            ch = str[i];

            if (ch >= ' ') {
                ch = ch - ' ';
            } else if (ch == '\n') {
                x = x0;
                y += 16;
                continue;
            } else {
                continue;
            }

            buff_color_num = 0;
            for (uint8_t j = 0; j < 16; j++) {
                temp = font_ascii_16x8[ch * 16 + j];
                for (uint8_t k = 0; k < 8; k++) {
                    if (temp & (0x80 >> k))
                        draw_buff[buff_using_num][buff_color_num++] = color;
                    else
                        draw_buff[buff_using_num][buff_color_num++] = bk_color;
                }
            }
            while (lcd_draw_is_busy()) {
            };
            lcd_draw_picture_nonblocking(x, y, x + 7, y + 15, draw_buff[buff_using_num]);
            buff_using_num = !buff_using_num;
            x += 8;

        } else {
            continue;
        }
    }
    while (lcd_draw_is_busy()) {
    };
    return 0;
}
#endif