/**
 * @file ethernet_phy.h
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
#ifndef __ETHERNET_PHY_H__
#define __ETHERNET_PHY_H__

#include "hal_emac.h"

typedef enum emac_phy_status {
    EMAC_PHY_STAT_EEROR,
    EMAC_PHY_STAT_LINK_DOWN,
    EMAC_PHY_STAT_LINK_INIT,
    EMAC_PHY_STAT_LINK_UP,
    EMAC_PHY_STAT_100MBITS_FULLDUPLEX,
    EMAC_PHY_STAT_100MBITS_HALFDUPLEX,
    EMAC_PHY_STAT_10MBITS_FULLDUPLEX,
    EMAC_PHY_STAT_10MBITS_HALFDUPLEX,
} emac_phy_status_t;

emac_phy_status_t ethernet_phy_status_get();
int emac_phy_init(emac_phy_cfg_t *cfg);

#endif
