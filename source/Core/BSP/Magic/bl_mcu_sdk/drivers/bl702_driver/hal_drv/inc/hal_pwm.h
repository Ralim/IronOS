/**
 * @file hal_pwm.h
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
#ifndef __HAL_PWM__H__
#define __HAL_PWM__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "drv_device.h"
#include "bl702_config.h"

#define DEVICE_CTRL_PWM_FREQUENCE_CONFIG      0x10
#define DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG      0x11
#define DEVICE_CTRL_PWM_IT_PULSE_COUNT_CONFIG 0x12

enum pwm_index_type {
#ifdef BSP_USING_PWM_CH0
    PWM_CH0_INDEX,
#endif
#ifdef BSP_USING_PWM_CH1
    PWM_CH1_INDEX,
#endif
#ifdef BSP_USING_PWM_CH2
    PWM_CH2_INDEX,
#endif
#ifdef BSP_USING_PWM_CH3
    PWM_CH3_INDEX,
#endif
#ifdef BSP_USING_PWM_CH4
    PWM_CH4_INDEX,
#endif
    PWM_MAX_INDEX
};

#define pwm_channel_start(dev)                 device_control(dev, DEVICE_CTRL_RESUME, NULL)
#define pwm_channel_stop(dev)                  device_control(dev, DEVICE_CTRL_SUSPEND, NULL)
#define pwm_channel_freq_update(dev, count)    device_control(dev, DEVICE_CTRL_PWM_FREQUENCE_CONFIG, (void *)count)
#define pwm_channel_dutycycle_update(dev, cfg) device_control(dev, DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG, cfg)
#define pwm_it_pulse_count_update(dev, count)  device_control(dev, DEVICE_CTRL_PWM_IT_PULSE_COUNT_CONFIG, (void *)count)

enum pwm_event_type {
    PWM_EVENT_COMPLETE,
};

typedef struct
{
    uint16_t threshold_low;
    uint16_t threshold_high;
} pwm_dutycycle_config_t;

typedef struct pwm_device {
    struct device parent;
    uint8_t ch;
    uint8_t polarity_invert_mode;
    uint16_t period;
    uint16_t threshold_low;
    uint16_t threshold_high;
    uint16_t it_pulse_count;

} pwm_device_t;

#define PWM_DEV(dev) ((pwm_device_t *)dev)

int pwm_register(enum pwm_index_type index, const char *name);
#ifdef __cplusplus
}
#endif
#endif
