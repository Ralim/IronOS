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
#include "bsp_sf_psram.h"
#include "bsp_image_sensor.h"
#include "hal_spi.h"
#include "hal_gpio.h"
#include "hal_dma.h"
// #include "bsp_il9341.h"
#include "mcu_lcd.h"

#include "picture.c"

#define CAMERA_RESOLUTION_X (128)
#define CAMERA_RESOLUTION_Y (128)

#define YUV422_FRAME_SIZE (CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y * 2)
#define YUV420_FRAME_SIZE (CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y * 2 * 3 / 4)
#define YUV400_FRAME_SIZE (CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y)

#define RGB_FRAME_SIZE (CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y * 3)

#define CAMERA_FRAME_SIZE   (YUV422_FRAME_SIZE)
#define CAMERA_WRITE_ADDR   (0x26000000)
#define CAMERA_BUFFER_SIZE  (CAMERA_FRAME_SIZE)
#define CAMERA_WRITE_ADDR1  (0x26000000 + CAMERA_BUFFER_SIZE)
#define CAMERA_BUFFER_SIZE1 (CAMERA_FRAME_SIZE)

#define YUV_USE (1)

#define TEST_TIM

/* Turn 24-bit RGB color to 16-bit */
#define RGB(r, g, b) (((r >> 3) << 3 | (g >> 5) | (g >> 2) << 13 | (b >> 3) << 8) & 0xffff)

static cam_device_t camera_cfg = {
    .software_mode = CAM_MANUAL_MODE,
    .frame_mode = CAM_FRAME_INTERLEAVE_MODE,
    .yuv_format = CAM_YUV_FORMAT_YUV422,
    .cam_write_ram_addr = CAMERA_WRITE_ADDR,
    .cam_write_ram_size = CAMERA_BUFFER_SIZE,
    .cam_frame_size = CAMERA_FRAME_SIZE,

    .cam_write_ram_addr1 = CAMERA_WRITE_ADDR1,
    .cam_write_ram_size1 = CAMERA_BUFFER_SIZE1,
    .cam_frame_size1 = CAMERA_FRAME_SIZE,
};

static mjpeg_device_t mjpeg_cfg;

#define RANGE_INT(iVal, iMin, iMax)                    (((iVal) > (iMin)) ? (((iVal) <= (iMax)) ? (iVal) : (iMax)) : (iMin))
#define ROUND_SHR_POSITIVE(Dividend, iShiftRightCount) (((Dividend) & (1 << ((iShiftRightCount)-1))) ? ((Dividend) >> (iShiftRightCount)) + 1 : ((Dividend) >> (iShiftRightCount)))
#define ROUND_SHR_NEGATIVE(Dividend, iShiftRightCount) (-(((-(Dividend)) & (1 << ((iShiftRightCount)-1))) ? ((-(Dividend)) >> (iShiftRightCount)) + 1 : ((-(Dividend)) >> (iShiftRightCount))))
#define ROUND_SHR(Dividend, iShiftRightCount)          (((Dividend) >= 0) ? ROUND_SHR_POSITIVE(Dividend, iShiftRightCount) : ROUND_SHR_NEGATIVE(Dividend, iShiftRightCount))

/**
 * @brief
 *
 * @param Y
 * @param Cb
 * @param Cr
 * @param R
 * @param G
 * @param B
 */
void YCbCrConvertToRGB(int Y, int Cb, int Cr, int *R, int *G, int *B)
{
    int iTmpR = 0;
    int iTmpG = 0;
    int iTmpB = 0;

    iTmpR = (((int)Y) << 14) + 22970 * (((int)Cr) - 128);
    iTmpG = (((int)Y) << 14) - 5638 * (((int)Cb) - 128) - 11700 * (((int)Cr) - 128);
    iTmpB = (((int)Y) << 14) + 29032 * (((int)Cb) - 128);

    iTmpR = ROUND_SHR(iTmpR, 14);
    iTmpG = ROUND_SHR(iTmpG, 14);
    iTmpB = ROUND_SHR(iTmpB, 14);

    *R = (int)RANGE_INT(iTmpR, 0, 255);
    *G = (int)RANGE_INT(iTmpG, 0, 255);
    *B = (int)RANGE_INT(iTmpB, 0, 255);
}

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
static long U[256], V[256], Y1[256], Y2[256];

/**
 * @brief
 *
 */
void init_yuv422p_table(void)
{
    int i;

    // Initialize table
    for (i = 0; i < 256; i++) {
        V[i] = 15938 * i - 2221300;
        U[i] = 20238 * i - 2771300;
        Y1[i] = 11644 * i;
        Y2[i] = 19837 * i - 311710;
    }
}
/**
memory structure
                    w
            +--------------------+
            |Y0Y1Y2Y3...         |
            |...                 |   h
            |...                 |
            |                    |
            +--------------------+
            |U0V0U1V1            |
            |...                 |   h
            |...                 |
            |                    |
            +--------------------+
                w/2
*/

/**
 * @brief
 *
 * @param yuv422sp
 * @param rgb
 * @param width
 * @param height
 */
void yuv422sp_to_rgb24(unsigned char *yuv422sp, unsigned char *rgb, int width, int height)
{
    int y, cb, cr;
    int r, g, b;
    int i = 0;
    unsigned char *p_y;
    unsigned char *p_uv;
    unsigned char *p_rgb;
    static int init_yuv422sp = 0; // just do it once

    p_y = yuv422sp;
    p_uv = p_y + width * height;
    p_rgb = rgb;

    if (init_yuv422sp == 0) {
        init_yuv422p_table();
        init_yuv422sp = 1;
    }

    for (i = 0; i < width * height / 2; i++) {
        y = p_y[0];
        cb = p_uv[0];
        cr = p_uv[1];

        r = MAX(0, MIN(255, (V[cr] + Y1[y]) / 10000));                 //R value
        b = MAX(0, MIN(255, (U[cb] + Y1[y]) / 10000));                 //B value
        g = MAX(0, MIN(255, (Y2[y] - 5094 * (r)-1942 * (b)) / 10000)); //G value

        // default rank：RGB
        p_rgb[0] = b;
        p_rgb[1] = g;
        p_rgb[2] = r;

        y = p_y[1];
        cb = p_uv[0];
        cr = p_uv[1];
        r = MAX(0, MIN(255, (V[cr] + Y1[y]) / 10000));                 //R value
        b = MAX(0, MIN(255, (U[cb] + Y1[y]) / 10000));                 //B value
        g = MAX(0, MIN(255, (Y2[y] - 5094 * (r)-1942 * (b)) / 10000)); //G value

        p_rgb[3] = b;
        p_rgb[4] = g;
        p_rgb[5] = r;

        p_y += 2;
        p_uv += 2;
        p_rgb += 6;
    }
}

/**
 * @brief
 *
 * @param rgb24
 * @param rgb16
 */
void rgb24_to_rgb565(uint8_t *rgb24, uint8_t *rgb16)
{
    int i = 0, j = 0;

    for (i = 0; i < RGB_FRAME_SIZE; i += 3) {
        rgb16[j + 1] = rgb24[i] >> 3;                 // B
        rgb16[j + 1] |= ((rgb24[i + 1] & 0x1C) << 3); // G
        rgb16[j] = rgb24[i + 2] & 0xF8;               // R
        rgb16[j] |= (rgb24[i + 1] >> 5);              // G
        j += 2;
    }
}

/**
 * @brief
 *
 * @return int
 */
int main(void)
{
    uint8_t *picture;
#ifdef USE_YUV422
    uint8_t *rgb_pic = (uint32_t *)(0x26500000);
    uint8_t *rgb16_pic = (uint32_t *)(0x26800000);
#endif
    uint32_t length;
#ifdef TEST_TIM
    uint8_t str[64];
    uint32_t timer_start = 0;
    uint32_t timer_end = 0;
    uint32_t time = 0;
#endif

    bflb_platform_init(0);
    // LCD_Init();
    // LCD_Clear(0);
    if (lcd_init()) {
        MSG("lcd err \r\n");
    }
    lcd_set_dir(0, 0);
    lcd_clear(0xFFFF);
    lcd_auto_swap_set(0);

    bsp_sf_psram_init(1);

    cam_clk_out();

    if (SUCCESS != image_sensor_init(DISABLE, &camera_cfg, &mjpeg_cfg)) {
        MSG("Init error!\n");
        BL_CASE_FAIL;
        while (1) {
        }
    }
    cam_frame_area_t cfg;
    cfg.x0 = 96;
    cfg.x1 = CAMERA_RESOLUTION_X + 96;
    cfg.y0 = 56;
    cfg.y1 = CAMERA_RESOLUTION_Y + 56;

    struct device *cam0 = device_find("camera0");
    device_control(cam0, DEVICE_CTRL_CAM_FRAME_CUT, &cfg);
    device_control(cam0, DEVICE_CTRL_RESUME, NULL);

    // LCD_Set_Addr(0, 0, CAMERA_RESOLUTION_X, CAMERA_RESOLUTION_Y);

    while (1) {
#ifdef TEST_TIM
        timer_start = bflb_platform_get_time_ms();
#endif

        while (SUCCESS != cam_get_one_frame_interleave(&picture, &length)) {
        }

        device_control(cam0, DEVICE_CTRL_SUSPEND, NULL);
#ifdef USE_YUV422
        yuv422sp_to_rgb24(picture, rgb_pic, CAMERA_RESOLUTION_X, CAMERA_RESOLUTION_Y);
        rgb24_to_rgb565(rgb_pic, rgb16_pic);
        // LCD_DrawPicture_cam(0, 0, CAMERA_RESOLUTION_X, CAMERA_RESOLUTION_Y, picture);
#else
        // LCD_WR_SPI_DMA((uint16_t *)picture, (CAMERA_FRAME_SIZE));
        while (ili9341_draw_is_busy()) {
        };
#ifdef TEST_TIM
        sprintf((char *)str, "camera lcd test: time:%ld fps:%ld", time, 1000 / time);
        lcd_set_dir(1, 0);
        lcd_draw_str_ascii16(0, 0, 0x00f8, 0xffff, str, sizeof(str));
        sprintf((char *)str, "camera size:%d x %d", CAMERA_RESOLUTION_X, CAMERA_RESOLUTION_Y);
        lcd_draw_str_ascii16(80, 196, 0x00f8, 0xffff, str, sizeof(str));
#endif
        lcd_set_dir(0, 0);
        lcd_draw_picture_nonblocking(50, 96, CAMERA_RESOLUTION_X + 50 - 1, CAMERA_RESOLUTION_Y + 96 - 1, (uint16_t *)picture);

#endif
        device_control(cam0, DEVICE_CTRL_CAM_FRAME_DROP, NULL);
        device_control(cam0, DEVICE_CTRL_RESUME, NULL);

#ifdef TEST_TIM
        timer_end = bflb_platform_get_time_ms();
        time = timer_end - timer_start;
        // MSG("time:%d,FPS:%d\r\n", (timer_end - timer_start), (1000 / (timer_end - timer_start)));

#endif
    }
}
