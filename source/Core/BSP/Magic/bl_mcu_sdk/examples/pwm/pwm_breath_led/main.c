/**
 * @file pwm_breath_led.c
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
    pwm_dutycycle_config_t pwm_cfg = {
        0,
        0
    };
    bflb_platform_init(0);

    gpio_set_mode(GPIO_PIN_29, GPIO_OUTPUT_PP_MODE);
    gpio_set_mode(GPIO_PIN_30, GPIO_OUTPUT_PP_MODE);
    gpio_set_mode(GPIO_PIN_31, GPIO_OUTPUT_PP_MODE);

    gpio_write(GPIO_PIN_29, 0);
    gpio_write(GPIO_PIN_30, 0);
    gpio_write(GPIO_PIN_31, 0);

    pwm_register(PWM_CH2_INDEX, "led_breath");

    struct device *led_breath = device_find("led_breath");

    if (led_breath) {
        PWM_DEV(led_breath)->period = 10; //frequence = 32K/(31+1)/10 = 100HZ
        PWM_DEV(led_breath)->threshold_low = 5;
        PWM_DEV(led_breath)->threshold_high = 10;
        device_open(led_breath, DEVICE_OFLAG_STREAM_TX);
        pwm_channel_start(led_breath);
    }

    while (1) {
        for (pwm_cfg.threshold_high = 0; pwm_cfg.threshold_high <= 32; pwm_cfg.threshold_high++) {
            device_control(led_breath, DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG, &pwm_cfg);
            bflb_platform_delay_ms(50);
        }

        for (pwm_cfg.threshold_high = 32; 0 <= pwm_cfg.threshold_high && pwm_cfg.threshold_high <= 32; pwm_cfg.threshold_high--) {
            device_control(led_breath, DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG, &pwm_cfg);
            bflb_platform_delay_ms(50);
        }
    }
}
