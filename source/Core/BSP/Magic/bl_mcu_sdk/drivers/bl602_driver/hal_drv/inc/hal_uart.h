/**
 * *****************************************************************************
 * @file hal_uart.h
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
#ifndef __HAL_UART__H__
#define __HAL_UART__H__

#include "hal_common.h"
#include "drv_device.h"
#include "bl602_config.h"

#define UART_FIFO_LEN 32

#define DEVICE_CTRL_UART_GET_TX_FIFO   0x10
#define DEVICE_CTRL_UART_GET_RX_FIFO   0x11
#define DEVICE_CTRL_UART_ATTACH_TX_DMA 0x12
#define DEVICE_CTRL_UART_ATTACH_RX_DMA 0x13

enum uart_index_type {
#ifdef BSP_USING_UART0
    UART0_INDEX,
#endif
#ifdef BSP_USING_UART1
    UART1_INDEX,
#endif
#ifdef BSP_USING_UART2
    UART2_INDEX,
#endif
#ifdef BSP_USING_UART3
    UART3_INDEX,
#endif
#ifdef BSP_USING_UART4
    UART4_INDEX,
#endif
    UART_MAX_INDEX
};

/*!
 *  @brief    UART data length settings
 *
 *  This enumeration defines the UART data lengths.
 */
typedef enum {
    UART_DATA_LEN_5 = 0, /*!< Data length is 5 bits */
    UART_DATA_LEN_6 = 1, /*!< Data length is 6 bits */
    UART_DATA_LEN_7 = 2, /*!< Data length is 7 bits */
    UART_DATA_LEN_8 = 3  /*!< Data length is 8 bits */
} uart_databits_t;

/*!
 *  @brief    UART stop bit settings
 *
 *  This enumeration defines the UART stop bits.
 */
typedef enum {
    UART_STOP_ZERO_D_FIVE = 0, /*!< One stop bit */
    UART_STOP_ONE = 1,         /*!< One stop bit */
    UART_STOP_ONE_D_FIVE = 2,  /*!< 1.5 stop bit */
    UART_STOP_TWO = 3          /*!< Two stop bits */
} uart_stopbits_t;

/*!
 *  @brief    UART parity type settings
 *
 *  This enumeration defines the UART parity types.
 */
typedef enum {
    UART_PAR_NONE = 0, /*!< No parity */
    UART_PAR_ODD = 1,  /*!< Parity bit is odd */
    UART_PAR_EVEN = 2, /*!< Parity bit is even */
} uart_parity_t;

enum uart_event_type {
    UART_EVENT_TX_END,
    UART_EVENT_TX_FIFO,
    UART_EVENT_RX_END,
    UART_EVENT_RX_FIFO,
    UART_EVENT_RTO,
    UART_EVENT_UNKNOWN
};

enum uart_it_type {
    UART_TX_END_IT = 1 << 0,
    UART_RX_END_IT = 1 << 1,
    UART_TX_FIFO_IT = 1 << 2,
    UART_RX_FIFO_IT = 1 << 3,
    UART_RTO_IT = 1 << 4,
    UART_PCE_IT = 1 << 5,
    UART_TX_FER_IT = 1 << 6,
    UART_RX_FER_IT = 1 << 7,
    UART_ALL_IT = 1 << 8
};

typedef struct
{
    uint32_t baudrate;
    uart_databits_t databits;
    uart_stopbits_t stopbits;
    uart_parity_t parity;
} uart_param_cfg_t;

typedef struct uart_device {
    struct device parent;
    uint8_t id;
    uint32_t baudrate;
    uart_databits_t databits;
    uart_stopbits_t stopbits;
    uart_parity_t parity;
    uint8_t fifo_threshold;
    void *tx_dma;
    void *rx_dma;
} uart_device_t;

#define UART_DEV(dev) ((uart_device_t *)dev)
int uart_register(enum uart_index_type index, const char *name);

#endif