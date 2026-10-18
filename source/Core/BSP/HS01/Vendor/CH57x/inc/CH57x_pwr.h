/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_pwr.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH57x_PWR_H__
#define __CH57x_PWR_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ROM_CFG_ADR_HW 0x7F00C // config address for hardware config for LDO&OSC and etc

/**
 * @brief	Peripher CLK control bit define
 */
#define BIT_SLP_CLK_TMR0  (0x00000001) /*!< TMR0 peripher clk bit */
#define BIT_SLP_CLK_TMR1  (0x00000002) /*!< TMR1 peripher clk bit */
#define BIT_SLP_CLK_TMR2  (0x00000004) /*!< TMR2 peripher clk bit */
#define BIT_SLP_CLK_TMR3  (0x00000008) /*!< TMR3 peripher clk bit */
#define BIT_SLP_CLK_UART0 (0x00000010) /*!< UART0 peripher clk bit */
#define BIT_SLP_CLK_UART1 (0x00000020) /*!< UART1 peripher clk bit */
#define BIT_SLP_CLK_UART2 (0x00000040) /*!< UART2 peripher clk bit */
#define BIT_SLP_CLK_UART3 (0x00000080) /*!< UART3 peripher clk bit */
#define BIT_SLP_CLK_SPI0  (0x00000100) /*!< SPI0 peripher clk bit */
// #define BIT_SLP_CLK_SPI1     (0x00000200)  /*!< SPI1 peripher clk bit */
#define BIT_SLP_CLK_PWMX (0x00000400) /*!< PWMX peripher clk bit */
// #define BIT_SLP_CLK_LCD      (0x00000800)  /*!< LCD peripher clk bit */
#define BIT_SLP_CLK_USB (0x00001000) /*!< USB peripher clk bit */
// #define BIT_SLP_CLK_ETH      (0x00002000)  /*!< ETH peripher clk bit */
// #define BIT_SLP_CLK_LED      (0x00004000)  /*!< LED peripher clk bit */
#define BIT_SLP_CLK_BLE (0x00008000) /*!< BLE peripher clk bit */

#define BIT_SLP_CLK_RAMX  (0x10000000) /*!< main SRAM RAM16K peripher clk bit */
#define BIT_SLP_CLK_RAM2K (0x20000000) /*!< RAM2K peripher clk bit */
#define BIT_SLP_CLK_ALL   (0x3000FFFF) /*!< All peripher clk bit */

/**
 * @brief  unit of controllable power supply
 */
#define UNIT_SYS_LSE RB_CLK_XT32K_PON  // 外部32K 时钟振荡
#define UNIT_SYS_LSI RB_CLK_INT32K_PON // 内部32K 时钟振荡
#define UNIT_SYS_HSE RB_CLK_XT32M_PON  // 外部32M 时钟振荡
#define UNIT_SYS_PLL RB_CLK_PLL_PON    // PLL 时钟振荡

/**
 * @brief  wakeup mode define
 */
typedef enum {
  Edge_LongDelay = 0,
  Edge_ShortDelay,
  Level_LongDelay,
  Level_ShortDelay,

} WakeUP_ModeypeDef;

/**
 * @brief  wakeup mode define
 */
typedef enum {
  /* 下面等级将使用高精度监控，210uA消耗 */
  HALevel_1V9 = 0, // 1.7-1.9
  HALevel_2V1,     // 1.9-2.1
  HALevel_2V3,     // 2.1-2.3
  HALevel_2V5,     // 2.3-2.5

  /* 下面等级将使用低功耗监控，1uA消耗 */
  LPLevel_1V8 = 0x80,
  LPLevel_1V9,
  LPLevel_2V0,
  LPLevel_2V1,
  LPLevel_2V2,
  LPLevel_2V3,
  LPLevel_2V4,
  LPLevel_2V5,

} VolM_LevelypeDef;

/**
 * @brief   启用内部DC/DC电源，用于节约系统功耗
 *
 * @param   s   - 是否打开DCDC电源
 */
void PWR_DCDCCfg(FunctionalState s);

/**
 * @brief   可控单元模块的电源控制
 *
 * @param   s       - 是否打开电源
 * @param   unit    - please refer to unit of controllable power supply
 */
void PWR_UnitModCfg(FunctionalState s, uint8_t unit);

/**
 * @brief   外设时钟控制位
 *
 * @param   s       - 是否打开对应外设时钟
 * @param   perph   - please refer to Peripher CLK control bit define
 */
void PWR_PeriphClkCfg(FunctionalState s, uint16_t perph);

/**
 * @brief   睡眠唤醒源配置
 *
 * @param   s       - 是否打开此外设睡眠唤醒功能
 * @param   perph   - 需要设置的唤醒源
 *                    RB_SLP_USB_WAKE   -  USB 为唤醒源
 *                    RB_SLP_RTC_WAKE   -  RTC 为唤醒源
 *                    RB_SLP_GPIO_WAKE  -  GPIO 为唤醒源
 *                    RB_SLP_BAT_WAKE   -  BAT 为唤醒源
 * @param   mode    - refer to WakeUP_ModeypeDef
 */
void PWR_PeriphWakeUpCfg(FunctionalState s, uint8_t perph, WakeUP_ModeypeDef mode);

/**
 * @brief   电源监控
 *
 * @param   s       - 是否打开此功能
 * @param   vl      - refer to VolM_LevelypeDef
 */
void PowerMonitor(FunctionalState s, VolM_LevelypeDef vl);

/**
 * @brief   低功耗-Idle模式
 */
void LowPower_Idle(void);

/**
 * @brief   低功耗-Halt模式，此低功耗切到HSI/5时钟运行，唤醒后需要用户自己重新选择系统时钟源
 */
void LowPower_Halt(void);

/**
 * @brief   低功耗-Sleep模式，此低功耗切到HSI/5时钟运行，唤醒后需要用户自己重新选择系统时钟源
 *          @note 注意调用此函数，DCDC功能强制关闭，唤醒后可以手动再次打开
 *
 * @param   rm      - 供电模块选择
 *                    RB_PWR_RAM2K  -   2K retention SRAM 供电
 *                    RB_PWR_RAM16K -   16K main SRAM 供电
 *                    RB_PWR_EXTEND -   USB 和 BLE 单元保留区域供电
 *                    RB_PWR_XROM   -   FlashROM 供电
 *                    NULL          -   以上单元都断电
 */
void LowPower_Sleep(uint8_t rm);

/**
 * @brief   低功耗-Shutdown模式，此低功耗切到HSI/5时钟运行，唤醒后需要用户自己重新选择系统时钟源
 *          @note 注意调用此函数，DCDC功能强制关闭，唤醒后可以手动再次打开
 *
 * @param   rm      - 供电模块选择
 *                    RB_PWR_RAM2K  -   2K retention SRAM 供电
 *                    RB_PWR_RAM16K -   16K main SRAM 供电
 *                    NULL          -   以上单元都断电
 */
void LowPower_Shutdown(uint8_t rm);

#ifdef __cplusplus
}
#endif

#endif // __CH57x_PWR_H__
