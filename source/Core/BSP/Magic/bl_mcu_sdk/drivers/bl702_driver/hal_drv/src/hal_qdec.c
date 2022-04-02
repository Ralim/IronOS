/**
 * @file hal_qdec.c
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

#include "hal_qdec.h"
#include "qdec_reg.h"
#include "bl702_qdec.h"
#include "bl702_gpio.h"
#include "bl702_glb.h"

static qdec_device_t qdecx_device[QDEC_MAX_INDEX] = {
#ifdef BSP_USING_QDEC0
    QDEC0_CONFIG,
#endif
#ifdef BSP_USING_QDEC1
    QDEC1_CONFIG,
#endif
#ifdef BSP_USING_QDEC2
    QDEC2_CONFIG,
#endif
};
#ifdef BSP_USING_QDEC0
static void QDEC0_IRQ(void);
#endif
#ifdef BSP_USING_QDEC1
static void QDEC1_IRQ(void);
#endif
#ifdef BSP_USING_QDEC2
static void QDEC2_IRQ(void);
#endif

int qdec_open(struct device *dev, uint16_t oflag)
{
    qdec_device_t *qdec_device = (qdec_device_t *)dev;
    QDEC_CFG_Type qdec_cfg = { 0 };

    QDEC_DeInit(qdec_device->id);

    qdec_cfg.sampleCfg.sampleMod = qdec_device->sample_mode;
    qdec_cfg.sampleCfg.samplePeriod = qdec_device->sample_period;
    qdec_cfg.reportCfg.reportMod = qdec_device->report_mode;
    qdec_cfg.reportCfg.reportPeriod = qdec_device->report_period;
    qdec_cfg.ledCfg.ledEn = qdec_device->led_en;
    qdec_cfg.ledCfg.ledSwap = qdec_device->led_swap;
    qdec_cfg.ledCfg.ledPeriod = qdec_device->led_period;
    qdec_cfg.deglitchCfg.deglitchEn = qdec_device->deglitch_en;
    qdec_cfg.deglitchCfg.deglitchStrength = qdec_device->deglitch_strength;
    qdec_cfg.accMod = qdec_device->acc_mode;

    QDEC_Init(qdec_device->id, &qdec_cfg);

    if (oflag & DEVICE_OFLAG_INT_RX) {
#ifdef BSP_USING_QDEC0
        if (qdec_device->id == QDEC0_ID) {
            Interrupt_Handler_Register(QDEC0_IRQn, QDEC0_IRQ);
        }
#endif
#ifdef BSP_USING_QDEC1
        if (qdec_device->id == QDEC1_ID) {
            Interrupt_Handler_Register(QDEC1_IRQn, QDEC1_IRQ);
            QDEC_SetIntMask(qdec_device->id, QDEC_INT_REPORT, MASK);
            QDEC_SetIntMask(qdec_device->id, QDEC_INT_SAMPLE, MASK);
            QDEC_SetIntMask(qdec_device->id, QDEC_INT_ERROR, MASK);
            QDEC_SetIntMask(qdec_device->id, QDEC_INT_OVERFLOW, MASK);
            // CPU_Interrupt_Enable(QDEC1_IRQn);
        }
#endif
#ifdef BSP_USING_QDEC2
        if (qdec_device->id == QDEC2_ID) {
            Interrupt_Handler_Register(QDEC2_IRQn, QDEC2_IRQ);
            QDEC_SetIntMask(qdec_device->id, QDEC_INT_REPORT, MASK);
            QDEC_SetIntMask(qdec_device->id, QDEC_INT_SAMPLE, MASK);
            QDEC_SetIntMask(qdec_device->id, QDEC_INT_ERROR, MASK);
            QDEC_SetIntMask(qdec_device->id, QDEC_INT_OVERFLOW, MASK);
            // CPU_Interrupt_Enable(QDEC2_IRQn);
        }
#endif
    }

    return 0;
}

int qdec_close(struct device *dev)
{
    qdec_device_t *qdec_device = (qdec_device_t *)dev;

    QDEC_Disable(qdec_device->id);
    return 0;
}

int qdec_control(struct device *dev, int cmd, void *args)
{
    qdec_device_t *qdec_device = (qdec_device_t *)dev;

    switch (cmd) {
        case DEVICE_CTRL_SET_INT: {
            uint32_t offset = __builtin_ctz((uint32_t)args);
            while (offset < 5) {
                if ((uint32_t)args & (1 << offset)) {
                    QDEC_SetIntMask(qdec_device->id, offset, UNMASK);
                }
                offset++;
            }
            if (qdec_device->id == QDEC0_ID) {
                CPU_Interrupt_Enable(QDEC0_IRQn);
            } else if (qdec_device->id == QDEC1_ID) {
                CPU_Interrupt_Enable(QDEC1_IRQn);
            } else if (qdec_device->id == QDEC2_ID) {
                CPU_Interrupt_Enable(QDEC2_IRQn);
            }
            break;
        }

        case DEVICE_CTRL_CLR_INT: {
            uint32_t offset = __builtin_ctz((uint32_t)args);
            while (offset < 5) {
                if ((uint32_t)args & (1 << offset)) {
                    QDEC_SetIntMask(qdec_device->id, offset, MASK);
                }
                offset++;
            }
            if (qdec_device->id == QDEC0_ID) {
                CPU_Interrupt_Disable(QDEC0_IRQn);
            } else if (qdec_device->id == QDEC1_ID) {
                CPU_Interrupt_Disable(QDEC1_IRQn);
            } else if (qdec_device->id == QDEC2_ID) {
                CPU_Interrupt_Disable(QDEC2_IRQn);
            }
            break;
        }

        case DEVICE_CTRL_GET_INT /* constant-expression */:
            /* code */
            break;

        case DEVICE_CTRL_CONFIG /* constant-expression */:
            /* code */
            break;

        case DEVICE_CTRL_RESUME /* constant-expression */: {
            /* Enable timer */
            QDEC_Enable(qdec_device->id);
            break;
        }

        case DEVICE_CTRL_SUSPEND /* constant-expression */: {
            QDEC_Disable(qdec_device->id);
            break;
        }

        case DEVICE_CTRL_GET_SAMPLE_VAL: {
            return QDEC_Get_Sample_Val(qdec_device->id);
        }

        case DEVICE_CTRL_GET_SAMPLE_DIR: {
            return QDEC_Get_Sample_Direction(qdec_device->id);
        }

        case DEVICE_CTRL_GET_ERROR_CNT: {
            return QDEC_Get_Err_Cnt(qdec_device->id);
        }
        default:
            break;
    }

    return 0;
}

int qdec_register(enum qdec_index_type index, const char *name)
{
    struct device *dev;

    if (QDEC_MAX_INDEX == 0) {
        return -DEVICE_EINVAL;
    }

    dev = &(qdecx_device[index].parent);

    dev->open = qdec_open;
    dev->close = qdec_close;
    dev->control = qdec_control;
    dev->write = NULL;
    dev->read = NULL; //qdec_read;

    dev->type = DEVICE_CLASS_QDEC;
    dev->handle = NULL;

    return device_register(dev, name);
}

void qdec_isr(qdec_device_t *handle)
{
    uint32_t tmp_sts = 0;
    uint32_t tmp_val = 0;
    uint32_t tmp_clr = 0;
    uint32_t tmp_clr_val = 0;

    if (handle->id == QDEC0_ID) {
        tmp_sts = BL_RD_WORD(QDEC0_BASE + QDEC0_INT_STS_OFFSET);
        tmp_clr = QDEC0_BASE + QDEC0_INT_CLR_OFFSET;
        tmp_clr_val = BL_RD_WORD(tmp_clr);
        tmp_val = BL_RD_REG(QDEC0_BASE, QDEC0_INT_EN);
    } else if (handle->id == QDEC1_ID) {
        tmp_sts = BL_RD_WORD(QDEC0_BASE + QDEC1_INT_STS_OFFSET);
        tmp_clr = QDEC0_BASE + QDEC1_INT_CLR_OFFSET;
        tmp_clr_val = BL_RD_WORD(tmp_clr);
        tmp_val = BL_RD_REG(QDEC1_BASE, QDEC0_INT_EN);
    } else if (handle->id == QDEC2_ID) {
        tmp_sts = BL_RD_WORD(QDEC0_BASE + QDEC2_INT_STS_OFFSET);
        tmp_clr = QDEC0_BASE + QDEC2_INT_CLR_OFFSET;
        tmp_clr_val = BL_RD_WORD(tmp_clr);
        tmp_val = BL_RD_REG(QDEC2_BASE, QDEC0_INT_EN);
    }

    if (!handle->parent.callback) {
        return;
    }

    /* qdec report intterupt */
    if (!(BL_GET_REG_BITS_VAL(tmp_val, QDEC_RPT_RDY_EN) ? UNMASK : MASK)) {
        if (BL_IS_REG_BIT_SET(tmp_sts, QDEC_RPT_RDY_STS)) {
            BL_WR_WORD(tmp_clr, BL_SET_REG_BIT(tmp_clr_val, QDEC_RPT_RDY_CLR));
            handle->parent.callback(&handle->parent, NULL, 0, QDEC_REPORT_EVENT);
        }
    }

    /* qdec sample intterupt */
    if (!(BL_GET_REG_BITS_VAL(tmp_val, QDEC_SPL_RDY_EN) ? UNMASK : MASK)) {
        if (BL_IS_REG_BIT_SET(tmp_sts, QDEC_SPL_RDY_STS)) {
            BL_WR_WORD(tmp_clr, BL_SET_REG_BIT(tmp_clr_val, QDEC_SPL_RDY_CLR));
            handle->parent.callback(&handle->parent, NULL, 0, QDEC_SAMPLE_EVENT);
        }
    }

    /* qdec error intterupt */
    if (!(BL_GET_REG_BITS_VAL(tmp_val, QDEC_DBL_RDY_EN) ? UNMASK : MASK)) {
        if (BL_IS_REG_BIT_SET(tmp_sts, QDEC_DBL_RDY_STS)) {
            BL_WR_WORD(tmp_clr, BL_SET_REG_BIT(tmp_clr_val, QDEC_DBL_RDY_CLR));
            handle->parent.callback(&handle->parent, NULL, 0, QDEC_ERROR_EVENT);
        }
    }

    /* qdec overflow intterupt */
    if (!(BL_GET_REG_BITS_VAL(tmp_val, QDEC_OVERFLOW_EN) ? UNMASK : MASK)) {
        if (BL_IS_REG_BIT_SET(tmp_sts, QDEC_OVERFLOW_STS)) {
            BL_WR_WORD(tmp_clr, BL_SET_REG_BIT(tmp_clr_val, QDEC_OVERFLOW_CLR));
            handle->parent.callback(&handle->parent, NULL, 0, QDEC_OVERFLOW_EVENT);
        }
    }
}

#ifdef BSP_USING_QDEC0
static void QDEC0_IRQ(void)
{
    qdec_isr(&qdecx_device[QDEC0_INDEX]);
}
#endif

#ifdef BSP_USING_QDEC1
static void QDEC1_IRQ(void)
{
    qdec_isr(&qdecx_device[QDEC1_INDEX]);
}
#endif

#ifdef BSP_USING_QDEC2
static void QDEC2_IRQ(void)
{
    qdec_isr(&qdecx_device[QDEC2_INDEX]);
}
#endif
