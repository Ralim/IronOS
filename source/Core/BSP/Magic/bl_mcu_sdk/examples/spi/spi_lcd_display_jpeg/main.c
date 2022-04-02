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
 * Note:
 *
 * This Case is currently based on BL706_AVB development board with ili9341 controller screen.
 * If you use other hardware platforms, please modify the pin Settings by yourself
 *
 * Your should Short connect func1 and func2 jump cap on 706_AVB Board,and Plug the screen into the HD13 interface
 * Then the hardware for lvgl case testing is available.
 *
 *  使用了 romfs 文件系统，需要另外烧入 romfs 文件系统镜像文件，镜像文件在目录下 jpeg_image.bin, 需要烧录到 0x80000 位置
 *  jpeg_image 文件夹下是 romfs文件系统镜像的源文件，jpeg_image.bin 由 jpeg_image 文件夹通过 components/romfs/genromfs 工具生成
 *  关于 romfs 文件系统相关，请查看 components/romfs/genromfs/readme.md 文档
 *  本例程是使用四分之一图片，通过镜像旋转到完整图片
 */
#include "bflb_platform.h"
#include "hal_spi.h"
#include "hal_gpio.h"
#include "hal_dma.h"

#include "mcu_lcd.h"
#include "bl_romfs.h"
#include "tjpgd.h"

/* Session identifier for input/output functions (name, members and usage are as user defined) */
typedef struct {
    romfs_file_t *p_file;
    uint16_t x_start;
    uint16_t y_start;
} IODEV;

#define SZ_WORK (12 * 1024) /* 解码工作区大小,过小会报错 */
uint8_t work_buff[SZ_WORK]; /* 工作区 */

/**
 * @brief tiny jpeg 解压数据输入函数，使用 romfs 文件系统
 *
 * @param jd
 * @param buff
 * @param nbyte
 * @return size_t
 */
size_t in_func(JDEC *jd, uint8_t *buff, size_t nbyte)
{
    IODEV *dev = jd->device;

    if (buff) {
        return romfs_read(dev->p_file, (char *)buff, nbyte);
    } else {
        romfs_lseek(dev->p_file, nbyte, ROMFS_SEEK_CUR);
        return nbyte;
    }
}

/* 定义显示的缓冲区 */
uint8_t flush_buff[2][1 * 1024] __attribute__((aligned(4), section(".tcm_data")));
uint8_t buff_using_flag;

/**
 * @brief tiny jpeg 解压数据输出函数
 *
 * @param jd
 * @param bitmap
 * @param rect
 * @return int
 */
int out_func(JDEC *jd, void *bitmap, JRECT *rect)
{
    uint16_t base_x = ((IODEV *)(jd->device))->x_start;
    uint16_t base_y = ((IODEV *)(jd->device))->y_start;
    uint16_t x1, x2, y1, y2;
    uint16_t w = rect->right - rect->left;
    uint16_t h = rect->bottom - rect->top;
    uint32_t lenth = (w + 1) * (h + 1);
    uint16_t *p_src, *p_dst;

    /* 关闭自动翻转数据大小端 */
    lcd_auto_swap_set(0);
    /* 手动翻转数据大小端 */
    lcd_swap_color_data16(bitmap, bitmap, lenth);

    /* 缓冲区够大，拷贝到缓冲区，非阻塞发送, */
    if (lenth * 2 < sizeof(flush_buff) / 2) {
        /* 1 四分之一原图 */
        x1 = base_x + rect->left;
        x2 = base_x + rect->right;
        y1 = base_y + rect->top;
        y2 = base_y + rect->bottom;
        while (lcd_draw_is_busy()) {
        }
        lcd_draw_picture_nonblocking(x1, y1, x2, y2, (lcd_color_t *)bitmap);

        /* 水平镜像 */
        p_src = (uint16_t *)bitmap;
        p_dst = (uint16_t *)flush_buff[0];
        for (int16_t i = 0; i <= h; i++) {
            for (int16_t j = 0; j <= w; j++) {
                p_dst[j] = p_src[w - j];
            }
            p_src += w + 1;
            p_dst += w + 1;
        }
        x1 = base_x + (jd->width * 2 - rect->right) - 1;
        x2 = base_x + (jd->width * 2 - rect->left) - 1;
        while (lcd_draw_is_busy()) {
        }
        lcd_draw_picture_nonblocking(x1, y1, x2, y2, (lcd_color_t *)flush_buff[0]);

        /* 左垂直镜像 */
        p_src = (uint16_t *)bitmap;
        p_dst = (uint16_t *)flush_buff[1] + (w + 1) * h;
        for (int16_t i = h; i >= 0; i--) {
            for (int16_t j = 0; j <= w; j++) {
                p_dst[j] = p_src[j];
            }
            p_src += w + 1;
            p_dst -= w + 1;
        }
        x1 = base_x + rect->left;
        x2 = base_x + rect->right;
        y1 = base_y + (jd->height * 2 - rect->bottom) - 1;
        y2 = base_y + (jd->height * 2 - rect->top) - 1;
        while (lcd_draw_is_busy()) {
        }
        lcd_draw_picture_nonblocking(x1, y1, x2, y2, (lcd_color_t *)flush_buff[1]);

        /* 右垂直镜像 */
        p_src = (uint16_t *)bitmap;
        p_dst = (uint16_t *)flush_buff[0] + (w + 1) * h;
        for (int16_t i = h; i >= 0; i--) {
            for (int16_t j = 0; j <= w; j++) {
                p_dst[j] = p_src[w - j];
            }
            p_src += w + 1;
            p_dst -= w + 1;
        }
        x1 = base_x + (jd->width * 2 - rect->right) - 1;
        x2 = base_x + (jd->width * 2 - rect->left) - 1;
        y1 = base_y + (jd->height * 2 - rect->bottom) - 1;
        y2 = base_y + (jd->height * 2 - rect->top) - 1;
        while (lcd_draw_is_busy()) {
        }
        lcd_draw_picture_nonblocking(x1, y1, x2, y2, (lcd_color_t *)flush_buff[0]);

    } else {
        /* 缓冲区不够大阻塞发送 */
        MSG("flush_buff too small\r\n");
    }

    /* 恢复自动翻转数据大小端 */
    lcd_auto_swap_set(1);

    return 1;
}

/**
 * @brief
 *
 * @return int
 */
int main(void)
{
    uint64_t time_node_1, time_node_2;
    JRESULT res; /* TJpgDec API f */
    JDEC jdec;   /* jdec obj */

    romfs_file_t img_file;

    IODEV devid = {
        .x_start = 10,
        .y_start = 10,
        .p_file = &img_file,
    }; /* 私有结构体,指针保存到 jdec.device中 */

    char img_path[32] = "/romfs/img_1.jpg";
    uint8_t num = 1;

    uint8_t str[64];

    bflb_platform_init(0);

    if (lcd_init()) {
        MSG("lcd err \r\n");
    }
    lcd_set_dir(2, 0);
    lcd_clear(0x001F);
    // lcd_draw_rectangle(200, 120, 300, 220, 0xffff);
    // lcd_draw_circle(250, 170, 50, 0xffff);
    // lcd_draw_circle(200, 120, 100, 0xffff);

    if (romfs_mount()) {
        MSG("romfs find err\r\n");
        while (1) {
        };
    } else {
        MSG("romfs mount success\r\n");
    }

    romfs_open(&img_file, "/romfs/img_1.jpg", 0);

    time_node_1 = bflb_platform_get_time_ms();

    while (1) {
        /* 工作区分配，注册输入函数，分析 JPEG 信息*/
        res = jd_prepare(&jdec, in_func, work_buff, SZ_WORK, &devid);

        if (res != JDR_OK) {
            MSG("jd_prepare err:%d\r\n", res);
            while (1)
                ;
        }

        /* 注册输出函数，解压 JPEG */
        res = jd_decomp(&jdec, out_func, 0);

        /* 性能计算与图片切换 */
        if (res == JDR_OK) {
            if (++num > 60) {
                num = 1;

                time_node_2 = bflb_platform_get_time_ms();
                uint32_t time = (uint32_t)(time_node_2 - time_node_1);

                sprintf((char *)str, "time:%ld\r\nfps :%ld", time, 1000 * 60 / time);
                MSG("%s", str);

                while (lcd_draw_is_busy()) {
                    __ASM volatile("nop");
                }
                lcd_draw_str_ascii16(5, 180, 0xFFFF, 0x0010, str, 0xff);

                time_node_1 = bflb_platform_get_time_ms();
            }
            romfs_close(&img_file);
            /* open next image */
            sprintf(img_path, "/romfs/img_%d.jpg", num);
            romfs_open(&img_file, img_path, 0);
        } else {
            MSG("jd_decomp err:%d\r\n", res);
        }
    }
}
