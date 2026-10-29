/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_timer0.c
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH57x_common.h"

/*********************************************************************
 * @fn      TMR0_TimerInit
 *
 * @brief   定时功能初始化
 *
 * @param   t       - 定时时间，基于当前系统时钟Tsys, 最长定时周期 67108864
 *
 * @return  none
 */
void TMR0_TimerInit(uint32_t t) {
  R32_TMR0_CNT_END = t;
  R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
  R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN;
}

/*********************************************************************
 * @fn      TMR0_EXTSingleCounterInit
 *
 * @brief   边沿计数功能初始化
 *
 * @param   cap     - 采集计数类型
 *
 * @return  none
 */
void TMR0_EXTSingleCounterInit(CapModeTypeDef cap) {
  R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
  R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN | RB_TMR_CAP_COUNT | RB_TMR_MODE_IN | (cap << 6);
}

/*********************************************************************
 * @fn      TMR0_PWMInit
 *
 * @brief   PWM 输出初始化
 *
 * @param   pr      - select wave polar, refer to PWMX_PolarTypeDef
 * @param   ts      - set pwm repeat times, refer to PWM_RepeatTsTypeDef
 *
 * @return  none
 */
void TMR0_PWMInit(PWMX_PolarTypeDef pr, PWM_RepeatTsTypeDef ts) {
  //    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
  R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN | RB_TMR_OUT_EN | (pr << 4) | (ts << 6);
}

/*********************************************************************
 * @fn      TMR0_CapInit
 *
 * @brief   外部信号捕捉功能初始化
 *
 * @param   cap     - select capture mode, refer to CapModeTypeDef
 *
 * @return  none
 */
void TMR0_CapInit(CapModeTypeDef cap) {
  R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
  R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN | RB_TMR_MODE_IN | (cap << 6);
}
