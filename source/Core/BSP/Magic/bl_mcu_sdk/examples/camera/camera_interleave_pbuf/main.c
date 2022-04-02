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

/**
 *
 *          w 480                                            w 480                                  psram  w 480
 *    +-------------------------+               buf1  +----------------------+  ---------┐     +-----------------------+
 *    |Y0Y1...                  |    -------------->  |...                   |  h 20      ↘   |Y0Y1...                |
 *    |...                      |                     |...                   |                 |...                    |
 *    |...                      |               buf2  +----------------------+                 |...                    |
 *    |...                      |    -------------->  |...                   |  h 20           |...                    |
 *    |...                      |  h 360              |...                   |                 |...                    |
 *    |...                      |                     +----------------------+                 |...                    |
 *    |...                      |                        double buffer            processing   |...                    |
 *    |...                      |   __________↗                                                ...
 *    |...                      |   ...
 *    |...                      |   ...
 *    |...                      |
 *    +-------------------------+
 *        camera input data
 *
 * */

#include "bflb_platform.h"
#include "bsp_image_sensor.h"
#include "bsp_sf_psram.h"
#include "hal_spi.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "bsp_il9341.h"

#define CAMERA_RESOLUTION_X (480)
#define CAMERA_RESOLUTION_Y (360)

// #define CAMERA_WRITE_ADDR  (uint32_t)(0x4202B500) // camera raw data write address
// #define CAMERA_BUFFER_SIZE (uint32_t)(0x4B00) // pingpong buf size 480*20*2
// #define CAMERA_FRAME_SIZE  (9600)             // pingpong buffer half size 480*20 ; full image size 480*20*18

#define PSRAM_START_ADDR (0x26000000) // psram write start address

#define YUV422_FRAME_SIZE (CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y * 2)
#define YUV420_FRAME_SIZE (CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y * 2 * 3 / 4)
#define YUV400_FRAME_SIZE (CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y)

// #define CAMERA_FRAME_SIZE (YUV400_FRAME_SIZE / 2)

// pingpong buf
static uint8_t pingpong_buff[2][480 * 20] __attribute__((section(".system_ram"), aligned(4)));

struct device *dma_cam;

static cam_device_t camera_cfg = {
    .software_mode = CAM_AUTO_MODE,
    .frame_mode = CAM_FRAME_INTERLEAVE_MODE,
    .yuv_format = CAM_YUV_FORMAT_YUV400_EVEN,
    .cam_write_ram_addr = (uint32_t)pingpong_buff[0],
    .cam_write_ram_size = sizeof(pingpong_buff),
    .cam_frame_size = sizeof(pingpong_buff[0]),

    .cam_write_ram_addr1 = 0,
    .cam_write_ram_size1 = 0,
    .cam_frame_size1 = 0,
};

static mjpeg_device_t mjpeg_cfg;

// uint32_t tim1, tim2, tim3, tim4;
uint8_t *picture;
uint32_t length;

volatile static uint8_t buff_using_num = 0;
volatile static uint8_t flag_normal = 0;

void my_memcpy(void *dst, void const *src, uint32_t size)
{
    size /= 4;
    for (uint32_t n = 0; n < size; n++) {
        *((uint32_t *)(dst) + n) = *((uint32_t *)(src) + n);
    }
}

void ATTR_TCM_SECTION cam_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    if (flag_normal == 0) {
        // memcpy((void *)(uint32_t)(0x42023000 + (8640 * buff_using_num)), (void *)CAMERA_WRITE_ADDR, CAMERA_FRAME_SIZE);
        // my_memcpy((void *)(uint32_t)(0x26000000 + (CAMERA_FRAME_SIZE * buff_using_num)), (void *)CAMERA_WRITE_ADDR, CAMERA_FRAME_SIZE);
        /*
        if (buff_using_num == 0) {
            tim1 = bflb_platform_get_time_ms();
        }
        */

        while (dma_channel_check_busy(dma_cam)) {
            __asm volatile("nop");
            __asm volatile("nop");
        }

        dma_reload(dma_cam, (uint32_t)pingpong_buff[0], (uint32_t)(PSRAM_START_ADDR + (sizeof(pingpong_buff[0]) * (buff_using_num))), sizeof(pingpong_buff[0]));
        dma_channel_start(dma_cam);

        /*
        if (buff_using_num == 0) {
            tim2 = bflb_platform_get_time_ms();
        }
        */

        flag_normal = 1;
        buff_using_num++;
        return;
    } else if (flag_normal == 1) {
        // if (buff_using_num == (18 * 3) - 1) {
        //     // cam_stop();
        // }
        // cam_stop();
        // my_memcpy((void *)(uint32_t)(0x26000000 + (CAMERA_FRAME_SIZE * buff_using_num)), (void *)(CAMERA_WRITE_ADDR + CAMERA_FRAME_SIZE), CAMERA_FRAME_SIZE);
        // memcpy((void *)(uint32_t)(0x42023000 + (8640 * buff_using_num)), (void *)(CAMERA_WRITE_ADDR + CAMERA_FRAME_SIZE), CAMERA_FRAME_SIZE);

        // cam_start();
        /*
        if (buff_using_num == 1) {
            tim3 = bflb_platform_get_time_ms();
        }
        */

        while (dma_channel_check_busy(dma_cam)) {
            __asm volatile("nop");
            __asm volatile("nop");
        }

        dma_reload(dma_cam, (uint32_t)pingpong_buff[1], (uint32_t)(PSRAM_START_ADDR + (sizeof(pingpong_buff[0]) * (buff_using_num))), sizeof(pingpong_buff[0]));
        dma_channel_start(dma_cam);

        /*
        if (buff_using_num == 1) {
            tim4 = bflb_platform_get_time_ms();
        }
        */

        flag_normal = 0;
        buff_using_num++;

        return;
    }

    return;
}

int ATTR_TCM_SECTION main(void)
{
    bflb_platform_init(0);
    bsp_sf_psram_init(1);

    struct device *uart0 = device_find("debug_log");

    dma_register(DMA0_CH0_INDEX, "dma_cam");

    dma_cam = device_find("dma_cam");

    if (dma_cam) {
        device_open(dma_cam, 0);
    }
    // MSG("dma open \r\n");

    cam_clk_out();

    if (SUCCESS != image_sensor_init(DISABLE, &camera_cfg, &mjpeg_cfg)) {
        MSG("Init error!\n");
        BL_CASE_FAIL;
        while (1) {
        }
    }

    cam_frame_area_t cfg;
    cfg.x0 = 0;
    cfg.x1 = CAMERA_RESOLUTION_X;
    cfg.y0 = 0;
    cfg.y1 = CAMERA_RESOLUTION_Y;

    struct device *cam0 = device_find("camera0");
    device_control(cam0, DEVICE_CTRL_CAM_FRAME_WRAP, (void *)0);
    device_control(cam0, DEVICE_CTRL_CAM_FRAME_CUT, &cfg);
    device_set_callback(cam0, cam_irq_callback);
    device_control(cam0, DEVICE_CTRL_SET_INT, (void *)CAM_FRAME_IT);
    device_control(cam0, DEVICE_CTRL_RESUME, NULL);

    while (1) {
        if (buff_using_num == 18 * 3) { // close cam when 3 pic saved,480x360 = 480x20x18
            device_control(cam0, DEVICE_CTRL_SUSPEND, NULL);

            //device_write(uart0, 0, (void *)(uint32_t *)PSRAM_START_ADDR, YUV400_FRAME_SIZE * 3); // uart log send 3 pic raw data

            // MSG("\r\ntim1:%d, tim2:%d, tim3:%d, tim4:%d", tim1, tim2, tim3, tim4);
            return 0;
        }
        bflb_platform_delay_ms(1);
    }

    // BL_CASE_SUCCESS;
}
