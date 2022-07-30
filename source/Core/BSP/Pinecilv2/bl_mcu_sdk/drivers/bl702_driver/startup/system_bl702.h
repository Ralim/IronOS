/**
 * @file system_bl702.h
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

#ifndef __SYSTEM_BL702_H__
#define __SYSTEM_BL702_H__

/**
 *  @brief PLL Clock type definition
 */

extern uint32_t SystemCoreClock;
typedef void (*pFunc)(void);

#define CPU_Interrupt_Enable        clic_enable_interrupt
#define CPU_Interrupt_Disable       clic_disable_interrupt
#define CPU_Interrupt_Pending_Clear clic_clear_pending

extern void SystemCoreClockUpdate(void);
extern void SystemInit(void);
void clic_enable_interrupt(uint32_t source);
void clic_disable_interrupt(uint32_t source);
void clic_clear_pending(uint32_t source);
void Interrupt_Handler_Register(IRQn_Type irq, pFunc interruptFun);
void System_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority);
#endif
