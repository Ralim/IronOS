/*!
    \file    gd32vf103_rtc.h
    \brief   definitions for the RTC

    \version 2019-06-05, V1.0.0, firmware for GD32VF103
    \version 2020-08-04, V1.1.0, firmware for GD32VF103
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#ifndef GD32VF103_RTC_H
#define GD32VF103_RTC_H

#include "gd32vf103.h"

/* RTC definitions */
#define RTC RTC_BASE

/* registers definitions */
#define RTC_INTEN REG32(RTC + 0x00U) /*!< interrupt enable register */
#define RTC_CTL   REG32(RTC + 0x04U) /*!< control register */
#define RTC_PSCH  REG32(RTC + 0x08U) /*!< prescaler high register */
#define RTC_PSCL  REG32(RTC + 0x0CU) /*!< prescaler low register */
#define RTC_DIVH  REG32(RTC + 0x10U) /*!< divider high register */
#define RTC_DIVL  REG32(RTC + 0x14U) /*!< divider low register */
#define RTC_CNTH  REG32(RTC + 0x18U) /*!< counter high register */
#define RTC_CNTL  REG32(RTC + 0x1CU) /*!< counter low register */
#define RTC_ALRMH REG32(RTC + 0x20U) /*!< alarm high register */
#define RTC_ALRML REG32(RTC + 0x24U) /*!< alarm low register */

/* bits definitions */
/* RTC_INTEN */
#define RTC_INTEN_SCIE   BIT(0) /*!< second interrupt enable */
#define RTC_INTEN_ALRMIE BIT(1) /*!< alarm interrupt enable */
#define RTC_INTEN_OVIE   BIT(2) /*!< overflow interrupt enable */

/* RTC_CTL */
#define RTC_CTL_SCIF   BIT(0) /*!< second interrupt flag */
#define RTC_CTL_ALRMIF BIT(1) /*!< alarm interrupt flag */
#define RTC_CTL_OVIF   BIT(2) /*!< overflow interrupt flag */
#define RTC_CTL_RSYNF  BIT(3) /*!< registers synchronized flag */
#define RTC_CTL_CMF    BIT(4) /*!< configuration mode flag */
#define RTC_CTL_LWOFF  BIT(5) /*!< last write operation finished flag */

/* RTC_PSCH */
#define RTC_PSCH_PSC BITS(0, 3) /*!< prescaler high value */

/* RTC_PSCL */
#define RTC_PSCL_PSC BITS(0, 15) /*!< prescaler low value */

/* RTC_DIVH */
#define RTC_DIVH_DIV BITS(0, 3) /*!< divider high value */

/* RTC_DIVL */
#define RTC_DIVL_DIV BITS(0, 15) /*!< divider low value */

/* RTC_CNTH */
#define RTC_CNTH_CNT BITS(0, 15) /*!< counter high value */

/* RTC_CNTL */
#define RTC_CNTL_CNT BITS(0, 15) /*!< counter low value */

/* RTC_ALRMH */
#define RTC_ALRMH_ALRM BITS(0, 15) /*!< alarm high value */

/* RTC_ALRML */
#define RTC_ALRML_ALRM BITS(0, 15) /*!< alarm low value */

/* constants definitions */
/* RTC interrupt enable or disable definitions */
#define RTC_INT_SECOND   RTC_INTEN_SCIE   /*!< second interrupt enable */
#define RTC_INT_ALARM    RTC_INTEN_ALRMIE /*!< alarm interrupt enable */
#define RTC_INT_OVERFLOW RTC_INTEN_OVIE   /*!< overflow interrupt enable */

/* RTC interrupt flag definitions */
#define RTC_INT_FLAG_SECOND   RTC_CTL_SCIF   /*!< second interrupt flag */
#define RTC_INT_FLAG_ALARM    RTC_CTL_ALRMIF /*!< alarm interrupt flag */
#define RTC_INT_FLAG_OVERFLOW RTC_CTL_OVIF   /*!< overflow interrupt flag */

/* RTC flag definitions */
#define RTC_FLAG_SECOND   RTC_CTL_SCIF   /*!< second interrupt flag */
#define RTC_FLAG_ALARM    RTC_CTL_ALRMIF /*!< alarm interrupt flag */
#define RTC_FLAG_OVERFLOW RTC_CTL_OVIF   /*!< overflow interrupt flag */
#define RTC_FLAG_RSYN     RTC_CTL_RSYNF  /*!< registers synchronized flag */
#define RTC_FLAG_LWOF     RTC_CTL_LWOFF  /*!< last write operation finished flag */

/* function declarations */
/* initialization functions */
/* enter RTC configuration mode */
void rtc_configuration_mode_enter(void);
/* exit RTC configuration mode */
void rtc_configuration_mode_exit(void);
/* set RTC counter value */
void rtc_counter_set(uint32_t cnt);
/* set RTC prescaler value */
void rtc_prescaler_set(uint32_t psc);

/* operation functions */
/* wait RTC last write operation finished flag set */
void rtc_lwoff_wait(void);
/* wait RTC registers synchronized flag set */
void rtc_register_sync_wait(void);
/* set RTC alarm value */
void rtc_alarm_config(uint32_t alarm);
/* get RTC counter value */
uint32_t rtc_counter_get(void);
/* get RTC divider value */
uint32_t rtc_divider_get(void);

/* flag & interrupt functions */
/* get RTC flag status */
FlagStatus rtc_flag_get(uint32_t flag);
/* clear RTC flag status */
void rtc_flag_clear(uint32_t flag);
/* get RTC interrupt flag status */
FlagStatus rtc_interrupt_flag_get(uint32_t flag);
/* clear RTC interrupt flag status */
void rtc_interrupt_flag_clear(uint32_t flag);
/* enable RTC interrupt */
void rtc_interrupt_enable(uint32_t interrupt);
/* disable RTC interrupt */
void rtc_interrupt_disable(uint32_t interrupt);

#endif /* GD32VF103_RTC_H */
