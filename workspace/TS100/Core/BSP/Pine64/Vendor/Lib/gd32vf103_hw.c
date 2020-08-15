/*!
    \file  gd32vf103_hw.c
    \brief USB hardware configuration for GD32VF103

    \version 2019-06-05, V1.0.0, firmware for GD32VF103
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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

#include "gd32vf103_timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TIM_MSEC_DELAY 0x01
#define TIM_USEC_DELAY 0x02

__IO uint32_t delay_time = 0;
__IO uint32_t timer_prescaler;
__IO uint32_t usbfs_prescaler = 0;

static void hw_time_set(uint8_t unit);
static void hw_delay(uint32_t ntime, uint8_t unit);

/*!
    \brief      configure USB clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_rcu_config(void) {
  uint32_t system_clock = rcu_clock_freq_get(CK_SYS);

  if (system_clock == 48000000) {
    usbfs_prescaler = RCU_CKUSB_CKPLL_DIV1;
    timer_prescaler = 3;
  } else if (system_clock == 72000000) {
    usbfs_prescaler = RCU_CKUSB_CKPLL_DIV1_5;
    timer_prescaler = 5;
  } else if (system_clock == 96000000) {
    usbfs_prescaler = RCU_CKUSB_CKPLL_DIV2;
    timer_prescaler = 7;
  } else {
    /*  reserved  */
  }

  rcu_usb_clock_config(usbfs_prescaler);
  rcu_periph_clock_enable(RCU_USBFS);
}

/*!
    \brief      configure USB interrupt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_intr_config(void) {
  eclic_irq_enable((uint8_t)USBFS_IRQn, 1, 0);

  /* enable the power module clock */
  rcu_periph_clock_enable(RCU_PMU);

  /* USB wakeup EXTI line configuration */
  exti_interrupt_flag_clear(EXTI_18);
  exti_init(EXTI_18, EXTI_INTERRUPT, EXTI_TRIG_RISING);
  exti_interrupt_enable(EXTI_18);

  eclic_irq_enable((uint8_t)USBFS_WKUP_IRQn, 3, 0);
}

/*!
    \brief      initializes delay unit using Timer2
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_timer_init(void) {
  rcu_periph_clock_enable(RCU_TIMER2);

  eclic_irq_enable(TIMER2_IRQn, 2, 0);
}

/*!
    \brief      delay in micro seconds
    \param[in]  usec: value of delay required in micro seconds
    \param[out] none
    \retval     none
*/
void usb_udelay(const uint32_t usec) {
  hw_delay(usec, TIM_USEC_DELAY);
}

/*!
    \brief      delay in milli seconds
    \param[in]  msec: value of delay required in milli seconds
    \param[out] none
    \retval     none
*/
void usb_mdelay(const uint32_t msec) {
  hw_delay(msec, TIM_MSEC_DELAY);
}

/*!
    \brief      time base IRQ
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usb_timer_irq(void) {
  if (RESET != timer_flag_get(TIMER2, TIMER_FLAG_UP)) {
    timer_flag_clear(TIMER2, TIMER_FLAG_UP);

    if (delay_time > 0x00U) {
      delay_time--;
    } else {
      timer_disable(TIMER2);
    }
  }
}

/*!
    \brief      delay routine based on TIM0
    \param[in]  nTime: delay Time 
    \param[in]  unit: delay Time unit = mili sec / micro sec
    \param[out] none
    \retval     none
*/
static void hw_delay(uint32_t ntime, uint8_t unit) {
  delay_time = ntime;

  hw_time_set(unit);

  while (0U != delay_time) {
  }

  timer_disable(TIMER2);
}

/*!
    \brief      configures TIM0 for delay routine based on TIM0
    \param[in]  unit: msec /usec
    \param[out] none
    \retval     none
*/
static void hw_time_set(uint8_t unit) {
  timer_parameter_struct timer_initpara;

  rcu_periph_clock_enable(RCU_TIMER2);
  timer_deinit(TIMER2);

  if (TIM_USEC_DELAY == unit) {
    timer_initpara.period = 11;
  } else if (TIM_MSEC_DELAY == unit) {
    timer_initpara.period = 11999;
  }

  timer_initpara.prescaler = timer_prescaler;
  timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
  timer_initpara.counterdirection = TIMER_COUNTER_UP;
  timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
  timer_initpara.repetitioncounter = 0;
  timer_init(TIMER2, &timer_initpara);

  timer_update_event_enable(TIMER2);
  timer_interrupt_enable(TIMER2, TIMER_INT_UP);
  timer_flag_clear(TIMER2, TIMER_FLAG_UP);
  timer_update_source_config(TIMER2, TIMER_UPDATE_SRC_GLOBAL);

  /* TIMER2 counter enable */
  timer_enable(TIMER2);
}
