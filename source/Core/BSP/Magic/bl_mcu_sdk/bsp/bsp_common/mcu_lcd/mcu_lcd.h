/**
 * @file mcu_lcd.h
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

#ifndef _MCU_LCD_H_
#define _MCU_LCD_H_

#include "font.h"

#define MCU_LCD_ILI9341
// #define MCU_LCD_ST7735S /* 未验证 */
// #define MCU_LCD_ST7789V

extern struct device *lcd_dev_ifs;

#if defined MCU_LCD_ILI9341

#include "ili9341.h"
#define LCD_W ILI9341_W
#define LCD_H ILI9341_H
typedef uint16_t lcd_color_t;

#elif defined MCU_LCD_ST7735S

#include "st7735s.h"
#define LCD_W ST7735S_W
#define LCD_H ST7735S_H
typedef uint16_t lcd_color_t;

#elif defined MCU_LCD_ST7789V

#include "st7789v.h"
#define LCD_W ST7789V_W
#define LCD_H ST7789V_H
typedef uint16_t lcd_color_t;

#endif

#define ABS(x) ((x) > 0 ? (x) : -(x))

extern struct device *lcd_dev_ifs;
extern uint8_t lcd_auto_swap_flag;

int lcd_auto_swap_set(uint8_t flag);
int lcd_swap_color_data16(uint16_t *dst, uint16_t *src, uint32_t num);
int lcd_init(void);
int lcd_set_dir(uint8_t dir, uint8_t mir_flag);
int lcd_get_dir();
int lcd_draw_point(uint16_t x, uint16_t y, lcd_color_t color);
int lcd_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color);
int lcd_clear(uint16_t color);
int lcd_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t *picture);
int lcd_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t *picture);
int lcd_draw_is_busy(void);
int lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color);
int lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, lcd_color_t color);
int lcd_draw_circle(uint16_t x, uint16_t y, uint16_t r, lcd_color_t color);
#if FONT_ASCII_16X8
int lcd_draw_str_ascii16(uint16_t x, uint16_t y, lcd_color_t color, lcd_color_t bk_color, uint8_t *str, uint8_t num);
#endif
#endif