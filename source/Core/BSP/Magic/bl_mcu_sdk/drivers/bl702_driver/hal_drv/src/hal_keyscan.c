/**
 * @file hal_keyscan.c
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
#include "hal_keyscan.h"
#include "kys_reg.h"
#include "bl702_glb.h"

#ifdef BSP_USING_KEYSCAN
static void KeyScan_IRQ(void);
#endif

static keyscan_device_t keyscan_device[KEYSCAN_MAX_INDEX] = {
#ifdef BSP_USING_KEYSCAN
    KEYSCAN_CONFIG
#endif
};

/**
 * @brief
 *
 * @param dev
 * @param oflag
 * @return int
 */
int keyscan_open(struct device *dev, uint16_t oflag)
{
    uint32_t tmpVal;
    keyscan_device_t *keyscan_device = (keyscan_device_t *)dev;

    tmpVal = BL_RD_REG(KYS_BASE, KYS_KS_CTRL);
    /* Set col and row */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_COL_NUM, keyscan_device->col_num - 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_ROW_NUM, keyscan_device->row_num - 1);

    /* Set idle duration between column scans */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_RC_EXT, 0);

    /* ghost key event detection not support*/
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_GHOST_EN, 0);

    if (keyscan_device->deglitch_count) {
        /* Enable or disable deglitch function */
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_DEG_EN, 1);

        /* Set deglitch count */
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_DEG_CNT, keyscan_device->deglitch_count);
    } else {
        /* Enable or disable deglitch function */
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_DEG_EN, 0);

        /* Set deglitch count */
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, KYS_DEG_CNT, 0);
    }

    /* Write back */
    BL_WR_REG(KYS_BASE, KYS_KS_CTRL, tmpVal);

    return 0;
}

int keyscan_control(struct device *dev, int cmd, void *args)
{
    switch (cmd) {
        case DEVICE_CTRL_SET_INT /* constant-expression */:
#ifdef BSP_USING_KEYSCAN
            Interrupt_Handler_Register(KYS_IRQn, KeyScan_IRQ);
#endif
            BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, 1);
            CPU_Interrupt_Enable(KYS_IRQn);
            break;
        case DEVICE_CTRL_CLR_INT /* constant-expression */:
            Interrupt_Handler_Register(KYS_IRQn, NULL);
            BL_WR_REG(KYS_BASE, KYS_KS_INT_EN, 0);
            CPU_Interrupt_Disable(KYS_IRQn);
            break;
        case DEVICE_CTRL_GET_INT:
            return (BL_RD_REG(KYS_BASE, KYS_KS_INT_STS) & 0xf);
        case DEVICE_CTRL_RESUME: {
            uint32_t tmpVal;

            tmpVal = BL_RD_REG(KYS_BASE, KYS_KS_CTRL);
            BL_WR_REG(KYS_BASE, KYS_KS_CTRL, BL_SET_REG_BIT(tmpVal, KYS_KS_EN));
        } break;
        case DEVICE_CTRL_SUSPEND: {
            uint32_t tmpVal;

            tmpVal = BL_RD_REG(KYS_BASE, KYS_KS_CTRL);
            BL_WR_REG(KYS_BASE, KYS_KS_CTRL, BL_CLR_REG_BIT(tmpVal, KYS_KS_EN));
        } break;
        case DEVICE_CTRL_KEYSCAN_GET_KEYCODE: {
            uint32_t *key_code = (uint32_t *)args;
            *key_code = BL_RD_REG(KYS_BASE, KYS_KEYCODE_VALUE);
            BL_WR_REG(KYS_BASE, KYS_KEYCODE_CLR, 0xf);
        } break;
        default:
            break;
    }

    return 0;
}
/**
 * @brief
 *
 * @param index
 * @param name
 * @param flag
 * @return int
 */
int keyscan_register(enum keyscan_index_type index, const char *name)
{
    struct device *dev;

    if (KEYSCAN_MAX_INDEX == 0) {
        return -DEVICE_EINVAL;
    }

    dev = &(keyscan_device[index].parent);

    dev->open = keyscan_open;
    dev->close = NULL;
    dev->control = keyscan_control;
    dev->write = NULL;
    dev->read = NULL;

    dev->type = DEVICE_CLASS_KEYSCAN;
    dev->handle = NULL;

    return device_register(dev, name);
}

#if defined(BSP_USING_KEYSCAN)
static void KeyScan_IRQ(void)
{
    if (keyscan_device[KEYSCAN_INDEX].parent.callback) {
        keyscan_device[KEYSCAN_INDEX].parent.callback(&keyscan_device[KEYSCAN_INDEX].parent, (void *)(BL_RD_REG(KYS_BASE, KYS_KEYCODE_VALUE)), 0, KEYSCAN_EVENT_TRIG);
    }

    BL_WR_REG(KYS_BASE, KYS_KEYCODE_CLR, 0xf);
}
#endif