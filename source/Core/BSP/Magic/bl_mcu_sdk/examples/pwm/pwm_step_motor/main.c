/**
 * @file pwm_step_motor.c
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
/* 一二相励磁：八拍（A+B+,B+,B+A-,A-,A-B-,B-,B-A+,A+。。。。）
       ___ ___                     ___
CH0           |___ ___ ___ ___ ___|
           ___ ___ ___
CH1    ___|           |___ ___ ___ ___
                   ___ ___ ___
CH2    ___ ___ ___|           |___ ___
                           ___ ___ ___
CH3    ___ ___ ___ ___ ___|

*/

struct device *motor_ch0;
struct device *motor_ch1;
struct device *motor_ch2;
struct device *motor_ch3;

enum motor_dir_type {
    CW,
    CCW,
    STOP
};

void motor_set_dir(enum motor_dir_type dir)
{
    pwm_dutycycle_config_t pwm_cfg[4];

    if (dir == CW) {
        pwm_cfg[0].threshold_low = 2;
        pwm_cfg[0].threshold_high = 7;
        pwm_cfg[1].threshold_low = 1;
        pwm_cfg[1].threshold_high = 4;
        pwm_cfg[2].threshold_low = 3;
        pwm_cfg[2].threshold_high = 6;
        pwm_cfg[3].threshold_low = 5;
        pwm_cfg[3].threshold_high = 8;
    }

    else if (dir == CCW) {
        pwm_cfg[0].threshold_low = 2;
        pwm_cfg[0].threshold_high = 7;
        pwm_cfg[1].threshold_low = 5;
        pwm_cfg[1].threshold_high = 8;
        pwm_cfg[2].threshold_low = 3;
        pwm_cfg[2].threshold_high = 6;
        pwm_cfg[3].threshold_low = 1;
        pwm_cfg[3].threshold_high = 4;
    } else if (dir == STOP) {
        pwm_cfg[0].threshold_low = 0;
        pwm_cfg[0].threshold_high = 0;
        pwm_cfg[1].threshold_low = 0;
        pwm_cfg[1].threshold_high = 0;
        pwm_cfg[2].threshold_low = 0;
        pwm_cfg[2].threshold_high = 0;
        pwm_cfg[3].threshold_low = 0;
        pwm_cfg[3].threshold_high = 0;
    }
    device_control(motor_ch0, DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG, &pwm_cfg[0]);
    device_control(motor_ch1, DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG, &pwm_cfg[1]);
    device_control(motor_ch2, DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG, &pwm_cfg[2]);
    device_control(motor_ch3, DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG, &pwm_cfg[3]);
}

int main(void)
{
    bflb_platform_init(0);

    pwm_register(PWM_CH0_INDEX, "motor_ch0");
    pwm_register(PWM_CH1_INDEX, "motor_ch1");
    pwm_register(PWM_CH2_INDEX, "motor_ch2");
    pwm_register(PWM_CH3_INDEX, "motor_ch3");

    motor_ch0 = device_find("motor_ch0");
    motor_ch1 = device_find("motor_ch1");
    motor_ch2 = device_find("motor_ch2");
    motor_ch3 = device_find("motor_ch3");

    if (motor_ch0) {
        PWM_DEV(motor_ch0)->period = 8; //frequence = 32K/(31+1)/8 = 125hz
        PWM_DEV(motor_ch0)->threshold_low = 2;
        PWM_DEV(motor_ch0)->threshold_high = 7;
        PWM_DEV(motor_ch0)->polarity_invert_mode = ENABLE;
        device_open(motor_ch0, DEVICE_OFLAG_STREAM_TX);
    }
    if (motor_ch1) {
        PWM_DEV(motor_ch1)->period = 8; //frequence = 32K/(31+1)/8 = 125hz
        PWM_DEV(motor_ch1)->threshold_low = 1;
        PWM_DEV(motor_ch1)->threshold_high = 4;
        device_open(motor_ch1, DEVICE_OFLAG_STREAM_TX);
    }
    if (motor_ch2) {
        PWM_DEV(motor_ch2)->period = 8; //frequence = 32K/(31+1)/8 = 125hz
        PWM_DEV(motor_ch2)->threshold_low = 3;
        PWM_DEV(motor_ch2)->threshold_high = 6;
        device_open(motor_ch2, DEVICE_OFLAG_STREAM_TX);
    }
    if (motor_ch3) {
        PWM_DEV(motor_ch3)->period = 8; //frequence = 32K/(31+1)/8 = 125hz
        PWM_DEV(motor_ch3)->threshold_low = 5;
        PWM_DEV(motor_ch3)->threshold_high = 8;
        device_open(motor_ch3, DEVICE_OFLAG_STREAM_TX);
    }
    pwm_channel_start(motor_ch0);
    pwm_channel_start(motor_ch1);
    pwm_channel_start(motor_ch2);
    pwm_channel_start(motor_ch3);

    while (1) {
        motor_set_dir(CW);
        bflb_platform_delay_ms(5000);
        motor_set_dir(CCW);
        bflb_platform_delay_ms(5000);
    }
}
