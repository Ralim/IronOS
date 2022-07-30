/**
 * @file hal_wdt.c
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
#include "hal_wdt.h"
#include "bl702_timer.h"

#ifdef BSP_USING_WDT
void WDT_IRQ(void);
#endif

static wdt_device_t wdtx_device[WDT_MAX_INDEX] = {
#ifdef BSP_USING_WDT
    WDT_CONFIG,
#endif
};

int wdt_open(struct device *dev, uint16_t oflag)
{
    // uint32_t tmpval;

    /* watchdog timer disable*/
    // tmpval = BL_RD_REG(TIMER_BASE, TIMER_WMER);
    // BL_WR_REG(TIMER_BASE, TIMER_WMER, BL_CLR_REG_BIT(tmpval, TIMER_WE));
    // // MSG("wdt timeout %d \r\n", wdt_device->wdt_timeout);
    // /* Set watchdog timer match register value */
    // BL_WR_REG(TIMER_BASE, TIMER_WMR, (uint16_t)wdt_device->wdt_timeout);

    if (oflag & DEVICE_OFLAG_INT_TX) {
#ifdef BSP_USING_WDT
        wdt_device_t *wdt_device = (wdt_device_t *)dev;
        // WDT_IntMask(WDT_INT, UNMASK);
        if (wdt_device->id == 0) {
            Interrupt_Handler_Register(TIMER_WDT_IRQn, WDT_IRQ);
        }
#endif
    } else {
        WDT_IntMask(WDT_INT, MASK);
    }

    // /* enable watchdog timer */
    WDT_Enable();
    return 0;
}

int wdt_close(struct device *dev)
{
    // wdt_device_t *wdt_device = (wdt_device_t *)(dev);
    WDT_Disable();
    return 0;
}

int wdt_control(struct device *dev, int cmd, void *args)
{
    // wdt_device_t *wdt_device = (wdt_device_t *)dev;

    switch (cmd) {
        case DEVICE_CTRL_SET_INT: {
            WDT_IntMask(WDT_INT, UNMASK);
            CPU_Interrupt_Pending_Clear(TIMER_WDT_IRQn);
            CPU_Interrupt_Enable(TIMER_WDT_IRQn);
            break;
        }
        case DEVICE_CTRL_CLR_INT: {
            WDT_IntMask(WDT_INT, MASK);
            CPU_Interrupt_Disable(TIMER_WDT_IRQn);
            break;
        }
        case DEVICE_CTRL_CONFIG: {
            break;
        }
        case DEVICE_CTRL_RESUME: {
            WDT_Enable();
            break;
        }
        case DEVICE_CTRL_SUSPEND: {
            WDT_Disable();
            break;
        }
        case DEVICE_CTRL_GET_WDT_COUNTER: {
            return WDT_GetCounterValue();
            break;
        }
        case DEVICE_CTRL_RST_WDT_COUNTER: {
            WDT_ResetCounterValue();
            break;
        }
        case DEVICE_CTRL_GET_RST_STATUS: {
            return WDT_GetResetStatus();
            break;
        }
        case DEVICE_CTRL_CLR_RST_STATUS: {
            WDT_ClearResetStatus();
            break;
        }
        default:
            break;
    }

    return 0;
}

int wdt_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size)
{
    // wdt_device_t *wdt_device = (wdt_device_t *)dev;
    uint16_t wdt_timeout = (uint16_t)(uint32_t)buffer;

    WDT_Disable();
    WDT_SetCompValue(wdt_timeout);
    WDT_Enable();

    return 0;
}

int wdt_register(enum wdt_index_type index, const char *name)
{
    struct device *dev;

    if (WDT_MAX_INDEX == 0) {
        return -DEVICE_EINVAL;
    }

    dev = &(wdtx_device[index].parent);

    dev->open = wdt_open;
    dev->close = wdt_close;
    dev->control = wdt_control;
    dev->write = wdt_write;
    // dev->read = NULL;

    dev->status = DEVICE_UNREGISTER;
    dev->type = DEVICE_CLASS_TIMER;
    dev->handle = NULL;

    return device_register(dev, name);
}

void wdt_isr(wdt_device_t *handle)
{
    uint32_t tmpval;

    if (!handle->parent.callback) {
        return;
    }

    tmpval = BL_RD_REG(TIMER_BASE, TIMER_WICR);
    BL_WR_REG(TIMER_BASE, TIMER_WICR, BL_SET_REG_BIT(tmpval, TIMER_WICLR));

    handle->parent.callback(&handle->parent, NULL, 0, WDT_EVENT);
}

#ifdef BSP_USING_WDT
/**
 * @brief
 *
 */
void WDT_IRQ(void)
{
    wdt_isr(&wdtx_device[WDT_INDEX]);
}
#endif
