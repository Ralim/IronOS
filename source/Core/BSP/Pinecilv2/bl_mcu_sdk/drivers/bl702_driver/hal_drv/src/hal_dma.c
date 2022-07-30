/**
 * @file hal_dma.c
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
#include "hal_dma.h"
#include "bl702_dma.h"

#define DMA_CHANNEL_BASE(id_base, ch) ((id_base) + DMA_CHANNEL_OFFSET + (ch)*0x100)

static const uint32_t dma_channel_base[][8] = {
    {
        DMA_CHANNEL_BASE(DMA_BASE, 0),
        DMA_CHANNEL_BASE(DMA_BASE, 1),
        DMA_CHANNEL_BASE(DMA_BASE, 2),
        DMA_CHANNEL_BASE(DMA_BASE, 3),
        DMA_CHANNEL_BASE(DMA_BASE, 4),
        DMA_CHANNEL_BASE(DMA_BASE, 5),
        DMA_CHANNEL_BASE(DMA_BASE, 6),
        DMA_CHANNEL_BASE(DMA_BASE, 7),
    }

};

static void DMA0_IRQ(void);

static dma_device_t dmax_device[DMA_MAX_INDEX] = {
#ifdef BSP_USING_DMA0_CH0
    DMA0_CH0_CONFIG,
#endif
#ifdef BSP_USING_DMA0_CH1
    DMA0_CH1_CONFIG,
#endif
#ifdef BSP_USING_DMA0_CH2
    DMA0_CH2_CONFIG,
#endif
#ifdef BSP_USING_DMA0_CH3
    DMA0_CH3_CONFIG,
#endif
#ifdef BSP_USING_DMA0_CH4
    DMA0_CH4_CONFIG,
#endif
#ifdef BSP_USING_DMA0_CH5
    DMA0_CH5_CONFIG,
#endif
#ifdef BSP_USING_DMA0_CH6
    DMA0_CH6_CONFIG,
#endif
#ifdef BSP_USING_DMA0_CH7
    DMA0_CH7_CONFIG,
#endif
};
/**
 * @brief
 *
 * @param dev
 * @param oflag
 * @return int
 */
int dma_open(struct device *dev, uint16_t oflag)
{
    dma_device_t *dma_device = (dma_device_t *)dev;
    DMA_Channel_Cfg_Type chCfg = { 0 };

    /* Disable all interrupt */
    DMA_IntMask(dma_device->ch, DMA_INT_ALL, MASK);
    /* Enable uart interrupt*/
    CPU_Interrupt_Disable(DMA_ALL_IRQn);

    DMA_Disable();

    DMA_Channel_Disable(dma_device->ch);

    chCfg.ch = dma_device->ch;
    chCfg.dir = dma_device->direction;
    chCfg.srcPeriph = dma_device->src_req;
    chCfg.dstPeriph = dma_device->dst_req;
    chCfg.srcAddrInc = dma_device->src_addr_inc;
    chCfg.destAddrInc = dma_device->dst_addr_inc;
    chCfg.srcBurstSzie = dma_device->src_burst_size;
    chCfg.dstBurstSzie = dma_device->dst_burst_size;
    chCfg.srcTransfWidth = dma_device->src_width;
    chCfg.dstTransfWidth = dma_device->dst_width;
    DMA_Channel_Init(&chCfg);

    DMA_Enable();

    Interrupt_Handler_Register(DMA_ALL_IRQn, DMA0_IRQ);
    /* Enable uart interrupt*/
    CPU_Interrupt_Enable(DMA_ALL_IRQn);
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
int dma_control(struct device *dev, int cmd, void *args)
{
    dma_device_t *dma_device = (dma_device_t *)dev;

    switch (cmd) {
        case DEVICE_CTRL_SET_INT:
            /* Dma interrupt configuration */
            DMA_IntMask(dma_device->ch, DMA_INT_TCOMPLETED, UNMASK);
            DMA_IntMask(dma_device->ch, DMA_INT_ERR, UNMASK);

            break;

        case DEVICE_CTRL_CLR_INT:
            /* Dma interrupt configuration */
            DMA_IntMask(dma_device->ch, DMA_INT_TCOMPLETED, MASK);
            DMA_IntMask(dma_device->ch, DMA_INT_ERR, MASK);

            break;

        case DEVICE_CTRL_GET_INT:
            break;

        case DEVICE_CTRL_CONFIG:
            break;

        case DEVICE_CTRL_DMA_CHANNEL_UPDATE:
            DMA_LLI_Update(dma_device->ch, (uint32_t)args);
            break;

        case DEVICE_CTRL_DMA_CHANNEL_GET_STATUS:
            return DMA_Channel_Is_Busy(dma_device->ch);

        case DEVICE_CTRL_DMA_CHANNEL_START:
            DMA_Channel_Enable(dma_device->ch);
            break;

        case DEVICE_CTRL_DMA_CHANNEL_STOP:
            DMA_Channel_Disable(dma_device->ch);
            break;
        case DEVICE_CTRL_DMA_CONFIG_SI: {
            uint32_t tmpVal = BL_RD_REG(dma_channel_base[dma_device->id][dma_device->ch], DMA_CONTROL);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_SI, ((uint32_t)args) & 0x01);
            BL_WR_REG(dma_channel_base[dma_device->id][dma_device->ch], DMA_CONTROL, tmpVal);

        } break;
        case DEVICE_CTRL_DMA_CONFIG_DI: {
            uint32_t tmpVal = BL_RD_REG(dma_channel_base[dma_device->id][dma_device->ch], DMA_CONTROL);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_DI, ((uint32_t)args) & 0x01);
            BL_WR_REG(dma_channel_base[dma_device->id][dma_device->ch], DMA_CONTROL, tmpVal);

        } break;
        default:
            break;
    }

    return 0;
}
/**
 * @brief
 *
 * @param dev
 * @return int
 */
int dma_close(struct device *dev)
{
    dma_device_t *dma_device = (dma_device_t *)dev;
    DMA_Channel_Cfg_Type chCfg = { 0 };

    DMA_Channel_Disable(dma_device->ch);
    DMA_Channel_Init(&chCfg);
    return 0;
}

int dma_register(enum dma_index_type index, const char *name)
{
    struct device *dev;

    if (DMA_MAX_INDEX == 0) {
        return -DEVICE_EINVAL;
    }

    dev = &(dmax_device[index].parent);

    dev->open = dma_open;
    dev->close = dma_close;
    dev->control = dma_control;
    // dev->write = dma_write;
    // dev->read = dma_read;

    dev->type = DEVICE_CLASS_DMA;
    dev->handle = NULL;

    return device_register(dev, name);
}

static BL_Err_Type dma_scan_unregister_device(uint8_t *allocate_index)
{
    struct device *dev;
    dlist_t *node;
    uint8_t dma_index = 0;
    uint32_t dma_handle[DMA_MAX_INDEX];

    for (dma_index = 0; dma_index < DMA_MAX_INDEX; dma_index++) {
        dma_handle[dma_index] = 0xff;
    }

    /* get registered dma handle list*/
    dlist_for_each(node, device_get_list_header())
    {
        dev = dlist_entry(node, struct device, list);

        if (dev->type == DEVICE_CLASS_DMA) {
            dma_handle[(((uint32_t)dev - (uint32_t)dmax_device) / sizeof(dma_device_t)) % DMA_MAX_INDEX] = SET;
        }
    }

    for (dma_index = 0; dma_index < DMA_MAX_INDEX; dma_index++) {
        if (dma_handle[dma_index] == 0xff) {
            *allocate_index = dma_index;
            return SUCCESS;
        }
    }

    return ERROR;
}

int dma_allocate_register(const char *name)
{
    struct device *dev;
    uint8_t index;

    if (DMA_MAX_INDEX == 0) {
        return -DEVICE_EINVAL;
    }

    if (dma_scan_unregister_device(&index) == ERROR) {
        return -DEVICE_ENOSPACE;
    }

    dev = &(dmax_device[index].parent);

    dev->open = dma_open;
    dev->close = dma_close;
    dev->control = dma_control;
    // dev->write = dma_write;
    // dev->read = dma_read;

    dev->status = DEVICE_UNREGISTER;
    dev->type = DEVICE_CLASS_DMA;
    dev->handle = NULL;

    return device_register(dev, name);
}

/**
 * @brief
 *
 * @param dev
 * @param src_addr
 * @param dst_addr
 * @param transfer_size
 * @return int
 */
int dma_reload(struct device *dev, uint32_t src_addr, uint32_t dst_addr, uint32_t transfer_size)
{
#if defined(BSP_USING_DMA0_CH0) || defined(BSP_USING_DMA0_CH1) || defined(BSP_USING_DMA0_CH2) || defined(BSP_USING_DMA0_CH3) || \
    defined(BSP_USING_DMA0_CH4) || defined(BSP_USING_DMA0_CH5) || defined(BSP_USING_DMA0_CH6) || defined(BSP_USING_DMA0_CH7)

    uint32_t malloc_count;
    uint32_t remain_len;
    uint32_t actual_transfer_len = 0;
    uint32_t actual_transfer_offset = 0;
    dma_control_data_t dma_ctrl_cfg;

    dma_device_t *dma_device = (dma_device_t *)dev;

    DMA_Channel_Disable(dma_device->ch);

    if (transfer_size == 0) {
        return 0;
    }

    switch (dma_device->src_width) {
        case DMA_TRANSFER_WIDTH_8BIT:
            actual_transfer_offset = 4095;
            actual_transfer_len = transfer_size;
            break;
        case DMA_TRANSFER_WIDTH_16BIT:
            if (transfer_size % 2) {
                return -1;
            }
            actual_transfer_offset = 4095 << 1;
            actual_transfer_len = transfer_size >> 1;
            break;
        case DMA_TRANSFER_WIDTH_32BIT:
            if (transfer_size % 4) {
                return -1;
            }

            actual_transfer_offset = 4095 << 2;
            actual_transfer_len = transfer_size >> 2;
            break;

        default:
            return -3;
            break;
    }

    dma_ctrl_cfg = (dma_control_data_t)(BL_RD_REG(dma_channel_base[dma_device->id][dma_device->ch], DMA_CONTROL));

    malloc_count = actual_transfer_len / 4095;
    remain_len = actual_transfer_len % 4095;

    if (remain_len) {
        malloc_count++;
    }

    dma_device->lli_cfg = (dma_lli_ctrl_t *)realloc(dma_device->lli_cfg, sizeof(dma_lli_ctrl_t) * malloc_count);

    if (dma_device->lli_cfg) {
        /*transfer_size will be integer multiple of 4095*n or 4095*2*n or 4095*4*n,(n>0) */
        for (uint32_t i = 0; i < malloc_count; i++) {
            dma_device->lli_cfg[i].src_addr = src_addr;
            dma_device->lli_cfg[i].dst_addr = dst_addr;
            dma_device->lli_cfg[i].nextlli = 0;

            dma_ctrl_cfg.bits.TransferSize = 4095;
            dma_ctrl_cfg.bits.I = 0;

            if (dma_ctrl_cfg.bits.SI) {
                src_addr += actual_transfer_offset;
            }

            if (dma_ctrl_cfg.bits.DI) {
                dst_addr += actual_transfer_offset;
            }

            if (i == malloc_count - 1) {
                if (remain_len) {
                    dma_ctrl_cfg.bits.TransferSize = remain_len;
                }
                dma_ctrl_cfg.bits.I = 1;

                if (dma_device->transfer_mode == DMA_LLI_CYCLE_MODE) {
                    dma_device->lli_cfg[i].nextlli = (uint32_t)&dma_device->lli_cfg[0];
                }
            }

            if (i) {
                dma_device->lli_cfg[i - 1].nextlli = (uint32_t)&dma_device->lli_cfg[i];
            }

            dma_device->lli_cfg[i].cfg = dma_ctrl_cfg;
        }
        BL_WR_REG(dma_channel_base[dma_device->id][dma_device->ch], DMA_SRCADDR, dma_device->lli_cfg[0].src_addr);
        BL_WR_REG(dma_channel_base[dma_device->id][dma_device->ch], DMA_DSTADDR, dma_device->lli_cfg[0].dst_addr);
        BL_WR_REG(dma_channel_base[dma_device->id][dma_device->ch], DMA_LLI, dma_device->lli_cfg[0].nextlli);
        BL_WR_REG(dma_channel_base[dma_device->id][dma_device->ch], DMA_CONTROL, dma_device->lli_cfg[0].cfg.WORD);
    } else {
        return -2;
    }
#endif
    return 0;
}

/**
 * @brief
 *
 * @param handle
 */
void dma_channel_isr(dma_device_t *handle)
{
    uint32_t tmpVal;
    uint32_t intClr;

    /* Get DMA register */
    uint32_t DMAChs = DMA_BASE;

    if (!handle->parent.callback) {
        return;
    }

    tmpVal = BL_RD_REG(DMAChs, DMA_INTTCSTATUS);
    if ((BL_GET_REG_BITS_VAL(tmpVal, DMA_INTTCSTATUS) & (1 << handle->ch)) != 0) {
        /* Clear interrupt */
        tmpVal = BL_RD_REG(DMAChs, DMA_INTTCCLEAR);
        intClr = BL_GET_REG_BITS_VAL(tmpVal, DMA_INTTCCLEAR);
        intClr |= (1 << handle->ch);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_INTTCCLEAR, intClr);
        BL_WR_REG(DMAChs, DMA_INTTCCLEAR, tmpVal);
        handle->parent.callback(&handle->parent, NULL, 0, DMA_INT_TCOMPLETED);
    }

    tmpVal = BL_RD_REG(DMAChs, DMA_INTERRORSTATUS);
    if ((BL_GET_REG_BITS_VAL(tmpVal, DMA_INTERRORSTATUS) & (1 << handle->ch)) != 0) {
        /*Clear interrupt */
        tmpVal = BL_RD_REG(DMAChs, DMA_INTERRCLR);
        intClr = BL_GET_REG_BITS_VAL(tmpVal, DMA_INTERRCLR);
        intClr |= (1 << handle->ch);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_INTERRCLR, intClr);
        BL_WR_REG(DMAChs, DMA_INTERRCLR, tmpVal);
        handle->parent.callback(&handle->parent, NULL, 0, DMA_INT_ERR);
    }
}
/**
 * @brief
 *
 */
void DMA0_IRQ(void)
{
    for (uint8_t i = 0; i < DMA_MAX_INDEX; i++) {
        dma_channel_isr(&dmax_device[i]);
    }
}
