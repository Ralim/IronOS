/**
 * @file hal_adc.c
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
#include "hal_adc.h"
#include "hal_clock.h"
#include "hal_dma.h"
#include "bl702_glb.h"
#include "bl702_dma.h"
#include "bl702_adc.h"
#include "adc_config.h"

#ifdef BSP_USING_ADC0
static void ADC_IRQ(void);
#endif

static adc_device_t adcx_device[ADC_MAX_INDEX] = {
#ifdef BSP_USING_ADC0
    ADC0_CONFIG,
#endif
};

/**
 * @brief Check whether Channel Corresponding IO is configed success by Board System
 *
 * @param pos_list pos channel list
 * @param neg_list negative channel list
 * @param channelNum channel number
 */
uint8_t adc_check_channel_status(uint8_t *pos_list, uint8_t *neg_list, uint16_t channelNum)
{
    uint16_t i = 0;

    uint8_t channel_io_reference_table[] = {
        GLB_GPIO_PIN_8,  /* CH0 IO */
        GLB_GPIO_PIN_15, /* CH1 IO */
        GLB_GPIO_PIN_17, /* CH2 IO */
        GLB_GPIO_PIN_11, /* CH3 IO */
        GLB_GPIO_PIN_12, /* CH4 IO */
        GLB_GPIO_PIN_14, /* CH5 IO */
        GLB_GPIO_PIN_7,  /* CH6 IO */
        GLB_GPIO_PIN_9,  /* CH7 IO */
        GLB_GPIO_PIN_18, /* CH8 IO */
        GLB_GPIO_PIN_19, /* CH9 IO */
        GLB_GPIO_PIN_20, /* CH10 IO */
        GLB_GPIO_PIN_21, /* CH11 IO */

    };

    for (i = 0; i < channelNum; i++) {
        if (pos_list[i] > ADC_CHANNEL11) {
            continue;
        }

        if (GLB_GPIO_Get_Fun(channel_io_reference_table[pos_list[i]]) != GPIO_FUN_ANALOG) {
            return ERROR;
        }
    }

    return SUCCESS;
}
/**
 * @brief
 *
 * @param dev
 * @param oflag
 * @return int
 */
int adc_open(struct device *dev, uint16_t oflag)
{
    adc_device_t *adc_device = (adc_device_t *)dev;
    ADC_CFG_Type adc_cfg = { 0 };
    ADC_FIFO_Cfg_Type adc_fifo_cfg = { 0 };

    CPU_Interrupt_Disable(GPADC_DMA_IRQn);
    ADC_IntMask(ADC_INT_ALL, MASK);

    adc_cfg.clkDiv = adc_device->clk_div;

    adc_cfg.vref = adc_device->vref;
    adc_cfg.resWidth = adc_device->data_width;
    adc_cfg.inputMode = adc_device->differential_mode;

    adc_cfg.v18Sel = ADC_V18_SELECT;
    adc_cfg.v11Sel = ADC_V11_SELECT;
    adc_cfg.gain1 = ADC_PGA_GAIN1;
    adc_cfg.gain2 = ADC_PGA_GAIN2;
    adc_cfg.chopMode = ADC_CHOP_MODE;
    adc_cfg.biasSel = ADC_BIAS_SELECT;
    adc_cfg.vcm = ADC_PGA_VCM;
    adc_cfg.offsetCalibEn = ADC_OFFSET_CALIB_EN;
    adc_cfg.offsetCalibVal = ADC_OFFSER_CALIB_VAL;

    adc_fifo_cfg.dmaEn = DISABLE;
    adc_fifo_cfg.fifoThreshold = adc_device->fifo_threshold;

    if (oflag & DEVICE_OFLAG_STREAM_TX) {
    }

    if ((oflag & DEVICE_OFLAG_INT_TX) || (oflag & DEVICE_OFLAG_INT_RX)) {
#ifdef BSP_USING_ADC0
        Interrupt_Handler_Register(GPADC_DMA_IRQn, ADC_IRQ);
#endif
    }

    if (oflag & DEVICE_OFLAG_DMA_TX) {
    }

    if (oflag & DEVICE_OFLAG_DMA_RX) {
        adc_fifo_cfg.dmaEn = ENABLE;
    }

    ADC_Disable();
    ADC_Enable();

    ADC_Reset();

    ADC_Init(&adc_cfg);

    ADC_FIFO_Cfg(&adc_fifo_cfg);

    return 0;
}
/**
 * @brief
 *
 * @param dev
 * @return int
 */
int adc_close(struct device *dev)
{
    uint32_t tmpVal;

    ADC_IntMask(ADC_INT_ALL, MASK);

    /* disable convert start */
    tmpVal = BL_RD_REG(AON_BASE, AON_GPADC_REG_CMD);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_GPADC_CONV_START);
    BL_WR_REG(AON_BASE, AON_GPADC_REG_CMD, tmpVal);

    /*disable adc */
    tmpVal = BL_RD_REG(AON_BASE, AON_GPADC_REG_CMD);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_GPADC_GLOBAL_EN);
    BL_WR_REG(AON_BASE, AON_GPADC_REG_CMD, tmpVal);

    /*disable vbat */
    tmpVal = BL_RD_REG(AON_BASE, AON_GPADC_REG_CONFIG2);
    tmpVal = BL_CLR_REG_BIT(tmpVal, AON_GPADC_VBAT_EN);
    BL_WR_REG(AON_BASE, AON_GPADC_REG_CONFIG2, tmpVal);

    ADC_Reset();
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
int adc_control(struct device *dev, int cmd, void *args)
{
    adc_device_t *adc_device = (adc_device_t *)dev;
    adc_channel_cfg_t *adc_channel_cfg = (adc_channel_cfg_t *)args;
    uint8_t rlt = 0;

    switch (cmd) {
        case DEVICE_CTRL_SET_INT: {
            uint32_t offset = __builtin_ctz((uint32_t)args);
            while ((2 <= offset) && (offset < 6)) {
                if ((uint32_t)args & (1 << offset)) {
                    ADC_IntMask(offset, UNMASK);
                }
                offset++;
            }
            CPU_Interrupt_Enable(GPADC_DMA_IRQn);

            break;
        }

        case DEVICE_CTRL_CLR_INT: {
            uint32_t offset = __builtin_ctz((uint32_t)args);
            while ((2 <= offset) && (offset < 6)) {
                if ((uint32_t)args & (1 << offset)) {
                    ADC_IntMask(offset, UNMASK);
                }
                offset++;
            }
            CPU_Interrupt_Disable(GPADC_DMA_IRQn);

            break;
        }

        case DEVICE_CTRL_GET_INT:
            break;

        case DEVICE_CTRL_CONFIG:
            break;

        case DEVICE_CTRL_ADC_CHANNEL_CONFIG:
            if (adc_channel_cfg->num == 1) {
                ADC_Channel_Config(*adc_channel_cfg->pos_channel, *adc_channel_cfg->neg_channel, adc_device->continuous_conv_mode);
                rlt = adc_check_channel_status(adc_channel_cfg->pos_channel, adc_channel_cfg->neg_channel, 1);
            } else {
                ADC_Scan_Channel_Config(adc_channel_cfg->pos_channel, adc_channel_cfg->neg_channel, adc_channel_cfg->num, adc_device->continuous_conv_mode);
                rlt = adc_check_channel_status(adc_channel_cfg->pos_channel, adc_channel_cfg->neg_channel, adc_channel_cfg->num);
            }

            break;

        case DEVICE_CTRL_ADC_CHANNEL_START: {
            uint32_t regCmd;

            /* disable convert start */
            regCmd = BL_RD_REG(AON_BASE, AON_GPADC_REG_CMD);
            regCmd = BL_CLR_REG_BIT(regCmd, AON_GPADC_CONV_START);
            BL_WR_REG(AON_BASE, AON_GPADC_REG_CMD, regCmd);

            BL702_Delay_US(100);

            /* enable convert start */
            regCmd = BL_RD_REG(AON_BASE, AON_GPADC_REG_CMD);
            regCmd = BL_SET_REG_BIT(regCmd, AON_GPADC_CONV_START);
            BL_WR_REG(AON_BASE, AON_GPADC_REG_CMD, regCmd);
        } break;

        case DEVICE_CTRL_ADC_CHANNEL_STOP: {
            uint32_t tmpVal;

            /* disable convert start */
            tmpVal = BL_RD_REG(AON_BASE, AON_GPADC_REG_CMD);
            tmpVal = BL_CLR_REG_BIT(tmpVal, AON_GPADC_CONV_START);
            BL_WR_REG(AON_BASE, AON_GPADC_REG_CMD, tmpVal);
            break;
        }

        case DEVICE_CTRL_ADC_VBAT_ON: {
            uint32_t tmpVal;

            tmpVal = BL_RD_REG(AON_BASE, AON_GPADC_REG_CONFIG2);
            tmpVal = BL_SET_REG_BIT(tmpVal, AON_GPADC_VBAT_EN);
            BL_WR_REG(AON_BASE, AON_GPADC_REG_CONFIG2, tmpVal);
            break;
        }

        case DEVICE_CTRL_ADC_VBAT_OFF: {
            uint32_t tmpVal;

            tmpVal = BL_RD_REG(AON_BASE, AON_GPADC_REG_CONFIG2);
            tmpVal = BL_CLR_REG_BIT(tmpVal, AON_GPADC_VBAT_EN);
            BL_WR_REG(AON_BASE, AON_GPADC_REG_CONFIG2, tmpVal);
            break;
        }

        case DEVICE_CTRL_ADC_TSEN_ON:
            ADC_Tsen_Init(ADC_TSEN_MOD_INTERNAL_DIODE);
            break;

        case DEVICE_CTRL_ADC_TSEN_OFF:

            break;
        case DEVICE_CTRL_ATTACH_RX_DMA:
            adc_device->rx_dma = (struct device *)args;
            break;
        case DEVICE_CTRL_ADC_DATA_PARSE: {
            adc_data_parse_t *parse = (adc_data_parse_t *)args;
            ADC_Parse_Result(parse->input, parse->num, (ADC_Result_Type *)parse->output);
        }
        break;
        default:
            break;
    }

    return rlt;
}
// int adc_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size)
// {

//     return 0;
// }
/**
 * @brief
 *
 * @param dev
 * @param pos
 * @param buffer
 * @param size
 * @return int
 */
int adc_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size)
{
    uint32_t adc_fifo_val[32];
    int ret = -1;
    adc_device_t *adc_device = (adc_device_t *)dev;

    if (dev->oflag & DEVICE_OFLAG_STREAM_RX) {
        if (size > 32)
            return -1;
        while (ADC_Get_FIFO_Count() < size) {
        }

        for (uint32_t i = 0; i < size; i++) {
            adc_fifo_val[i] = ADC_Read_FIFO();
        }
        adc_channel_val_t *adc_parse_val = (adc_channel_val_t *)buffer;
        ADC_Parse_Result(adc_fifo_val, size, (ADC_Result_Type *)adc_parse_val);
        return size;
    } else if (dev->oflag & DEVICE_OFLAG_DMA_RX) {
        struct device *dma_ch = (struct device *)adc_device->rx_dma;
        if (!dma_ch)
            return -1;

        ret = dma_reload(dma_ch, (uint32_t)DMA_ADDR_ADC_RDR, (uint32_t)buffer, size);
        dma_channel_start(dma_ch);

        return ret;
    }
    return ret;
}

int adc_trim_tsen(uint16_t *tsen_offset)
{
    return ADC_Trim_TSEN(tsen_offset);
}

float adc_get_tsen(uint16_t tsen_offset)
{
    return TSEN_Get_Temp(tsen_offset);
}

/**
 * @brief
 *
 * @param index
 * @param name
 * @return int
 */
int adc_register(enum adc_index_type index, const char *name)
{
    struct device *dev;

    if (ADC_MAX_INDEX == 0) {
        return -DEVICE_EINVAL;
    }

    dev = &(adcx_device[index].parent);

    dev->open = adc_open;
    dev->close = adc_close;
    dev->control = adc_control;
    dev->write = NULL;
    dev->read = adc_read;

    dev->type = DEVICE_CLASS_ADC;
    dev->handle = NULL;

    return device_register(dev, name);
}

/**
 * @brief
 *
 * @param handle
 */
void adc_isr(adc_device_t *handle)
{
    if (!handle->parent.callback)
        return;

    if (ADC_GetIntStatus(ADC_INT_POS_SATURATION) == SET && ADC_IntGetMask(ADC_INT_POS_SATURATION) == UNMASK) {
        //handle->parent.callback(&handle->parent, NULL, 0, ADC_EVEN_INT_POS_SATURATION);
        ADC_IntClr(ADC_INT_POS_SATURATION);
    }

    if (ADC_GetIntStatus(ADC_INT_NEG_SATURATION) == SET && ADC_IntGetMask(ADC_INT_NEG_SATURATION) == UNMASK) {
        //handle->parent.callback(&handle->parent, NULL, 0, ADC_EVEN_INT_NEG_SATURATION);
        ADC_IntClr(ADC_INT_NEG_SATURATION);
    }

    if (ADC_GetIntStatus(ADC_INT_FIFO_UNDERRUN) == SET && ADC_IntGetMask(ADC_INT_FIFO_UNDERRUN) == UNMASK) {
        ADC_IntClr(ADC_INT_FIFO_UNDERRUN);
        handle->parent.callback(&handle->parent, NULL, 0, ADC_EVENT_UNDERRUN);
    }

    if (ADC_GetIntStatus(ADC_INT_FIFO_OVERRUN) == SET && ADC_IntGetMask(ADC_INT_FIFO_OVERRUN) == UNMASK) {
        ADC_IntClr(ADC_INT_FIFO_OVERRUN);
        handle->parent.callback(&handle->parent, NULL, 0, ADC_EVENT_OVERRUN);
    }

    if (ADC_GetIntStatus(ADC_INT_FIFO_READY) == SET && ADC_IntGetMask(ADC_INT_FIFO_READY) == UNMASK) {
        ADC_IntClr(ADC_INT_FIFO_READY);
        uint32_t adc_count = ADC_Get_FIFO_Count();
        uint32_t adc_fifo_val[32];
        adc_channel_val_t adc_parse_val[32];
        for (uint32_t i = 0; i < adc_count; i++) {
            adc_fifo_val[i] = ADC_Read_FIFO();
        }
        ADC_Parse_Result(adc_fifo_val, adc_count, (ADC_Result_Type *)adc_parse_val);
        handle->parent.callback(&handle->parent, (void *)adc_parse_val, adc_count, ADC_EVENT_FIFO);
    }
}
#ifdef BSP_USING_ADC0
void ADC_IRQ(void)
{
    adc_isr(&adcx_device[ADC0_INDEX]);
}
#endif