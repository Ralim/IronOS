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
#include "hal_spi.h"
#include "hal_gpio.h"
#include "hal_dma.h"

#define TRANSFER_LEN 256
static uint32_t txBuff[TRANSFER_LEN] = { 0 };
static uint32_t rxBuff[TRANSFER_LEN] = { 0 };

struct device *spi;
uint32_t i;
int main(void)
{
    bflb_platform_init(0);
    spi_register(SPI0_INDEX, "spi");
    spi = device_find("spi");

    if (spi) {
        device_open(spi, DEVICE_OFLAG_STREAM_TX | DEVICE_OFLAG_STREAM_RX);
    }

    /* TX buffer init */
    for (i = 0; i < TRANSFER_LEN; i++) {
        txBuff[i] = i * 0x1010101;
    }

    MSG("SPI send and receive data by 32-bit frame size\r\n");
    spi_transmit_receive(spi, txBuff, rxBuff, TRANSFER_LEN, 3);

    /* Check result */
    for (i = 0; i < TRANSFER_LEN; i++) {
        if (rxBuff[i] != txBuff[i]) {
            MSG("Received %08x expect %08x\n", rxBuff[i], txBuff[i]);
            return ERROR;
        }
    }

    BL702_MemSet4(rxBuff, 0, TRANSFER_LEN);

    /* Wait for slave done */
    bflb_platform_delay_ms(100);

    MSG("SPI send and receive data by 24-bit frame size\r\n");
    spi_transmit_receive(spi, txBuff, rxBuff, TRANSFER_LEN, 2);

    /* Check result */
    for (i = 0; i < TRANSFER_LEN; i++) {
        if (rxBuff[i] != (txBuff[i] & 0xffffff)) {
            MSG("Received %08x expect %08x\n", rxBuff[i], txBuff[i] & 0xffffff);
            return ERROR;
        }
    }

    BL702_MemSet4(rxBuff, 0, TRANSFER_LEN);

    /* Wait for slave done */
    bflb_platform_delay_ms(100);

    MSG("SPI send and receive data by 16-bit frame size\r\n");
    spi_transmit_receive(spi, (uint16_t *)txBuff, (uint16_t *)rxBuff, 2 * TRANSFER_LEN, 1);

    /* Check result */
    for (i = 0; i < TRANSFER_LEN; i++) {
        if (rxBuff[i] != txBuff[i]) {
            MSG("Received %08x expect %08x\r\n", rxBuff[i], txBuff[i]);
            return ERROR;
        }
    }

    BL702_MemSet4(rxBuff, 0, TRANSFER_LEN);

    /* Wait for slave done */
    bflb_platform_delay_ms(100);

    MSG("SPI send and receive data by 8-bit frame size\r\n");
    spi_transmit_receive(spi, (uint8_t *)txBuff, (uint8_t *)rxBuff, 4 * TRANSFER_LEN, 0);

    /* Check result */
    for (i = 0; i < TRANSFER_LEN; i++) {
        if (rxBuff[i] != txBuff[i]) {
            MSG("Received %08x expect %08x\r\n", rxBuff[i], txBuff[i]);
            return ERROR;
        }
    }

    /* Disable spi master mode */
    device_close(spi);

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}