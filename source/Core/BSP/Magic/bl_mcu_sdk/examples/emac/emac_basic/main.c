/**
 * @file emac_basic.c
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
#include "hal_emac.h"
#include "ethernet_phy.h"

#define EMAC_TEST_INTERVAL       (9)
#define EMAC_TEST_TX_INTERVAL_US (3000)

static const uint8_t test_frame[42] = {
    /* ARP reply to 192.168.31.136(e4:54:e8:ca:31:16): 192.168.31.145 is at 18:b9:05:12:34:56 */
    0x80, 0xFA, 0x5B, 0x74, 0xED, 0x16,                         // dst mac
    0x18, 0xB9, 0x05, 0x12, 0x34, 0x56,                         // src mac
    0x08, 0x06, 0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x02, // arp reply
    0x18, 0xB9, 0x05, 0x12, 0x34, 0x56,                         // src mac
    0xc0, 0xa8, 0x1f, 0x91,                                     // src ip
    0xe4, 0x54, 0xe8, 0xca, 0x31, 0x16,                         // dst mac
    0xc0, 0xa8, 0x1f, 0x88                                      // dst ip
};
#define TEST_PATTERN_LEN (ETH_MAX_PACKET_SIZE - 32)
static uint8_t test_pattern[TEST_PATTERN_LEN] = { 0 };

static volatile uint32_t tx_pkg_cnt = 0;
static volatile uint32_t tx_err_cnt = 0;

static volatile uint32_t rx_pkg_cnt = 0;
static volatile uint32_t rx_err_cnt = 0;

static volatile uint32_t rx_bytes = 0;

static uint32_t time = 0;
static uint32_t last_time = 0;
#define ETH_RXBUFNB 5
#define ETH_TXBUFNB 5
// uint8_t ethRxBuff[ETH_RXBUFNB][ETH_MAX_PACKET_SIZE] __attribute__((aligned(32))) __attribute__((section(".dma_coherent_pool.emac"))) = { 0 }; /* Ethernet Receive Buffers */
// uint8_t ethTxBuff[ETH_TXBUFNB][ETH_MAX_PACKET_SIZE] __attribute__((aligned(32))) __attribute__((section(".dma_coherent_pool.emac")));         /* Ethernet Transmit Buffers */
__attribute__((align(4))) uint8_t ethRxBuff[ETH_RXBUFNB][ETH_MAX_PACKET_SIZE] = { 0 }; /* Ethernet Receive Buffers */
__attribute__((align(4))) uint8_t ethTxBuff[ETH_TXBUFNB][ETH_MAX_PACKET_SIZE] = { 0 }; /* Ethernet Transmit Buffers */

void emac_tx_done_callback_app(void)
{
    tx_pkg_cnt++;
}

void emac_rx_done_callback_app(void)
{
    uint32_t rx_len;
    emac_bd_rx_dequeue(-1, &rx_len, NULL);

    if (rx_len) {
        rx_pkg_cnt++;
        rx_bytes += rx_len;
    }
}

void emac_tx_error_callback_app()
{
    MSG("EMAC tx error callback\r\n");
    tx_err_cnt++;
}

void emac_rx_error_callback_app()
{
    MSG("EMAC rx error callback\r\n");
    rx_err_cnt++;
}

void emac_rx_busy_callback_app()
{
    MSG("EMAC rx busy callback\r\n");
}

static BL_Err_Type emac_basic_case()
{
    uint32_t rx_len = 0;

    emac_device_t emac_cfg = {
        .mac_addr[0] = 0x18,
        .mac_addr[1] = 0xB9,
        .mac_addr[2] = 0x05,
        .mac_addr[3] = 0x12,
        .mac_addr[4] = 0x34,
        .mac_addr[5] = 0x56,
    };

    emac_phy_cfg_t phy_cfg = {
        .auto_negotiation = 1, /*!< Speed and mode auto negotiation */
        .full_duplex = 0,      /*!< Duplex mode */
        .speed = 0,            /*!< Speed mode */
        .phy_address = 1,      /*!< PHY address */
        .phy_id = 0x7c0f0,     /*!< PHY OUI, masked */
        .phy_state = PHY_STATE_DOWN,
    };

    cpu_global_irq_disable();
    /* emac init,configure ethernet peripheral (GPIOs, clocks, MAC, DMA) */
    emac_init(&emac_cfg);

    MSG("EMAC Phy init\r\n");
    emac_phy_init(&phy_cfg);
    if (PHY_STATE_UP == phy_cfg.phy_state) {
        MSG("PHY[%x] @%d ready on %dMbps, %s duplex\n\r", phy_cfg.phy_id, phy_cfg.phy_address,
            phy_cfg.speed,
            phy_cfg.full_duplex ? "full" : "half");
    } else {
        MSG("PHY Init fail\n\r");
        BL_CASE_FAIL;
        while (1)
            ;
    }

    emac_bd_init((uint8_t *)ethTxBuff, ETH_TXBUFNB, (uint8_t *)ethRxBuff, ETH_RXBUFNB);
    MSG("EMAC start\r\n");
    emac_start();
    cpu_global_irq_enable();

    memset(test_pattern, 0x5a, TEST_PATTERN_LEN);
    memcpy(test_pattern, test_frame, sizeof(test_frame));

    while (1) {
        emac_bd_tx_enqueue(-1, sizeof(test_frame), test_pattern);
#if 1
        emac_bd_tx_enqueue(-1, sizeof(test_frame), test_pattern);
        emac_bd_tx_enqueue(-1, sizeof(test_frame), test_pattern);
        emac_bd_rx_dequeue(-1, &rx_len, NULL);
        emac_bd_rx_dequeue(-1, &rx_len, NULL);
#else
        arch_delay_ms(1);
        time = bflb_platform_get_time_ms() / 1000;

        if ((time - last_time) > EMAC_TEST_INTERVAL) {
            last_time = time;
            MSG("tx: %d, err: %d\n\r", tx_pkg_cnt, tx_err_cnt);
            MSG("rx: %d, err: %d, bytes: %d\n\r", rx_pkg_cnt, rx_err_cnt, rx_bytes);
        }
#endif
    }

    return SUCCESS;
}

int main(void)
{
    bflb_platform_init(0);
    MSG("EMAC basic case\n");

    emac_basic_case();

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
