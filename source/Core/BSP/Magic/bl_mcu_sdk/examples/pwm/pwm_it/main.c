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

static void pwm_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    if (state == PWM_EVENT_COMPLETE) {
        pwm_channel_stop(dev);
        MSG("pwm complete\r\n");
    }
}
int main(void)
{
    bflb_platform_init(0);

    pwm_register(PWM_CH2_INDEX, "pwm");

    struct device *pwm = device_find("pwm");

    if (pwm) {
        PWM_DEV(pwm)->period = 10; //frequence = 32k/(31+1)/10 = 100hz
        PWM_DEV(pwm)->threshold_low = 5;
        PWM_DEV(pwm)->threshold_high = 10;
        PWM_DEV(pwm)->it_pulse_count = 200; //after 2s go isr 2s = 1/100hz*200
        device_open(pwm, DEVICE_OFLAG_INT_TX);
        device_set_callback(pwm, pwm_irq_callback);
        //pwm_it_pulse_count_update(pwm, 200);
        pwm_channel_start(pwm);
    }

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
