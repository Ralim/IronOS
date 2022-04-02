/**
 * @file hal_spi.h
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
#ifndef __HAL_SPI__H__
#define __HAL_SPI__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_common.h"
#include "drv_device.h"
#include "bl702_config.h"

#define SPI_FIFO_LEN 4

#define DEVICE_CTRL_SPI_CONFIG_CLOCK 0x10

#define DEVICE_CTRL_SPI_GET_TX_FIFO   0x11
#define DEVICE_CTRL_SPI_GET_RX_FIFO   0x12
#define DEVICE_CTRL_SPI_CLEAR_TX_FIFO 0x13
#define DEVICE_CTRL_SPI_CLEAR_RX_FIFO 0x14

#define DEVICE_CTRL_SPI_GET_BUS_BUSY_STATUS 0x15

enum spi_index_type {
#ifdef BSP_USING_SPI0
    SPI0_INDEX,
#endif
    SPI_MAX_INDEX
};

/** @defgroup SPI_Direction SPI Direction Mode
  * @{
  */
#define SPI_LSB_BYTE0_DIRECTION_FIRST 0
#define SPI_LSB_BYTE3_DIRECTION_FIRST 1
#define SPI_MSB_BYTE0_DIRECTION_FIRST 2
#define SPI_MSB_BYTE3_DIRECTION_FIRST 3

/** @defgroup SPI_Data_Size SPI Data Size
  * @{
  */
#define SPI_DATASIZE_8BIT  0
#define SPI_DATASIZE_16BIT 1
#define SPI_DATASIZE_24BIT 2
#define SPI_DATASIZE_32BIT 3

/** @defgroup SPI_Clock_Polarity SPI Clock Polarity
  * @{
  */
#define SPI_POLARITY_LOW  0
#define SPI_POLARITY_HIGH 1

/** @defgroup SPI_Clock_Phase SPI Clock Phase
  * @{
  */
#define SPI_PHASE_1EDGE 0
#define SPI_PHASE_2EDGE 1

/** @defgroup
  * @{
  */
#define SPI_SLVAE_MODE  0
#define SPI_MASTER_MODE 1

#define SPI_TRANSFER_TYPE_8BIT  0
#define SPI_TRANSFER_TYPE_16BIT 1
#define SPI_TRANSFER_TPYE_24BIT 2
#define SPI_TRANSFER_TYPE_32BIT 3

enum spi_event_type {
    SPI_EVENT_TX_FIFO,
    SPI_EVENT_RX_FIFO,
    SPI_EVENT_UNKNOWN
};

typedef struct spi_device {
    struct device parent;
    uint8_t id;
    uint32_t clk;
    uint8_t mode;
    uint8_t direction;
    uint8_t clk_polaraity;
    uint8_t clk_phase;
    uint8_t datasize;
    uint8_t fifo_threshold;
    uint8_t pin_swap_enable; /*swap mosi and miso*/
    uint8_t delitch_cnt;
    void *tx_dma;
    void *rx_dma;
} spi_device_t;

#define SPI_DEV(dev) ((spi_device_t *)dev)

int spi_register(enum spi_index_type index, const char *name);

int spi_transmit(struct device *dev, void *buffer, uint32_t size, uint8_t type);
int spi_receive(struct device *dev, void *buffer, uint32_t size, uint8_t type);
int spi_transmit_receive(struct device *dev, const void *send_buf, void *recv_buf, uint32_t length, uint8_t type);
#ifdef __cplusplus
}
#endif
#endif