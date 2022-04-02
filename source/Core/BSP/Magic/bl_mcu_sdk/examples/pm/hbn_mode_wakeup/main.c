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
#include "hal_pm.h"
#include "hal_acomp.h"
#include "hal_uart.h"
#include "shell.h"

void shell_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    uint8_t data;
    if (state == UART_EVENT_RX_FIFO) {
        for (size_t i = 0; i < size; i++) {
            data = *(uint8_t *)(args + i);
            shell_handler(data);
        }
    }
}

int main(void)
{
    bflb_platform_init(0);
    shell_init();
    struct device *uart = device_find("debug_log");
    if (uart) {
        device_set_callback(uart, shell_irq_callback);
        device_control(uart, DEVICE_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT));
    }

    acomp_device_t acomp_device;
    acomp_device.pos_ch = ACOMP_CHANNEL_ADC_CHANNEL3; /*from gpio11 adc func*/
    acomp_device.neg_ch = ACOMP_CHANNEL_0P375VBAT;
    acomp_device.pos_hysteresis_vol = ACOMP_HYSTERESIS_VOLT_50MV;
    acomp_device.neg_hysteresis_vol = ACOMP_HYSTERESIS_VOLT_50MV;
    acomp_init(0, &acomp_device);

    while (1) {
        bflb_platform_delay_ms(100);
    }
}

int hbn0_enter(int argc, char *argv[])
{
    acomp_enable(0);
    acomp_interrupt_unmask(0,ACOMP_POSITIVE_IT|ACOMP_NEGATIVE_IT);
    bflb_platform_delay_ms(50); /*delay for acomp*/

    for (uint32_t i = 0; i < 30; i++) {
        MSG("status:%d\r\n", acomp_get_result(0));
        bflb_platform_delay_ms(100);
    }

    MSG("acomp wake up case,enter hbn0 mode\r\n");
    bflb_platform_delay_ms(50);

    /** cpu will wakeup when acomp status change
    * please note that if you set gpio9-gpio12 with GPIO_FUN_WAKEUP,it will also wakeup this level
    * rtc can wakeup this level when sleep time do not equal 0
    **/
    pm_hbn_mode_enter(PM_HBN_LEVEL_0, 0);

    return 0;
}

int hbn1_enter(int argc, char *argv[])
{
    int second = 0;

    if (argc == 2) {
        second = atoi(argv[1]);
    } else {
        second = 1;
    }

    MSG("rtc wake up case,enter hbn1 mode\r\n");
    bflb_platform_delay_ms(50);
    /** cpu will wakeup when rtc sleep time timeout
    * please note that if you set gpio9-gpio12 with GPIO_FUN_WAKEUP,it will also wakeup this level
    **/
    pm_hbn_mode_enter(PM_HBN_LEVEL_1, second);

    return 0;
}

int hbn2_enter(int argc, char *argv[])
{
    MSG("gpio wake up case,enter hbn2 mode\r\n");
    bflb_platform_delay_ms(50);

    /*cpu will wakeup when you set gpio9-gpio12 with GPIO_FUN_WAKEUP
    * rtc can not wakeup level2
    **/
    pm_hbn_mode_enter(PM_HBN_LEVEL_2, 0);

    return 0;
}

ATTR_HBN_RAM_SECTION void led_blink(void)
{
    /*do something*/
}

int hbn_run_in_wakeup_addr(int argc, char *argv[])
{
    MSG("gpio wake up case,enter hbn2 mode\r\n");
    bflb_platform_delay_ms(50);

    /*cpu will wakeup when you set gpio9-gpio12 with GPIO_FUN_WAKEUP
    * rtc can not wakeup level2
    **/
    pm_set_wakeup_callback(led_blink); /*cpu will run in wakeup callback not reset when it awakes*/
    pm_hbn_mode_enter(PM_HBN_LEVEL_0, 0);

    return 0;
}

SHELL_CMD_EXPORT(hbn0_enter, hbn0 acomp wakeup test)
SHELL_CMD_EXPORT(hbn1_enter, hbn1 rtc wakeup test)
SHELL_CMD_EXPORT(hbn2_enter, hbn2 gpio wakeup test)
SHELL_CMD_EXPORT(hbn_run_in_wakeup_addr, hbn run in wakeup addr test)