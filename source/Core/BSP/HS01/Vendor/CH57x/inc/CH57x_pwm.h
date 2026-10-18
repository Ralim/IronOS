/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_pwm.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH57x_PWM_H__
#define __CH57x_PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  channel of PWM define
 */
#define CH_PWM4  0x01 // PWM4 通道
#define CH_PWM5  0x02 // PWM5 通道
#define CH_PWM6  0x04 // PWM6 通道
#define CH_PWM7  0x08 // PWM7 通道
#define CH_PWM8  0x10 // PWM8 通道
#define CH_PWM9  0x20 // PWM9 通道
#define CH_PWM10 0x40 // PWM10 通道
#define CH_PWM11 0x80 // PWM11 通道

/**
 * @brief  channel of PWM define
 */
typedef enum {
  High_Level = 0, // 默认低电平，高电平有效
  Low_Level,      // 默认高电平，低电平有效
} PWMX_PolarTypeDef;

/**
 * @brief  Configuration PWM4_11 Cycle size
 */
typedef enum {
  PWMX_Cycle_256 = 0, // 256 个PWMX周期
  PWMX_Cycle_255,     // 255 个PWMX周期
  PWMX_Cycle_128,     // 128 个PWMX周期
  PWMX_Cycle_127,     // 127 个PWMX周期
  PWMX_Cycle_64,      // 64 个PWMX周期
  PWMX_Cycle_63,      // 63 个PWMX周期
  PWMX_Cycle_32,      // 32 个PWMX周期
  PWMX_Cycle_31,      // 31 个PWMX周期
} PWMX_CycleTypeDef;

/**
 * @brief   PWM4-PWM11 通道基准时钟配置
 *
 * @param   d   - 通道基准时钟 = d*Tsys
 */
#define PWMX_CLKCfg(d) (R8_PWM_CLOCK_DIV = d)

/**
 * @brief   PWM4-PWM11基准时钟配置
 *
 * @param   cyc - refer to PWMX_CycleTypeDef
 */
void PWMX_CycleCfg(PWMX_CycleTypeDef cyc);

/**
 * @brief   设置 PWM4 有效数据脉宽
 *
 * @param   d   - 有效数据脉宽
 */
#define PWM4_ActDataWidth(d) (R8_PWM4_DATA = d)

/**
 * @brief   设置 PWM5 有效数据脉宽
 *
 * @param   d   - 有效数据脉宽
 */
#define PWM5_ActDataWidth(d) (R8_PWM5_DATA = d)

/**
 * @brief   设置 PWM6 有效数据脉宽
 *
 * @param   d   - 有效数据脉宽
 */
#define PWM6_ActDataWidth(d) (R8_PWM6_DATA = d)

/**
 * @brief   设置 PWM7 有效数据脉宽
 *
 * @param   d   - 有效数据脉宽
 */
#define PWM7_ActDataWidth(d) (R8_PWM7_DATA = d)

/**
 * @brief   设置 PWM8 有效数据脉宽
 *
 * @param   d   - 有效数据脉宽
 */
#define PWM8_ActDataWidth(d) (R8_PWM8_DATA = d)

/**
 * @brief   设置 PWM9 有效数据脉宽
 *
 * @param   d   - 有效数据脉宽
 */
#define PWM9_ActDataWidth(d) (R8_PWM9_DATA = d)

/**
 * @brief   设置 PWM10 有效数据脉宽
 *
 * @param   d   - 有效数据脉宽
 */
#define PWM10_ActDataWidth(d) (R8_PWM10_DATA = d)

/**
 * @brief   设置 PWM11 有效数据脉宽
 *
 * @param   d   - 有效数据脉宽
 */
#define PWM11_ActDataWidth(d) (R8_PWM11_DATA = d)

/**
 * @brief   PWM4-PWM11通道输出波形配置
 *
 * @param   ch      - select channel of pwm, refer to channel of PWM define
 * @param   da      - effective pulse width
 * @param   pr      - select wave polar, refer to PWMX_PolarTypeDef
 * @param   s       - control pwmx function, ENABLE or DISABLE
 */
void PWMX_ACTOUT(uint8_t ch, uint8_t da, PWMX_PolarTypeDef pr, FunctionalState s);

/**
 * @brief   PWM 交替输出模式配置
 *
 * @param   ch      - select group of PWM alternate output
 *                    RB_PWM4_5_STAG_EN     -  PWM4 和 PWM5 通道交替输出
 *                    RB_PWM6_7_STAG_EN     -  PWM6 和 PWM7 通道交替输出
 *                    RB_PWM8_9_STAG_EN     -  PWM8 和 PWM9 通道交替输出
 *                    RB_PWM10_11_STAG_EN   -  PWM10 和 PWM11 通道交替输出
 * @param   s       - control pwmx function, ENABLE or DISABLE
 */
void PWMX_AlterOutCfg(uint8_t ch, FunctionalState s);

#ifdef __cplusplus
}
#endif

#endif // __CH57x_PWM_H__
