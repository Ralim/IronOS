/**
  ******************************************************************************
  * @file    pwm_reg.h
  * @version V1.2
  * @date    2020-04-30
  * @brief   This file is the description of.IP register
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef __PWM_REG_H__
#define __PWM_REG_H__

#include "bl602.h"

/* 0x0 : pwm_int_config */
#define PWM_INT_CONFIG_OFFSET  (0x0)
#define PWM_INTERRUPT_STS      PWM_INTERRUPT_STS
#define PWM_INTERRUPT_STS_POS  (0U)
#define PWM_INTERRUPT_STS_LEN  (6U)
#define PWM_INTERRUPT_STS_MSK  (((1U << PWM_INTERRUPT_STS_LEN) - 1) << PWM_INTERRUPT_STS_POS)
#define PWM_INTERRUPT_STS_UMSK (~(((1U << PWM_INTERRUPT_STS_LEN) - 1) << PWM_INTERRUPT_STS_POS))
#define PWM_INT_CLEAR          PWM_INT_CLEAR
#define PWM_INT_CLEAR_POS      (8U)
#define PWM_INT_CLEAR_LEN      (6U)
#define PWM_INT_CLEAR_MSK      (((1U << PWM_INT_CLEAR_LEN) - 1) << PWM_INT_CLEAR_POS)
#define PWM_INT_CLEAR_UMSK     (~(((1U << PWM_INT_CLEAR_LEN) - 1) << PWM_INT_CLEAR_POS))

/* 0x20 : pwm0_clkdiv */
#define PWM0_CLKDIV_OFFSET (0x20)
#define PWM_CLK_DIV        PWM_CLK_DIV
#define PWM_CLK_DIV_POS    (0U)
#define PWM_CLK_DIV_LEN    (16U)
#define PWM_CLK_DIV_MSK    (((1U << PWM_CLK_DIV_LEN) - 1) << PWM_CLK_DIV_POS)
#define PWM_CLK_DIV_UMSK   (~(((1U << PWM_CLK_DIV_LEN) - 1) << PWM_CLK_DIV_POS))

/* 0x24 : pwm0_thre1 */
#define PWM0_THRE1_OFFSET (0x24)
#define PWM_THRE1         PWM_THRE1
#define PWM_THRE1_POS     (0U)
#define PWM_THRE1_LEN     (16U)
#define PWM_THRE1_MSK     (((1U << PWM_THRE1_LEN) - 1) << PWM_THRE1_POS)
#define PWM_THRE1_UMSK    (~(((1U << PWM_THRE1_LEN) - 1) << PWM_THRE1_POS))

/* 0x28 : pwm0_thre2 */
#define PWM0_THRE2_OFFSET (0x28)
#define PWM_THRE2         PWM_THRE2
#define PWM_THRE2_POS     (0U)
#define PWM_THRE2_LEN     (16U)
#define PWM_THRE2_MSK     (((1U << PWM_THRE2_LEN) - 1) << PWM_THRE2_POS)
#define PWM_THRE2_UMSK    (~(((1U << PWM_THRE2_LEN) - 1) << PWM_THRE2_POS))

/* 0x2C : pwm0_period */
#define PWM0_PERIOD_OFFSET (0x2C)
#define PWM_PERIOD         PWM_PERIOD
#define PWM_PERIOD_POS     (0U)
#define PWM_PERIOD_LEN     (16U)
#define PWM_PERIOD_MSK     (((1U << PWM_PERIOD_LEN) - 1) << PWM_PERIOD_POS)
#define PWM_PERIOD_UMSK    (~(((1U << PWM_PERIOD_LEN) - 1) << PWM_PERIOD_POS))

/* 0x30 : pwm0_config */
#define PWM0_CONFIG_OFFSET    (0x30)
#define PWM_REG_CLK_SEL       PWM_REG_CLK_SEL
#define PWM_REG_CLK_SEL_POS   (0U)
#define PWM_REG_CLK_SEL_LEN   (2U)
#define PWM_REG_CLK_SEL_MSK   (((1U << PWM_REG_CLK_SEL_LEN) - 1) << PWM_REG_CLK_SEL_POS)
#define PWM_REG_CLK_SEL_UMSK  (~(((1U << PWM_REG_CLK_SEL_LEN) - 1) << PWM_REG_CLK_SEL_POS))
#define PWM_OUT_INV           PWM_OUT_INV
#define PWM_OUT_INV_POS       (2U)
#define PWM_OUT_INV_LEN       (1U)
#define PWM_OUT_INV_MSK       (((1U << PWM_OUT_INV_LEN) - 1) << PWM_OUT_INV_POS)
#define PWM_OUT_INV_UMSK      (~(((1U << PWM_OUT_INV_LEN) - 1) << PWM_OUT_INV_POS))
#define PWM_STOP_MODE         PWM_STOP_MODE
#define PWM_STOP_MODE_POS     (3U)
#define PWM_STOP_MODE_LEN     (1U)
#define PWM_STOP_MODE_MSK     (((1U << PWM_STOP_MODE_LEN) - 1) << PWM_STOP_MODE_POS)
#define PWM_STOP_MODE_UMSK    (~(((1U << PWM_STOP_MODE_LEN) - 1) << PWM_STOP_MODE_POS))
#define PWM_SW_FORCE_VAL      PWM_SW_FORCE_VAL
#define PWM_SW_FORCE_VAL_POS  (4U)
#define PWM_SW_FORCE_VAL_LEN  (1U)
#define PWM_SW_FORCE_VAL_MSK  (((1U << PWM_SW_FORCE_VAL_LEN) - 1) << PWM_SW_FORCE_VAL_POS)
#define PWM_SW_FORCE_VAL_UMSK (~(((1U << PWM_SW_FORCE_VAL_LEN) - 1) << PWM_SW_FORCE_VAL_POS))
#define PWM_SW_MODE           PWM_SW_MODE
#define PWM_SW_MODE_POS       (5U)
#define PWM_SW_MODE_LEN       (1U)
#define PWM_SW_MODE_MSK       (((1U << PWM_SW_MODE_LEN) - 1) << PWM_SW_MODE_POS)
#define PWM_SW_MODE_UMSK      (~(((1U << PWM_SW_MODE_LEN) - 1) << PWM_SW_MODE_POS))
#define PWM_STOP_EN           PWM_STOP_EN
#define PWM_STOP_EN_POS       (6U)
#define PWM_STOP_EN_LEN       (1U)
#define PWM_STOP_EN_MSK       (((1U << PWM_STOP_EN_LEN) - 1) << PWM_STOP_EN_POS)
#define PWM_STOP_EN_UMSK      (~(((1U << PWM_STOP_EN_LEN) - 1) << PWM_STOP_EN_POS))
#define PWM_STS_TOP           PWM_STS_TOP
#define PWM_STS_TOP_POS       (7U)
#define PWM_STS_TOP_LEN       (1U)
#define PWM_STS_TOP_MSK       (((1U << PWM_STS_TOP_LEN) - 1) << PWM_STS_TOP_POS)
#define PWM_STS_TOP_UMSK      (~(((1U << PWM_STS_TOP_LEN) - 1) << PWM_STS_TOP_POS))

/* 0x34 : pwm0_interrupt */
#define PWM0_INTERRUPT_OFFSET   (0x34)
#define PWM_INT_PERIOD_CNT      PWM_INT_PERIOD_CNT
#define PWM_INT_PERIOD_CNT_POS  (0U)
#define PWM_INT_PERIOD_CNT_LEN  (16U)
#define PWM_INT_PERIOD_CNT_MSK  (((1U << PWM_INT_PERIOD_CNT_LEN) - 1) << PWM_INT_PERIOD_CNT_POS)
#define PWM_INT_PERIOD_CNT_UMSK (~(((1U << PWM_INT_PERIOD_CNT_LEN) - 1) << PWM_INT_PERIOD_CNT_POS))
#define PWM_INT_ENABLE          PWM_INT_ENABLE
#define PWM_INT_ENABLE_POS      (16U)
#define PWM_INT_ENABLE_LEN      (1U)
#define PWM_INT_ENABLE_MSK      (((1U << PWM_INT_ENABLE_LEN) - 1) << PWM_INT_ENABLE_POS)
#define PWM_INT_ENABLE_UMSK     (~(((1U << PWM_INT_ENABLE_LEN) - 1) << PWM_INT_ENABLE_POS))

/* 0x40 : pwm1_clkdiv */
#define PWM1_CLKDIV_OFFSET (0x40)
#define PWM_CLK_DIV        PWM_CLK_DIV
#define PWM_CLK_DIV_POS    (0U)
#define PWM_CLK_DIV_LEN    (16U)
#define PWM_CLK_DIV_MSK    (((1U << PWM_CLK_DIV_LEN) - 1) << PWM_CLK_DIV_POS)
#define PWM_CLK_DIV_UMSK   (~(((1U << PWM_CLK_DIV_LEN) - 1) << PWM_CLK_DIV_POS))

/* 0x44 : pwm1_thre1 */
#define PWM1_THRE1_OFFSET (0x44)
#define PWM_THRE1         PWM_THRE1
#define PWM_THRE1_POS     (0U)
#define PWM_THRE1_LEN     (16U)
#define PWM_THRE1_MSK     (((1U << PWM_THRE1_LEN) - 1) << PWM_THRE1_POS)
#define PWM_THRE1_UMSK    (~(((1U << PWM_THRE1_LEN) - 1) << PWM_THRE1_POS))

/* 0x48 : pwm1_thre2 */
#define PWM1_THRE2_OFFSET (0x48)
#define PWM_THRE2         PWM_THRE2
#define PWM_THRE2_POS     (0U)
#define PWM_THRE2_LEN     (16U)
#define PWM_THRE2_MSK     (((1U << PWM_THRE2_LEN) - 1) << PWM_THRE2_POS)
#define PWM_THRE2_UMSK    (~(((1U << PWM_THRE2_LEN) - 1) << PWM_THRE2_POS))

/* 0x4C : pwm1_period */
#define PWM1_PERIOD_OFFSET (0x4C)
#define PWM_PERIOD         PWM_PERIOD
#define PWM_PERIOD_POS     (0U)
#define PWM_PERIOD_LEN     (16U)
#define PWM_PERIOD_MSK     (((1U << PWM_PERIOD_LEN) - 1) << PWM_PERIOD_POS)
#define PWM_PERIOD_UMSK    (~(((1U << PWM_PERIOD_LEN) - 1) << PWM_PERIOD_POS))

/* 0x50 : pwm1_config */
#define PWM1_CONFIG_OFFSET    (0x50)
#define PWM_REG_CLK_SEL       PWM_REG_CLK_SEL
#define PWM_REG_CLK_SEL_POS   (0U)
#define PWM_REG_CLK_SEL_LEN   (2U)
#define PWM_REG_CLK_SEL_MSK   (((1U << PWM_REG_CLK_SEL_LEN) - 1) << PWM_REG_CLK_SEL_POS)
#define PWM_REG_CLK_SEL_UMSK  (~(((1U << PWM_REG_CLK_SEL_LEN) - 1) << PWM_REG_CLK_SEL_POS))
#define PWM_OUT_INV           PWM_OUT_INV
#define PWM_OUT_INV_POS       (2U)
#define PWM_OUT_INV_LEN       (1U)
#define PWM_OUT_INV_MSK       (((1U << PWM_OUT_INV_LEN) - 1) << PWM_OUT_INV_POS)
#define PWM_OUT_INV_UMSK      (~(((1U << PWM_OUT_INV_LEN) - 1) << PWM_OUT_INV_POS))
#define PWM_STOP_MODE         PWM_STOP_MODE
#define PWM_STOP_MODE_POS     (3U)
#define PWM_STOP_MODE_LEN     (1U)
#define PWM_STOP_MODE_MSK     (((1U << PWM_STOP_MODE_LEN) - 1) << PWM_STOP_MODE_POS)
#define PWM_STOP_MODE_UMSK    (~(((1U << PWM_STOP_MODE_LEN) - 1) << PWM_STOP_MODE_POS))
#define PWM_SW_FORCE_VAL      PWM_SW_FORCE_VAL
#define PWM_SW_FORCE_VAL_POS  (4U)
#define PWM_SW_FORCE_VAL_LEN  (1U)
#define PWM_SW_FORCE_VAL_MSK  (((1U << PWM_SW_FORCE_VAL_LEN) - 1) << PWM_SW_FORCE_VAL_POS)
#define PWM_SW_FORCE_VAL_UMSK (~(((1U << PWM_SW_FORCE_VAL_LEN) - 1) << PWM_SW_FORCE_VAL_POS))
#define PWM_SW_MODE           PWM_SW_MODE
#define PWM_SW_MODE_POS       (5U)
#define PWM_SW_MODE_LEN       (1U)
#define PWM_SW_MODE_MSK       (((1U << PWM_SW_MODE_LEN) - 1) << PWM_SW_MODE_POS)
#define PWM_SW_MODE_UMSK      (~(((1U << PWM_SW_MODE_LEN) - 1) << PWM_SW_MODE_POS))
#define PWM_STOP_EN           PWM_STOP_EN
#define PWM_STOP_EN_POS       (6U)
#define PWM_STOP_EN_LEN       (1U)
#define PWM_STOP_EN_MSK       (((1U << PWM_STOP_EN_LEN) - 1) << PWM_STOP_EN_POS)
#define PWM_STOP_EN_UMSK      (~(((1U << PWM_STOP_EN_LEN) - 1) << PWM_STOP_EN_POS))
#define PWM_STS_TOP           PWM_STS_TOP
#define PWM_STS_TOP_POS       (7U)
#define PWM_STS_TOP_LEN       (1U)
#define PWM_STS_TOP_MSK       (((1U << PWM_STS_TOP_LEN) - 1) << PWM_STS_TOP_POS)
#define PWM_STS_TOP_UMSK      (~(((1U << PWM_STS_TOP_LEN) - 1) << PWM_STS_TOP_POS))

/* 0x54 : pwm1_interrupt */
#define PWM1_INTERRUPT_OFFSET   (0x54)
#define PWM_INT_PERIOD_CNT      PWM_INT_PERIOD_CNT
#define PWM_INT_PERIOD_CNT_POS  (0U)
#define PWM_INT_PERIOD_CNT_LEN  (16U)
#define PWM_INT_PERIOD_CNT_MSK  (((1U << PWM_INT_PERIOD_CNT_LEN) - 1) << PWM_INT_PERIOD_CNT_POS)
#define PWM_INT_PERIOD_CNT_UMSK (~(((1U << PWM_INT_PERIOD_CNT_LEN) - 1) << PWM_INT_PERIOD_CNT_POS))
#define PWM_INT_ENABLE          PWM_INT_ENABLE
#define PWM_INT_ENABLE_POS      (16U)
#define PWM_INT_ENABLE_LEN      (1U)
#define PWM_INT_ENABLE_MSK      (((1U << PWM_INT_ENABLE_LEN) - 1) << PWM_INT_ENABLE_POS)
#define PWM_INT_ENABLE_UMSK     (~(((1U << PWM_INT_ENABLE_LEN) - 1) << PWM_INT_ENABLE_POS))

/* 0x60 : pwm2_clkdiv */
#define PWM2_CLKDIV_OFFSET (0x60)
#define PWM_CLK_DIV        PWM_CLK_DIV
#define PWM_CLK_DIV_POS    (0U)
#define PWM_CLK_DIV_LEN    (16U)
#define PWM_CLK_DIV_MSK    (((1U << PWM_CLK_DIV_LEN) - 1) << PWM_CLK_DIV_POS)
#define PWM_CLK_DIV_UMSK   (~(((1U << PWM_CLK_DIV_LEN) - 1) << PWM_CLK_DIV_POS))

/* 0x64 : pwm2_thre1 */
#define PWM2_THRE1_OFFSET (0x64)
#define PWM_THRE1         PWM_THRE1
#define PWM_THRE1_POS     (0U)
#define PWM_THRE1_LEN     (16U)
#define PWM_THRE1_MSK     (((1U << PWM_THRE1_LEN) - 1) << PWM_THRE1_POS)
#define PWM_THRE1_UMSK    (~(((1U << PWM_THRE1_LEN) - 1) << PWM_THRE1_POS))

/* 0x68 : pwm2_thre2 */
#define PWM2_THRE2_OFFSET (0x68)
#define PWM_THRE2         PWM_THRE2
#define PWM_THRE2_POS     (0U)
#define PWM_THRE2_LEN     (16U)
#define PWM_THRE2_MSK     (((1U << PWM_THRE2_LEN) - 1) << PWM_THRE2_POS)
#define PWM_THRE2_UMSK    (~(((1U << PWM_THRE2_LEN) - 1) << PWM_THRE2_POS))

/* 0x6C : pwm2_period */
#define PWM2_PERIOD_OFFSET (0x6C)
#define PWM_PERIOD         PWM_PERIOD
#define PWM_PERIOD_POS     (0U)
#define PWM_PERIOD_LEN     (16U)
#define PWM_PERIOD_MSK     (((1U << PWM_PERIOD_LEN) - 1) << PWM_PERIOD_POS)
#define PWM_PERIOD_UMSK    (~(((1U << PWM_PERIOD_LEN) - 1) << PWM_PERIOD_POS))

/* 0x70 : pwm2_config */
#define PWM2_CONFIG_OFFSET    (0x70)
#define PWM_REG_CLK_SEL       PWM_REG_CLK_SEL
#define PWM_REG_CLK_SEL_POS   (0U)
#define PWM_REG_CLK_SEL_LEN   (2U)
#define PWM_REG_CLK_SEL_MSK   (((1U << PWM_REG_CLK_SEL_LEN) - 1) << PWM_REG_CLK_SEL_POS)
#define PWM_REG_CLK_SEL_UMSK  (~(((1U << PWM_REG_CLK_SEL_LEN) - 1) << PWM_REG_CLK_SEL_POS))
#define PWM_OUT_INV           PWM_OUT_INV
#define PWM_OUT_INV_POS       (2U)
#define PWM_OUT_INV_LEN       (1U)
#define PWM_OUT_INV_MSK       (((1U << PWM_OUT_INV_LEN) - 1) << PWM_OUT_INV_POS)
#define PWM_OUT_INV_UMSK      (~(((1U << PWM_OUT_INV_LEN) - 1) << PWM_OUT_INV_POS))
#define PWM_STOP_MODE         PWM_STOP_MODE
#define PWM_STOP_MODE_POS     (3U)
#define PWM_STOP_MODE_LEN     (1U)
#define PWM_STOP_MODE_MSK     (((1U << PWM_STOP_MODE_LEN) - 1) << PWM_STOP_MODE_POS)
#define PWM_STOP_MODE_UMSK    (~(((1U << PWM_STOP_MODE_LEN) - 1) << PWM_STOP_MODE_POS))
#define PWM_SW_FORCE_VAL      PWM_SW_FORCE_VAL
#define PWM_SW_FORCE_VAL_POS  (4U)
#define PWM_SW_FORCE_VAL_LEN  (1U)
#define PWM_SW_FORCE_VAL_MSK  (((1U << PWM_SW_FORCE_VAL_LEN) - 1) << PWM_SW_FORCE_VAL_POS)
#define PWM_SW_FORCE_VAL_UMSK (~(((1U << PWM_SW_FORCE_VAL_LEN) - 1) << PWM_SW_FORCE_VAL_POS))
#define PWM_SW_MODE           PWM_SW_MODE
#define PWM_SW_MODE_POS       (5U)
#define PWM_SW_MODE_LEN       (1U)
#define PWM_SW_MODE_MSK       (((1U << PWM_SW_MODE_LEN) - 1) << PWM_SW_MODE_POS)
#define PWM_SW_MODE_UMSK      (~(((1U << PWM_SW_MODE_LEN) - 1) << PWM_SW_MODE_POS))
#define PWM_STOP_EN           PWM_STOP_EN
#define PWM_STOP_EN_POS       (6U)
#define PWM_STOP_EN_LEN       (1U)
#define PWM_STOP_EN_MSK       (((1U << PWM_STOP_EN_LEN) - 1) << PWM_STOP_EN_POS)
#define PWM_STOP_EN_UMSK      (~(((1U << PWM_STOP_EN_LEN) - 1) << PWM_STOP_EN_POS))
#define PWM_STS_TOP           PWM_STS_TOP
#define PWM_STS_TOP_POS       (7U)
#define PWM_STS_TOP_LEN       (1U)
#define PWM_STS_TOP_MSK       (((1U << PWM_STS_TOP_LEN) - 1) << PWM_STS_TOP_POS)
#define PWM_STS_TOP_UMSK      (~(((1U << PWM_STS_TOP_LEN) - 1) << PWM_STS_TOP_POS))

/* 0x74 : pwm2_interrupt */
#define PWM2_INTERRUPT_OFFSET   (0x74)
#define PWM_INT_PERIOD_CNT      PWM_INT_PERIOD_CNT
#define PWM_INT_PERIOD_CNT_POS  (0U)
#define PWM_INT_PERIOD_CNT_LEN  (16U)
#define PWM_INT_PERIOD_CNT_MSK  (((1U << PWM_INT_PERIOD_CNT_LEN) - 1) << PWM_INT_PERIOD_CNT_POS)
#define PWM_INT_PERIOD_CNT_UMSK (~(((1U << PWM_INT_PERIOD_CNT_LEN) - 1) << PWM_INT_PERIOD_CNT_POS))
#define PWM_INT_ENABLE          PWM_INT_ENABLE
#define PWM_INT_ENABLE_POS      (16U)
#define PWM_INT_ENABLE_LEN      (1U)
#define PWM_INT_ENABLE_MSK      (((1U << PWM_INT_ENABLE_LEN) - 1) << PWM_INT_ENABLE_POS)
#define PWM_INT_ENABLE_UMSK     (~(((1U << PWM_INT_ENABLE_LEN) - 1) << PWM_INT_ENABLE_POS))

/* 0x80 : pwm3_clkdiv */
#define PWM3_CLKDIV_OFFSET (0x80)
#define PWM_CLK_DIV        PWM_CLK_DIV
#define PWM_CLK_DIV_POS    (0U)
#define PWM_CLK_DIV_LEN    (16U)
#define PWM_CLK_DIV_MSK    (((1U << PWM_CLK_DIV_LEN) - 1) << PWM_CLK_DIV_POS)
#define PWM_CLK_DIV_UMSK   (~(((1U << PWM_CLK_DIV_LEN) - 1) << PWM_CLK_DIV_POS))

/* 0x84 : pwm3_thre1 */
#define PWM3_THRE1_OFFSET (0x84)
#define PWM_THRE1         PWM_THRE1
#define PWM_THRE1_POS     (0U)
#define PWM_THRE1_LEN     (16U)
#define PWM_THRE1_MSK     (((1U << PWM_THRE1_LEN) - 1) << PWM_THRE1_POS)
#define PWM_THRE1_UMSK    (~(((1U << PWM_THRE1_LEN) - 1) << PWM_THRE1_POS))

/* 0x88 : pwm3_thre2 */
#define PWM3_THRE2_OFFSET (0x88)
#define PWM_THRE2         PWM_THRE2
#define PWM_THRE2_POS     (0U)
#define PWM_THRE2_LEN     (16U)
#define PWM_THRE2_MSK     (((1U << PWM_THRE2_LEN) - 1) << PWM_THRE2_POS)
#define PWM_THRE2_UMSK    (~(((1U << PWM_THRE2_LEN) - 1) << PWM_THRE2_POS))

/* 0x8C : pwm3_period */
#define PWM3_PERIOD_OFFSET (0x8C)
#define PWM_PERIOD         PWM_PERIOD
#define PWM_PERIOD_POS     (0U)
#define PWM_PERIOD_LEN     (16U)
#define PWM_PERIOD_MSK     (((1U << PWM_PERIOD_LEN) - 1) << PWM_PERIOD_POS)
#define PWM_PERIOD_UMSK    (~(((1U << PWM_PERIOD_LEN) - 1) << PWM_PERIOD_POS))

/* 0x90 : pwm3_config */
#define PWM3_CONFIG_OFFSET    (0x90)
#define PWM_REG_CLK_SEL       PWM_REG_CLK_SEL
#define PWM_REG_CLK_SEL_POS   (0U)
#define PWM_REG_CLK_SEL_LEN   (2U)
#define PWM_REG_CLK_SEL_MSK   (((1U << PWM_REG_CLK_SEL_LEN) - 1) << PWM_REG_CLK_SEL_POS)
#define PWM_REG_CLK_SEL_UMSK  (~(((1U << PWM_REG_CLK_SEL_LEN) - 1) << PWM_REG_CLK_SEL_POS))
#define PWM_OUT_INV           PWM_OUT_INV
#define PWM_OUT_INV_POS       (2U)
#define PWM_OUT_INV_LEN       (1U)
#define PWM_OUT_INV_MSK       (((1U << PWM_OUT_INV_LEN) - 1) << PWM_OUT_INV_POS)
#define PWM_OUT_INV_UMSK      (~(((1U << PWM_OUT_INV_LEN) - 1) << PWM_OUT_INV_POS))
#define PWM_STOP_MODE         PWM_STOP_MODE
#define PWM_STOP_MODE_POS     (3U)
#define PWM_STOP_MODE_LEN     (1U)
#define PWM_STOP_MODE_MSK     (((1U << PWM_STOP_MODE_LEN) - 1) << PWM_STOP_MODE_POS)
#define PWM_STOP_MODE_UMSK    (~(((1U << PWM_STOP_MODE_LEN) - 1) << PWM_STOP_MODE_POS))
#define PWM_SW_FORCE_VAL      PWM_SW_FORCE_VAL
#define PWM_SW_FORCE_VAL_POS  (4U)
#define PWM_SW_FORCE_VAL_LEN  (1U)
#define PWM_SW_FORCE_VAL_MSK  (((1U << PWM_SW_FORCE_VAL_LEN) - 1) << PWM_SW_FORCE_VAL_POS)
#define PWM_SW_FORCE_VAL_UMSK (~(((1U << PWM_SW_FORCE_VAL_LEN) - 1) << PWM_SW_FORCE_VAL_POS))
#define PWM_SW_MODE           PWM_SW_MODE
#define PWM_SW_MODE_POS       (5U)
#define PWM_SW_MODE_LEN       (1U)
#define PWM_SW_MODE_MSK       (((1U << PWM_SW_MODE_LEN) - 1) << PWM_SW_MODE_POS)
#define PWM_SW_MODE_UMSK      (~(((1U << PWM_SW_MODE_LEN) - 1) << PWM_SW_MODE_POS))
#define PWM_STOP_EN           PWM_STOP_EN
#define PWM_STOP_EN_POS       (6U)
#define PWM_STOP_EN_LEN       (1U)
#define PWM_STOP_EN_MSK       (((1U << PWM_STOP_EN_LEN) - 1) << PWM_STOP_EN_POS)
#define PWM_STOP_EN_UMSK      (~(((1U << PWM_STOP_EN_LEN) - 1) << PWM_STOP_EN_POS))
#define PWM_STS_TOP           PWM_STS_TOP
#define PWM_STS_TOP_POS       (7U)
#define PWM_STS_TOP_LEN       (1U)
#define PWM_STS_TOP_MSK       (((1U << PWM_STS_TOP_LEN) - 1) << PWM_STS_TOP_POS)
#define PWM_STS_TOP_UMSK      (~(((1U << PWM_STS_TOP_LEN) - 1) << PWM_STS_TOP_POS))

/* 0x94 : pwm3_interrupt */
#define PWM3_INTERRUPT_OFFSET   (0x94)
#define PWM_INT_PERIOD_CNT      PWM_INT_PERIOD_CNT
#define PWM_INT_PERIOD_CNT_POS  (0U)
#define PWM_INT_PERIOD_CNT_LEN  (16U)
#define PWM_INT_PERIOD_CNT_MSK  (((1U << PWM_INT_PERIOD_CNT_LEN) - 1) << PWM_INT_PERIOD_CNT_POS)
#define PWM_INT_PERIOD_CNT_UMSK (~(((1U << PWM_INT_PERIOD_CNT_LEN) - 1) << PWM_INT_PERIOD_CNT_POS))
#define PWM_INT_ENABLE          PWM_INT_ENABLE
#define PWM_INT_ENABLE_POS      (16U)
#define PWM_INT_ENABLE_LEN      (1U)
#define PWM_INT_ENABLE_MSK      (((1U << PWM_INT_ENABLE_LEN) - 1) << PWM_INT_ENABLE_POS)
#define PWM_INT_ENABLE_UMSK     (~(((1U << PWM_INT_ENABLE_LEN) - 1) << PWM_INT_ENABLE_POS))

/* 0xA0 : pwm4_clkdiv */
#define PWM4_CLKDIV_OFFSET (0xA0)
#define PWM_CLK_DIV        PWM_CLK_DIV
#define PWM_CLK_DIV_POS    (0U)
#define PWM_CLK_DIV_LEN    (16U)
#define PWM_CLK_DIV_MSK    (((1U << PWM_CLK_DIV_LEN) - 1) << PWM_CLK_DIV_POS)
#define PWM_CLK_DIV_UMSK   (~(((1U << PWM_CLK_DIV_LEN) - 1) << PWM_CLK_DIV_POS))

/* 0xA4 : pwm4_thre1 */
#define PWM4_THRE1_OFFSET (0xA4)
#define PWM_THRE1         PWM_THRE1
#define PWM_THRE1_POS     (0U)
#define PWM_THRE1_LEN     (16U)
#define PWM_THRE1_MSK     (((1U << PWM_THRE1_LEN) - 1) << PWM_THRE1_POS)
#define PWM_THRE1_UMSK    (~(((1U << PWM_THRE1_LEN) - 1) << PWM_THRE1_POS))

/* 0xA8 : pwm4_thre2 */
#define PWM4_THRE2_OFFSET (0xA8)
#define PWM_THRE2         PWM_THRE2
#define PWM_THRE2_POS     (0U)
#define PWM_THRE2_LEN     (16U)
#define PWM_THRE2_MSK     (((1U << PWM_THRE2_LEN) - 1) << PWM_THRE2_POS)
#define PWM_THRE2_UMSK    (~(((1U << PWM_THRE2_LEN) - 1) << PWM_THRE2_POS))

/* 0xAC : pwm4_period */
#define PWM4_PERIOD_OFFSET (0xAC)
#define PWM_PERIOD         PWM_PERIOD
#define PWM_PERIOD_POS     (0U)
#define PWM_PERIOD_LEN     (16U)
#define PWM_PERIOD_MSK     (((1U << PWM_PERIOD_LEN) - 1) << PWM_PERIOD_POS)
#define PWM_PERIOD_UMSK    (~(((1U << PWM_PERIOD_LEN) - 1) << PWM_PERIOD_POS))

/* 0xB0 : pwm4_config */
#define PWM4_CONFIG_OFFSET    (0xB0)
#define PWM_REG_CLK_SEL       PWM_REG_CLK_SEL
#define PWM_REG_CLK_SEL_POS   (0U)
#define PWM_REG_CLK_SEL_LEN   (2U)
#define PWM_REG_CLK_SEL_MSK   (((1U << PWM_REG_CLK_SEL_LEN) - 1) << PWM_REG_CLK_SEL_POS)
#define PWM_REG_CLK_SEL_UMSK  (~(((1U << PWM_REG_CLK_SEL_LEN) - 1) << PWM_REG_CLK_SEL_POS))
#define PWM_OUT_INV           PWM_OUT_INV
#define PWM_OUT_INV_POS       (2U)
#define PWM_OUT_INV_LEN       (1U)
#define PWM_OUT_INV_MSK       (((1U << PWM_OUT_INV_LEN) - 1) << PWM_OUT_INV_POS)
#define PWM_OUT_INV_UMSK      (~(((1U << PWM_OUT_INV_LEN) - 1) << PWM_OUT_INV_POS))
#define PWM_STOP_MODE         PWM_STOP_MODE
#define PWM_STOP_MODE_POS     (3U)
#define PWM_STOP_MODE_LEN     (1U)
#define PWM_STOP_MODE_MSK     (((1U << PWM_STOP_MODE_LEN) - 1) << PWM_STOP_MODE_POS)
#define PWM_STOP_MODE_UMSK    (~(((1U << PWM_STOP_MODE_LEN) - 1) << PWM_STOP_MODE_POS))
#define PWM_SW_FORCE_VAL      PWM_SW_FORCE_VAL
#define PWM_SW_FORCE_VAL_POS  (4U)
#define PWM_SW_FORCE_VAL_LEN  (1U)
#define PWM_SW_FORCE_VAL_MSK  (((1U << PWM_SW_FORCE_VAL_LEN) - 1) << PWM_SW_FORCE_VAL_POS)
#define PWM_SW_FORCE_VAL_UMSK (~(((1U << PWM_SW_FORCE_VAL_LEN) - 1) << PWM_SW_FORCE_VAL_POS))
#define PWM_SW_MODE           PWM_SW_MODE
#define PWM_SW_MODE_POS       (5U)
#define PWM_SW_MODE_LEN       (1U)
#define PWM_SW_MODE_MSK       (((1U << PWM_SW_MODE_LEN) - 1) << PWM_SW_MODE_POS)
#define PWM_SW_MODE_UMSK      (~(((1U << PWM_SW_MODE_LEN) - 1) << PWM_SW_MODE_POS))
#define PWM_STOP_EN           PWM_STOP_EN
#define PWM_STOP_EN_POS       (6U)
#define PWM_STOP_EN_LEN       (1U)
#define PWM_STOP_EN_MSK       (((1U << PWM_STOP_EN_LEN) - 1) << PWM_STOP_EN_POS)
#define PWM_STOP_EN_UMSK      (~(((1U << PWM_STOP_EN_LEN) - 1) << PWM_STOP_EN_POS))
#define PWM_STS_TOP           PWM_STS_TOP
#define PWM_STS_TOP_POS       (7U)
#define PWM_STS_TOP_LEN       (1U)
#define PWM_STS_TOP_MSK       (((1U << PWM_STS_TOP_LEN) - 1) << PWM_STS_TOP_POS)
#define PWM_STS_TOP_UMSK      (~(((1U << PWM_STS_TOP_LEN) - 1) << PWM_STS_TOP_POS))

/* 0xB4 : pwm4_interrupt */
#define PWM4_INTERRUPT_OFFSET   (0xB4)
#define PWM_INT_PERIOD_CNT      PWM_INT_PERIOD_CNT
#define PWM_INT_PERIOD_CNT_POS  (0U)
#define PWM_INT_PERIOD_CNT_LEN  (16U)
#define PWM_INT_PERIOD_CNT_MSK  (((1U << PWM_INT_PERIOD_CNT_LEN) - 1) << PWM_INT_PERIOD_CNT_POS)
#define PWM_INT_PERIOD_CNT_UMSK (~(((1U << PWM_INT_PERIOD_CNT_LEN) - 1) << PWM_INT_PERIOD_CNT_POS))
#define PWM_INT_ENABLE          PWM_INT_ENABLE
#define PWM_INT_ENABLE_POS      (16U)
#define PWM_INT_ENABLE_LEN      (1U)
#define PWM_INT_ENABLE_MSK      (((1U << PWM_INT_ENABLE_LEN) - 1) << PWM_INT_ENABLE_POS)
#define PWM_INT_ENABLE_UMSK     (~(((1U << PWM_INT_ENABLE_LEN) - 1) << PWM_INT_ENABLE_POS))

struct pwm_reg {
    /* 0x0 : pwm_int_config */
    union {
        struct
        {
            uint32_t pwm_interrupt_sts : 6;  /* [ 5: 0],          r,        0x0 */
            uint32_t reserved_6_7      : 2;  /* [ 7: 6],       rsvd,        0x0 */
            uint32_t pwm_int_clear     : 6;  /* [13: 8],          w,        0x0 */
            uint32_t reserved_14_31    : 18; /* [31:14],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm_int_config;

    /* 0x4  reserved */
    uint8_t RESERVED0x4[28];

    /* 0x20 : pwm0_clkdiv */
    union {
        struct
        {
            uint32_t pwm_clk_div    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm0_clkdiv;

    /* 0x24 : pwm0_thre1 */
    union {
        struct
        {
            uint32_t pwm_thre1      : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm0_thre1;

    /* 0x28 : pwm0_thre2 */
    union {
        struct
        {
            uint32_t pwm_thre2      : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm0_thre2;

    /* 0x2C : pwm0_period */
    union {
        struct
        {
            uint32_t pwm_period     : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm0_period;

    /* 0x30 : pwm0_config */
    union {
        struct
        {
            uint32_t reg_clk_sel      : 2;  /* [ 1: 0],        r/w,        0x0 */
            uint32_t pwm_out_inv      : 1;  /* [    2],        r/w,        0x0 */
            uint32_t pwm_stop_mode    : 1;  /* [    3],        r/w,        0x1 */
            uint32_t pwm_sw_force_val : 1;  /* [    4],        r/w,        0x0 */
            uint32_t pwm_sw_mode      : 1;  /* [    5],        r/w,        0x0 */
            uint32_t pwm_stop_en      : 1;  /* [    6],        r/w,        0x0 */
            uint32_t pwm_sts_top      : 1;  /* [    7],          r,        0x0 */
            uint32_t reserved_8_31    : 24; /* [31: 8],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm0_config;

    /* 0x34 : pwm0_interrupt */
    union {
        struct
        {
            uint32_t pwm_int_period_cnt : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t pwm_int_enable     : 1;  /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31     : 15; /* [31:17],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm0_interrupt;

    /* 0x38  reserved */
    uint8_t RESERVED0x38[8];

    /* 0x40 : pwm1_clkdiv */
    union {
        struct
        {
            uint32_t pwm_clk_div    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm1_clkdiv;

    /* 0x44 : pwm1_thre1 */
    union {
        struct
        {
            uint32_t pwm_thre1      : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm1_thre1;

    /* 0x48 : pwm1_thre2 */
    union {
        struct
        {
            uint32_t pwm_thre2      : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm1_thre2;

    /* 0x4C : pwm1_period */
    union {
        struct
        {
            uint32_t pwm_period     : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm1_period;

    /* 0x50 : pwm1_config */
    union {
        struct
        {
            uint32_t reg_clk_sel      : 2;  /* [ 1: 0],        r/w,        0x0 */
            uint32_t pwm_out_inv      : 1;  /* [    2],        r/w,        0x0 */
            uint32_t pwm_stop_mode    : 1;  /* [    3],        r/w,        0x1 */
            uint32_t pwm_sw_force_val : 1;  /* [    4],        r/w,        0x0 */
            uint32_t pwm_sw_mode      : 1;  /* [    5],        r/w,        0x0 */
            uint32_t pwm_stop_en      : 1;  /* [    6],        r/w,        0x0 */
            uint32_t pwm_sts_top      : 1;  /* [    7],          r,        0x0 */
            uint32_t reserved_8_31    : 24; /* [31: 8],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm1_config;

    /* 0x54 : pwm1_interrupt */
    union {
        struct
        {
            uint32_t pwm_int_period_cnt : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t pwm_int_enable     : 1;  /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31     : 15; /* [31:17],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm1_interrupt;

    /* 0x58  reserved */
    uint8_t RESERVED0x58[8];

    /* 0x60 : pwm2_clkdiv */
    union {
        struct
        {
            uint32_t pwm_clk_div    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm2_clkdiv;

    /* 0x64 : pwm2_thre1 */
    union {
        struct
        {
            uint32_t pwm_thre1      : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm2_thre1;

    /* 0x68 : pwm2_thre2 */
    union {
        struct
        {
            uint32_t pwm_thre2      : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm2_thre2;

    /* 0x6C : pwm2_period */
    union {
        struct
        {
            uint32_t pwm_period     : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm2_period;

    /* 0x70 : pwm2_config */
    union {
        struct
        {
            uint32_t reg_clk_sel      : 2;  /* [ 1: 0],        r/w,        0x0 */
            uint32_t pwm_out_inv      : 1;  /* [    2],        r/w,        0x0 */
            uint32_t pwm_stop_mode    : 1;  /* [    3],        r/w,        0x1 */
            uint32_t pwm_sw_force_val : 1;  /* [    4],        r/w,        0x0 */
            uint32_t pwm_sw_mode      : 1;  /* [    5],        r/w,        0x0 */
            uint32_t pwm_stop_en      : 1;  /* [    6],        r/w,        0x0 */
            uint32_t pwm_sts_top      : 1;  /* [    7],          r,        0x0 */
            uint32_t reserved_8_31    : 24; /* [31: 8],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm2_config;

    /* 0x74 : pwm2_interrupt */
    union {
        struct
        {
            uint32_t pwm_int_period_cnt : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t pwm_int_enable     : 1;  /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31     : 15; /* [31:17],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm2_interrupt;

    /* 0x78  reserved */
    uint8_t RESERVED0x78[8];

    /* 0x80 : pwm3_clkdiv */
    union {
        struct
        {
            uint32_t pwm_clk_div    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm3_clkdiv;

    /* 0x84 : pwm3_thre1 */
    union {
        struct
        {
            uint32_t pwm_thre1      : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm3_thre1;

    /* 0x88 : pwm3_thre2 */
    union {
        struct
        {
            uint32_t pwm_thre2      : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm3_thre2;

    /* 0x8C : pwm3_period */
    union {
        struct
        {
            uint32_t pwm_period     : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm3_period;

    /* 0x90 : pwm3_config */
    union {
        struct
        {
            uint32_t reg_clk_sel      : 2;  /* [ 1: 0],        r/w,        0x0 */
            uint32_t pwm_out_inv      : 1;  /* [    2],        r/w,        0x0 */
            uint32_t pwm_stop_mode    : 1;  /* [    3],        r/w,        0x1 */
            uint32_t pwm_sw_force_val : 1;  /* [    4],        r/w,        0x0 */
            uint32_t pwm_sw_mode      : 1;  /* [    5],        r/w,        0x0 */
            uint32_t pwm_stop_en      : 1;  /* [    6],        r/w,        0x0 */
            uint32_t pwm_sts_top      : 1;  /* [    7],          r,        0x0 */
            uint32_t reserved_8_31    : 24; /* [31: 8],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm3_config;

    /* 0x94 : pwm3_interrupt */
    union {
        struct
        {
            uint32_t pwm_int_period_cnt : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t pwm_int_enable     : 1;  /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31     : 15; /* [31:17],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm3_interrupt;

    /* 0x98  reserved */
    uint8_t RESERVED0x98[8];

    /* 0xA0 : pwm4_clkdiv */
    union {
        struct
        {
            uint32_t pwm_clk_div    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm4_clkdiv;

    /* 0xA4 : pwm4_thre1 */
    union {
        struct
        {
            uint32_t pwm_thre1      : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm4_thre1;

    /* 0xA8 : pwm4_thre2 */
    union {
        struct
        {
            uint32_t pwm_thre2      : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm4_thre2;

    /* 0xAC : pwm4_period */
    union {
        struct
        {
            uint32_t pwm_period     : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm4_period;

    /* 0xB0 : pwm4_config */
    union {
        struct
        {
            uint32_t reg_clk_sel      : 2;  /* [ 1: 0],        r/w,        0x0 */
            uint32_t pwm_out_inv      : 1;  /* [    2],        r/w,        0x0 */
            uint32_t pwm_stop_mode    : 1;  /* [    3],        r/w,        0x1 */
            uint32_t pwm_sw_force_val : 1;  /* [    4],        r/w,        0x0 */
            uint32_t pwm_sw_mode      : 1;  /* [    5],        r/w,        0x0 */
            uint32_t pwm_stop_en      : 1;  /* [    6],        r/w,        0x0 */
            uint32_t pwm_sts_top      : 1;  /* [    7],          r,        0x0 */
            uint32_t reserved_8_31    : 24; /* [31: 8],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm4_config;

    /* 0xB4 : pwm4_interrupt */
    union {
        struct
        {
            uint32_t pwm_int_period_cnt : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t pwm_int_enable     : 1;  /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31     : 15; /* [31:17],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm4_interrupt;
};

typedef volatile struct pwm_reg pwm_reg_t;

/*Following is reg patch*/

/* 0x0 : pwm_clkdiv */
#define PWM_CLKDIV_OFFSET (0x0)
#define PWM_CLK_DIV       PWM_CLK_DIV
#define PWM_CLK_DIV_POS   (0U)
#define PWM_CLK_DIV_LEN   (16U)
#define PWM_CLK_DIV_MSK   (((1U << PWM_CLK_DIV_LEN) - 1) << PWM_CLK_DIV_POS)
#define PWM_CLK_DIV_UMSK  (~(((1U << PWM_CLK_DIV_LEN) - 1) << PWM_CLK_DIV_POS))

/* 0x4 : pwm_thre1 */
#define PWM_THRE1_OFFSET (0x4)
#define PWM_THRE1        PWM_THRE1
#define PWM_THRE1_POS    (0U)
#define PWM_THRE1_LEN    (16U)
#define PWM_THRE1_MSK    (((1U << PWM_THRE1_LEN) - 1) << PWM_THRE1_POS)
#define PWM_THRE1_UMSK   (~(((1U << PWM_THRE1_LEN) - 1) << PWM_THRE1_POS))

/* 0x8 : pwm_thre2 */
#define PWM_THRE2_OFFSET (0x8)
#define PWM_THRE2        PWM_THRE2
#define PWM_THRE2_POS    (0U)
#define PWM_THRE2_LEN    (16U)
#define PWM_THRE2_MSK    (((1U << PWM_THRE2_LEN) - 1) << PWM_THRE2_POS)
#define PWM_THRE2_UMSK   (~(((1U << PWM_THRE2_LEN) - 1) << PWM_THRE2_POS))

/* 0xc : pwm_period */
#define PWM_PERIOD_OFFSET (0xc)
#define PWM_PERIOD        PWM_PERIOD
#define PWM_PERIOD_POS    (0U)
#define PWM_PERIOD_LEN    (16U)
#define PWM_PERIOD_MSK    (((1U << PWM_PERIOD_LEN) - 1) << PWM_PERIOD_POS)
#define PWM_PERIOD_UMSK   (~(((1U << PWM_PERIOD_LEN) - 1) << PWM_PERIOD_POS))

/* 0x10 : pwm_config */
#define PWM_CONFIG_OFFSET     (0x10)
#define PWM_REG_CLK_SEL       PWM_REG_CLK_SEL
#define PWM_REG_CLK_SEL_POS   (0U)
#define PWM_REG_CLK_SEL_LEN   (2U)
#define PWM_REG_CLK_SEL_MSK   (((1U << PWM_REG_CLK_SEL_LEN) - 1) << PWM_REG_CLK_SEL_POS)
#define PWM_REG_CLK_SEL_UMSK  (~(((1U << PWM_REG_CLK_SEL_LEN) - 1) << PWM_REG_CLK_SEL_POS))
#define PWM_OUT_INV           PWM_OUT_INV
#define PWM_OUT_INV_POS       (2U)
#define PWM_OUT_INV_LEN       (1U)
#define PWM_OUT_INV_MSK       (((1U << PWM_OUT_INV_LEN) - 1) << PWM_OUT_INV_POS)
#define PWM_OUT_INV_UMSK      (~(((1U << PWM_OUT_INV_LEN) - 1) << PWM_OUT_INV_POS))
#define PWM_STOP_MODE         PWM_STOP_MODE
#define PWM_STOP_MODE_POS     (3U)
#define PWM_STOP_MODE_LEN     (1U)
#define PWM_STOP_MODE_MSK     (((1U << PWM_STOP_MODE_LEN) - 1) << PWM_STOP_MODE_POS)
#define PWM_STOP_MODE_UMSK    (~(((1U << PWM_STOP_MODE_LEN) - 1) << PWM_STOP_MODE_POS))
#define PWM_SW_FORCE_VAL      PWM_SW_FORCE_VAL
#define PWM_SW_FORCE_VAL_POS  (4U)
#define PWM_SW_FORCE_VAL_LEN  (1U)
#define PWM_SW_FORCE_VAL_MSK  (((1U << PWM_SW_FORCE_VAL_LEN) - 1) << PWM_SW_FORCE_VAL_POS)
#define PWM_SW_FORCE_VAL_UMSK (~(((1U << PWM_SW_FORCE_VAL_LEN) - 1) << PWM_SW_FORCE_VAL_POS))
#define PWM_SW_MODE           PWM_SW_MODE
#define PWM_SW_MODE_POS       (5U)
#define PWM_SW_MODE_LEN       (1U)
#define PWM_SW_MODE_MSK       (((1U << PWM_SW_MODE_LEN) - 1) << PWM_SW_MODE_POS)
#define PWM_SW_MODE_UMSK      (~(((1U << PWM_SW_MODE_LEN) - 1) << PWM_SW_MODE_POS))
#define PWM_STOP_EN           PWM_STOP_EN
#define PWM_STOP_EN_POS       (6U)
#define PWM_STOP_EN_LEN       (1U)
#define PWM_STOP_EN_MSK       (((1U << PWM_STOP_EN_LEN) - 1) << PWM_STOP_EN_POS)
#define PWM_STOP_EN_UMSK      (~(((1U << PWM_STOP_EN_LEN) - 1) << PWM_STOP_EN_POS))
#define PWM_STS_TOP           PWM_STS_TOP
#define PWM_STS_TOP_POS       (7U)
#define PWM_STS_TOP_LEN       (1U)
#define PWM_STS_TOP_MSK       (((1U << PWM_STS_TOP_LEN) - 1) << PWM_STS_TOP_POS)
#define PWM_STS_TOP_UMSK      (~(((1U << PWM_STS_TOP_LEN) - 1) << PWM_STS_TOP_POS))

/* 0x14 : pwm_interrupt */
#define PWM_INTERRUPT_OFFSET    (0x14)
#define PWM_INT_PERIOD_CNT      PWM_INT_PERIOD_CNT
#define PWM_INT_PERIOD_CNT_POS  (0U)
#define PWM_INT_PERIOD_CNT_LEN  (16U)
#define PWM_INT_PERIOD_CNT_MSK  (((1U << PWM_INT_PERIOD_CNT_LEN) - 1) << PWM_INT_PERIOD_CNT_POS)
#define PWM_INT_PERIOD_CNT_UMSK (~(((1U << PWM_INT_PERIOD_CNT_LEN) - 1) << PWM_INT_PERIOD_CNT_POS))
#define PWM_INT_ENABLE          PWM_INT_ENABLE
#define PWM_INT_ENABLE_POS      (16U)
#define PWM_INT_ENABLE_LEN      (1U)
#define PWM_INT_ENABLE_MSK      (((1U << PWM_INT_ENABLE_LEN) - 1) << PWM_INT_ENABLE_POS)
#define PWM_INT_ENABLE_UMSK     (~(((1U << PWM_INT_ENABLE_LEN) - 1) << PWM_INT_ENABLE_POS))

struct pwm_channel_reg {
    /* 0x0 : pwm_clkdiv */
    union {
        struct
        {
            uint32_t pwm_clk_div    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm_clkdiv;

    /* 0x4 : pwm_thre1 */
    union {
        struct
        {
            uint32_t pwm_thre1      : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm_thre1;

    /* 0x8 : pwm_thre2 */
    union {
        struct
        {
            uint32_t pwm_thre2      : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm_thre2;

    /* 0xc : pwm_period */
    union {
        struct
        {
            uint32_t pwm_period     : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm_period;

    /* 0x10 : pwm_config */
    union {
        struct
        {
            uint32_t reg_clk_sel      : 2;  /* [ 1: 0],        r/w,        0x0 */
            uint32_t pwm_out_inv      : 1;  /* [    2],        r/w,        0x0 */
            uint32_t pwm_stop_mode    : 1;  /* [    3],        r/w,        0x1 */
            uint32_t pwm_sw_force_val : 1;  /* [    4],        r/w,        0x0 */
            uint32_t pwm_sw_mode      : 1;  /* [    5],        r/w,        0x0 */
            uint32_t pwm_stop_en      : 1;  /* [    6],        r/w,        0x0 */
            uint32_t pwm_sts_top      : 1;  /* [    7],          r,        0x0 */
            uint32_t reserved_8_31    : 24; /* [31: 8],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm_config;

    /* 0x14 : pwm_interrupt */
    union {
        struct
        {
            uint32_t pwm_int_period_cnt : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t pwm_int_enable     : 1;  /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31     : 15; /* [31:17],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } pwm_interrupt;
};

typedef volatile struct pwm_channel_reg pwm_channel_reg_t;

#define PWM_CHANNEL_OFFSET 0x20

#endif /* __PWM_REG_H__ */
