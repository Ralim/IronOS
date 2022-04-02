/**
 * @file hal_cam.h
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

#ifndef __HAL_CAM_H__
#define __HAL_CAM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_common.h"
#include "drv_device.h"
#include "bl702_config.h"

#define DEVICE_CTRL_CAM_FRAME_CUT  0x10
#define DEVICE_CTRL_CAM_FRAME_DROP 0x11
#define DEVICE_CTRL_CAM_FRAME_WRAP 0x12

enum cam_index_type {
#ifdef BSP_USING_CAM0
    CAM0_INDEX,
#endif
    CAM_MAX_INDEX
};

#define CAM_AUTO_MODE   0
#define CAM_MANUAL_MODE 1

#define CAM_FRAME_PLANAR_MODE     0
#define CAM_FRAME_INTERLEAVE_MODE 1

#define CAM_HSPOLARITY_LOW  0
#define CAM_HSPOLARITY_HIGH 1

#define CAM_VSPOLARITY_LOW  0
#define CAM_VSPOLARITY_HIGH 1

#define CAM_YUV_FORMAT_YUV422      0
#define CAM_YUV_FORMAT_YUV420_EVEN 1
#define CAM_YUV_FORMAT_YUV420_ODD  2
#define CAM_YUV_FORMAT_YUV400_EVEN 3
#define CAM_YUV_FORMAT_YUV400_ODD  4

enum cam_it_type {
    CAM_FRAME_IT = 1 << 0,
};

enum cam_event_type {
    CAM_EVENT_FRAME = 0,
};

typedef struct {
    int16_t x0;
    int16_t x1;
    int16_t y0;
    int16_t y1;
} cam_frame_area_t;

typedef struct {
    uint32_t frame_addr;
    uint32_t frame_count;
} cam_frame_info_t;

typedef struct cam_device {
    struct device parent;
    uint8_t id;
    uint8_t software_mode;
    uint8_t frame_mode;
    uint8_t yuv_format;
    uint8_t hsp;
    uint8_t vsp;
    uint32_t cam_write_ram_addr;
    uint32_t cam_write_ram_size;
    uint32_t cam_frame_size;

    // planar mode need use:
    uint32_t cam_write_ram_addr1;
    uint32_t cam_write_ram_size1;
    uint32_t cam_frame_size1;
} cam_device_t;

#define CAM_DEV(dev) ((cam_device_t *)dev)

int cam_register(enum cam_index_type index, const char *name);
void cam_drop_one_frame_interleave(void);
uint8_t cam_get_one_frame_interleave(uint8_t **pic, uint32_t *len);

#ifdef __cplusplus
}
#endif

#endif
