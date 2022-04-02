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
#include "hal_i2c.h"
#include "hal_uart.h"

int main(void)
{
    i2c_msg_t msg1;
    uint8_t buf[2] = { 0 };

    bflb_platform_init(0);

    i2c_register(I2C0_INDEX, "i2c");
    struct device *i2c0 = device_find("i2c");

    if (i2c0) {
        MSG("device find success\r\n");
        device_open(i2c0, 0);
    }

    msg1.buf = buf;
    msg1.flags = SUB_ADDR_1BYTE | I2C_RD;
    msg1.len = 2;
    msg1.slaveaddr = 0xEE >> 1;
    msg1.subaddr = 0xaa;

    if (i2c_transfer(i2c0, &msg1, 1) == 0) {
        MSG("\r\n read:%0x\r\n", msg1.buf[0] << 8 | msg1.buf[1]);
        BL_CASE_SUCCESS;
    } else
        BL_CASE_FAIL;

    while (1) {
        bflb_platform_delay_ms(100);
    }
}