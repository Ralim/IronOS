/**
 * @file hal_mtimer.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include "hal_mtimer.h"
#include "bl702_glb.h"
#include "risc-v/Core/Include/clic.h"

static void (*systick_callback)(void);
static uint64_t next_compare_tick = 0;
static uint64_t current_set_ticks = 0;

static void Systick_Handler(void)
{
    *(volatile uint64_t *)(CLIC_CTRL_ADDR + CLIC_MTIMECMP) = next_compare_tick;
    systick_callback();
    next_compare_tick += current_set_ticks;
}

/**
 * @brief
 *
 * @param time
 * @param interruptFun
 */
void mtimer_set_alarm_time(uint64_t ticks, void (*interruptfun)(void))
{
    CPU_Interrupt_Disable(MTIME_IRQn);

    uint32_t ulCurrentTimeHigh, ulCurrentTimeLow;
    volatile uint32_t *const pulTimeHigh = (volatile uint32_t *const)(CLIC_CTRL_ADDR + CLIC_MTIME + 4);
    volatile uint32_t *const pulTimeLow = (volatile uint32_t *const)(CLIC_CTRL_ADDR + CLIC_MTIME);
    volatile uint32_t ulHartId = 0;

    current_set_ticks = ticks;
    systick_callback = interruptfun;

    __asm volatile("csrr %0, mhartid"
                   : "=r"(ulHartId));

    do {
        ulCurrentTimeHigh = *pulTimeHigh;
        ulCurrentTimeLow = *pulTimeLow;
    } while (ulCurrentTimeHigh != *pulTimeHigh);

    next_compare_tick = (uint64_t)ulCurrentTimeHigh;
    next_compare_tick <<= 32ULL;
    next_compare_tick |= (uint64_t)ulCurrentTimeLow;
    next_compare_tick += (uint64_t)current_set_ticks;

    *(volatile uint64_t *)(CLIC_CTRL_ADDR + CLIC_MTIMECMP) = next_compare_tick;

    /* Prepare the time to use after the next tick interrupt. */
    next_compare_tick += (uint64_t)current_set_ticks;

    Interrupt_Handler_Register(MTIME_IRQn, Systick_Handler);
    CPU_Interrupt_Enable(MTIME_IRQn);
}

/**
 * @brief
 *
 * @return uint64_t
 */
uint64_t mtimer_get_time_ms()
{
    return mtimer_get_time_us() / 1000;
}
/**
 * @brief
 *
 * @return uint64_t
 */
uint64_t mtimer_get_time_us()
{
    uint32_t tmpValLow, tmpValHigh, tmpValHigh1;

    do {
        tmpValLow = *(volatile uint32_t *)(CLIC_CTRL_ADDR + CLIC_MTIME);
        tmpValHigh = *(volatile uint32_t *)(CLIC_CTRL_ADDR + CLIC_MTIME + 4);
        tmpValHigh1 = *(volatile uint32_t *)(CLIC_CTRL_ADDR + CLIC_MTIME + 4);
    } while (tmpValHigh != tmpValHigh1);

    return (((uint64_t)tmpValHigh << 32) + tmpValLow);
}
/**
 * @brief
 *
 * @param time
 */
void mtimer_delay_ms(uint32_t time)
{
    uint64_t cnt = 0;
    uint32_t clock = SystemCoreClockGet();
    uint64_t startTime = mtimer_get_time_ms();

    while (mtimer_get_time_ms() - startTime < time) {
        cnt++;

        /* assume BFLB_BSP_Get_Time_Ms take 32 cycles*/
        if (cnt > (time * (clock >> (10 + 5))) * 2) {
            break;
        }
    }
}
/**
 * @brief
 *
 * @param time
 */
void mtimer_delay_us(uint32_t time)
{
    uint64_t cnt = 0;
    uint32_t clock = SystemCoreClockGet();
    uint64_t startTime = mtimer_get_time_us();

    while (mtimer_get_time_us() - startTime < time) {
        cnt++;

        /* assume BFLB_BSP_Get_Time_Ms take 32 cycles*/
        if (cnt > (time * (clock >> (10 + 5))) * 2) {
            break;
        }
    }
}
