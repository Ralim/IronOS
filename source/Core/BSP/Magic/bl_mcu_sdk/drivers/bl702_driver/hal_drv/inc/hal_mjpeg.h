/**
 * @file hal_mjpeg.h
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
#ifndef __HAL_MJPEG__H__
#define __HAL_MJPEG__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "drv_device.h"
#include "bl702_config.h"

#define MJPEG_PACKET_ADD_NONE       0
#define MJPEG_PACKET_ADD_DEFAULT    1 << 0
#define MJPEG_PACKET_ADD_FRAME_HEAD 1 << 1
#define MJPEG_PACKET_ADD_FRAME_TAIL 1 << 2
#define MJPEG_PACKET_ADD_END_TAIL   1 << 3

/**
 *  @brief MJPEG YUV format definition
 */
typedef enum {
    MJPEG_YUV_FORMAT_YUV420,            /*!< MJPEG YUV420 planar mode */
    MJPEG_YUV_FORMAT_YUV400,            /*!< MJPEG YUV400 grey scale mode */
    MJPEG_YUV_FORMAT_YUV422_PLANAR,     /*!< MJPEG YUV422 planar mode */
    MJPEG_YUV_FORMAT_YUV422_INTERLEAVE, /*!< MJPEG YUV422 interleave mode */
} mjpeg_yuv_format_t;

typedef struct mjpeg_device {
    struct device parent;
    uint8_t quality;
    mjpeg_yuv_format_t yuv_format;
    uint32_t write_buffer_addr; /*!< MJPEG buffer addr */
    uint32_t write_buffer_size; /*!< MJPEG buffer size */
    uint32_t read_buffer_addr;
    uint32_t read_buffer_size;
    uint16_t resolution_x; /*!< CAM RESOLUTION X */
    uint16_t resolution_y; /*!< CAM RESOLUTION Y */

    uint8_t packet_cut_mode;
    uint16_t frame_head_length;
    uint16_t packet_head_length;
    uint16_t packet_body_length;
    uint16_t packet_tail_length;
} mjpeg_device_t;

void mjpeg_init(mjpeg_device_t *mjpeg_cfg);
void mjpeg_start(void);
void mjpeg_stop(void);
uint8_t mjpeg_get_one_frame(uint8_t **pic, uint32_t *len, uint8_t *q);
void mjpeg_drop_one_frame(void);

#ifdef __cplusplus
}
#endif
#endif