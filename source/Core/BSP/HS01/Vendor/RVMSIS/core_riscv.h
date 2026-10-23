/********************************** (C) COPYRIGHT  *******************************
 * File Name          : core_riscv.h
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2021/10/28
 * Description        : CH573 RISC-V Core Peripheral Access Layer Header File
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/
#ifndef __CORE_RV3A_H__
#define __CORE_RV3A_H__

#ifdef __cplusplus
extern "C" {
#endif

/* IO definitions */
#ifdef __cplusplus
#define __I volatile /*!< defines 'read only' permissions      */
#else
#define __I volatile const /*!< defines 'read only' permissions     */
#endif
#define __O              volatile /*!< defines 'write only' permissions     */
#define __IO             volatile /*!< defines 'read / write' permissions   */
#define RV_STATIC_INLINE static inline

// typedef enum {SUCCESS = 0, ERROR = !SUCCESS} ErrorStatus;

typedef enum { DISABLE = 0, ENABLE = !DISABLE } FunctionalState;
typedef enum { RESET = 0, SET = !RESET } FlagStatus, ITStatus;

/* memory mapped structure for Program Fast Interrupt Controller (PFIC) */
typedef struct __attribute__((packed)) {
  __I uint32_t  ISR[8];
  __I uint32_t  IPR[8];
  __IO uint32_t ITHRESDR;
  __IO uint32_t FIBADDRR;
  __IO uint32_t CFGR;
  __I uint32_t  GISR;
  uint8_t       RESERVED0[0x10];
  __IO uint32_t FIOFADDRR[4];
  uint8_t       RESERVED1[0x90];
  __O uint32_t  IENR[8];
  uint8_t       RESERVED2[0x60];
  __O uint32_t  IRER[8];
  uint8_t       RESERVED3[0x60];
  __O uint32_t  IPSR[8];
  uint8_t       RESERVED4[0x60];
  __O uint32_t  IPRR[8];
  uint8_t       RESERVED5[0x60];
  __IO uint32_t IACTR[8];
  uint8_t       RESERVED6[0xE0];
  __IO uint8_t  IPRIOR[256];
  uint8_t       RESERVED7[0x810];
  __IO uint32_t SCTLR;
} PFIC_Type;

/* memory mapped structure for SysTick */
typedef struct __attribute__((packed)) {
  __IO uint32_t CTLR;
  __IO uint64_t CNT;
  __IO uint64_t CMP;
  __IO uint32_t CNTFG;
} SysTick_Type;

#define PFIC    ((PFIC_Type *)0xE000E000)
#define SysTick ((SysTick_Type *)0xE000F000)

#define PFIC_KEY1 ((uint32_t)0xFA050000)
#define PFIC_KEY2 ((uint32_t)0xBCAF0000)
#define PFIC_KEY3 ((uint32_t)0xBEEF0000)

/* ##########################   define  #################################### */
#define __nop() asm volatile("nop")

/* ##########################   PFIC functions  #################################### */

/*******************************************************************************
 * @fn      PFIC_EnableIRQ
 *
 * @brief   Enable Interrupt
 *
 * @param   IRQn    - Interrupt Numbers
 */
RV_STATIC_INLINE void PFIC_EnableIRQ(IRQn_Type IRQn) { PFIC->IENR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F)); }

/*******************************************************************************
 * @fn      PFIC_DisableIRQ
 *
 * @brief   Disable Interrupt
 *
 * @param   IRQn    - Interrupt Numbers
 */
RV_STATIC_INLINE void PFIC_DisableIRQ(IRQn_Type IRQn) {
  uint32_t tem;
  tem                                 = PFIC->ITHRESDR;
  PFIC->ITHRESDR                      = 0x10;
  PFIC->IRER[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
  PFIC->ITHRESDR                      = tem;
  __nop();
  __nop();
}

/*******************************************************************************
 * @fn      PFIC_GetStatusIRQ
 *
 * @brief   Get Interrupt Enable State
 *
 * @param   IRQn    - Interrupt Numbers
 *
 * @return  1: Interrupt Enable
 *          0: Interrupt Disable
 */
RV_STATIC_INLINE uint32_t PFIC_GetStatusIRQ(IRQn_Type IRQn) { return ((uint32_t)((PFIC->ISR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F))) ? 1 : 0)); }

/*******************************************************************************
 * @fn      PFIC_GetPendingIRQ
 *
 * @brief   Get Interrupt Pending State
 *
 * @param   IRQn    - Interrupt Numbers
 *
 * @return  1: Interrupt Pending Enable
 *          0: Interrupt Pending Disable
 */
RV_STATIC_INLINE uint32_t PFIC_GetPendingIRQ(IRQn_Type IRQn) { return ((uint32_t)((PFIC->IPR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F))) ? 1 : 0)); }

/*******************************************************************************
 * @fn      PFIC_SetPendingIRQ
 *
 * @brief   Set Interrupt Pending
 *
 * @param   IRQn    - Interrupt Numbers
 */
RV_STATIC_INLINE void PFIC_SetPendingIRQ(IRQn_Type IRQn) { PFIC->IPSR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F)); }

/*******************************************************************************
 * @fn      PFIC_ClearPendingIRQ
 *
 * @brief   Clear Interrupt Pending
 *
 * @param   IRQn    - Interrupt Numbers
 */
RV_STATIC_INLINE void PFIC_ClearPendingIRQ(IRQn_Type IRQn) { PFIC->IPRR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F)); }

/*******************************************************************************
 * @fn      PFIC_GetActive
 *
 * @brief   Get Interrupt Active State
 *
 * @param   IRQn    - Interrupt Numbers
 *
 * @return  1: Interrupt Active
 *          0: Interrupt No Active.
 */
RV_STATIC_INLINE uint32_t PFIC_GetActive(IRQn_Type IRQn) { return ((uint32_t)((PFIC->IACTR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F))) ? 1 : 0)); }

/*******************************************************************************
 * @fn      PFIC_SetPriority
 *
 * @brief   Set Interrupt Priority
 *
 * @param   IRQn        - Interrupt Numbers
 * @param   priority    - bit7:         pre-emption priority
 *                        bit6-bit4:    subpriority
 */
RV_STATIC_INLINE void PFIC_SetPriority(IRQn_Type IRQn, uint8_t priority) { PFIC->IPRIOR[(uint32_t)(IRQn)] = priority; }

/*******************************************************************************
 * @fn      PFIC_FastINT0CFG
 *
 * @brief   Set fast Interrupt 0
 *
 * @param   IRQn    - Interrupt Numbers
 * @param   addr    - interrupt service addr
 */
RV_STATIC_INLINE void PFIC_FastINT0CFG(IRQn_Type IRQn, uint32_t addr) {
  PFIC->FIBADDRR     = addr & 0xF0000000;
  PFIC->FIOFADDRR[0] = (IRQn << 24) | (addr & 0x000FFFFF);
}

/*******************************************************************************
 * @fn      PFIC_FastINT1CFG
 *
 * @brief   Set fast Interrupt 1
 *
 * @param   IRQn    - Interrupt Numbers
 * @param   addr    - interrupt service addr
 */
RV_STATIC_INLINE void PFIC_FastINT1CFG(IRQn_Type IRQn, uint32_t addr) {
  PFIC->FIBADDRR     = addr & 0xF0000000;
  PFIC->FIOFADDRR[1] = (IRQn << 24) | (addr & 0x000FFFFF);
}

/*******************************************************************************
 * @fn      PFIC_FastINT2CFG
 *
 * @brief   Set fast Interrupt 2, occupied if use BLE.
 *
 * @param   IRQn    - Interrupt Numbers
 * @param   addr    - interrupt service addr
 */
RV_STATIC_INLINE void PFIC_FastINT2CFG(IRQn_Type IRQn, uint32_t addr) {
  PFIC->FIBADDRR     = addr & 0xF0000000;
  PFIC->FIOFADDRR[2] = (IRQn << 24) | (addr & 0x000FFFFF);
}

/*******************************************************************************
 * @fn      PFIC_FastINT3CFG
 *
 * @brief   Set fast Interrupt 3, occupied if use BLE.
 *
 * @param   IRQn    - Interrupt Numbers
 * @param   addr    - interrupt service addr
 */
RV_STATIC_INLINE void PFIC_FastINT3CFG(IRQn_Type IRQn, uint32_t addr) {
  PFIC->FIBADDRR     = addr & 0xF0000000;
  PFIC->FIOFADDRR[3] = (IRQn << 24) | (addr & 0x000FFFFF);
}

/*******************************************************************************
 * @fn      __SEV
 *
 * @brief   Wait for Events
 */
__attribute__((always_inline)) RV_STATIC_INLINE void __SEV(void) { PFIC->SCTLR |= (1 << 3); }

/*******************************************************************************
 * @fn      __WFI
 *
 * @brief   Wait for Interrupt
 */
__attribute__((always_inline)) RV_STATIC_INLINE void __WFI(void) {
  PFIC->SCTLR &= ~(1 << 3); // wfi
  asm volatile("wfi");
}

/*******************************************************************************
 * @fn      __WFE
 *
 * @brief   Wait for Events
 */
__attribute__((always_inline)) RV_STATIC_INLINE void __WFE(void) {
  PFIC->SCTLR |= (1 << 3) | (1 << 5); // (wfi->wfe)+(__sev)
  asm volatile("wfi");
  PFIC->SCTLR |= (1 << 3);
  asm volatile("wfi");
}

/*******************************************************************************
 * @fn      PFIC_SetFastIRQ
 *
 * @brief   Set Fast Interrupt
 *
 * @param   addr    - Fast interrupt service function base address
 * @param   IRQn    - Interrupt Numbers
 * @param   num     - Fast Interrupt Numbers
 */
RV_STATIC_INLINE void PFIC_SetFastIRQ(uint32_t addr, IRQn_Type IRQn, uint8_t num) {
  if (num > 3)
    return;
  PFIC->FIBADDRR       = addr;
  PFIC->FIOFADDRR[num] = ((uint32_t)IRQn << 24) | (addr & 0xfffff);
}

/*******************************************************************************
 * @fn      PFIC_SystemReset
 *
 * @brief   Initiate a system reset request
 */
RV_STATIC_INLINE void PFIC_SystemReset(void) { PFIC->CFGR = PFIC_KEY3 | (1 << 7); }

/*******************************************************************************
 * @fn      PFIC_HaltPushCfg
 *
 * @brief   Enable Hardware Stack
 *
 * @param   NewState    - DISABLE or ENABLE
 */
RV_STATIC_INLINE void PFIC_HaltPushCfg(FunctionalState NewState) {
  if (NewState != DISABLE) {
    PFIC->CFGR = PFIC_KEY1;
  } else {
    PFIC->CFGR = PFIC_KEY1 | (1 << 0);
  }
}

/*******************************************************************************
 * @fn      PFIC_INTNestCfg
 *
 * @brief   Enable Interrupt Nesting
 *
 * @param   NewState    - DISABLE or ENABLE
 */
RV_STATIC_INLINE void PFIC_INTNestCfg(FunctionalState NewState) {
  if (NewState != DISABLE) {
    PFIC->CFGR = PFIC_KEY1;
  } else {
    PFIC->CFGR = PFIC_KEY1 | (1 << 1);
  }
}

#define SysTick_LOAD_RELOAD_Msk    (0xFFFFFFFFFFFFFFFF)
#define SysTick_CTRL_RELOAD_Msk    (1 << 8)
#define SysTick_CTRL_CLKSOURCE_Msk (1 << 2)
#define SysTick_CTRL_TICKINT_Msk   (1 << 1)
#define SysTick_CTRL_ENABLE_Msk    (1 << 0)

/*******************************************************************************
 * @fn      SysTick_Config
 *
 * @brief   Set reload register, enable SysTick IRQ and SysTick Timer
 *
 * @param   ticks   - reload value.
 *
 * @return  0 - success, 1 - error.
 */
RV_STATIC_INLINE uint32_t SysTick_Config(uint64_t ticks) {
  if ((ticks - 1) > SysTick_LOAD_RELOAD_Msk)
    return (1); /* Reload value impossible */

  SysTick->CMP = ticks - 1; /* set reload register */
  PFIC_EnableIRQ(SysTick_IRQn);
  SysTick->CTLR = SysTick_CTRL_RELOAD_Msk | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk; /* Enable SysTick IRQ and SysTick Timer */
  return (0);                                                                                                                /* Function successful */
}

#ifdef __cplusplus
}
#endif

#endif /* __CORE_RV3A_H__ */
