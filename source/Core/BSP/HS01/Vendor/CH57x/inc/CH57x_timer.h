/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_timer.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH57x_TIMER_H__
#define __CH57x_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define DataBit_25 (1 << 25)

/**
 * @brief  TMR interrupt bit define
 */

#define TMR0_3_IT_CYC_END  0x01 // 周期结束标志：捕捉-超时，定时-周期结束，PWM-周期结束
#define TMR0_3_IT_DATA_ACT 0x02 // 数据有效标志：捕捉-新数据，PWM-有效电平结束
#define TMR0_3_IT_FIFO_HF  0x04 // FIFO 使用过半：捕捉- FIFO>=4， PWM- FIFO<4
#define TMR1_2_IT_DMA_END  0x08 // DMA 结束，支持TMR1和TMR2
#define TMR0_3_IT_FIFO_OV  0x10 // FIFO 溢出：捕捉- FIFO满， PWM- FIFO空

/**
 * @brief  Configuration PWM effective level repeat times
 */
typedef enum {
  PWM_Times_1 = 0, // PWM 有效输出重复1次数
  PWM_Times_4,     // PWM 有效输出重复4次数
  PWM_Times_8,     // PWM 有效输出重复8次数
  PWM_Times_16,    // PWM 有效输出重复16次数
} PWM_RepeatTsTypeDef;

/**
 * @brief  Configuration Cap mode
 */
typedef enum {
  CAP_NULL = 0,         // 不捕捉 & 不计数
  Edge_To_Edge,         // 任意边沿之间  &  计数任意边沿
  FallEdge_To_FallEdge, // 下降沿到下降沿  & 计数下降沿
  RiseEdge_To_RiseEdge, // 上升沿到上升沿  &  计数上升沿
} CapModeTypeDef;

/**
 * @brief  Configuration DMA mode
 */
typedef enum {
  Mode_Single = 0, // 单次模式
  Mode_LOOP,       // 循环模式
} DMAModeTypeDef;

/**
 * @brief   定时功能初始化
 *
 * @param   t       - 定时时间，基于当前系统时钟Tsys, 最长定时周期 67108864
 */
void TMR0_TimerInit(uint32_t t);

/**
 * @brief   获取当前定时器值，最大67108864
 *
 * @return  当前定时器值
 */
#define TMR0_GetCurrentTimer() R32_TMR0_COUNT

/**
 * @brief   边沿计数功能初始化
 *
 * @param   cap     - 采集计数类型
 */
void TMR0_EXTSingleCounterInit(CapModeTypeDef cap);

/**
 * @brief   设置计数统计溢出大小，最大67108862
 *
 * @param   cyc     - 计数统计溢出大小
 */
#define TMR0_CountOverflowCfg(cyc) (R32_TMR0_CNT_END = (cyc + 2))

/**
 * @brief   获取当前计数值，最大67108862
 *
 * @return  当前计数值
 */
#define TMR0_GetCurrentCount() R32_TMR0_COUNT

/**
 * @brief   PWM0 通道输出波形周期配置, 最大67108864
 *
 * @param   cyc     - 输出波形周期
 */
#define TMR0_PWMCycleCfg(cyc) (R32_TMR0_CNT_END = cyc)

/**
 * @brief   PWM 输出初始化
 *
 * @param   pr      - select wave polar, refer to PWMX_PolarTypeDef
 * @param   ts      - set pwm repeat times, refer to PWM_RepeatTsTypeDef
 */
void TMR0_PWMInit(PWMX_PolarTypeDef pr, PWM_RepeatTsTypeDef ts);

/**
 * @brief   PWM0 有效数据脉宽, 最大67108864
 *
 * @param   d       - 有效数据脉宽
 */
#define TMR0_PWMActDataWidth(d) (R32_TMR0_FIFO = d)

/**
 * @brief   CAP0 捕捉电平超时配置, 最大33554432
 *
 * @param   cyc     - 捕捉电平超时
 */
#define TMR0_CAPTimeoutCfg(cyc) (R32_TMR0_CNT_END = cyc)

/**
 * @brief   外部信号捕捉功能初始化
 *
 * @param   cap     - select capture mode, refer to CapModeTypeDef
 */
void TMR0_CapInit(CapModeTypeDef cap);

/**
 * @brief   获取脉冲数据
 *
 * @return  脉冲数据
 */
#define TMR0_CAPGetData() R32_TMR0_FIFO

/**
 * @brief   获取当前已捕获数据个数
 *
 * @return  当前已捕获数据个数
 */
#define TMR0_CAPDataCounter() R8_TMR0_FIFO_COUNT

/**
 * @brief   关闭 TMR0
 */
#define TMR0_Disable() (R8_TMR0_CTRL_MOD &= ~RB_TMR_COUNT_EN)

/**
 * @brief   开启 TMR0
 */
#define TMR0_Enable() (R8_TMR0_CTRL_MOD |= RB_TMR_COUNT_EN)

/**
 * @brief   中断配置
 *
 * @param   s       - 使能/关闭
 * @param   f       - refer to TMR interrupt bit define
 */
#define TMR0_ITCfg(s, f) ((s) ? (R8_TMR0_INTER_EN |= f) : (R8_TMR0_INTER_EN &= ~f))

/**
 * @brief   清除中断标志
 *
 * @param   f       - refer to TMR interrupt bit define
 */
#define TMR0_ClearITFlag(f) (R8_TMR0_INT_FLAG = f)

/**
 * @brief   查询中断标志状态
 *
 * @param   f       - refer to TMR interrupt bit define
 */
#define TMR0_GetITFlag(f) (R8_TMR0_INT_FLAG & f)

/**
 * @brief   定时功能初始化
 *
 * @param   t       - 定时时间，基于当前系统时钟Tsys, 最长定时周期 67108864
 */
void TMR1_TimerInit(uint32_t t);

/**
 * @brief   获取当前定时器值，最大67108864
 *
 * @return  当前定时器值
 */
#define TMR1_GetCurrentTimer() R32_TMR1_COUNT

/**
 * @brief   边沿计数功能初始化
 *
 * @param   cap     - 采集计数类型
 */
void TMR1_EXTSingleCounterInit(CapModeTypeDef cap);

/**
 * @brief   设置计数统计溢出大小，最大67108862
 *
 * @param   cyc     - 计数统计溢出大小
 */
#define TMR1_CountOverflowCfg(cyc) (R32_TMR1_CNT_END = (cyc + 2))

/**
 * @brief   获取当前计数值，最大67108862
 *
 * @return  当前计数值
 */
#define TMR1_GetCurrentCount() R32_TMR1_COUNT

/**
 * @brief   PWM1 通道输出波形周期配置, 最大67108864
 *
 * @param   cyc     - 输出波形周期
 */
#define TMR1_PWMCycleCfg(cyc) (R32_TMR1_CNT_END = cyc)

/**
 * @brief   PWM 输出初始化
 *
 * @param   pr      - select wave polar, refer to PWMX_PolarTypeDef
 * @param   ts      - set pwm repeat times, refer to PWM_RepeatTsTypeDef
 */
void TMR1_PWMInit(PWMX_PolarTypeDef pr, PWM_RepeatTsTypeDef ts);

/**
 * @brief   PWM1 有效数据脉宽, 最大67108864
 *
 * @param   d       - 有效数据脉宽
 */
#define TMR1_PWMActDataWidth(d) (R32_TMR1_FIFO = d)

/**
 * @brief   CAP1 捕捉电平超时配置, 最大33554432
 *
 * @param   cyc     - 捕捉电平超时
 */
#define TMR1_CAPTimeoutCfg(cyc) (R32_TMR1_CNT_END = cyc)

/**
 * @brief   外部信号捕捉功能初始化
 *
 * @param   cap     - select capture mode, refer to CapModeTypeDef
 */
void TMR1_CapInit(CapModeTypeDef cap);

/**
 * @brief   获取脉冲数据
 *
 * @return  脉冲数据
 */
#define TMR1_CAPGetData() R32_TMR1_FIFO

/**
 * @brief   获取当前已捕获数据个数
 *
 * @return  当前已捕获数据个数
 */
#define TMR1_CAPDataCounter() R8_TMR1_FIFO_COUNT

/**
 * @brief   配置DMA功能
 *
 * @param   s           - 是否打开DMA功能
 * @param   startAddr   - DMA 起始地址
 * @param   endAddr     - DMA 结束地址
 * @param   m           - 配置DMA模式
 */
void TMR1_DMACfg(uint8_t s, uint16_t startAddr, uint16_t endAddr, DMAModeTypeDef m);

/**
 * @brief   关闭 TMR1
 */
#define TMR1_Disable() (R8_TMR1_CTRL_MOD &= ~RB_TMR_COUNT_EN)

/**
 * @brief   开启 TMR1
 */
#define TMR1_Enable() (R8_TMR1_CTRL_MOD |= RB_TMR_COUNT_EN)

/**
 * @brief   中断配置
 *
 * @param   s       - 使能/关闭
 * @param   f       - refer to TMR interrupt bit define
 */
#define TMR1_ITCfg(s, f) ((s) ? (R8_TMR1_INTER_EN |= f) : (R8_TMR1_INTER_EN &= ~f))

/**
 * @brief   清除中断标志
 *
 * @param   f       - refer to TMR interrupt bit define
 */
#define TMR1_ClearITFlag(f) (R8_TMR1_INT_FLAG = f)

/**
 * @brief   查询中断标志状态
 *
 * @param   f       - refer to TMR interrupt bit define
 */
#define TMR1_GetITFlag(f) (R8_TMR1_INT_FLAG & f)

/**
 * @brief   定时功能初始化
 *
 * @param   t       - 定时时间，基于当前系统时钟Tsys, 最长定时周期 67108864
 */
void TMR2_TimerInit(uint32_t t);

/**
 * @brief   获取当前定时器值，最大67108864
 *
 * @return  当前定时器值
 */
#define TMR2_GetCurrentTimer() R32_TMR2_COUNT

/**
 * @brief   边沿计数功能初始化
 *
 * @param   cap     - 采集计数类型
 */
void TMR2_EXTSingleCounterInit(CapModeTypeDef cap);

/**
 * @brief   设置计数统计溢出大小，最大67108862
 *
 * @param   cyc     - 计数统计溢出大小
 */
#define TMR2_CountOverflowCfg(cyc) (R32_TMR2_CNT_END = (cyc + 2))

/**
 * @brief   获取当前计数值，最大67108862
 *
 * @return  当前计数值
 */
#define TMR2_GetCurrentCount() R32_TMR2_COUNT

/**
 * @brief   PWM2 通道输出波形周期配置, 最大67108864
 *
 * @param   cyc     - 输出波形周期
 */
#define TMR2_PWMCycleCfg(cyc) (R32_TMR2_CNT_END = cyc)

/**
 * @brief   PWM 输出初始化
 *
 * @param   pr      - select wave polar, refer to PWMX_PolarTypeDef
 * @param   ts      - set pwm repeat times, refer to PWM_RepeatTsTypeDef
 */
void TMR2_PWMInit(PWMX_PolarTypeDef pr, PWM_RepeatTsTypeDef ts);

/**
 * @brief   PWM2 有效数据脉宽, 最大67108864
 *
 * @param   d       - 有效数据脉宽
 */
#define TMR2_PWMActDataWidth(d) (R32_TMR2_FIFO = d)

/**
 * @brief   CAP2 捕捉电平超时配置, 最大33554432
 *
 * @param   cyc     - 捕捉电平超时
 */
#define TMR2_CAPTimeoutCfg(cyc) (R32_TMR2_CNT_END = cyc)

/**
 * @brief   外部信号捕捉功能初始化
 *
 * @param   cap     - select capture mode, refer to CapModeTypeDef
 */
void TMR2_CapInit(CapModeTypeDef cap);

/**
 * @brief   获取脉冲数据
 *
 * @return  脉冲数据
 */
#define TMR2_CAPGetData() R32_TMR2_FIFO

/**
 * @brief   获取当前已捕获数据个数
 *
 * @return  当前已捕获数据个数
 */
#define TMR2_CAPDataCounter() R8_TMR2_FIFO_COUNT

/**
 * @brief   配置DMA功能
 *
 * @param   s           - 是否打开DMA功能
 * @param   startAddr   - DMA 起始地址
 * @param   endAddr     - DMA 结束地址
 * @param   m           - 配置DMA模式
 */
void TMR2_DMACfg(uint8_t s, uint16_t startAddr, uint16_t endAddr, DMAModeTypeDef m);

/**
 * @brief   关闭 TMR2
 */
#define TMR2_Disable() (R8_TMR2_CTRL_MOD &= ~RB_TMR_COUNT_EN)

/**
 * @brief   开启 TMR2
 */
#define TMR2_Enable() (R8_TMR2_CTRL_MOD |= RB_TMR_COUNT_EN)

/**
 * @brief   中断配置
 *
 * @param   s       - 使能/关闭
 * @param   f       - refer to TMR interrupt bit define
 */
#define TMR2_ITCfg(s, f) ((s) ? (R8_TMR2_INTER_EN |= f) : (R8_TMR2_INTER_EN &= ~f))

/**
 * @brief   清除中断标志
 *
 * @param   f       - refer to TMR interrupt bit define
 */
#define TMR2_ClearITFlag(f) (R8_TMR2_INT_FLAG = f)

/**
 * @brief   查询中断标志状态
 *
 * @param   f       - refer to TMR interrupt bit define
 */
#define TMR2_GetITFlag(f) (R8_TMR2_INT_FLAG & f)

/**
 * @brief   定时功能初始化
 *
 * @param   t       - 定时时间，基于当前系统时钟Tsys, 最长定时周期 67108864
 */
void TMR3_TimerInit(uint32_t t);

/**
 * @brief   获取当前定时器值，最大67108864
 *
 * @return  当前定时器值
 */
#define TMR3_GetCurrentTimer() R32_TMR3_COUNT

/**
 * @brief   边沿计数功能初始化
 *
 * @param   cap     - 采集计数类型
 */
void TMR3_EXTSingleCounterInit(CapModeTypeDef cap);

/**
 * @brief   设置计数统计溢出大小，最大67108862
 *
 * @param   cyc     - 计数统计溢出大小
 */
#define TMR3_CountOverflowCfg(cyc) (R32_TMR3_CNT_END = (cyc + 2))

/**
 * @brief   获取当前计数值，最大67108862
 *
 * @return  当前计数值
 */
#define TMR3_GetCurrentCount() R32_TMR3_COUNT

/**
 * @brief   PWM3 通道输出波形周期配置, 最大67108864
 *
 * @param   cyc     - 输出波形周期
 */
#define TMR3_PWMCycleCfg(cyc) (R32_TMR3_CNT_END = cyc)

/**
 * @brief   PWM 输出初始化
 *
 * @param   pr      - select wave polar, refer to PWMX_PolarTypeDef
 * @param   ts      - set pwm repeat times, refer to PWM_RepeatTsTypeDef
 */
void TMR3_PWMInit(PWMX_PolarTypeDef pr, PWM_RepeatTsTypeDef ts);

/**
 * @brief   PWM3 有效数据脉宽, 最大67108864
 *
 * @param   d       - 有效数据脉宽
 */
#define TMR3_PWMActDataWidth(d) (R32_TMR3_FIFO = d)

/**
 * @brief   CAP3 捕捉电平超时配置, 最大33554432
 *
 * @param   cyc     - 捕捉电平超时
 */
#define TMR3_CAPTimeoutCfg(cyc) (R32_TMR3_CNT_END = cyc)

/**
 * @brief   外部信号捕捉功能初始化
 *
 * @param   cap     - select capture mode, refer to CapModeTypeDef
 */
void TMR3_CapInit(CapModeTypeDef cap);

/**
 * @brief   获取脉冲数据
 *
 * @return  脉冲数据
 */
#define TMR3_CAPGetData() R32_TMR3_FIFO

/**
 * @brief   获取当前已捕获数据个数
 *
 * @return  当前已捕获数据个数
 */
#define TMR3_CAPDataCounter() R8_TMR3_FIFO_COUNT

/**
 * @brief   关闭 TMR3
 */
#define TMR3_Disable() (R8_TMR3_CTRL_MOD &= ~RB_TMR_COUNT_EN)

/**
 * @brief   开启 TMR3
 */
#define TMR3_Enable() (R8_TMR3_CTRL_MOD |= RB_TMR_COUNT_EN)

/**
 * @brief   中断配置
 *
 * @param   s       - 使能/关闭
 * @param   f       - refer to TMR interrupt bit define
 */
#define TMR3_ITCfg(s, f) ((s) ? (R8_TMR3_INTER_EN |= f) : (R8_TMR3_INTER_EN &= ~f))

/**
 * @brief   清除中断标志
 *
 * @param   f       - refer to TMR interrupt bit define
 */
#define TMR3_ClearITFlag(f) (R8_TMR3_INT_FLAG = f)

/**
 * @brief   查询中断标志状态
 *
 * @param   f       - refer to TMR interrupt bit define
 */
#define TMR3_GetITFlag(f) (R8_TMR3_INT_FLAG & f)

#ifdef __cplusplus
}
#endif

#endif // __CH57x_TIMER_H__
