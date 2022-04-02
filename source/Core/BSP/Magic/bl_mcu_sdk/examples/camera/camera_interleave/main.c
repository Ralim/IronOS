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
#include "bsp_image_sensor.h"
#include "bl702_uart.h"

#define CAMERA_RESOLUTION_X (120)
#define CAMERA_RESOLUTION_Y (100)
#define CAMERA_FRAME_SIZE   (CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y * 2)
#define CAMERA_WRITE_ADDR   (0x4201E000)
#define CAMERA_BUFFER_SIZE  (0x12000)

#define YUV422_FRAME_SIZE (CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y * 2)
#define YUV420_FRAME_SIZE (CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y * 2 * 3 / 4)
#define YUV400_FRAME_SIZE (CAMERA_RESOLUTION_X * CAMERA_RESOLUTION_Y)
#define YUV_USE           (1)

static cam_device_t camera_cfg = {
    .software_mode = CAM_MANUAL_MODE,
    .frame_mode = CAM_FRAME_INTERLEAVE_MODE,
    .yuv_format = CAM_YUV_FORMAT_YUV422,
    .cam_write_ram_addr = CAMERA_WRITE_ADDR,
    .cam_write_ram_size = CAMERA_BUFFER_SIZE,
    .cam_frame_size = CAMERA_FRAME_SIZE,

    .cam_write_ram_addr1 = 0,
    .cam_write_ram_size1 = 0,
    .cam_frame_size1 = 0,
};

static mjpeg_device_t mjpeg_cfg;

int main(void)
{
    uint32_t i;
    uint8_t *picture;
    uint32_t length;

    bflb_platform_init(0);

    MSG("camera case in interleave mode in ocram \n");

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
    device_control(cam0, DEVICE_CTRL_CAM_FRAME_CUT, &cfg);
    device_control(cam0, DEVICE_CTRL_RESUME, NULL);

    for (i = 0; i < 3; i++) {
        while (SUCCESS != cam_get_one_frame_interleave(&picture, &length)) {
        }

        MSG("picturestartuyvy");
        UART_SendData(0, picture, length);
        MSG("pictureend");
        device_control(cam0, DEVICE_CTRL_CAM_FRAME_DROP, NULL);
    }

    device_control(cam0, DEVICE_CTRL_SUSPEND, NULL);

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
