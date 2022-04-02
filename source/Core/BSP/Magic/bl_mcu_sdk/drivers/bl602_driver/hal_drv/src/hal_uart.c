/**
 * *****************************************************************************
 * @file hal_uart.c
 * @version 0.1
 * @date 2021-03-01
 * @brief
 * *****************************************************************************
 * @attention
 *
 *  <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *    3. Neither the name of Bouffalo Lab nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * *****************************************************************************
 */
#include "hal_uart.h"
#include "hal_dma.h"
#include "hal_clock.h"
#include "bl602_uart.h"
#include "bl602_glb.h"
#include "uart_config.h"

#ifdef BSP_USING_UART0
void UART0_IRQ(void);
#endif
#ifdef BSP_USING_UART1
void UART1_IRQ(void);
#endif

uart_device_t uartx_device[UART_MAX_INDEX] = {
#ifdef BSP_USING_UART0
    UART0_CONFIG,
#endif
#ifdef BSP_USING_UART1
    UART1_CONFIG,
#endif
};
/**
 * @brief
 *
 * @param dev
 * @param oflag
 * @return int
 */
int uart_open(struct device *dev, uint16_t oflag)
{
    uart_device_t *uart_device = (uart_device_t *)dev;
    UART_FifoCfg_Type fifoCfg = { 0 };
    UART_CFG_Type uart_cfg = { 0 };

    /* disable all interrupt */
    UART_IntMask(uart_device->id, UART_INT_ALL, MASK);
    /* disable uart before config */
    UART_Disable(uart_device->id, UART_TXRX);

    uint32_t uart_clk = peripheral_clock_get(PERIPHERAL_CLOCK_UART);

    uart_cfg.baudRate = uart_device->baudrate;
    uart_cfg.dataBits = uart_device->databits;
    uart_cfg.stopBits = uart_device->stopbits;
    uart_cfg.parity = uart_device->parity;
    uart_cfg.uartClk = uart_clk;
    uart_cfg.ctsFlowControl = UART_CTS_FLOWCONTROL_ENABLE;
    uart_cfg.rtsSoftwareControl = UART_RTS_FLOWCONTROL_ENABLE;
    uart_cfg.byteBitInverse = UART_MSB_FIRST_ENABLE;

    /* uart init with default configuration */
    UART_Init(uart_device->id, &uart_cfg);

    /* Enable tx free run mode */
    UART_TxFreeRun(uart_device->id, ENABLE);

    /* Set rx time-out value */
    UART_SetRxTimeoutValue(uart_device->id, UART_DEFAULT_RTO_TIMEOUT);

    fifoCfg.txFifoDmaThreshold = uart_device->fifo_threshold;
    fifoCfg.txFifoDmaEnable = DISABLE;
    fifoCfg.rxFifoDmaThreshold = uart_device->fifo_threshold;
    fifoCfg.rxFifoDmaEnable = DISABLE;

    if (oflag & DEVICE_OFLAG_STREAM_TX) {
    }

    if ((oflag & DEVICE_OFLAG_INT_TX) || (oflag & DEVICE_OFLAG_INT_RX)) {
#ifdef BSP_USING_UART0

        if (uart_device->id == UART0_ID) {
            Interrupt_Handler_Register(UART0_IRQn, UART0_IRQ);
        }

#endif
#ifdef BSP_USING_UART1

        if (uart_device->id == UART1_ID) {
            Interrupt_Handler_Register(UART1_IRQn, UART1_IRQ);
        }

#endif
    }

    if (oflag & DEVICE_OFLAG_DMA_TX) {
        fifoCfg.txFifoDmaEnable = ENABLE;
    }

    if (oflag & DEVICE_OFLAG_DMA_RX) {
        fifoCfg.rxFifoDmaEnable = ENABLE;
    }

    UART_FifoConfig(uart_device->id, &fifoCfg);
    /* enable uart */
    UART_Enable(uart_device->id, UART_TXRX);
    return 0;
}
/**
 * @brief
 *
 * @param dev
 * @return int
 */
int uart_close(struct device *dev)
{
    uart_device_t *uart_device = (uart_device_t *)dev;

    UART_Disable(uart_device->id, UART_TXRX);
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
int uart_control(struct device *dev, int cmd, void *args)
{
    uart_device_t *uart_device = (uart_device_t *)dev;

    switch (cmd) {
        case DEVICE_CTRL_SET_INT /* constant-expression */: {
            uint32_t offset = __builtin_ctz((uint32_t)args);

            while ((0 <= offset) && (offset < 9)) {
                if ((uint32_t)args & (1 << offset)) {
                    UART_IntMask(uart_device->id, offset, UNMASK);
                }

                offset++;
            }

            if (uart_device->id == UART0_ID) {
                CPU_Interrupt_Enable(UART0_IRQn);
            } else if (uart_device->id == UART1_ID) {
                CPU_Interrupt_Enable(UART1_IRQn);
            }

            break;
        }

        case DEVICE_CTRL_CLR_INT /* constant-expression */: {
            uint32_t offset = __builtin_ctz((uint32_t)args);

            while ((0 <= offset) && (offset < 9)) {
                if ((uint32_t)args & (1 << offset)) {
                    UART_IntMask(uart_device->id, offset, MASK);
                }

                offset++;
            }

            if (uart_device->id == UART0_ID) {
                CPU_Interrupt_Disable(UART0_IRQn);
            } else if (uart_device->id == UART1_ID) {
                CPU_Interrupt_Disable(UART1_IRQn);
            }

            break;
        }

        case DEVICE_CTRL_GET_INT /* constant-expression */:
            /* code */
            break;

        case DEVICE_CTRL_CONFIG /* constant-expression */: {
            uart_param_cfg_t *cfg = (uart_param_cfg_t *)args;
            UART_CFG_Type uart_cfg = { 0 };

            /* Disable uart before config */
            UART_Disable(uart_device->id, UART_TXRX);

            uart_cfg.uartClk = 160 * 1000 * 1000;
            uart_cfg.baudRate = cfg->baudrate;
            uart_cfg.stopBits = cfg->stopbits;
            uart_cfg.parity = cfg->parity;
            uart_cfg.dataBits = cfg->databits;
            uart_cfg.ctsFlowControl = UART_CTS_FLOWCONTROL_ENABLE;
            uart_cfg.rtsSoftwareControl = UART_RTS_FLOWCONTROL_ENABLE;
            uart_cfg.byteBitInverse = UART_MSB_FIRST_ENABLE;
            UART_Init(uart_device->id, &uart_cfg);
#ifdef BSP_USING_UART0

            if (uart_device->id == UART0_ID) {
                Interrupt_Handler_Register(UART0_IRQn, UART0_IRQ);
            }
#endif
#ifdef BSP_USING_UART1

            if (uart_device->id == UART1_ID) {
                Interrupt_Handler_Register(UART1_IRQn, UART1_IRQ);
            }
#endif
            /* Enable uart */
            UART_Enable(uart_device->id, UART_TXRX);
            break;
        }

        case DEVICE_CTRL_GET_CONFIG /* constant-expression */:
            break;

        case DEVICE_CTRL_ATTACH_TX_DMA /* constant-expression */:
            uart_device->tx_dma = (struct device *)args;
            break;

        case DEVICE_CTRL_ATTACH_RX_DMA /* constant-expression */:
            uart_device->rx_dma = (struct device *)args;
            break;

        case DEVICE_CTRL_UART_GET_TX_FIFO /* constant-expression */:
            return UART_GetTxFifoCount(uart_device->id);

        case DEVICE_CTRL_UART_GET_RX_FIFO /* constant-expression */:
            return UART_GetRxFifoCount(uart_device->id);

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
int uart_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size)
{
    int ret = -1;
    uart_device_t *uart_device = (uart_device_t *)dev;

    if (dev->oflag & DEVICE_OFLAG_DMA_TX) {
        struct device *dma_ch = (struct device *)uart_device->tx_dma;

        if (!dma_ch) {
            return -1;
        }

        if (uart_device->id == 0) {
            ret = dma_reload(dma_ch, (uint32_t)buffer, (uint32_t)DMA_ADDR_UART0_TDR, size);
            dma_channel_start(dma_ch);
        } else if (uart_device->id == 1) {
            ret = dma_reload(dma_ch, (uint32_t)buffer, (uint32_t)DMA_ADDR_UART1_TDR, size);
            dma_channel_start(dma_ch);
        }
        return ret;
    } else if (dev->oflag & DEVICE_OFLAG_INT_TX) {
        return -2;
    } else
        return UART_SendData(uart_device->id, (uint8_t *)buffer, size);
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
int uart_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size)
{
    uart_device_t *uart_device = (uart_device_t *)dev;

    if (dev->oflag & DEVICE_OFLAG_DMA_RX) {
        struct device *dma_ch = (struct device *)uart_device->rx_dma;

        if (!dma_ch) {
            return -1;
        }
        return 0;
    } else if (dev->oflag & DEVICE_OFLAG_INT_RX) {
        return -2;
    } else {
        return UART_ReceiveData(uart_device->id, (uint8_t *)buffer, size);
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
int uart_register(enum uart_index_type index, const char *name)
{
    struct device *dev;

    if (UART_MAX_INDEX == 0) {
        return -DEVICE_EINVAL;
    }

    dev = &(uartx_device[index].parent);

    dev->open = uart_open;
    dev->close = uart_close;
    dev->control = uart_control;
    dev->write = uart_write;
    dev->read = uart_read;

    dev->type = DEVICE_CLASS_UART;
    dev->handle = NULL;

    return device_register(dev, name);
}
/**
 * @brief
 *
 * @param handle
 */
void uart_isr(uart_device_t *handle)
{
    uint32_t tmpVal = 0;
    uint32_t maskVal = 0;
    uint32_t UARTx = (UART0_BASE + handle->id * 0x100);
    uint16_t len;

    tmpVal = BL_RD_REG(UARTx, UART_INT_STS);
    maskVal = BL_RD_REG(UARTx, UART_INT_MASK);

    if (!handle->parent.callback) {
        return;
    }

    /* Length of uart tx data transfer arrived interrupt */
    if (BL_IS_REG_BIT_SET(tmpVal, UART_UTX_END_INT) && !BL_IS_REG_BIT_SET(maskVal, UART_CR_UTX_END_MASK)) {
        BL_WR_REG(UARTx, UART_INT_CLEAR, 0x1);
        handle->parent.callback(&handle->parent, NULL, 0, UART_EVENT_TX_END);
    }

    /* Length of uart rx data transfer arrived interrupt */
    if (BL_IS_REG_BIT_SET(tmpVal, UART_URX_END_INT) && !BL_IS_REG_BIT_SET(maskVal, UART_CR_URX_END_MASK)) {
        BL_WR_REG(UARTx, UART_INT_CLEAR, 0x2);
        handle->parent.callback(&handle->parent, NULL, 0, UART_EVENT_RX_END);
    }

    /* Tx fifo ready interrupt,auto-cleared when data is pushed */
    if (BL_IS_REG_BIT_SET(tmpVal, UART_UTX_FIFO_INT) && !BL_IS_REG_BIT_SET(maskVal, UART_CR_UTX_FIFO_MASK)) {
        handle->parent.callback(&handle->parent, NULL, 0, UART_EVENT_TX_FIFO);
    }

    /* Rx fifo ready interrupt,auto-cleared when data is popped */
    if (BL_IS_REG_BIT_SET(tmpVal, UART_URX_FIFO_INT) && !BL_IS_REG_BIT_SET(maskVal, UART_CR_URX_FIFO_MASK)) {
        uint8_t buffer[UART_FIFO_MAX_LEN];
        len = UART_ReceiveData(handle->id, buffer, UART_FIFO_MAX_LEN);
        handle->parent.callback(&handle->parent, &buffer[0], len, UART_EVENT_RX_FIFO);
    }

    /* Rx time-out interrupt */
    if (BL_IS_REG_BIT_SET(tmpVal, UART_URX_RTO_INT) && !BL_IS_REG_BIT_SET(maskVal, UART_CR_URX_RTO_MASK)) {
        uint8_t buffer[UART_FIFO_MAX_LEN];
        len = UART_ReceiveData(handle->id, buffer, UART_FIFO_MAX_LEN);
        handle->parent.callback(&handle->parent, &buffer[0], len, UART_EVENT_RTO);
        BL_WR_REG(UARTx, UART_INT_CLEAR, 0x10);
    }

    /* Rx parity check error interrupt */
    if (BL_IS_REG_BIT_SET(tmpVal, UART_URX_PCE_INT) && !BL_IS_REG_BIT_SET(maskVal, UART_CR_URX_PCE_MASK)) {
        BL_WR_REG(UARTx, UART_INT_CLEAR, 0x20);
        handle->parent.callback(&handle->parent, NULL, 0, UART_PCE_IT);
    }

    /* Tx fifo overflow/underflow error interrupt */
    if (BL_IS_REG_BIT_SET(tmpVal, UART_UTX_FER_INT) && !BL_IS_REG_BIT_SET(maskVal, UART_CR_UTX_FER_MASK)) {
        handle->parent.callback(&handle->parent, NULL, 0, UART_TX_FER_IT);
    }

    /* Rx fifo overflow/underflow error interrupt */
    if (BL_IS_REG_BIT_SET(tmpVal, UART_URX_FER_INT) && !BL_IS_REG_BIT_SET(maskVal, UART_CR_URX_FER_MASK)) {
        handle->parent.callback(&handle->parent, NULL, 0, UART_RX_FER_IT);
    }
}

#ifdef BSP_USING_UART0
/**
 * @brief
 *
 */
void UART0_IRQ(void)
{
    uart_isr(&uartx_device[UART0_INDEX]);
}
#endif
#ifdef BSP_USING_UART1
/**
 * @brief
 *
 */
void UART1_IRQ(void)
{
    uart_isr(&uartx_device[UART1_INDEX]);
}
#endif