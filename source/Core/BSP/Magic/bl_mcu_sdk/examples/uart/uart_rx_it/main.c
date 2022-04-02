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

void uart_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    uint8_t i = 0;

    if (state == UART_EVENT_RX_FIFO) {
        MSG("rf\r\n");

        for (i = 0; i < size; i++) {
            MSG("0x%x ", (*(uint8_t *)(args + i)));
        }

        MSG("\r\n");
    } else if (state == UART_EVENT_RTO) {
        MSG("rto\r\n");

        for (i = 0; i < size; i++) {
            MSG("0x%x ", (*(uint8_t *)(args + i)));
        }

        MSG("\r\n");
    } else {
    }
}

const char *string = "uart1 poll tx and rx irq test\r\n";

int main(void)
{
    bflb_platform_init(0);
    uart_register(UART1_INDEX, "uart1");
    struct device *uart1 = device_find("uart1");

    if (uart1) {
        device_open(uart1, DEVICE_OFLAG_STREAM_TX | DEVICE_OFLAG_INT_RX);
        device_set_callback(uart1, uart_irq_callback);
        device_control(uart1, DEVICE_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT | UART_RTO_IT));
    }

    while (1) {
        device_write(uart1, 0, string, strlen(string));
        bflb_platform_delay_ms(1000);
    }
}