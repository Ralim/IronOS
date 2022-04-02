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
#include "hal_uart.h"
#include "hal_pm.h"
#include "hal_gpio.h"
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

static void gpio_int_callback(uint32_t pin)
{
    MSG("gpio :%d rising trigger !\r\n", pin);
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
    gpio_attach_irq(GPIO_PIN_10, gpio_int_callback);
    gpio_irq_enable(GPIO_PIN_10, ENABLE); //only used for level3
    while (1) {
        bflb_platform_delay_ms(100);
    }
}

int pds3_enter(int argc, char *argv[])
{
    MSG("gpio wake up case,enter pds3 mode\r\n");
    bflb_platform_delay_ms(50);

    pm_pds_mode_enter(PM_PDS_LEVEL_3, 0); /*hbn、pds、exti gpio can wakeup*/
    BL_CASE_SUCCESS;                      /*level 3 can run here*/
    return 0;
}
int pds31_enter(int argc, char *argv[])
{
    int second = 0;

    if (argc == 2) {
        second = atoi(argv[1]);
    } else {
        second = 0;
    }
    MSG("gpio wake up case,enter pds31 mode\r\n");
    bflb_platform_delay_ms(50);

    pm_pds_mode_enter(PM_PDS_LEVEL_31, second); /*hbn、pds gpio can wakeup*/

    return 0;
}
SHELL_CMD_EXPORT(pds3_enter, pds3 gpio wakeup test)
SHELL_CMD_EXPORT(pds31_enter, pds31 gpio wakeup test)