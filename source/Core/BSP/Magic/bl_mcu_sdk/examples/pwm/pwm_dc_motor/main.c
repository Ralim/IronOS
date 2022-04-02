/**
 * @file pwm_dc_motor.c
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
#include "hal_gpio.h"
#include "hal_clock.h"

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
        PWM_DEV(dc_motor_ch0)->period = 32; //frequence = 32K/(0+1)/32 = 1Khz
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

    while (1) {
        pwm_cfg[1].threshold_low = 0;
        pwm_cfg[1].threshold_high = 0;
        device_control(dc_motor_ch1, DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG, &pwm_cfg[1]);
        for (pwm_cfg[0].threshold_high = 16; pwm_cfg[0].threshold_high <= 32; pwm_cfg[0].threshold_high++) {
            device_control(dc_motor_ch0, DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG, &pwm_cfg[0]);
            bflb_platform_delay_ms(1000);
        }

        for (pwm_cfg[0].threshold_high = 32; 16 <= pwm_cfg[0].threshold_high && pwm_cfg[0].threshold_high <= 32; pwm_cfg[0].threshold_high--) {
            device_control(dc_motor_ch0, DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG, &pwm_cfg[0]);
            bflb_platform_delay_ms(1000);
        }

        pwm_cfg[0].threshold_low = 0;
        pwm_cfg[0].threshold_high = 0;
        device_control(dc_motor_ch0, DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG, &pwm_cfg[0]);
        for (pwm_cfg[1].threshold_high = 16; pwm_cfg[1].threshold_high <= 32; pwm_cfg[1].threshold_high++) {
            device_control(dc_motor_ch1, DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG, &pwm_cfg[1]);
            bflb_platform_delay_ms(1000);
        }

        for (pwm_cfg[1].threshold_high = 32; 16 <= pwm_cfg[1].threshold_high && pwm_cfg[1].threshold_high <= 32; pwm_cfg[1].threshold_high--) {
            device_control(dc_motor_ch1, DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG, &pwm_cfg[1]);
            bflb_platform_delay_ms(1000);
        }
    }
}
