/**
  ******************************************************************************
  * @file    timer_reg.h
  * @version V1.2
  * @date    2020-03-30
  * @brief   This file is the description of.IP register
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 Bouffalo Lab</center></h2>
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
#ifndef __TIMER_REG_H__
#define __TIMER_REG_H__

#include "bl702.h"

/* 0x0 : TCCR */
#define TIMER_TCCR_OFFSET (0x0)
#define TIMER_CS_1        TIMER_CS_1
#define TIMER_CS_1_POS    (2U)
#define TIMER_CS_1_LEN    (2U)
#define TIMER_CS_1_MSK    (((1U << TIMER_CS_1_LEN) - 1) << TIMER_CS_1_POS)
#define TIMER_CS_1_UMSK   (~(((1U << TIMER_CS_1_LEN) - 1) << TIMER_CS_1_POS))
#define TIMER_CS_2        TIMER_CS_2
#define TIMER_CS_2_POS    (5U)
#define TIMER_CS_2_LEN    (2U)
#define TIMER_CS_2_MSK    (((1U << TIMER_CS_2_LEN) - 1) << TIMER_CS_2_POS)
#define TIMER_CS_2_UMSK   (~(((1U << TIMER_CS_2_LEN) - 1) << TIMER_CS_2_POS))
#define TIMER_CS_WDT      TIMER_CS_WDT
#define TIMER_CS_WDT_POS  (8U)
#define TIMER_CS_WDT_LEN  (2U)
#define TIMER_CS_WDT_MSK  (((1U << TIMER_CS_WDT_LEN) - 1) << TIMER_CS_WDT_POS)
#define TIMER_CS_WDT_UMSK (~(((1U << TIMER_CS_WDT_LEN) - 1) << TIMER_CS_WDT_POS))

/* 0x10 : TMR2_0 */
#define TIMER_TMR2_0_OFFSET (0x10)
#define TIMER_TMR           TIMER_TMR
#define TIMER_TMR_POS       (0U)
#define TIMER_TMR_LEN       (32U)
#define TIMER_TMR_MSK       (((1U << TIMER_TMR_LEN) - 1) << TIMER_TMR_POS)
#define TIMER_TMR_UMSK      (~(((1U << TIMER_TMR_LEN) - 1) << TIMER_TMR_POS))

/* 0x14 : TMR2_1 */
#define TIMER_TMR2_1_OFFSET (0x14)
#define TIMER_TMR           TIMER_TMR
#define TIMER_TMR_POS       (0U)
#define TIMER_TMR_LEN       (32U)
#define TIMER_TMR_MSK       (((1U << TIMER_TMR_LEN) - 1) << TIMER_TMR_POS)
#define TIMER_TMR_UMSK      (~(((1U << TIMER_TMR_LEN) - 1) << TIMER_TMR_POS))

/* 0x18 : TMR2_2 */
#define TIMER_TMR2_2_OFFSET (0x18)
#define TIMER_TMR           TIMER_TMR
#define TIMER_TMR_POS       (0U)
#define TIMER_TMR_LEN       (32U)
#define TIMER_TMR_MSK       (((1U << TIMER_TMR_LEN) - 1) << TIMER_TMR_POS)
#define TIMER_TMR_UMSK      (~(((1U << TIMER_TMR_LEN) - 1) << TIMER_TMR_POS))

/* 0x1C : TMR3_0 */
#define TIMER_TMR3_0_OFFSET (0x1C)
#define TIMER_TMR           TIMER_TMR
#define TIMER_TMR_POS       (0U)
#define TIMER_TMR_LEN       (32U)
#define TIMER_TMR_MSK       (((1U << TIMER_TMR_LEN) - 1) << TIMER_TMR_POS)
#define TIMER_TMR_UMSK      (~(((1U << TIMER_TMR_LEN) - 1) << TIMER_TMR_POS))

/* 0x20 : TMR3_1 */
#define TIMER_TMR3_1_OFFSET (0x20)
#define TIMER_TMR           TIMER_TMR
#define TIMER_TMR_POS       (0U)
#define TIMER_TMR_LEN       (32U)
#define TIMER_TMR_MSK       (((1U << TIMER_TMR_LEN) - 1) << TIMER_TMR_POS)
#define TIMER_TMR_UMSK      (~(((1U << TIMER_TMR_LEN) - 1) << TIMER_TMR_POS))

/* 0x24 : TMR3_2 */
#define TIMER_TMR3_2_OFFSET (0x24)
#define TIMER_TMR           TIMER_TMR
#define TIMER_TMR_POS       (0U)
#define TIMER_TMR_LEN       (32U)
#define TIMER_TMR_MSK       (((1U << TIMER_TMR_LEN) - 1) << TIMER_TMR_POS)
#define TIMER_TMR_UMSK      (~(((1U << TIMER_TMR_LEN) - 1) << TIMER_TMR_POS))

/* 0x2C : TCR2 */
#define TIMER_TCR2_OFFSET (0x2C)
#define TIMER_TCR         TIMER_TCR
#define TIMER_TCR_POS     (0U)
#define TIMER_TCR_LEN     (32U)
#define TIMER_TCR_MSK     (((1U << TIMER_TCR_LEN) - 1) << TIMER_TCR_POS)
#define TIMER_TCR_UMSK    (~(((1U << TIMER_TCR_LEN) - 1) << TIMER_TCR_POS))

/* 0x30 : TCR3 */
#define TIMER_TCR3_OFFSET (0x30)
#define TIMER_TCR         TIMER_TCR
#define TIMER_TCR_POS     (0U)
#define TIMER_TCR_LEN     (32U)
#define TIMER_TCR_MSK     (((1U << TIMER_TCR_LEN) - 1) << TIMER_TCR_POS)
#define TIMER_TCR_UMSK    (~(((1U << TIMER_TCR_LEN) - 1) << TIMER_TCR_POS))

/* 0x38 : TMSR2 */
#define TIMER_TMSR2_OFFSET (0x38)
#define TIMER_TMSR_0       TIMER_TMSR_0
#define TIMER_TMSR_0_POS   (0U)
#define TIMER_TMSR_0_LEN   (1U)
#define TIMER_TMSR_0_MSK   (((1U << TIMER_TMSR_0_LEN) - 1) << TIMER_TMSR_0_POS)
#define TIMER_TMSR_0_UMSK  (~(((1U << TIMER_TMSR_0_LEN) - 1) << TIMER_TMSR_0_POS))
#define TIMER_TMSR_1       TIMER_TMSR_1
#define TIMER_TMSR_1_POS   (1U)
#define TIMER_TMSR_1_LEN   (1U)
#define TIMER_TMSR_1_MSK   (((1U << TIMER_TMSR_1_LEN) - 1) << TIMER_TMSR_1_POS)
#define TIMER_TMSR_1_UMSK  (~(((1U << TIMER_TMSR_1_LEN) - 1) << TIMER_TMSR_1_POS))
#define TIMER_TMSR_2       TIMER_TMSR_2
#define TIMER_TMSR_2_POS   (2U)
#define TIMER_TMSR_2_LEN   (1U)
#define TIMER_TMSR_2_MSK   (((1U << TIMER_TMSR_2_LEN) - 1) << TIMER_TMSR_2_POS)
#define TIMER_TMSR_2_UMSK  (~(((1U << TIMER_TMSR_2_LEN) - 1) << TIMER_TMSR_2_POS))

/* 0x3C : TMSR3 */
#define TIMER_TMSR3_OFFSET (0x3C)
#define TIMER_TMSR_0       TIMER_TMSR_0
#define TIMER_TMSR_0_POS   (0U)
#define TIMER_TMSR_0_LEN   (1U)
#define TIMER_TMSR_0_MSK   (((1U << TIMER_TMSR_0_LEN) - 1) << TIMER_TMSR_0_POS)
#define TIMER_TMSR_0_UMSK  (~(((1U << TIMER_TMSR_0_LEN) - 1) << TIMER_TMSR_0_POS))
#define TIMER_TMSR_1       TIMER_TMSR_1
#define TIMER_TMSR_1_POS   (1U)
#define TIMER_TMSR_1_LEN   (1U)
#define TIMER_TMSR_1_MSK   (((1U << TIMER_TMSR_1_LEN) - 1) << TIMER_TMSR_1_POS)
#define TIMER_TMSR_1_UMSK  (~(((1U << TIMER_TMSR_1_LEN) - 1) << TIMER_TMSR_1_POS))
#define TIMER_TMSR_2       TIMER_TMSR_2
#define TIMER_TMSR_2_POS   (2U)
#define TIMER_TMSR_2_LEN   (1U)
#define TIMER_TMSR_2_MSK   (((1U << TIMER_TMSR_2_LEN) - 1) << TIMER_TMSR_2_POS)
#define TIMER_TMSR_2_UMSK  (~(((1U << TIMER_TMSR_2_LEN) - 1) << TIMER_TMSR_2_POS))

/* 0x44 : TIER2 */
#define TIMER_TIER2_OFFSET (0x44)
#define TIMER_TIER_0       TIMER_TIER_0
#define TIMER_TIER_0_POS   (0U)
#define TIMER_TIER_0_LEN   (1U)
#define TIMER_TIER_0_MSK   (((1U << TIMER_TIER_0_LEN) - 1) << TIMER_TIER_0_POS)
#define TIMER_TIER_0_UMSK  (~(((1U << TIMER_TIER_0_LEN) - 1) << TIMER_TIER_0_POS))
#define TIMER_TIER_1       TIMER_TIER_1
#define TIMER_TIER_1_POS   (1U)
#define TIMER_TIER_1_LEN   (1U)
#define TIMER_TIER_1_MSK   (((1U << TIMER_TIER_1_LEN) - 1) << TIMER_TIER_1_POS)
#define TIMER_TIER_1_UMSK  (~(((1U << TIMER_TIER_1_LEN) - 1) << TIMER_TIER_1_POS))
#define TIMER_TIER_2       TIMER_TIER_2
#define TIMER_TIER_2_POS   (2U)
#define TIMER_TIER_2_LEN   (1U)
#define TIMER_TIER_2_MSK   (((1U << TIMER_TIER_2_LEN) - 1) << TIMER_TIER_2_POS)
#define TIMER_TIER_2_UMSK  (~(((1U << TIMER_TIER_2_LEN) - 1) << TIMER_TIER_2_POS))

/* 0x48 : TIER3 */
#define TIMER_TIER3_OFFSET (0x48)
#define TIMER_TIER_0       TIMER_TIER_0
#define TIMER_TIER_0_POS   (0U)
#define TIMER_TIER_0_LEN   (1U)
#define TIMER_TIER_0_MSK   (((1U << TIMER_TIER_0_LEN) - 1) << TIMER_TIER_0_POS)
#define TIMER_TIER_0_UMSK  (~(((1U << TIMER_TIER_0_LEN) - 1) << TIMER_TIER_0_POS))
#define TIMER_TIER_1       TIMER_TIER_1
#define TIMER_TIER_1_POS   (1U)
#define TIMER_TIER_1_LEN   (1U)
#define TIMER_TIER_1_MSK   (((1U << TIMER_TIER_1_LEN) - 1) << TIMER_TIER_1_POS)
#define TIMER_TIER_1_UMSK  (~(((1U << TIMER_TIER_1_LEN) - 1) << TIMER_TIER_1_POS))
#define TIMER_TIER_2       TIMER_TIER_2
#define TIMER_TIER_2_POS   (2U)
#define TIMER_TIER_2_LEN   (1U)
#define TIMER_TIER_2_MSK   (((1U << TIMER_TIER_2_LEN) - 1) << TIMER_TIER_2_POS)
#define TIMER_TIER_2_UMSK  (~(((1U << TIMER_TIER_2_LEN) - 1) << TIMER_TIER_2_POS))

/* 0x50 : TPLVR2 */
#define TIMER_TPLVR2_OFFSET (0x50)
#define TIMER_TPLVR         TIMER_TPLVR
#define TIMER_TPLVR_POS     (0U)
#define TIMER_TPLVR_LEN     (32U)
#define TIMER_TPLVR_MSK     (((1U << TIMER_TPLVR_LEN) - 1) << TIMER_TPLVR_POS)
#define TIMER_TPLVR_UMSK    (~(((1U << TIMER_TPLVR_LEN) - 1) << TIMER_TPLVR_POS))

/* 0x54 : TPLVR3 */
#define TIMER_TPLVR3_OFFSET (0x54)
#define TIMER_TPLVR         TIMER_TPLVR
#define TIMER_TPLVR_POS     (0U)
#define TIMER_TPLVR_LEN     (32U)
#define TIMER_TPLVR_MSK     (((1U << TIMER_TPLVR_LEN) - 1) << TIMER_TPLVR_POS)
#define TIMER_TPLVR_UMSK    (~(((1U << TIMER_TPLVR_LEN) - 1) << TIMER_TPLVR_POS))

/* 0x5C : TPLCR2 */
#define TIMER_TPLCR2_OFFSET (0x5C)
#define TIMER_TPLCR         TIMER_TPLCR
#define TIMER_TPLCR_POS     (0U)
#define TIMER_TPLCR_LEN     (2U)
#define TIMER_TPLCR_MSK     (((1U << TIMER_TPLCR_LEN) - 1) << TIMER_TPLCR_POS)
#define TIMER_TPLCR_UMSK    (~(((1U << TIMER_TPLCR_LEN) - 1) << TIMER_TPLCR_POS))

/* 0x60 : TPLCR3 */
#define TIMER_TPLCR3_OFFSET (0x60)
#define TIMER_TPLCR         TIMER_TPLCR
#define TIMER_TPLCR_POS     (0U)
#define TIMER_TPLCR_LEN     (2U)
#define TIMER_TPLCR_MSK     (((1U << TIMER_TPLCR_LEN) - 1) << TIMER_TPLCR_POS)
#define TIMER_TPLCR_UMSK    (~(((1U << TIMER_TPLCR_LEN) - 1) << TIMER_TPLCR_POS))

/* 0x64 : WMER */
#define TIMER_WMER_OFFSET (0x64)
#define TIMER_WE          TIMER_WE
#define TIMER_WE_POS      (0U)
#define TIMER_WE_LEN      (1U)
#define TIMER_WE_MSK      (((1U << TIMER_WE_LEN) - 1) << TIMER_WE_POS)
#define TIMER_WE_UMSK     (~(((1U << TIMER_WE_LEN) - 1) << TIMER_WE_POS))
#define TIMER_WRIE        TIMER_WRIE
#define TIMER_WRIE_POS    (1U)
#define TIMER_WRIE_LEN    (1U)
#define TIMER_WRIE_MSK    (((1U << TIMER_WRIE_LEN) - 1) << TIMER_WRIE_POS)
#define TIMER_WRIE_UMSK   (~(((1U << TIMER_WRIE_LEN) - 1) << TIMER_WRIE_POS))

/* 0x68 : WMR */
#define TIMER_WMR_OFFSET (0x68)
#define TIMER_WMR        TIMER_WMR
#define TIMER_WMR_POS    (0U)
#define TIMER_WMR_LEN    (16U)
#define TIMER_WMR_MSK    (((1U << TIMER_WMR_LEN) - 1) << TIMER_WMR_POS)
#define TIMER_WMR_UMSK   (~(((1U << TIMER_WMR_LEN) - 1) << TIMER_WMR_POS))

/* 0x6C : WVR */
#define TIMER_WVR_OFFSET (0x6C)
#define TIMER_WVR        TIMER_WVR
#define TIMER_WVR_POS    (0U)
#define TIMER_WVR_LEN    (16U)
#define TIMER_WVR_MSK    (((1U << TIMER_WVR_LEN) - 1) << TIMER_WVR_POS)
#define TIMER_WVR_UMSK   (~(((1U << TIMER_WVR_LEN) - 1) << TIMER_WVR_POS))

/* 0x70 : WSR */
#define TIMER_WSR_OFFSET (0x70)
#define TIMER_WTS        TIMER_WTS
#define TIMER_WTS_POS    (0U)
#define TIMER_WTS_LEN    (1U)
#define TIMER_WTS_MSK    (((1U << TIMER_WTS_LEN) - 1) << TIMER_WTS_POS)
#define TIMER_WTS_UMSK   (~(((1U << TIMER_WTS_LEN) - 1) << TIMER_WTS_POS))

/* 0x78 : TICR2 */
#define TIMER_TICR2_OFFSET (0x78)
#define TIMER_TCLR_0       TIMER_TCLR_0
#define TIMER_TCLR_0_POS   (0U)
#define TIMER_TCLR_0_LEN   (1U)
#define TIMER_TCLR_0_MSK   (((1U << TIMER_TCLR_0_LEN) - 1) << TIMER_TCLR_0_POS)
#define TIMER_TCLR_0_UMSK  (~(((1U << TIMER_TCLR_0_LEN) - 1) << TIMER_TCLR_0_POS))
#define TIMER_TCLR_1       TIMER_TCLR_1
#define TIMER_TCLR_1_POS   (1U)
#define TIMER_TCLR_1_LEN   (1U)
#define TIMER_TCLR_1_MSK   (((1U << TIMER_TCLR_1_LEN) - 1) << TIMER_TCLR_1_POS)
#define TIMER_TCLR_1_UMSK  (~(((1U << TIMER_TCLR_1_LEN) - 1) << TIMER_TCLR_1_POS))
#define TIMER_TCLR_2       TIMER_TCLR_2
#define TIMER_TCLR_2_POS   (2U)
#define TIMER_TCLR_2_LEN   (1U)
#define TIMER_TCLR_2_MSK   (((1U << TIMER_TCLR_2_LEN) - 1) << TIMER_TCLR_2_POS)
#define TIMER_TCLR_2_UMSK  (~(((1U << TIMER_TCLR_2_LEN) - 1) << TIMER_TCLR_2_POS))

/* 0x7C : TICR3 */
#define TIMER_TICR3_OFFSET (0x7C)
#define TIMER_TCLR_0       TIMER_TCLR_0
#define TIMER_TCLR_0_POS   (0U)
#define TIMER_TCLR_0_LEN   (1U)
#define TIMER_TCLR_0_MSK   (((1U << TIMER_TCLR_0_LEN) - 1) << TIMER_TCLR_0_POS)
#define TIMER_TCLR_0_UMSK  (~(((1U << TIMER_TCLR_0_LEN) - 1) << TIMER_TCLR_0_POS))
#define TIMER_TCLR_1       TIMER_TCLR_1
#define TIMER_TCLR_1_POS   (1U)
#define TIMER_TCLR_1_LEN   (1U)
#define TIMER_TCLR_1_MSK   (((1U << TIMER_TCLR_1_LEN) - 1) << TIMER_TCLR_1_POS)
#define TIMER_TCLR_1_UMSK  (~(((1U << TIMER_TCLR_1_LEN) - 1) << TIMER_TCLR_1_POS))
#define TIMER_TCLR_2       TIMER_TCLR_2
#define TIMER_TCLR_2_POS   (2U)
#define TIMER_TCLR_2_LEN   (1U)
#define TIMER_TCLR_2_MSK   (((1U << TIMER_TCLR_2_LEN) - 1) << TIMER_TCLR_2_POS)
#define TIMER_TCLR_2_UMSK  (~(((1U << TIMER_TCLR_2_LEN) - 1) << TIMER_TCLR_2_POS))

/* 0x80 : WICR */
#define TIMER_WICR_OFFSET (0x80)
#define TIMER_WICLR       TIMER_WICLR
#define TIMER_WICLR_POS   (0U)
#define TIMER_WICLR_LEN   (1U)
#define TIMER_WICLR_MSK   (((1U << TIMER_WICLR_LEN) - 1) << TIMER_WICLR_POS)
#define TIMER_WICLR_UMSK  (~(((1U << TIMER_WICLR_LEN) - 1) << TIMER_WICLR_POS))

/* 0x84 : TCER */
#define TIMER_TCER_OFFSET (0x84)
#define TIMER2_EN         TIMER2_EN
#define TIMER2_EN_POS     (1U)
#define TIMER2_EN_LEN     (1U)
#define TIMER2_EN_MSK     (((1U << TIMER2_EN_LEN) - 1) << TIMER2_EN_POS)
#define TIMER2_EN_UMSK    (~(((1U << TIMER2_EN_LEN) - 1) << TIMER2_EN_POS))
#define TIMER3_EN         TIMER3_EN
#define TIMER3_EN_POS     (2U)
#define TIMER3_EN_LEN     (1U)
#define TIMER3_EN_MSK     (((1U << TIMER3_EN_LEN) - 1) << TIMER3_EN_POS)
#define TIMER3_EN_UMSK    (~(((1U << TIMER3_EN_LEN) - 1) << TIMER3_EN_POS))

/* 0x88 : TCMR */
#define TIMER_TCMR_OFFSET (0x88)
#define TIMER2_MODE       TIMER2_MODE
#define TIMER2_MODE_POS   (1U)
#define TIMER2_MODE_LEN   (1U)
#define TIMER2_MODE_MSK   (((1U << TIMER2_MODE_LEN) - 1) << TIMER2_MODE_POS)
#define TIMER2_MODE_UMSK  (~(((1U << TIMER2_MODE_LEN) - 1) << TIMER2_MODE_POS))
#define TIMER3_MODE       TIMER3_MODE
#define TIMER3_MODE_POS   (2U)
#define TIMER3_MODE_LEN   (1U)
#define TIMER3_MODE_MSK   (((1U << TIMER3_MODE_LEN) - 1) << TIMER3_MODE_POS)
#define TIMER3_MODE_UMSK  (~(((1U << TIMER3_MODE_LEN) - 1) << TIMER3_MODE_POS))

/* 0x90 : TILR2 */
#define TIMER_TILR2_OFFSET (0x90)
#define TIMER_TILR_0       TIMER_TILR_0
#define TIMER_TILR_0_POS   (0U)
#define TIMER_TILR_0_LEN   (1U)
#define TIMER_TILR_0_MSK   (((1U << TIMER_TILR_0_LEN) - 1) << TIMER_TILR_0_POS)
#define TIMER_TILR_0_UMSK  (~(((1U << TIMER_TILR_0_LEN) - 1) << TIMER_TILR_0_POS))
#define TIMER_TILR_1       TIMER_TILR_1
#define TIMER_TILR_1_POS   (1U)
#define TIMER_TILR_1_LEN   (1U)
#define TIMER_TILR_1_MSK   (((1U << TIMER_TILR_1_LEN) - 1) << TIMER_TILR_1_POS)
#define TIMER_TILR_1_UMSK  (~(((1U << TIMER_TILR_1_LEN) - 1) << TIMER_TILR_1_POS))
#define TIMER_TILR_2       TIMER_TILR_2
#define TIMER_TILR_2_POS   (2U)
#define TIMER_TILR_2_LEN   (1U)
#define TIMER_TILR_2_MSK   (((1U << TIMER_TILR_2_LEN) - 1) << TIMER_TILR_2_POS)
#define TIMER_TILR_2_UMSK  (~(((1U << TIMER_TILR_2_LEN) - 1) << TIMER_TILR_2_POS))

/* 0x94 : TILR3 */
#define TIMER_TILR3_OFFSET (0x94)
#define TIMER_TILR_0       TIMER_TILR_0
#define TIMER_TILR_0_POS   (0U)
#define TIMER_TILR_0_LEN   (1U)
#define TIMER_TILR_0_MSK   (((1U << TIMER_TILR_0_LEN) - 1) << TIMER_TILR_0_POS)
#define TIMER_TILR_0_UMSK  (~(((1U << TIMER_TILR_0_LEN) - 1) << TIMER_TILR_0_POS))
#define TIMER_TILR_1       TIMER_TILR_1
#define TIMER_TILR_1_POS   (1U)
#define TIMER_TILR_1_LEN   (1U)
#define TIMER_TILR_1_MSK   (((1U << TIMER_TILR_1_LEN) - 1) << TIMER_TILR_1_POS)
#define TIMER_TILR_1_UMSK  (~(((1U << TIMER_TILR_1_LEN) - 1) << TIMER_TILR_1_POS))
#define TIMER_TILR_2       TIMER_TILR_2
#define TIMER_TILR_2_POS   (2U)
#define TIMER_TILR_2_LEN   (1U)
#define TIMER_TILR_2_MSK   (((1U << TIMER_TILR_2_LEN) - 1) << TIMER_TILR_2_POS)
#define TIMER_TILR_2_UMSK  (~(((1U << TIMER_TILR_2_LEN) - 1) << TIMER_TILR_2_POS))

/* 0x98 : WCR */
#define TIMER_WCR_OFFSET (0x98)
#define TIMER_WCR        TIMER_WCR
#define TIMER_WCR_POS    (0U)
#define TIMER_WCR_LEN    (1U)
#define TIMER_WCR_MSK    (((1U << TIMER_WCR_LEN) - 1) << TIMER_WCR_POS)
#define TIMER_WCR_UMSK   (~(((1U << TIMER_WCR_LEN) - 1) << TIMER_WCR_POS))

/* 0x9C : WFAR */
#define TIMER_WFAR_OFFSET (0x9C)
#define TIMER_WFAR        TIMER_WFAR
#define TIMER_WFAR_POS    (0U)
#define TIMER_WFAR_LEN    (16U)
#define TIMER_WFAR_MSK    (((1U << TIMER_WFAR_LEN) - 1) << TIMER_WFAR_POS)
#define TIMER_WFAR_UMSK   (~(((1U << TIMER_WFAR_LEN) - 1) << TIMER_WFAR_POS))

/* 0xA0 : WSAR */
#define TIMER_WSAR_OFFSET (0xA0)
#define TIMER_WSAR        TIMER_WSAR
#define TIMER_WSAR_POS    (0U)
#define TIMER_WSAR_LEN    (16U)
#define TIMER_WSAR_MSK    (((1U << TIMER_WSAR_LEN) - 1) << TIMER_WSAR_POS)
#define TIMER_WSAR_UMSK   (~(((1U << TIMER_WSAR_LEN) - 1) << TIMER_WSAR_POS))

/* 0xA8 : TCVWR2 */
#define TIMER_TCVWR2_OFFSET (0xA8)
#define TIMER_TCVWR         TIMER_TCVWR
#define TIMER_TCVWR_POS     (0U)
#define TIMER_TCVWR_LEN     (32U)
#define TIMER_TCVWR_MSK     (((1U << TIMER_TCVWR_LEN) - 1) << TIMER_TCVWR_POS)
#define TIMER_TCVWR_UMSK    (~(((1U << TIMER_TCVWR_LEN) - 1) << TIMER_TCVWR_POS))

/* 0xAC : TCVWR3 */
#define TIMER_TCVWR3_OFFSET (0xAC)
#define TIMER_TCVWR         TIMER_TCVWR
#define TIMER_TCVWR_POS     (0U)
#define TIMER_TCVWR_LEN     (32U)
#define TIMER_TCVWR_MSK     (((1U << TIMER_TCVWR_LEN) - 1) << TIMER_TCVWR_POS)
#define TIMER_TCVWR_UMSK    (~(((1U << TIMER_TCVWR_LEN) - 1) << TIMER_TCVWR_POS))

/* 0xB4 : TCVSYN2 */
#define TIMER_TCVSYN2_OFFSET (0xB4)
#define TIMER_TCVSYN2        TIMER_TCVSYN2
#define TIMER_TCVSYN2_POS    (0U)
#define TIMER_TCVSYN2_LEN    (32U)
#define TIMER_TCVSYN2_MSK    (((1U << TIMER_TCVSYN2_LEN) - 1) << TIMER_TCVSYN2_POS)
#define TIMER_TCVSYN2_UMSK   (~(((1U << TIMER_TCVSYN2_LEN) - 1) << TIMER_TCVSYN2_POS))

/* 0xB8 : TCVSYN3 */
#define TIMER_TCVSYN3_OFFSET (0xB8)
#define TIMER_TCVSYN3        TIMER_TCVSYN3
#define TIMER_TCVSYN3_POS    (0U)
#define TIMER_TCVSYN3_LEN    (32U)
#define TIMER_TCVSYN3_MSK    (((1U << TIMER_TCVSYN3_LEN) - 1) << TIMER_TCVSYN3_POS)
#define TIMER_TCVSYN3_UMSK   (~(((1U << TIMER_TCVSYN3_LEN) - 1) << TIMER_TCVSYN3_POS))

/* 0xBC : TCDR */
#define TIMER_TCDR_OFFSET (0xBC)
#define TIMER_TCDR2       TIMER_TCDR2
#define TIMER_TCDR2_POS   (8U)
#define TIMER_TCDR2_LEN   (8U)
#define TIMER_TCDR2_MSK   (((1U << TIMER_TCDR2_LEN) - 1) << TIMER_TCDR2_POS)
#define TIMER_TCDR2_UMSK  (~(((1U << TIMER_TCDR2_LEN) - 1) << TIMER_TCDR2_POS))
#define TIMER_TCDR3       TIMER_TCDR3
#define TIMER_TCDR3_POS   (16U)
#define TIMER_TCDR3_LEN   (8U)
#define TIMER_TCDR3_MSK   (((1U << TIMER_TCDR3_LEN) - 1) << TIMER_TCDR3_POS)
#define TIMER_TCDR3_UMSK  (~(((1U << TIMER_TCDR3_LEN) - 1) << TIMER_TCDR3_POS))
#define TIMER_WCDR        TIMER_WCDR
#define TIMER_WCDR_POS    (24U)
#define TIMER_WCDR_LEN    (8U)
#define TIMER_WCDR_MSK    (((1U << TIMER_WCDR_LEN) - 1) << TIMER_WCDR_POS)
#define TIMER_WCDR_UMSK   (~(((1U << TIMER_WCDR_LEN) - 1) << TIMER_WCDR_POS))

struct timer_reg {
    /* 0x0 : TCCR */
    union {
        struct
        {
            uint32_t reserved_0_1   : 2;  /* [ 1: 0],       rsvd,        0x0 */
            uint32_t cs_1           : 2;  /* [ 3: 2],        r/w,        0x0 */
            uint32_t RESERVED_4     : 1;  /* [    4],       rsvd,        0x0 */
            uint32_t cs_2           : 2;  /* [ 6: 5],        r/w,        0x0 */
            uint32_t RESERVED_7     : 1;  /* [    7],       rsvd,        0x0 */
            uint32_t cs_wdt         : 2;  /* [ 9: 8],        r/w,        0x0 */
            uint32_t reserved_10_31 : 22; /* [31:10],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } TCCR;

    /* 0x4  reserved */
    uint8_t RESERVED0x4[12];

    /* 0x10 : TMR2_0 */
    union {
        struct
        {
            uint32_t tmr : 32; /* [31: 0],        r/w, 0xffffffffL */
        } BF;
        uint32_t WORD;
    } TMR2_0;

    /* 0x14 : TMR2_1 */
    union {
        struct
        {
            uint32_t tmr : 32; /* [31: 0],        r/w, 0xffffffffL */
        } BF;
        uint32_t WORD;
    } TMR2_1;

    /* 0x18 : TMR2_2 */
    union {
        struct
        {
            uint32_t tmr : 32; /* [31: 0],        r/w, 0xffffffffL */
        } BF;
        uint32_t WORD;
    } TMR2_2;

    /* 0x1C : TMR3_0 */
    union {
        struct
        {
            uint32_t tmr : 32; /* [31: 0],        r/w, 0xffffffffL */
        } BF;
        uint32_t WORD;
    } TMR3_0;

    /* 0x20 : TMR3_1 */
    union {
        struct
        {
            uint32_t tmr : 32; /* [31: 0],        r/w, 0xffffffffL */
        } BF;
        uint32_t WORD;
    } TMR3_1;

    /* 0x24 : TMR3_2 */
    union {
        struct
        {
            uint32_t tmr : 32; /* [31: 0],        r/w, 0xffffffffL */
        } BF;
        uint32_t WORD;
    } TMR3_2;

    /* 0x28  reserved */
    uint8_t RESERVED0x28[4];

    /* 0x2C : TCR2 */
    union {
        struct
        {
            uint32_t tcr : 32; /* [31: 0],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } TCR2;

    /* 0x30 : TCR3 */
    union {
        struct
        {
            uint32_t tcr : 32; /* [31: 0],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } TCR3;

    /* 0x34  reserved */
    uint8_t RESERVED0x34[4];

    /* 0x38 : TMSR2 */
    union {
        struct
        {
            uint32_t tmsr_0        : 1;  /* [    0],          r,        0x0 */
            uint32_t tmsr_1        : 1;  /* [    1],          r,        0x0 */
            uint32_t tmsr_2        : 1;  /* [    2],          r,        0x0 */
            uint32_t reserved_3_31 : 29; /* [31: 3],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } TMSR2;

    /* 0x3C : TMSR3 */
    union {
        struct
        {
            uint32_t tmsr_0        : 1;  /* [    0],          r,        0x0 */
            uint32_t tmsr_1        : 1;  /* [    1],          r,        0x0 */
            uint32_t tmsr_2        : 1;  /* [    2],          r,        0x0 */
            uint32_t reserved_3_31 : 29; /* [31: 3],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } TMSR3;

    /* 0x40  reserved */
    uint8_t RESERVED0x40[4];

    /* 0x44 : TIER2 */
    union {
        struct
        {
            uint32_t tier_0        : 1;  /* [    0],        r/w,        0x0 */
            uint32_t tier_1        : 1;  /* [    1],        r/w,        0x0 */
            uint32_t tier_2        : 1;  /* [    2],        r/w,        0x0 */
            uint32_t reserved_3_31 : 29; /* [31: 3],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } TIER2;

    /* 0x48 : TIER3 */
    union {
        struct
        {
            uint32_t tier_0        : 1;  /* [    0],        r/w,        0x0 */
            uint32_t tier_1        : 1;  /* [    1],        r/w,        0x0 */
            uint32_t tier_2        : 1;  /* [    2],        r/w,        0x0 */
            uint32_t reserved_3_31 : 29; /* [31: 3],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } TIER3;

    /* 0x4c  reserved */
    uint8_t RESERVED0x4c[4];

    /* 0x50 : TPLVR2 */
    union {
        struct
        {
            uint32_t tplvr : 32; /* [31: 0],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } TPLVR2;

    /* 0x54 : TPLVR3 */
    union {
        struct
        {
            uint32_t tplvr : 32; /* [31: 0],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } TPLVR3;

    /* 0x58  reserved */
    uint8_t RESERVED0x58[4];

    /* 0x5C : TPLCR2 */
    union {
        struct
        {
            uint32_t tplcr         : 2;  /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_31 : 30; /* [31: 2],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } TPLCR2;

    /* 0x60 : TPLCR3 */
    union {
        struct
        {
            uint32_t tplcr         : 2;  /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_31 : 30; /* [31: 2],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } TPLCR3;

    /* 0x64 : WMER */
    union {
        struct
        {
            uint32_t we            : 1;  /* [    0],        r/w,        0x0 */
            uint32_t wrie          : 1;  /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_31 : 30; /* [31: 2],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } WMER;

    /* 0x68 : WMR */
    union {
        struct
        {
            uint32_t wmr            : 16; /* [15: 0],        r/w,     0xffff */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } WMR;

    /* 0x6C : WVR */
    union {
        struct
        {
            uint32_t wvr            : 16; /* [15: 0],          r,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } WVR;

    /* 0x70 : WSR */
    union {
        struct
        {
            uint32_t wts           : 1;  /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_31 : 31; /* [31: 1],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } WSR;

    /* 0x74  reserved */
    uint8_t RESERVED0x74[4];

    /* 0x78 : TICR2 */
    union {
        struct
        {
            uint32_t tclr_0        : 1;  /* [    0],          w,        0x0 */
            uint32_t tclr_1        : 1;  /* [    1],          w,        0x0 */
            uint32_t tclr_2        : 1;  /* [    2],          w,        0x0 */
            uint32_t reserved_3_31 : 29; /* [31: 3],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } TICR2;

    /* 0x7C : TICR3 */
    union {
        struct
        {
            uint32_t tclr_0        : 1;  /* [    0],          w,        0x0 */
            uint32_t tclr_1        : 1;  /* [    1],          w,        0x0 */
            uint32_t tclr_2        : 1;  /* [    2],          w,        0x0 */
            uint32_t reserved_3_31 : 29; /* [31: 3],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } TICR3;

    /* 0x80 : WICR */
    union {
        struct
        {
            uint32_t wiclr         : 1;  /* [    0],          w,        0x0 */
            uint32_t reserved_1_31 : 31; /* [31: 1],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } WICR;

    /* 0x84 : TCER */
    union {
        struct
        {
            uint32_t reserved_0    : 1;  /* [    0],       rsvd,        0x0 */
            uint32_t timer2_en     : 1;  /* [    1],        r/w,        0x0 */
            uint32_t timer3_en     : 1;  /* [    2],        r/w,        0x0 */
            uint32_t reserved_3_31 : 29; /* [31: 3],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } TCER;

    /* 0x88 : TCMR */
    union {
        struct
        {
            uint32_t reserved_0    : 1;  /* [    0],       rsvd,        0x0 */
            uint32_t timer2_mode   : 1;  /* [    1],        r/w,        0x0 */
            uint32_t timer3_mode   : 1;  /* [    2],        r/w,        0x0 */
            uint32_t reserved_3_31 : 29; /* [31: 3],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } TCMR;

    /* 0x8c  reserved */
    uint8_t RESERVED0x8c[4];

    /* 0x90 : TILR2 */
    union {
        struct
        {
            uint32_t tilr_0        : 1;  /* [    0],        r/w,        0x0 */
            uint32_t tilr_1        : 1;  /* [    1],        r/w,        0x0 */
            uint32_t tilr_2        : 1;  /* [    2],        r/w,        0x0 */
            uint32_t reserved_3_31 : 29; /* [31: 3],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } TILR2;

    /* 0x94 : TILR3 */
    union {
        struct
        {
            uint32_t tilr_0        : 1;  /* [    0],        r/w,        0x0 */
            uint32_t tilr_1        : 1;  /* [    1],        r/w,        0x0 */
            uint32_t tilr_2        : 1;  /* [    2],        r/w,        0x0 */
            uint32_t reserved_3_31 : 29; /* [31: 3],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } TILR3;

    /* 0x98 : WCR */
    union {
        struct
        {
            uint32_t wcr           : 1;  /* [    0],          w,        0x0 */
            uint32_t reserved_1_31 : 31; /* [31: 1],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } WCR;

    /* 0x9C : WFAR */
    union {
        struct
        {
            uint32_t wfar           : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } WFAR;

    /* 0xA0 : WSAR */
    union {
        struct
        {
            uint32_t wsar           : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31 : 16; /* [31:16],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } WSAR;

    /* 0xa4  reserved */
    uint8_t RESERVED0xa4[4];

    /* 0xA8 : TCVWR2 */
    union {
        struct
        {
            uint32_t tcvwr : 32; /* [31: 0],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } TCVWR2;

    /* 0xAC : TCVWR3 */
    union {
        struct
        {
            uint32_t tcvwr : 32; /* [31: 0],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } TCVWR3;

    /* 0xb0  reserved */
    uint8_t RESERVED0xb0[4];

    /* 0xB4 : TCVSYN2 */
    union {
        struct
        {
            uint32_t tcvsyn2 : 32; /* [31: 0],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } TCVSYN2;

    /* 0xB8 : TCVSYN3 */
    union {
        struct
        {
            uint32_t tcvsyn3 : 32; /* [31: 0],          r,        0x0 */
        } BF;
        uint32_t WORD;
    } TCVSYN3;

    /* 0xBC : TCDR */
    union {
        struct
        {
            uint32_t reserved_0_7 : 8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t tcdr2        : 8; /* [15: 8],        r/w,        0x0 */
            uint32_t tcdr3        : 8; /* [23:16],        r/w,        0x0 */
            uint32_t wcdr         : 8; /* [31:24],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } TCDR;
};

typedef volatile struct timer_reg timer_reg_t;

#endif /* __TIMER_REG_H__ */
