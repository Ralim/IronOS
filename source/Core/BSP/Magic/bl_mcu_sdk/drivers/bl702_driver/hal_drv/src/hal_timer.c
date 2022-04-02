/**
 * @file hal_timer.c
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
#include "hal_timer.h"
#include "hal_clock.h"
#include "bl702_glb.h"
#include "bl702_timer.h"

#ifdef BSP_USING_TIMER0
void TIMER0_IRQ(void);
#endif
#ifdef BSP_USING_TIMER1
void TIMER1_IRQ(void);
#endif

static timer_device_t timerx_device[TIMER_MAX_INDEX] = {
#ifdef BSP_USING_TIMER0
    TIMER0_CONFIG,
#endif
#ifdef BSP_USING_TIMER1
    TIMER1_CONFIG,
#endif
};

/**
 * @brief
 *
 * @param dev
 * @param oflag
 * @return int
 */
int timer_open(struct device *dev, uint16_t oflag)
{
    timer_device_t *timer_device = (timer_device_t *)dev;

    uint32_t tmpval;
    uint32_t compare_count1 = 0;
    uint32_t compare_count2 = 0;
    uint32_t compare_count3 = 0;
    uint32_t reload_val = 0;
    uint32_t clkval = 0;
    uint32_t unit = 0;

    /* Disable all interrupt */
    TIMER_IntMask(timer_device->id, TIMER_INT_ALL, MASK);
    /* Disable timer before config */
    TIMER_Disable(timer_device->id);

    /* Configure timer count mode: preload or free run */
    tmpval = BL_RD_WORD(TIMER_BASE + TIMER_TCMR_OFFSET);
    tmpval &= (~(1 << (timer_device->id + 1)));
    tmpval |= (timer_device->cnt_mode << (timer_device->id + 1));

    BL_WR_WORD(TIMER_BASE + TIMER_TCMR_OFFSET, tmpval);

    /* Configure timer preload trigger source */
    BL_WR_WORD(TIMER_BASE + TIMER_TPLCR2_OFFSET + 4 * timer_device->id, timer_device->trigger);

    if (timer_device->cnt_mode == TIMER_CNT_PRELOAD) {
        BL_WR_WORD(TIMER_BASE + TIMER_TPLVR2_OFFSET + 4 * timer_device->id, timer_device->reload);
        reload_val = timer_device->reload;
    }

    if (timer_device->id == TIMER_CH0) {
        clkval = peripheral_clock_get(PERIPHERAL_CLOCK_TIMER0);
    } else {
        clkval = peripheral_clock_get(PERIPHERAL_CLOCK_TIMER1);
    }

    if (clkval % 1000000 == 0) {
        unit = 1000000; //1us
    } else if (clkval % 100000 == 0) {
        unit = 100000; //10us
    } else if (clkval % 10000 == 0) {
        unit = 10000; //100us
    } else if (clkval % 1000 == 0) {
        unit = 1000; //1ms
    } else if (clkval % 100 == 0) {
        unit = 100; //10ms
    } else if (clkval % 10 == 0) {
        unit = 10; //100ms
    } else if (clkval % 1 == 0) {
        unit = 1; //s
    } else {
    }

    compare_count1 = timer_device->timeout1 / (1000000 / unit) * (clkval / unit) + reload_val;
    compare_count2 = timer_device->timeout2 / (1000000 / unit) * (clkval / unit) + reload_val;
    compare_count3 = timer_device->timeout3 / (1000000 / unit) * (clkval / unit) + reload_val;

    /* Configure match compare values */
    if ((compare_count1 < 1) || (compare_count2 < 1) || (compare_count3 < 1)) {
        return -1;
    }

    TIMER_SetCompValue(timer_device->id, TIMER_COMP_ID_0, compare_count1 - 2);
    TIMER_SetCompValue(timer_device->id, TIMER_COMP_ID_1, compare_count2 - 2);
    TIMER_SetCompValue(timer_device->id, TIMER_COMP_ID_2, compare_count3 - 2);

    /* Clear interrupt status*/
    TIMER_ClearIntStatus(timer_device->id, TIMER_COMP_ID_0);
    TIMER_ClearIntStatus(timer_device->id, TIMER_COMP_ID_1);
    TIMER_ClearIntStatus(timer_device->id, TIMER_COMP_ID_2);

    if (oflag & DEVICE_OFLAG_STREAM_TX) {
        /* Enable timer match interrupt */
        /* Note: if not enable match interrupt, TIMER_GetMatchStatus will not work
      and status bit will not set */
        TIMER_IntMask(timer_device->id, TIMER_INT_COMP_0, UNMASK);
        TIMER_IntMask(timer_device->id, TIMER_INT_COMP_1, UNMASK);
        TIMER_IntMask(timer_device->id, TIMER_INT_COMP_2, UNMASK);
    }

    if (oflag & DEVICE_OFLAG_INT_TX) {
#ifdef BSP_USING_TIMER0
        if (timer_device->id == TIMER_CH0) {
            Interrupt_Handler_Register(TIMER_CH0_IRQn, TIMER0_IRQ);
        }
#endif
#ifdef BSP_USING_TIMER1
        if (timer_device->id == TIMER_CH1) {
            Interrupt_Handler_Register(TIMER_CH1_IRQn, TIMER1_IRQ);
        }
#endif
    }
    /* Enable timer */
    TIMER_Enable(timer_device->id);
    return 0;
}

/**
 * @brief
 *
 * @param dev
 * @return int
 */
int timer_close(struct device *dev)
{
    timer_device_t *timer_device = (timer_device_t *)(dev);
    TIMER_Disable(timer_device->id);
    return 0;
}

/**
 * @brief
 *
 * @param dev
 * @param cmd
 * @param args
 * @return int
 */
int timer_control(struct device *dev, int cmd, void *args)
{
    timer_device_t *timer_device = (timer_device_t *)dev;

    switch (cmd) {
        case DEVICE_CTRL_SET_INT /* constant-expression */: {
            uint32_t offset = __builtin_ctz((uint32_t)args);

            while (offset < 3) {
                if ((uint32_t)args & (1 << offset)) {
                    TIMER_IntMask(timer_device->id, offset, UNMASK);
                }
                offset++;
            }

            if (timer_device->id == TIMER_CH0) {
                CPU_Interrupt_Pending_Clear(TIMER_CH0_IRQn);
                CPU_Interrupt_Enable(TIMER_CH0_IRQn);
            } else if (timer_device->id == TIMER_CH1) {
                CPU_Interrupt_Pending_Clear(TIMER_CH1_IRQn);
                CPU_Interrupt_Enable(TIMER_CH1_IRQn);
            }

            break;
        }

        case DEVICE_CTRL_CLR_INT /* constant-expression */: {
            uint32_t offset = __builtin_ctz((uint32_t)args);

            while (offset < 3) {
                if ((uint32_t)args & (1 << offset)) {
                    TIMER_IntMask(timer_device->id, offset, MASK);
                }
                offset++;
            }
            if (timer_device->id == TIMER_CH0) {
                CPU_Interrupt_Disable(TIMER_CH0_IRQn);
            } else if (timer_device->id == TIMER_CH1) {
                CPU_Interrupt_Disable(TIMER_CH1_IRQn);
            }
            break;
        }

        case DEVICE_CTRL_GET_INT /* constant-expression */: {
            uint32_t offset = __builtin_ctz((uint32_t)args);
            uint32_t intstatus = TIMER_GetMatchStatus(timer_device->id, offset);
            /* Clear interrupt status*/
            TIMER_ClearIntStatus(timer_device->id, TIMER_COMP_ID_0);
            TIMER_ClearIntStatus(timer_device->id, TIMER_COMP_ID_1);
            TIMER_ClearIntStatus(timer_device->id, TIMER_COMP_ID_2);
            return intstatus;
        }
        case DEVICE_CTRL_CONFIG /* constant-expression */:
            /* code */
            break;

        case DEVICE_CTRL_RESUME /* constant-expression */: {
            /* Enable timer */
            TIMER_Enable(timer_device->id);
            break;
        }

        case DEVICE_CTRL_SUSPEND /* constant-expression */: {
            TIMER_Disable(timer_device->id);
            break;
        }
        case DEVICE_CTRL_GET_CONFIG:
            return TIMER_GetCounterValue(timer_device->id);
        default:
            break;
    }

    return 0;
}

int timer_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size)
{
    timer_device_t *timer_device = (timer_device_t *)dev;
    timer_timeout_cfg_t *timeout_cfg = (timer_timeout_cfg_t *)buffer;
    uint32_t compare_count = 0;
    uint32_t reload_val = 0;
    uint32_t clkval = 0;
    uint32_t unit = 0;

    if (size % sizeof(timer_timeout_cfg_t)) {
        return -1;
    }
    /* Disable timer before config */
    TIMER_Disable(timer_device->id);

    if (timer_device->cnt_mode == TIMER_CNT_PRELOAD) {
        reload_val = timer_device->reload;
    }

    if (timer_device->id == TIMER_CH0) {
        clkval = peripheral_clock_get(PERIPHERAL_CLOCK_TIMER0);
    } else {
        clkval = peripheral_clock_get(PERIPHERAL_CLOCK_TIMER1);
    }

    if (clkval % 1000000 == 0) {
        unit = 1000000; //1us
    } else if (clkval % 100000 == 0) {
        unit = 100000; //10us
    } else if (clkval % 10000 == 0) {
        unit = 10000; //100us
    } else if (clkval % 1000 == 0) {
        unit = 1000; //1ms
    } else if (clkval % 100 == 0) {
        unit = 100; //10ms
    } else if (clkval % 10 == 0) {
        unit = 10; //100ms
    } else if (clkval % 1 == 0) {
        unit = 1; //s
    } else {
    }

    for (uint32_t i = 0; i < size / sizeof(timer_timeout_cfg_t); i++) {
        compare_count = timeout_cfg->timeout_val / (1000000 / unit) * (clkval / unit) + reload_val;

        if (compare_count < 1) {
            return -1;
        }
        TIMER_SetCompValue(timer_device->id, timeout_cfg->timeout_id, compare_count - 2);
    }
    TIMER_Enable(timer_device->id);
    return 0;
}
int timer_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size)
{
    return 0;
}

/**
 * @brief
 *
 * @param index
 * @param name
 * @return int
 */
int timer_register(enum timer_index_type index, const char *name)
{
    struct device *dev;

    if (TIMER_MAX_INDEX == 0) {
        return -DEVICE_EINVAL;
    }

    dev = &(timerx_device[index].parent);

    dev->open = timer_open;
    dev->close = timer_close;
    dev->control = timer_control;
    dev->write = timer_write;
    // dev->read = NULL;

    dev->type = DEVICE_CLASS_TIMER;
    dev->handle = NULL;

    return device_register(dev, name);
}

void timer_isr(timer_device_t *handle)
{
    uint32_t intId = 0;
    uint32_t tmpVal = 0;
    uint32_t tmpAddr = 0;

    if (!handle->parent.callback) {
        return;
    }

    intId = BL_RD_WORD(TIMER_BASE + TIMER_TMSR2_OFFSET + 4 * handle->id);
    tmpAddr = TIMER_BASE + TIMER_TICR2_OFFSET + 4 * handle->id;
    tmpVal = BL_RD_WORD(tmpAddr);

    /* Comparator 0 match interrupt */
    if (BL_IS_REG_BIT_SET(intId, TIMER_TMSR_0)) {
        BL_WR_WORD(tmpAddr, BL_SET_REG_BIT(tmpVal, TIMER_TCLR_0));
        handle->parent.callback(&handle->parent, NULL, 0, TIMER_EVENT_COMP0);

    }

    /* Comparator 1 match interrupt */
    if (BL_IS_REG_BIT_SET(intId, TIMER_TMSR_1)) {
        BL_WR_WORD(tmpAddr, BL_SET_REG_BIT(tmpVal, TIMER_TCLR_1));
        handle->parent.callback(&handle->parent, NULL, 0, TIMER_EVENT_COMP1);
    }

    /* Comparator 2 match interrupt */
    if (BL_IS_REG_BIT_SET(intId, TIMER_TMSR_2)) {
        BL_WR_WORD(tmpAddr, BL_SET_REG_BIT(tmpVal, TIMER_TCLR_2));
        handle->parent.callback(&handle->parent, NULL, 0, TIMER_EVENT_COMP2);
    }
}

#ifdef BSP_USING_TIMER0
/**
 * @brief
 *
 */
void TIMER0_IRQ(void)
{
    timer_isr(&timerx_device[TIMER0_INDEX]);
}

#endif

#ifdef BSP_USING_TIMER1
/**
 * @brief
 *
 */
void TIMER1_IRQ(void)
{
    timer_isr(&timerx_device[TIMER1_INDEX]);
}

#endif
