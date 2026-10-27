/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_clk.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH57x_CLK_H__
#define __CH57x_CLK_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  系统主频定义
 */
typedef enum {
  CLK_SOURCE_LSI = 0x00,
  CLK_SOURCE_LSE,

  CLK_SOURCE_HSE_8MHz   = 0x24,
  CLK_SOURCE_HSE_6_4MHz = 0x25,
  CLK_SOURCE_HSE_4MHz   = 0x28,
  CLK_SOURCE_HSE_2MHz   = (0x20 | 16),
  CLK_SOURCE_HSE_1MHz   = (0x20 | 0),

  CLK_SOURCE_PLL_60MHz   = 0x48,
  CLK_SOURCE_PLL_48MHz   = (0x40 | 10),
  CLK_SOURCE_PLL_40MHz   = (0x40 | 12),
  CLK_SOURCE_PLL_36_9MHz = (0x40 | 13),
  CLK_SOURCE_PLL_32MHz   = (0x40 | 15),
  CLK_SOURCE_PLL_30MHz   = (0x40 | 16),
  CLK_SOURCE_PLL_24MHz   = (0x40 | 20),
  CLK_SOURCE_PLL_20MHz   = (0x40 | 24),
  CLK_SOURCE_PLL_15MHz   = (0x40 | 0),
} SYS_CLKTypeDef;

/**
 * @brief  32K时钟选择
 */
typedef enum {
  Clk32K_LSI = 0,
  Clk32K_LSE,

} LClk32KTypeDef;

/**
 * @brief  32M晶振电流挡位
 */
typedef enum {
  HSE_RCur_75 = 0,
  HSE_RCur_100,
  HSE_RCur_125,
  HSE_RCur_150

} HSECurrentTypeDef;

/**
 * @brief  32M晶振内部电容挡位
 */
typedef enum {
  HSECap_10p = 0,
  HSECap_12p,
  HSECap_14p,
  HSECap_16p,
  HSECap_18p,
  HSECap_20p,
  HSECap_22p,
  HSECap_24p

} HSECapTypeDef;

/**
 * @brief  32K晶振电流挡位
 */
typedef enum {
  LSE_RCur_70 = 0,
  LSE_RCur_100,
  LSE_RCur_140,
  LSE_RCur_200

} LSECurrentTypeDef;

/**
 * @brief  32K晶振内部电容挡位
 */
typedef enum {
  LSECap_2p = 0,
  LSECap_13p,
  LSECap_14p,
  LSECap_15p,
  LSECap_16p,
  LSECap_17p,
  LSECap_18p,
  LSECap_19p,
  LSECap_20p,
  LSECap_21p,
  LSECap_22p,
  LSECap_23p,
  LSECap_24p,
  LSECap_25p,
  LSECap_26p,
  LSECap_27p

} LSECapTypeDef;

#define MAX_DAY   0x00004000
#define MAX_2_SEC 0x0000A8C0
// #define	 MAX_SEC		0x545FFFFF

#define BEGYEAR                2020
#define IsLeapYear(yr)         (!((yr) % 400) || (((yr) % 100) && !((yr) % 4)))
#define YearLength(yr)         (IsLeapYear(yr) ? 366 : 365)
#define monthLength(lpyr, mon) ((mon == 1) ? (28 + lpyr) : ((mon > 6) ? ((mon & 1) ? 31 : 30) : ((mon & 1) ? 30 : 31)))

/**
 * @brief  rtc timer mode period define
 */
typedef enum {
  Period_0_125_S = 0, // 0.125s 周期
  Period_0_25_S,      // 0.25s 周期
  Period_0_5_S,       // 0.5s 周期
  Period_1_S,         // 1s 周期
  Period_2_S,         // 2s 周期
  Period_4_S,         // 4s 周期
  Period_8_S,         // 8s 周期
  Period_16_S,        // 16s 周期
} RTC_TMRCycTypeDef;

/**
 * @brief  rtc interrupt event define
 */
typedef enum {
  RTC_TRIG_EVENT = 0, // RTC 触发事件
  RTC_TMR_EVENT,      // RTC 周期定时事件

} RTC_EVENTTypeDef;

/**
 * @brief  rtc interrupt event define
 */
typedef enum {
  RTC_TRIG_MODE = 0, // RTC 触发模式
  RTC_TMR_MODE,      // RTC 周期定时模式

} RTC_MODETypeDef;

extern uint16_t Int32K_Tune_FLASH;
extern uint16_t Int32K_Tune_RAM;

/**
 * @brief   32K 低频时钟来源
 *
 * @param   hc  - 选择32K使用内部还是外部
 */
void LClk32K_Select(LClk32KTypeDef hc);

/**
 * @brief   HSE晶体 偏置电流配置
 *
 * @param   c   - 75%,100%,125%,150%
 */
void HSECFG_Current(HSECurrentTypeDef c);

/**
 * @brief   HSE晶体 负载电容配置
 *
 * @param   c   - refer to HSECapTypeDef
 */
void HSECFG_Capacitance(HSECapTypeDef c);

/**
 * @brief   LSE晶体 偏置电流配置
 *
 * @param   c   - 70%,100%,140%,200%
 */
void LSECFG_Current(LSECurrentTypeDef c);

/**
 * @brief   LSE晶体 负载电容配置
 *
 * @param   c   - refer to LSECapTypeDef
 */
void LSECFG_Capacitance(LSECapTypeDef c);

/**
 * @brief   校准在FLASH中运行时的内部32K时钟
 *
 * @return  误差：万分之（单位）
 */
uint16_t Calibration_LSI_FLASH(void);

/**
 * @brief   校准在RAM中运行时的内部32K时钟
 *
 * @return  误差：万分之（单位）
 */
uint16_t Calibration_LSI_RAM(void);

/**
 * @brief   设置在FLASH中运行时的内部32k校准值
 */
void LSI_SetTune_FLASH(void);

/**
 * @brief   设置在RAM中运行时的内部32k校准值
 */
void LSI_SetTune_RAM(void);

/**
 * @brief   RTC时钟初始化当前时间
 *
 * @param   y       - 配置年，MAX_Y = BEGYEAR + 44
 * @param   mon     - 配置月，MAX_MON = 12
 * @param   d       - 配置日，MAX_D = 31
 * @param   h       - 配置小时，MAX_H = 23
 * @param   m       - 配置分钟，MAX_M = 59
 * @param   s       - 配置秒，MAX_S = 59
 */
void RTC_InitTime(uint16_t y, uint16_t mon, uint16_t d, uint16_t h, uint16_t m, uint16_t s);

/**
 * @brief   获取当前时间
 *
 * @param   py      - 获取到的年，MAX_Y = BEGYEAR + 44
 * @param   pmon    - 获取到的月，MAX_MON = 12
 * @param   pd      - 获取到的日，MAX_D = 31
 * @param   ph      - 获取到的小时，MAX_H = 23
 * @param   pm      - 获取到的分钟，MAX_M = 59
 * @param   ps      - 获取到的秒，MAX_S = 59
 */
void RTC_GetTime(uint16_t *py, uint16_t *pmon, uint16_t *pd, uint16_t *ph, uint16_t *pm, uint16_t *ps);

/**
 * @brief   基于LSE/LSI时钟，配置当前RTC 周期数
 *
 * @param   cyc     - 配置周期计数初值，MAX_CYC = 0xA8BFFFFF = 2831155199
 */
void RTC_SetCycle32k(uint32_t cyc);

/**
 * @brief   基于LSE/LSI时钟，获取当前RTC 周期数
 *
 * @return  当前周期数，MAX_CYC = 0xA8BFFFFF = 2831155199
 */
uint32_t RTC_GetCycle32k(void);

/**
 * @brief   RTC定时模式配置（注意定时基准固定为32768Hz）
 *
 * @param   t   - refer to RTC_TMRCycTypeDef
 */
void RTC_TRIGFunCfg(uint32_t cyc);

/**
 * @brief   RTC定时模式配置（注意定时基准固定为32768Hz）
 *
 * @param   t   - refer to RTC_TMRCycTypeDef
 */
void RTC_TMRFunCfg(RTC_TMRCycTypeDef t);

/**
 * @brief   RTC 模式功能关闭
 *
 * @param   m   - 需要关闭的当前模式
 */
void RTC_ModeFunDisable(RTC_MODETypeDef m);

/**
 * @brief   获取RTC中断标志
 *
 * @param   f   - refer to RTC_EVENTTypeDef
 *
 * @return  中断标志状态
 */
uint8_t RTC_GetITFlag(RTC_EVENTTypeDef f);

/**
 * @brief   清除RTC中断标志
 *
 * @param   f   - refer to RTC_EVENTTypeDef
 */
void RTC_ClearITFlag(RTC_EVENTTypeDef f);

#ifdef __cplusplus
}
#endif

#endif // __CH57x_CLK_H__
