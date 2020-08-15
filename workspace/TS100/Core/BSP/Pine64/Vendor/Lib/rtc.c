/*!
    \file  rtc.c
    \brief RTC check and config,time_show and time_adjust function

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

#include "rtc.h"

/* enter the second interruption,set the second interrupt flag to 1 */
__IO uint32_t timedisplay;

/*!
    \brief      configure the RTC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_configuration(void)
{
    /* enable PMU and BKPI clocks */
    rcu_periph_clock_enable(RCU_BKPI);
    rcu_periph_clock_enable(RCU_PMU);
    /* allow access to BKP domain */
    pmu_backup_write_enable();

    /* reset backup domain */
    bkp_deinit();

    /* enable LXTAL */
    rcu_osci_on(RCU_LXTAL);
    /* wait till LXTAL is ready */
    rcu_osci_stab_wait(RCU_LXTAL);
    
    /* select RCU_LXTAL as RTC clock source */
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

    /* enable RTC Clock */
    rcu_periph_clock_enable(RCU_RTC);

    /* wait for RTC registers synchronization */
    rtc_register_sync_wait();

    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();

    /* enable the RTC second interrupt*/
    rtc_interrupt_enable(RTC_INT_SECOND);

    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();

    /* set RTC prescaler: set RTC period to 1s */
    rtc_prescaler_set(32767);

    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();
}

/*!
    \brief      return the time entered by user, using Hyperterminal
    \param[in]  none
    \param[out] none
    \retval     current time of RTC counter value
*/
uint32_t time_regulate(void)
{
    uint32_t tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;

    printf("\r\n==============Time Settings=====================================");
    printf("\r\n  Please Set Hours");

    while (tmp_hh == 0xFF){
        tmp_hh = usart_scanf(23);
    }
    printf(":  %d", tmp_hh);
    printf("\r\n  Please Set Minutes");
    while (tmp_mm == 0xFF){
        tmp_mm = usart_scanf(59);
    }
    printf(":  %d", tmp_mm);
    printf("\r\n  Please Set Seconds");
    while (tmp_ss == 0xFF){
        tmp_ss = usart_scanf(59);
    }
    printf(":  %d", tmp_ss);

    /* return the value  store in RTC counter register */
    return((tmp_hh*3600 + tmp_mm*60 + tmp_ss));
}

/*!
    \brief      adjust time 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void time_adjust(void)
{
    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();
    /* change the current time */
    rtc_counter_set(time_regulate());
    /* wait until last write operation on RTC registers has finished */
    rtc_lwoff_wait();
}

/*!
    \brief      display the current time 
    \param[in]  timeVar: RTC counter value
    \param[out] none
    \retval     none
*/
void time_display(uint32_t timevar)
{
    uint32_t thh = 0, tmm = 0, tss = 0;

    /* compute  hours */
    thh = timevar / 3600;
    /* compute minutes */
    tmm = (timevar % 3600) / 60;
    /* compute seconds */
    tss = (timevar % 3600) % 60;

    printf(" Time: %0.2d:%0.2d:%0.2d\r\n", thh, tmm, tss);
}

/*!
    \brief      show the current time (HH:MM:SS) on the Hyperterminal
    \param[in]  none
    \param[out] none
    \retval     none
*/
void time_show(void)
{
    printf("\n\r");

    /* infinite loop */
    while (1){
        /* if 1s has paased */
        if (timedisplay == 1){
            /* display current time */
            time_display(rtc_counter_get());
            timedisplay = 0;
        }
    }
}

/*!
    \brief      get numeric values from the hyperterminal
    \param[in]  value: input value from the hyperterminal
    \param[out] none
    \retval     input value in BCD mode
*/
uint8_t usart_scanf(uint32_t value)
{
    uint32_t index = 0;
    uint32_t tmp[2] = {0, 0};

    while (index < 2){
        /* loop until RBNE = 1 */
        while (usart_flag_get(USART0, USART_FLAG_RBNE) == RESET);
        tmp[index++] = (usart_data_receive(USART0));

        if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39)){
            printf("\n\rPlease enter valid number between 0 and 9\n");
            index--;
        }
    }
    /* calculate the Corresponding value */
    index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
    /* check */
    if (index > value){
        printf("\n\rPlease enter valid number between 0 and %d\n", value);
        return 0xFF;
    }
    return index;
}
