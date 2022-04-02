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

static uint8_t buffer[100];

int main(void)
{
    bflb_platform_init(0);
    uart_register(UART1_INDEX, "uart1");
    struct device *uart1 = device_find("uart1");

    if (uart1) {
        device_open(uart1, DEVICE_OFLAG_STREAM_TX | DEVICE_OFLAG_STREAM_RX);
        MSG("device find success\r\n");
    }

    device_read(uart1, 0, buffer, 10);
    device_write(uart1, 0, buffer, 10);

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}