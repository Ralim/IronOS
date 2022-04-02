/**
 * @file ili9341.h
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

#ifndef _ILI9341_H_
#define _ILI9341_H_

#include "bflb_platform.h"

#define ILI9341_CS_PIN GPIO_PIN_10
#define ILI9341_DC_PIN GPIO_PIN_22

#define ILI9341_CS_HIGH gpio_write(ILI9341_CS_PIN, 1)
#define ILI9341_CS_LOW  gpio_write(ILI9341_CS_PIN, 0)
#define ILI9341_DC_HIGH gpio_write(ILI9341_DC_PIN, 1)
#define ILI9341_DC_LOW  gpio_write(ILI9341_DC_PIN, 0)

#define ILI9341_W 240 /* ILI9341 LCD width */
#define ILI9341_H 320 /* ILI9341 LCD height */

#define ILI9341_OFFSET_X 0
#define ILI9341_OFFSET_Y 0

typedef struct {
    uint8_t cmd;
    const char *data;
    uint8_t databytes; /* Num of data in data; bit 7 = delay after set */
} ili9341_init_cmd_t;

int ili9341_init();
int ili9341_set_dir(uint8_t dir, uint8_t mir_flag);
void ili9341_set_draw_window(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
void ili9341_draw_point(uint16_t x, uint16_t y, uint16_t color);
void ili9341_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void ili9341_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *picture);
void ili9341_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *picture);
int ili9341_draw_is_busy(void);

#endif