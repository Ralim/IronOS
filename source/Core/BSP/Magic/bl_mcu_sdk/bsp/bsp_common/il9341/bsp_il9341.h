/**
 * @file bsp_il9341.h
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
#ifndef __TFT_LCD_H__
#define __TFT_LCD_H__



#include "hal_gpio.h"
/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  TFT_LCD
 *  @{
 */

/** @defgroup  TFT_LCD_Public_Types
 *  @{
 */

/*@} end of group TFT_LCD_Public_Types */

/** @defgroup  TFT_LCD_Public_Constants
 *  @{
 */

/*@} end of group TFT_LCD_Public_Constants */

/** @defgroup  TFT_LCD_Public_Macros
 *  @{
 */
#define LCD_CS_PIN GPIO_PIN_10
#define LCD_DC_PIN GPIO_PIN_22

#define CS1_HIGH gpio_write(LCD_CS_PIN, 1)
#define CS1_LOW  gpio_write(LCD_CS_PIN, 0)
#define DC_HIGH  gpio_write(LCD_DC_PIN, 1)
#define DC_LOW   gpio_write(LCD_DC_PIN, 0)
#define LCD_W    240 /* LCD width */
#define LCD_H    320 /* LCD height */
/* Turn 24-bit RGB color to 16-bit */
#define RGB(r, g, b) (((r >> 3) << 3 | (g >> 5) | (g >> 2) << 13 | (b >> 3) << 8) & 0xffff)
/* Calculate 32-bit or 16-bit absolute value */
#define ABS32(value) ((value ^ (value >> 31)) - (value >> 31))
#define ABS16(value) ((value ^ (value >> 15)) - (value >> 15))

/*@} end of group TFT_LCD_Public_Macros */

/** @defgroup  TFT_LCD_Public_Functions
 *  @{
 */

/**
 *  @brief Tft_lcd Functions
 */
#ifdef __cplusplus
extern "C"{
#endif

void LCD_WR_Cmd(uint8_t command);
void LCD_WR_Byte(uint8_t data);
void LCD_WR_HalfWord(uint16_t data);
void LCD_WR_Word(uint32_t data);
void LCD_Set_Dir(uint8_t dir);
void LCD_Set_Addr(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
void LCD_Init(void);
void LCD_Clear(uint16_t color);
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_DrawCircle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);
void LCD_DrawArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_DrawPicture(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *picture);
void LCD_DrawPicture_cam(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *picture);
void LCD_WR_SPI_DMA(uint16_t *img, uint32_t len);
// void LCD_UartDrawPicture(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,UART_ID_Type uartId);
void LCD_DrawChinese(uint16_t x, uint16_t y, uint8_t *character, uint16_t bColor, uint16_t cColor);
void LCD_DrawFont(uint16_t x, uint16_t y, uint16_t color, uint16_t bk_color, uint8_t *str, uint8_t num);
void LCD_DrawFont_64x32(uint16_t x, uint16_t y, uint16_t color, uint16_t bk_color, uint8_t *str, uint8_t num);

#ifdef __cplusplus
}
#endif
/*@} end of group TFT_LCD_Public_Functions */

/*@} end of group TFT_LCD */

/*@} end of group BL702_Peripheral_Driver */

#endif /* __TFT_LCD_H__ */

