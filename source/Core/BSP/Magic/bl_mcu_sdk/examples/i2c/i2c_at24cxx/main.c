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
    i2c_msg_t msg[2];
    uint8_t buf1[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    uint8_t buf2[8] = { 0 };

    bflb_platform_init(0);

    i2c_register(I2C0_INDEX, "i2c");
    struct device *i2c0 = device_find("i2c");

    if (i2c0) {
        MSG("device find success\r\n");
        device_open(i2c0, 0);
    }

    msg[0].buf = buf1;
    msg[0].flags = SUB_ADDR_2BYTE | I2C_WR;
    msg[0].len = 8;
    msg[0].slaveaddr = 0x50;
    msg[0].subaddr = 0x00;

    msg[1].buf = buf2;
    msg[1].flags = SUB_ADDR_2BYTE | I2C_RD;
    msg[1].len = 8;
    msg[1].slaveaddr = 0x50;
    msg[1].subaddr = 0x00;

    if (i2c_transfer(i2c0, &msg[0], 1) == 1) {
        BL_CASE_FAIL;
        while (1) {
        }
    }
    bflb_platform_delay_ms(10);
    if (i2c_transfer(i2c0, &msg[1], 1) == 1) {
        BL_CASE_FAIL;
        while (1) {
        }
    }
    for (uint8_t i = 0; i < 8; i++) {
        if (buf1[i] != buf2[i]) {
            BL_CASE_FAIL;
            while (1) {
            }
        }
    }
    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}