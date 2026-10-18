/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_uart.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH57x_UART_H__
#define __CH57x_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	LINE error and status define
 */
#define STA_ERR_BREAK  RB_LSR_BREAK_ERR // 数据间隔错误
#define STA_ERR_FRAME  RB_LSR_FRAME_ERR // 数据帧错误
#define STA_ERR_PAR    RB_LSR_PAR_ERR   // 奇偶校验位出错
#define STA_ERR_FIFOOV RB_LSR_OVER_ERR  // 接收数据溢出

#define STA_TXFIFO_EMP RB_LSR_TX_FIFO_EMP // 当前发送FIFO空，可以继续填充发送数据
#define STA_TXALL_EMP  RB_LSR_TX_ALL_EMP  // 当前所有发送数据都发送完成
#define STA_RECV_DATA  RB_LSR_DATA_RDY    // 当前有接收到数据

/**
 * @brief  Configuration UART TrigByte num
 */
typedef enum {
  UART_1BYTE_TRIG = 0, // 1字节触发
  UART_2BYTE_TRIG,     // 2字节触发
  UART_4BYTE_TRIG,     // 4字节触发
  UART_7BYTE_TRIG,     // 7字节触发

} UARTByteTRIGTypeDef;

/**
 * @brief   串口默认初始化配置
 */
void UART0_DefInit(void);

/**
 * @brief   串口波特率配置
 *
 * @param   baudrate    - 波特率
 */
void UART0_BaudRateCfg(uint32_t baudrate);

/**
 * @brief   串口字节触发中断配置
 *
 * @param   b       - 触发字节数 refer to UARTByteTRIGTypeDef
 */
void UART0_ByteTrigCfg(UARTByteTRIGTypeDef b);

/**
 * @brief   串口中断配置
 *
 * @param   s       - 中断控制状态，是否使能相应中断
 * @param   i       - 中断类型
 *                    RB_IER_MODEM_CHG  - 调制解调器输入状态变化中断使能位（仅 UART0 支持）
 *                    RB_IER_LINE_STAT  - 接收线路状态中断
 *                    RB_IER_THR_EMPTY  - 发送保持寄存器空中断
 *                    RB_IER_RECV_RDY   - 接收数据中断
 */
void UART0_INTCfg(FunctionalState s, uint8_t i);

/**
 * @brief   串口软件复位
 */
void UART0_Reset(void);

/**
 * @brief   清除当前接收FIFO
 */
#define UART0_CLR_RXFIFO() (R8_UART0_FCR |= RB_FCR_RX_FIFO_CLR)

/**
 * @brief   清除当前发送FIFO
 */
#define UART0_CLR_TXFIFO() (R8_UART0_FCR |= RB_FCR_TX_FIFO_CLR)

/**
 * @brief   获取当前中断标志
 *
 * @return  当前中断标志
 */
#define UART0_GetITFlag() (R8_UART0_IIR & RB_IIR_INT_MASK)

/**
 * @brief   获取当前通讯状态
 *
 * @return  refer to LINE error and status define
 */
#define UART0_GetLinSTA() (R8_UART0_LSR)

/**
 * @brief   串口单字节发送
 *
 * @param   b       待发送的字节
 */
#define UART0_SendByte(b) (R8_UART0_THR = b)

/**
 * @brief   串口多字节发送
 *
 * @param   buf     - 待发送的数据内容首地址
 * @param   l       - 待发送的数据长度
 */
void UART0_SendString(uint8_t *buf, uint16_t l);

/**
 * @brief   串口读取单字节
 *
 * @return  读取到的单字节
 */
#define UART0_RecvByte() (R8_UART0_RBR)

/**
 * @brief   串口读取多字节
 *
 * @param   buf     - 读取数据存放缓存区首地址
 *
 * @return  读取数据长度
 */
uint16_t UART0_RecvString(uint8_t *buf);

/**
 * @brief   串口默认初始化配置
 */
void UART1_DefInit(void);

/**
 * @brief   串口波特率配置
 *
 * @param   baudrate    - 波特率
 */
void UART1_BaudRateCfg(uint32_t baudrate);

/**
 * @brief   串口字节触发中断配置
 *
 * @param   b       - 触发字节数 refer to UARTByteTRIGTypeDef
 */
void UART1_ByteTrigCfg(UARTByteTRIGTypeDef b);

/**
 * @brief   串口中断配置
 *
 * @param   s       - 中断控制状态，是否使能相应中断
 * @param   i       - 中断类型
 *                    RB_IER_MODEM_CHG  - 调制解调器输入状态变化中断使能位（仅 UART0 支持）
 *                    RB_IER_LINE_STAT  - 接收线路状态中断
 *                    RB_IER_THR_EMPTY  - 发送保持寄存器空中断
 *                    RB_IER_RECV_RDY   - 接收数据中断
 */
void UART1_INTCfg(FunctionalState s, uint8_t i);

/**
 * @brief   串口软件复位
 */
void UART1_Reset(void);

/**
 * @brief   清除当前接收FIFO
 */
#define UART1_CLR_RXFIFO() (R8_UART1_FCR |= RB_FCR_RX_FIFO_CLR)

/**
 * @brief   清除当前发送FIFO
 */
#define UART1_CLR_TXFIFO() (R8_UART1_FCR |= RB_FCR_TX_FIFO_CLR)

/**
 * @brief   获取当前中断标志
 *
 * @return  当前中断标志
 */
#define UART1_GetITFlag() (R8_UART1_IIR & RB_IIR_INT_MASK)

/**
 * @brief   获取当前通讯状态
 *
 * @return  refer to LINE error and status define
 */
#define UART1_GetLinSTA() (R8_UART1_LSR)

/**
 * @brief   串口单字节发送
 *
 * @param   b       待发送的字节
 */
#define UART1_SendByte(b) (R8_UART1_THR = b)

/**
 * @brief   串口多字节发送
 *
 * @param   buf     - 待发送的数据内容首地址
 * @param   l       - 待发送的数据长度
 */
void UART1_SendString(uint8_t *buf, uint16_t l);

/**
 * @brief   串口读取单字节
 *
 * @return  读取到的单字节
 */
#define UART1_RecvByte() (R8_UART1_RBR)

/**
 * @brief   串口读取多字节
 *
 * @param   buf     - 读取数据存放缓存区首地址
 *
 * @return  读取数据长度
 */
uint16_t UART1_RecvString(uint8_t *buf);

/**
 * @brief   串口默认初始化配置
 */
void UART2_DefInit(void);

/**
 * @brief   串口波特率配置
 *
 * @param   baudrate    - 波特率
 */
void UART2_BaudRateCfg(uint32_t baudrate);

/**
 * @brief   串口字节触发中断配置
 *
 * @param   b       - 触发字节数 refer to UARTByteTRIGTypeDef
 */
void UART2_ByteTrigCfg(UARTByteTRIGTypeDef b);

/**
 * @brief   串口中断配置
 *
 * @param   s       - 中断控制状态，是否使能相应中断
 * @param   i       - 中断类型
 *                    RB_IER_MODEM_CHG  - 调制解调器输入状态变化中断使能位（仅 UART0 支持）
 *                    RB_IER_LINE_STAT  - 接收线路状态中断
 *                    RB_IER_THR_EMPTY  - 发送保持寄存器空中断
 *                    RB_IER_RECV_RDY   - 接收数据中断
 */
void UART2_INTCfg(FunctionalState s, uint8_t i);

/**
 * @brief   串口软件复位
 */
void UART2_Reset(void);

/**
 * @brief   清除当前接收FIFO
 */
#define UART2_CLR_RXFIFO() (R8_UART2_FCR |= RB_FCR_RX_FIFO_CLR)

/**
 * @brief   清除当前发送FIFO
 */
#define UART2_CLR_TXFIFO() (R8_UART2_FCR |= RB_FCR_TX_FIFO_CLR)

/**
 * @brief   获取当前中断标志
 *
 * @return  当前中断标志
 */
#define UART2_GetITFlag() (R8_UART2_IIR & RB_IIR_INT_MASK)

/**
 * @brief   获取当前通讯状态
 *
 * @return  refer to LINE error and status define
 */
#define UART2_GetLinSTA() (R8_UART2_LSR)

/**
 * @brief   串口单字节发送
 *
 * @param   b       待发送的字节
 */
#define UART2_SendByte(b) (R8_UART2_THR = b)

/**
 * @brief   串口多字节发送
 *
 * @param   buf     - 待发送的数据内容首地址
 * @param   l       - 待发送的数据长度
 */
void UART2_SendString(uint8_t *buf, uint16_t l);

/**
 * @brief   串口读取单字节
 *
 * @return  读取到的单字节
 */
#define UART2_RecvByte() (R8_UART2_RBR)

/**
 * @brief   串口读取多字节
 *
 * @param   buf     - 读取数据存放缓存区首地址
 *
 * @return  读取数据长度
 */
uint16_t UART2_RecvString(uint8_t *buf);

/**
 * @brief   串口默认初始化配置
 */
void UART3_DefInit(void);

/**
 * @brief   串口波特率配置
 *
 * @param   baudrate    - 波特率
 */
void UART3_BaudRateCfg(uint32_t baudrate);

/**
 * @brief   串口字节触发中断配置
 *
 * @param   b       - 触发字节数 refer to UARTByteTRIGTypeDef
 */
void UART3_ByteTrigCfg(UARTByteTRIGTypeDef b);

/**
 * @brief   串口中断配置
 *
 * @param   s       - 中断控制状态，是否使能相应中断
 * @param   i       - 中断类型
 *                    RB_IER_MODEM_CHG  - 调制解调器输入状态变化中断使能位（仅 UART0 支持）
 *                    RB_IER_LINE_STAT  - 接收线路状态中断
 *                    RB_IER_THR_EMPTY  - 发送保持寄存器空中断
 *                    RB_IER_RECV_RDY   - 接收数据中断
 */
void UART3_INTCfg(FunctionalState s, uint8_t i);

/**
 * @brief   串口软件复位
 */
void UART3_Reset(void);

/**
 * @brief   清除当前接收FIFO
 */
#define UART3_CLR_RXFIFO() (R8_UART3_FCR |= RB_FCR_RX_FIFO_CLR)

/**
 * @brief   清除当前发送FIFO
 */
#define UART3_CLR_TXFIFO() (R8_UART3_FCR |= RB_FCR_TX_FIFO_CLR)

/**
 * @brief   获取当前中断标志
 *
 * @return  当前中断标志
 */
#define UART3_GetITFlag() (R8_UART3_IIR & RB_IIR_INT_MASK)

/**
 * @brief   获取当前通讯状态
 *
 * @return  refer to LINE error and status define
 */
#define UART3_GetLinSTA() (R8_UART3_LSR)

/**
 * @brief   串口单字节发送
 *
 * @param   b       待发送的字节
 */
#define UART3_SendByte(b) (R8_UART3_THR = b)

/**
 * @brief   串口多字节发送
 *
 * @param   buf     - 待发送的数据内容首地址
 * @param   l       - 待发送的数据长度
 */
void UART3_SendString(uint8_t *buf, uint16_t l);

/**
 * @brief   串口读取单字节
 *
 * @return  读取到的单字节
 */
#define UART3_RecvByte() (R8_UART3_RBR)

/**
 * @brief   串口读取多字节
 *
 * @param   buf     - 读取数据存放缓存区首地址
 *
 * @return  读取数据长度
 */
uint16_t UART3_RecvString(uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif // __CH57x_UART_H__
