/**
 * @file xip2046.h
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
#ifndef _XPT2046_H_
#define _XPT2046_H_

#include "bflb_platform.h"

#define TOUCH_PIN_CS GPIO_PIN_11
//#define TOUCH_PIN_PEN GPIO_PIN_11

#define XPT2046_TOUCH_THRESHOLD 500 // Threshold for touch detection

#define XPT2046_AVG_NUM 5

#define TOUCH_ROT_NONE 0
#define TOUCH_ROT_90   1
#define TOUCH_ROT_180  2
#define TOUCH_ROT_270  3

#define XPT2046_X_MIN 310
#define XPT2046_Y_MIN 175
#define XPT2046_X_MAX 3850
#define XPT2046_Y_MAX 3700

extern struct device *touch_spi;
#ifdef __cplusplus
extern "C"{
#endif
void xpt2046_init(void);
uint8_t xpt2046_read(int16_t *x, int16_t *y);
#ifdef __cplusplus
}
#endif
#endif