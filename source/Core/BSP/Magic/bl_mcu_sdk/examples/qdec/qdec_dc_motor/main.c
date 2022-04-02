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
#include "hal_pwm.h"
#include "hal_qdec.h"
#include "hal_gpio.h"

uint16_t num = 0;
int qdec_val = 0;

static void qdec_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    if (state == QDEC_REPORT_EVENT) {
        num = device_control(dev, DEVICE_CTRL_GET_SAMPLE_VAL, 0);
        if (num & (1 << 15)) {
            qdec_val = -(32768 - (num & 0x7FFF));
            // MSG("QDEC1 :d=%d,v=-%5d\r\n", device_control(dev, DEVICE_CTRL_GET_SAMPLE_DIR, 0), qdec_val);
        } else {
            // MSG("QDEC1 :d=%d,v= %5d\r\n", device_control(dev, DEVICE_CTRL_GET_SAMPLE_DIR, 0), num);
            qdec_val = num;
        }
    }
}

int main(void)
{
    pwm_dutycycle_config_t pwm_cfg[2];

    pwm_cfg[0].threshold_low = 0;
    pwm_cfg[0].threshold_high = 0;

    pwm_cfg[1].threshold_low = 0;
    pwm_cfg[1].threshold_high = 0;

    bflb_platform_init(0);

    pwm_register(PWM_CH0_INDEX, "dc_motor_ch0");
    pwm_register(PWM_CH1_INDEX, "dc_motor_ch1");

    struct device *dc_motor_ch0 = device_find("dc_motor_ch0");
    struct device *dc_motor_ch1 = device_find("dc_motor_ch1");

    if (dc_motor_ch0) {
        PWM_DEV(dc_motor_ch0)->period = 32; //frequence = 32K/1/32 = 1Khz
        PWM_DEV(dc_motor_ch0)->threshold_low = 16;
        PWM_DEV(dc_motor_ch0)->threshold_high = 32;
        device_open(dc_motor_ch0, DEVICE_OFLAG_STREAM_TX);
        pwm_channel_start(dc_motor_ch0);
    }
    if (dc_motor_ch1) {
        PWM_DEV(dc_motor_ch1)->period = 32; //frequence = 32K/1/32 = 1Khz
        PWM_DEV(dc_motor_ch1)->threshold_low = 16;
        PWM_DEV(dc_motor_ch1)->threshold_high = 32;
        device_open(dc_motor_ch1, DEVICE_OFLAG_STREAM_TX);
        pwm_channel_start(dc_motor_ch1);
    }

    qdec_register(QDEC1_INDEX, "qdec_it");

    struct device *qdec_it = device_find("qdec_it");

    if (qdec_it) {
        device_open(qdec_it, DEVICE_OFLAG_INT_RX);
        device_set_callback(qdec_it, qdec_irq_callback);
    } else {
        MSG("device find fail \r\n");
    }

    device_control(qdec_it, DEVICE_CTRL_SET_INT, (void *)QDEC_REPORT_EVENT);
    device_control(qdec_it, DEVICE_CTRL_RESUME, 0);

    while (1) {
        if (qdec_val > 0 && qdec_val <= 300) {
            pwm_cfg[1].threshold_low = 0;
            pwm_cfg[1].threshold_high = 0;
            device_control(dc_motor_ch1, DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG, &pwm_cfg[1]);

            pwm_cfg[0].threshold_high = (qdec_val * 0.1);
            device_control(dc_motor_ch0, DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG, &pwm_cfg[0]);
        } else if (qdec_val < 0 && qdec_val >= -300) {
            pwm_cfg[0].threshold_low = 0;
            pwm_cfg[0].threshold_high = 0;
            device_control(dc_motor_ch0, DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG, &pwm_cfg[0]);

            pwm_cfg[1].threshold_high = -(qdec_val * 0.1);
            device_control(dc_motor_ch1, DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG, &pwm_cfg[1]);
        }
        // MSG("qdec_val:%d\r\n", qdec_val);
        bflb_platform_delay_ms(100);
    }
}
