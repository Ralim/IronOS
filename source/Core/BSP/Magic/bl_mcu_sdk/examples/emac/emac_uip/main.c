/**
 * @file emac_uip.c
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

#define ETH_RXBUFNB 8
#define ETH_TXBUFNB 4
uint8_t ethRxBuff[ETH_RXBUFNB][ETH_MAX_PACKET_SIZE] ATTR_EALIGN(32) __attribute__((section(".dma_coherent_pool.emac"))) = { 0 }; /* Ethernet Receive Buffers */
uint8_t ethTxBuff[ETH_TXBUFNB][ETH_MAX_PACKET_SIZE] ATTR_EALIGN(32) __attribute__((section(".dma_coherent_pool.emac")));

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

static BL_Err_Type emac_uip_case()
{
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

    emac_bd_init(ethTxBuff, ETH_TXBUFNB, ethRxBuff, ETH_RXBUFNB);

    emac_start();
    cpu_global_irq_enable();

    uip_main();

    return SUCCESS;
}

void emac_tx_error_callback_app()
{
    MSG("EMAC tx error callback\r\n");
}

void emac_rx_error_callback_app()
{
    MSG("EMAC rx error callback\r\n");
}

void emac_rx_busy_callback_app()
{
    MSG("EMAC rx busy callback\r\n");
}

int main(void)
{
    bflb_platform_init(0);
    MSG("EMAC uip test\n");

    emac_uip_case();

    BL_CASE_SUCCESS;
    while (1) {
    }
}
