/**
 * @file bsp_gc0308.h
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
#ifndef _IMAGE_SENSOR_H_
#define _IMAGE_SENSOR_H_

#include "bflb_platform.h"
#include "bl702_cam.h"
#include "bl702_mjpeg.h"
#include "hal_cam.h"
#include "hal_mjpeg.h"

uint8_t image_sensor_init(BL_Fun_Type mjpeg_en, cam_device_t *cam_cfg, mjpeg_device_t *mjpeg_cfg);
void cam_clk_out(void);
void trigger_init(void);

#endif