/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_SPI.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH57x_SPI_H__
#define __CH57x_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  SPI0 interrupt bit define
 */
#define SPI0_IT_FST_BYTE RB_SPI_IE_FST_BYTE // 从机模式的首字节命令模式下，接收到首字节中断
#define SPI0_IT_FIFO_OV  RB_SPI_IE_FIFO_OV  // FIFO 溢出
#define SPI0_IT_DMA_END  RB_SPI_IE_DMA_END  // DMA 传输结束
#define SPI0_IT_FIFO_HF  RB_SPI_IE_FIFO_HF  // FIFO 使用过半
#define SPI0_IT_BYTE_END RB_SPI_IE_BYTE_END // 单字节传输完成
#define SPI0_IT_CNT_END  RB_SPI_IE_CNT_END  // 全部字节传输完成

/**
 * @brief  Configuration data mode
 */
typedef enum {
  Mode0_LowBitINFront = 0, // 模式0，低位在前
  Mode0_HighBitINFront,    // 模式0，高位在前
  Mode3_LowBitINFront,     // 模式3，低位在前
  Mode3_HighBitINFront,    // 模式3，高位在前
} ModeBitOrderTypeDef;

/**
 * @brief  Configuration SPI0 slave mode
 */
typedef enum {
  Mode_DataStream = 0, // 数据流模式
  Mose_FirstCmd,       // 首字节命令模式
} Slave_ModeTypeDef;

/**
 * @brief   主机模式默认初始化：模式0+3线全双工+8MHz
 */
void SPI0_MasterDefInit(void);

/**
 * @brief   SPI0 基准时钟配置，= d*Tsys
 *
 * @param   c       - 时钟分频系数
 */
void SPI0_CLKCfg(uint8_t c);

/**
 * @brief   设置数据流模式
 *
 * @param   m       - 数据流模式 refer to ModeBitOrderTypeDef
 */
void SPI0_DataMode(ModeBitOrderTypeDef m);

/**
 * @brief   发送单字节 (buffer)
 *
 * @param   d       - 发送字节
 */
void SPI0_MasterSendByte(uint8_t d);

/**
 * @brief   接收单字节 (buffer)
 *
 * @param   none
 */
uint8_t SPI0_MasterRecvByte(void);

/**
 * @brief   使用FIFO连续发送多字节
 *
 * @param   pbuf    - 待发送的数据内容首地址
 * @param   len     - 请求发送的数据长度，最大4095
 */
void SPI0_MasterTrans(uint8_t *pbuf, uint16_t len);

/**
 * @brief   使用FIFO连续接收多字节
 *
 * @param   pbuf    - 待接收的数据首地址
 * @param   len     - 待接收的数据长度，最大4095
 */
void SPI0_MasterRecv(uint8_t *pbuf, uint16_t len);

/**
 * @brief   DMA方式连续发送数据
 *
 * @param   pbuf    - 待发送数据起始地址,需要四字节对其
 * @param   len     - 待发送数据长度
 */
void SPI0_MasterDMATrans(uint8_t *pbuf, uint16_t len);

/**
 * @brief   DMA方式连续接收数据
 *
 * @param   pbuf    - 待接收数据存放起始地址,需要四字节对其
 * @param   len     - 待接收数据长度
 */
void SPI0_MasterDMARecv(uint8_t *pbuf, uint16_t len);

/**
 * @brief   设备模式默认初始化，建议设置MISO的GPIO对应为输入模式
 */
void SPI0_SlaveInit(void);

/**
 * @brief   加载首字节数据内容
 *
 * @param   d       - 首字节数据内容
 */
#define SetFirstData(d) (R8_SPI0_SLAVE_PRE = d)

/**
 * @brief   从机模式，发送一字节数据
 *
 * @param   d       - 待发送数据
 */
void SPI0_SlaveSendByte(uint8_t d);

/**
 * @brief   从机模式，接收一字节数据
 *
 * @return  接收到数据
 */
uint8_t SPI0_SlaveRecvByte(void);

/**
 * @brief   从机模式，发送多字节数据
 *
 * @param   pbuf    - 待发送的数据内容首地址
 * @param   len     - 请求发送的数据长度，最大4095
 */
void SPI0_SlaveTrans(uint8_t *pbuf, uint16_t len);

/**
 * @brief   从机模式，接收多字节数据
 *
 * @param   pbuf    - 接收收数据存放起始地址
 * @param   len     - 请求接收数据长度
 */
void SPI0_SlaveRecv(uint8_t *pbuf, uint16_t len);

/**
 * @brief   DMA方式连续发送数据
 *
 * @param   pbuf    - 待发送数据起始地址,需要四字节对其
 * @param   len     - 待发送数据长度
 */
void SPI0_SlaveDMATrans(uint8_t *pbuf, uint16_t len);

/**
 * @brief   DMA方式连续接收数据
 *
 * @param   pbuf    - 待接收数据存放起始地址,需要四字节对其
 * @param   len     - 待接收数据长度
 */
void SPI0_SlaveDMARecv(uint8_t *pbuf, uint16_t len);

/**
 * @brief   配置SPI0中断
 *
 * @param   s       - 使能/关闭
 * @param   f       - refer to SPI0 interrupt bit define
 */
#define SPI0_ITCfg(s, f) ((s) ? (R8_SPI0_INTER_EN |= f) : (R8_SPI0_INTER_EN &= ~f))

/**
 * @brief   获取中断标志状态，0-未置位，(!0)-触发
 *
 * @param   f       - refer to SPI0 interrupt bit define
 */
#define SPI0_GetITFlag(f) (R8_SPI0_INT_FLAG & f)

/**
 * @brief   清除当前中断标志
 *
 * @param   f       - refer to SPI0 interrupt bit define
 */
#define SPI0_ClearITFlag(f) (R8_SPI0_INT_FLAG = f)

#ifdef __cplusplus
}
#endif

#endif // __CH57x_SPI_H__
