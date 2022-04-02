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

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}

void hellowd()
{
    MSG("hello World\r\n");
}

int echo(int argc, char *argv[])
{
    MSG("%dparameter(s)\r\n", argc);

    for (uint8_t i = 1; i < argc; i++) {
        MSG("%s\r\n", argv[i]);
    }

    return 0;
}

SHELL_CMD_EXPORT(hellowd, hellowd test)
SHELL_CMD_EXPORT(echo, echo test)