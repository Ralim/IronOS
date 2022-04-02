/**
 * @file hal_spi.c
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
#include "hal_spi.h"
#include "hal_dma.h"
#include "bl702_glb.h"
#include "bl702_spi.h"

#ifdef BSP_USING_SPI0
static void SPI0_IRQ(void);
#endif

static spi_device_t spix_device[SPI_MAX_INDEX] = {
#ifdef BSP_USING_SPI0
    SPI0_CONFIG,
#endif
};
/**
 * @brief
 *
 * @param dev
 * @param oflag
 * @return int
 */
int spi_open(struct device *dev, uint16_t oflag)
{
    spi_device_t *spi_device = (spi_device_t *)dev;
    SPI_CFG_Type spiCfg = { 0 };
    SPI_FifoCfg_Type fifoCfg = { 0 };

    if (spi_device->pin_swap_enable) {
        GLB_Swap_SPI_0_MOSI_With_MISO(ENABLE);
    }

    /* Enable uart interrupt*/
    CPU_Interrupt_Disable(SPI_IRQn);
    SPI_IntMask(spi_device->id, SPI_INT_ALL, MASK);

    SPI_Disable(spi_device->id, spi_device->mode);

    GLB_Set_SPI_0_ACT_MOD_Sel(spi_device->mode);

    /* Set SPI clock */
    SPI_ClockCfg_Type clockCfg = {
        2, /* Length of start condition */
        2, /* Length of stop condition */
        2, /* Length of data phase 0,affecting clock */
        2, /* Length of data phase 1,affecting clock */
        2  /* Length of interval between frame */
    };

    if (spi_device->clk > 72 * 1000000) {
        return -1;
    }

    uint8_t length = 72 * 1000000 / spi_device->clk;

    if (!(length % 2)) {
        clockCfg.dataPhase0Len = length / 2;
        clockCfg.dataPhase1Len = length / 2;
    } else {
        clockCfg.dataPhase0Len = length / 2;
        clockCfg.dataPhase1Len = length / 2 + 1;
    }

    SPI_ClockConfig(spi_device->id, &clockCfg);

    spiCfg.continuousEnable = 1;

    if (spi_device->direction == SPI_LSB_BYTE0_DIRECTION_FIRST) {
        spiCfg.bitSequence = SPI_BIT_INVERSE_LSB_FIRST;
        spiCfg.byteSequence = SPI_BYTE_INVERSE_BYTE0_FIRST;
    } else if (spi_device->direction == SPI_LSB_BYTE3_DIRECTION_FIRST) {
        spiCfg.bitSequence = SPI_BIT_INVERSE_LSB_FIRST;
        spiCfg.byteSequence = SPI_BYTE_INVERSE_BYTE3_FIRST;
    } else if (spi_device->direction == SPI_MSB_BYTE0_DIRECTION_FIRST) {
        spiCfg.bitSequence = SPI_BIT_INVERSE_MSB_FIRST;
        spiCfg.byteSequence = SPI_BYTE_INVERSE_BYTE0_FIRST;
    } else if (spi_device->direction == SPI_MSB_BYTE3_DIRECTION_FIRST) {
        spiCfg.bitSequence = SPI_BIT_INVERSE_MSB_FIRST;
        spiCfg.byteSequence = SPI_BYTE_INVERSE_BYTE3_FIRST;
    }

    spiCfg.clkPolarity = spi_device->clk_polaraity;
    spiCfg.clkPhaseInv = spi_device->clk_phase;
    spiCfg.frameSize = spi_device->datasize;

    if (spi_device->delitch_cnt) {
        spiCfg.deglitchEnable = 1;
    }

    /* SPI config */
    SPI_Init(spi_device->id, &spiCfg);

    SPI_SetDeglitchCount(spi_device->id, spi_device->delitch_cnt);

    fifoCfg.txFifoThreshold = spi_device->fifo_threshold;
    fifoCfg.txFifoDmaEnable = DISABLE;
    fifoCfg.rxFifoThreshold = spi_device->fifo_threshold;
    fifoCfg.rxFifoDmaEnable = DISABLE;

    if (oflag & DEVICE_OFLAG_INT_TX || oflag & DEVICE_OFLAG_INT_RX) {
#ifdef BSP_USING_SPI0
        Interrupt_Handler_Register(SPI_IRQn, SPI0_IRQ);
#endif
    }

    if (oflag & DEVICE_OFLAG_DMA_TX) {
        fifoCfg.txFifoDmaEnable = ENABLE;
    }

    if (oflag & DEVICE_OFLAG_DMA_RX) {
        fifoCfg.rxFifoDmaEnable = ENABLE;
    }

    SPI_FifoConfig(spi_device->id, &fifoCfg);
    /* Enable spi master mode */
    SPI_Enable(spi_device->id, spi_device->mode);

    return 0;
}
/**
 * @brief
 *
 * @param dev
 * @return int
 */
int spi_close(struct device *dev)
{
    spi_device_t *spi_device = (spi_device_t *)dev;

    SPI_Disable(spi_device->id, spi_device->mode);
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_SPI);
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
int spi_control(struct device *dev, int cmd, void *args)
{
    spi_device_t *spi_device = (spi_device_t *)dev;

    switch (cmd) {
        case DEVICE_CTRL_SET_INT /* constant-expression */:

            break;

        case DEVICE_CTRL_CLR_INT /* constant-expression */:
            /* code */
            break;

        case DEVICE_CTRL_GET_INT /* constant-expression */:
            /* code */
            break;

        case DEVICE_CTRL_RESUME:
            /* code */
            SPI_Enable(spi_device->id, spi_device->mode);
            break;

        case DEVICE_CTRL_SUSPEND:
            SPI_Disable(spi_device->id, spi_device->mode);
            break;

        case DEVICE_CTRL_CONFIG /* constant-expression */:
            /* code */
            break;

        case DEVICE_CTRL_ATTACH_TX_DMA /* constant-expression */:
            spi_device->tx_dma = (struct device *)args;
            break;

        case DEVICE_CTRL_ATTACH_RX_DMA /* constant-expression */:
            spi_device->rx_dma = (struct device *)args;
            break;

        case DEVICE_CTRL_SPI_CONFIG_CLOCK /* constant-expression */:
            SPI_SetClock(spi_device->id, (uint32_t)args);
            break;

        case DEVICE_CTRL_TX_DMA_SUSPEND: {
            uint32_t tmpVal = BL_RD_REG(SPI_BASE + spi_device->id * 0x100, SPI_FIFO_CONFIG_0);
            tmpVal = BL_CLR_REG_BIT(tmpVal, SPI_DMA_TX_EN);
            BL_WR_REG(SPI_BASE + spi_device->id * 0x100, SPI_FIFO_CONFIG_0, tmpVal);
            dev->oflag &= ~DEVICE_OFLAG_DMA_TX;
            break;
        }

        case DEVICE_CTRL_RX_DMA_SUSPEND: {
            uint32_t tmpVal = BL_RD_REG(SPI_BASE + spi_device->id * 0x100, SPI_FIFO_CONFIG_0);
            tmpVal = BL_CLR_REG_BIT(tmpVal, SPI_DMA_RX_EN);
            BL_WR_REG(SPI_BASE + spi_device->id * 0x100, SPI_FIFO_CONFIG_0, tmpVal);
            dev->oflag &= ~DEVICE_OFLAG_DMA_RX;
            break;
        }

        case DEVICE_CTRL_TX_DMA_RESUME: {
            uint32_t tmpVal = BL_RD_REG(SPI_BASE + spi_device->id * 0x100, SPI_FIFO_CONFIG_0);
            tmpVal = BL_SET_REG_BIT(tmpVal, SPI_DMA_TX_EN);
            BL_WR_REG(SPI_BASE + spi_device->id * 0x100, SPI_FIFO_CONFIG_0, tmpVal);
            dev->oflag |= DEVICE_OFLAG_DMA_TX;
            break;
        }

        case DEVICE_CTRL_RX_DMA_RESUME: {
            uint32_t tmpVal = BL_RD_REG(SPI_BASE + spi_device->id * 0x100, SPI_FIFO_CONFIG_0);
            tmpVal = BL_SET_REG_BIT(tmpVal, SPI_DMA_RX_EN);
            BL_WR_REG(SPI_BASE + spi_device->id * 0x100, SPI_FIFO_CONFIG_0, tmpVal);
            dev->oflag |= DEVICE_OFLAG_DMA_RX;
            break;
        }

        case DEVICE_CTRL_SPI_GET_TX_FIFO :
            return SPI_GetTxFifoCount(spi_device->id);

        case DEVICE_CTRL_SPI_GET_RX_FIFO :
            return SPI_GetRxFifoCount(spi_device->id);

        case DEVICE_CTRL_SPI_CLEAR_TX_FIFO :
            return SPI_ClrTxFifo(spi_device->id);

        case DEVICE_CTRL_SPI_CLEAR_RX_FIFO :
            return SPI_ClrRxFifo(spi_device->id);

        case DEVICE_CTRL_SPI_GET_BUS_BUSY_STATUS :
            return SPI_GetBusyStatus(spi_device->id);

        default:
            break;
    }

    return 0;
}
/**
 * @brief
 *
 * @param dev
 * @param pos
 * @param buffer
 * @param size
 * @return int
 */
int spi_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size)
{
    int ret = 0;
    spi_device_t *spi_device = (spi_device_t *)dev;

    if (dev->oflag & DEVICE_OFLAG_DMA_TX) {
        struct device *dma_ch = (struct device *)spi_device->tx_dma;

        if (!dma_ch) {
            return -1;
        }

        if (spi_device->id == 0) {
            /* Set valid width for each fifo entry */
            uint32_t tmpVal;
            uint32_t SPIx = SPI_BASE;
            tmpVal = BL_RD_REG(SPIx, SPI_CONFIG);
            switch (DMA_DEV(dma_ch)->dst_width) {
                case DMA_TRANSFER_WIDTH_8BIT:
                    BL_WR_REG(SPIx, SPI_CONFIG, BL_SET_REG_BITS_VAL(tmpVal, SPI_CR_SPI_FRAME_SIZE, 0));
                    break;
                case DMA_TRANSFER_WIDTH_16BIT:
                    BL_WR_REG(SPIx, SPI_CONFIG, BL_SET_REG_BITS_VAL(tmpVal, SPI_CR_SPI_FRAME_SIZE, 1));
                    break;

                case DMA_TRANSFER_WIDTH_32BIT:
                    BL_WR_REG(SPIx, SPI_CONFIG, BL_SET_REG_BITS_VAL(tmpVal, SPI_CR_SPI_FRAME_SIZE, 3));
                    break;
                default:
                    break;
            }

            ret = dma_reload(dma_ch, (uint32_t)buffer, (uint32_t)DMA_ADDR_SPI_TDR, size);
            dma_channel_start(dma_ch);
        }
        return ret;
    } else if (dev->oflag & DEVICE_OFLAG_INT_TX) {
        return -2;
    } else {
        if (spi_device->datasize == SPI_DATASIZE_8BIT) {
            return SPI_Send_8bits(spi_device->id, (uint8_t *)buffer, size, SPI_TIMEOUT_DISABLE);
        } else if (spi_device->datasize == SPI_DATASIZE_16BIT) {
            return SPI_Send_16bits(spi_device->id, (uint16_t *)buffer, size, SPI_TIMEOUT_DISABLE);
        } else if (spi_device->datasize == SPI_DATASIZE_24BIT) {
            return SPI_Send_24bits(spi_device->id, (uint32_t *)buffer, size, SPI_TIMEOUT_DISABLE);
        } else {
            return SPI_Send_32bits(spi_device->id, (uint32_t *)buffer, size, SPI_TIMEOUT_DISABLE);
        }
    }
}
/**
 * @brief
 *
 * @param dev
 * @param pos
 * @param buffer
 * @param size
 * @return int
 */
int spi_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size)
{
    int ret = 0;
    spi_device_t *spi_device = (spi_device_t *)dev;

    if (dev->oflag & DEVICE_OFLAG_DMA_RX) {
        struct device *dma_ch = (struct device *)spi_device->rx_dma;

        if (!dma_ch) {
            return -1;
        }

        if (spi_device->id == 0) {
            /* Set valid width for each fifo entry */
            uint32_t tmpVal;
            uint32_t SPIx = SPI_BASE;
            tmpVal = BL_RD_REG(SPIx, SPI_CONFIG);
            switch (DMA_DEV(dma_ch)->src_width) {
                case DMA_TRANSFER_WIDTH_8BIT:
                    BL_WR_REG(SPIx, SPI_CONFIG, BL_SET_REG_BITS_VAL(tmpVal, SPI_CR_SPI_FRAME_SIZE, 0));
                    break;
                case DMA_TRANSFER_WIDTH_16BIT:
                    BL_WR_REG(SPIx, SPI_CONFIG, BL_SET_REG_BITS_VAL(tmpVal, SPI_CR_SPI_FRAME_SIZE, 1));
                    break;

                case DMA_TRANSFER_WIDTH_32BIT:
                    BL_WR_REG(SPIx, SPI_CONFIG, BL_SET_REG_BITS_VAL(tmpVal, SPI_CR_SPI_FRAME_SIZE, 3));
                    break;
                default:
                    break;
            }
            ret = dma_reload(dma_ch, (uint32_t)DMA_ADDR_SPI_RDR, (uint32_t)buffer, size);
            dma_channel_start(dma_ch);
        }
        return ret;
    } else if (dev->oflag & DEVICE_OFLAG_INT_TX) {
        return -2;
    } else {
        if (spi_device->datasize == SPI_DATASIZE_8BIT) {
            return SPI_Recv_8bits(spi_device->id, (uint8_t *)buffer, size, SPI_TIMEOUT_DISABLE);
        } else if (spi_device->datasize == SPI_DATASIZE_16BIT) {
            return SPI_Recv_16bits(spi_device->id, (uint16_t *)buffer, size, SPI_TIMEOUT_DISABLE);
        } else if (spi_device->datasize == SPI_DATASIZE_24BIT) {
            return SPI_Recv_24bits(spi_device->id, (uint32_t *)buffer, size, SPI_TIMEOUT_DISABLE);
        } else {
            return SPI_Recv_32bits(spi_device->id, (uint32_t *)buffer, size, SPI_TIMEOUT_DISABLE);
        }
    }
}
/**
 * @brief
 *
 * @param index
 * @param name
 * @param flag
 * @return int
 */
int spi_register(enum spi_index_type index, const char *name)
{
    struct device *dev;

    if (SPI_MAX_INDEX == 0) {
        return -DEVICE_EINVAL;
    }

    dev = &(spix_device[index].parent);

    dev->open = spi_open;
    dev->close = spi_close;
    dev->control = spi_control;
    dev->write = spi_write;
    dev->read = spi_read;

    dev->type = DEVICE_CLASS_SPI;
    dev->handle = NULL;

    return device_register(dev, name);
}

/**
 * @brief
 *
 * @param dev
 * @param buffer
 * @param size
 * @param type
 * @return int
 */
int spi_transmit(struct device *dev, void *buffer, uint32_t size, uint8_t type)
{
    spi_device_t *spi_device = (spi_device_t *)dev;

    if (type == 0) {
        return SPI_Send_8bits(spi_device->id, (uint8_t *)buffer, size, SPI_TIMEOUT_DISABLE);
    } else if (type == 1) {
        return SPI_Send_16bits(spi_device->id, (uint16_t *)buffer, size, SPI_TIMEOUT_DISABLE);
    } else if (type == 2) {
        return SPI_Send_24bits(spi_device->id, (uint32_t *)buffer, size, SPI_TIMEOUT_DISABLE);
    } else if (type == 3) {
        return SPI_Send_32bits(spi_device->id, (uint32_t *)buffer, size, SPI_TIMEOUT_DISABLE);
    }

    return -1;
}
/**
 * @brief
 *
 * @param dev
 * @param buffer
 * @param size
 * @param type
 * @return int
 */
int spi_receive(struct device *dev, void *buffer, uint32_t size, uint8_t type)
{
    spi_device_t *spi_device = (spi_device_t *)dev;

    if (type == 0) {
        return SPI_Recv_8bits(spi_device->id, (uint8_t *)buffer, size, SPI_TIMEOUT_DISABLE);
    } else if (type == 1) {
        return SPI_Recv_16bits(spi_device->id, (uint16_t *)buffer, size, SPI_TIMEOUT_DISABLE);
    } else if (type == 2) {
        return SPI_Recv_24bits(spi_device->id, (uint32_t *)buffer, size, SPI_TIMEOUT_DISABLE);
    } else if (type == 3) {
        return SPI_Recv_32bits(spi_device->id, (uint32_t *)buffer, size, SPI_TIMEOUT_DISABLE);
    }

    return -1;
}

/**
 * @brief
 *
 * @param dev
 * @param send_buf
 * @param recv_buf
 * @param length
 * @param type
 * @return int
 */
int spi_transmit_receive(struct device *dev, const void *send_buf, void *recv_buf, uint32_t length, uint8_t type)
{
    spi_device_t *spi_device = (spi_device_t *)dev;

    if (type == 0) {
        return SPI_SendRecv_8bits(spi_device->id, (uint8_t *)send_buf, (uint8_t *)recv_buf, length, SPI_TIMEOUT_DISABLE);
    } else if (type == 1) {
        return SPI_SendRecv_16bits(spi_device->id, (uint16_t *)send_buf, (uint16_t *)recv_buf, length, SPI_TIMEOUT_DISABLE);
    } else if (type == 2) {
        return SPI_SendRecv_24bits(spi_device->id, (uint32_t *)send_buf, (uint32_t *)recv_buf, length, SPI_TIMEOUT_DISABLE);
    } else if (type == 3) {
        return SPI_SendRecv_32bits(spi_device->id, (uint32_t *)send_buf, (uint32_t *)recv_buf, length, SPI_TIMEOUT_DISABLE);
    }

    return -1;
}
/**
 * @brief
 *
 * @param handle
 */
void spi_isr(spi_device_t *handle)
{
    uint32_t tmpVal;
    uint32_t SPIx = SPI_BASE + handle->id * 0x100;

    tmpVal = BL_RD_REG(SPIx, SPI_INT_STS);

    if (!handle->parent.callback) {
        return;
    }

    /* Transfer end interrupt,shared by both master and slave mode */
    if (BL_IS_REG_BIT_SET(tmpVal, SPI_END_INT) && !BL_IS_REG_BIT_SET(tmpVal, SPI_CR_SPI_END_MASK)) {
        BL_WR_REG(SPIx, SPI_INT_STS, BL_SET_REG_BIT(tmpVal, SPI_CR_SPI_END_CLR));
        handle->parent.callback(&handle->parent, NULL, 0, SPI_INT_END);
    }

    /* TX fifo ready interrupt(fifo count > fifo threshold) */
    if (BL_IS_REG_BIT_SET(tmpVal, SPI_TXF_INT) && !BL_IS_REG_BIT_SET(tmpVal, SPI_CR_SPI_TXF_MASK)) {
        handle->parent.callback(&handle->parent, NULL, 0, SPI_INT_TX_FIFO_REQ);
    }

    /*  RX fifo ready interrupt(fifo count > fifo threshold) */
    if (BL_IS_REG_BIT_SET(tmpVal, SPI_RXF_INT) && !BL_IS_REG_BIT_SET(tmpVal, SPI_CR_SPI_RXF_MASK)) {
        handle->parent.callback(&handle->parent, NULL, 0, SPI_INT_RX_FIFO_REQ);
    }

    /* Slave mode transfer time-out interrupt,triggered when bus is idle for the given value */
    if (BL_IS_REG_BIT_SET(tmpVal, SPI_STO_INT) && !BL_IS_REG_BIT_SET(tmpVal, SPI_CR_SPI_STO_MASK)) {
        BL_WR_REG(SPIx, SPI_INT_STS, BL_SET_REG_BIT(tmpVal, SPI_CR_SPI_STO_CLR));
        handle->parent.callback(&handle->parent, NULL, 0, SPI_INT_SLAVE_TIMEOUT);
    }

    /* Slave mode tx underrun error interrupt,trigged when tx is not ready during transfer */
    if (BL_IS_REG_BIT_SET(tmpVal, SPI_TXU_INT) && !BL_IS_REG_BIT_SET(tmpVal, SPI_CR_SPI_TXU_MASK)) {
        BL_WR_REG(SPIx, SPI_INT_STS, BL_SET_REG_BIT(tmpVal, SPI_CR_SPI_TXU_CLR));
        handle->parent.callback(&handle->parent, NULL, 0, SPI_INT_SLAVE_UNDERRUN);
    }

    /* TX/RX fifo overflow/underflow interrupt */
    if (BL_IS_REG_BIT_SET(tmpVal, SPI_FER_INT) && !BL_IS_REG_BIT_SET(tmpVal, SPI_CR_SPI_FER_MASK)) {
        handle->parent.callback(&handle->parent, NULL, 0, SPI_INT_FIFO_ERROR);
    }
}

#ifdef BSP_USING_SPI0
void SPI0_IRQ()
{
    spi_isr(&spix_device[SPI0_INDEX]);
}
#endif