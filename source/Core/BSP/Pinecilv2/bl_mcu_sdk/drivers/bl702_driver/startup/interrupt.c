/**
 * @file interrupt.c
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
#include "bl702_common.h"
#include "bflb_platform.h"
#include "risc-v/Core/Include/clic.h"
#include "risc-v/Core/Include/riscv_encoding.h"

pFunc __Interrupt_Handlers[IRQn_LAST] = { 0 };

void Interrupt_Handler_Stub(void);

void clic_msip_handler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void clic_mtimer_handler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void clic_mext_handler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void clic_csoft_handler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void BMX_ERR_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void BMX_TO_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void L1C_BMX_ERR_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void L1C_BMX_TO_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void SEC_BMX_ERR_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void RF_TOP_INT0_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void RF_TOP_INT1_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void DMA_BMX_ERR_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void SEC_GMAC_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void SEC_CDET_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void SEC_PKA_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void SEC_TRNG_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void SEC_AES_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void SEC_SHA_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void DMA_ALL_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void MJPEG_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void CAM_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void I2S_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void IRTX_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void IRRX_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void USB_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void EMAC_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void SF_CTRL_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void GPADC_DMA_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void EFUSE_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void SPI_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void UART0_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void UART1_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void I2C_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void PWM_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void TIMER_CH0_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void TIMER_CH1_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void TIMER_WDT_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void KYS_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void QDEC0_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void QDEC1_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void QDEC2_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void GPIO_INT0_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void TOUCH_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void M154_REQ_ENH_ACK_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void M154_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void M154_AES_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void PDS_WAKEUP_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void HBN_OUT0_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void HBN_OUT1_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void BOR_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void WIFI_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void BZ_PHY_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void BLE_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void MAC_TXRX_TIMER_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void MAC_TXRX_MISC_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void MAC_RX_TRG_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void MAC_TX_TRG_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void MAC_GEN_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void MAC_PORT_TRG_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));
void WIFI_IPC_PUBLIC_IRQHandler_Wrapper(void) __attribute__((weak, alias("Interrupt_Handler_Stub")));

const pFunc __Vectors[] __attribute__((section(".init"), aligned(64))) = {
    0,                                   /*         */
    0,                                   /*         */
    0,                                   /*         */
    clic_msip_handler_Wrapper,           /* 3       */
    0,                                   /*         */
    0,                                   /*         */
    0,                                   /*         */
    clic_mtimer_handler_Wrapper,         /* 7       */
    (pFunc)0x00000001,                   /*         */
    0,                                   /*         */
    (pFunc)0x00000102, /*         */     //disable log as default
    clic_mext_handler_Wrapper,           /* 11      */
    clic_csoft_handler_Wrapper,          /* 12      */
    0,                                   /*         */
    0,                                   /*         */
    0,                                   /*         */
    BMX_ERR_IRQHandler_Wrapper,          /* 16 +  0 */
    BMX_TO_IRQHandler_Wrapper,           /* 16 +  1 */
    L1C_BMX_ERR_IRQHandler_Wrapper,      /* 16 +  2 */
    L1C_BMX_TO_IRQHandler_Wrapper,       /* 16 +  3 */
    SEC_BMX_ERR_IRQHandler_Wrapper,      /* 16 +  4 */
    RF_TOP_INT0_IRQHandler_Wrapper,      /* 16 +  5 */
    RF_TOP_INT1_IRQHandler_Wrapper,      /* 16 +  6 */
    0,                                   /* 16 +  7 */
    DMA_BMX_ERR_IRQHandler_Wrapper,      /* 16 +  8 */
    SEC_GMAC_IRQHandler_Wrapper,         /* 16 +  9 */
    SEC_CDET_IRQHandler_Wrapper,         /* 16 + 10 */
    SEC_PKA_IRQHandler_Wrapper,          /* 16 + 11 */
    SEC_TRNG_IRQHandler_Wrapper,         /* 16 + 12 */
    SEC_AES_IRQHandler_Wrapper,          /* 16 + 13 */
    SEC_SHA_IRQHandler_Wrapper,          /* 16 + 14 */
    DMA_ALL_IRQHandler_Wrapper,          /* 16 + 15 */
    MJPEG_IRQHandler_Wrapper,            /* 16 + 16 */
    CAM_IRQHandler_Wrapper,              /* 16 + 17 */
    I2S_IRQHandler_Wrapper,              /* 16 + 18 */
    IRTX_IRQHandler_Wrapper,             /* 16 + 19 */
    IRRX_IRQHandler_Wrapper,             /* 16 + 20 */
    USB_IRQHandler_Wrapper,              /* 16 + 21 */
    EMAC_IRQHandler_Wrapper,             /* 16 + 22 */
    SF_CTRL_IRQHandler_Wrapper,          /* 16 + 23 */
    0,                                   /* 16 + 24 */
    GPADC_DMA_IRQHandler_Wrapper,        /* 16 + 25 */
    EFUSE_IRQHandler_Wrapper,            /* 16 + 26 */
    SPI_IRQHandler_Wrapper,              /* 16 + 27 */
    0,                                   /* 16 + 28 */
    UART0_IRQHandler_Wrapper,            /* 16 + 29 */
    UART1_IRQHandler_Wrapper,            /* 16 + 30 */
    0,                                   /* 16 + 31 */
    I2C_IRQHandler_Wrapper,              /* 16 + 32 */
    0,                                   /* 16 + 33 */
    PWM_IRQHandler_Wrapper,              /* 16 + 34 */
    0,                                   /* 16 + 35 */
    TIMER_CH0_IRQHandler_Wrapper,        /* 16 + 36 */
    TIMER_CH1_IRQHandler_Wrapper,        /* 16 + 37 */
    TIMER_WDT_IRQHandler_Wrapper,        /* 16 + 38 */
    KYS_IRQHandler_Wrapper,              /* 16 + 39 */
    QDEC0_IRQHandler_Wrapper,            /* 16 + 40 */
    QDEC1_IRQHandler_Wrapper,            /* 16 + 41 */
    QDEC2_IRQHandler_Wrapper,            /* 16 + 42 */
    0,                                   /* 16 + 43 */
    GPIO_INT0_IRQHandler_Wrapper,        /* 16 + 44 */
    TOUCH_IRQHandler_Wrapper,            /* 16 + 45 */
    0,                                   /* 16 + 46 */
    M154_REQ_ENH_ACK_IRQHandler_Wrapper, /* 16 + 47 */
    M154_IRQHandler_Wrapper,             /* 16 + 48 */
    M154_AES_IRQHandler_Wrapper,         /* 16 + 49 */
    PDS_WAKEUP_IRQHandler_Wrapper,       /* 16 + 50 */
    HBN_OUT0_IRQHandler_Wrapper,         /* 16 + 51 */
    HBN_OUT1_IRQHandler_Wrapper,         /* 16 + 52 */
    BOR_IRQHandler_Wrapper,              /* 16 + 53 */
    WIFI_IRQHandler_Wrapper,             /* 16 + 54 */
    BZ_PHY_IRQHandler_Wrapper,           /* 16 + 55 */
    BLE_IRQHandler_Wrapper,              /* 16 + 56 */
    MAC_TXRX_TIMER_IRQHandler_Wrapper,   /* 16 + 57 */
    MAC_TXRX_MISC_IRQHandler_Wrapper,    /* 16 + 58 */
    MAC_RX_TRG_IRQHandler_Wrapper,       /* 16 + 59 */
    MAC_TX_TRG_IRQHandler_Wrapper,       /* 16 + 60 */
    MAC_GEN_IRQHandler_Wrapper,          /* 16 + 61 */
    MAC_PORT_TRG_IRQHandler_Wrapper,     /* 16 + 62 */
    WIFI_IPC_PUBLIC_IRQHandler_Wrapper,  /* 16 + 63 */
};

void Trap_Handler(void)
{
    unsigned long cause;
    unsigned long epc;
    unsigned long tval;
    uint8_t isecall = 0;

    MSG("Trap_Handler\r\n");

    cause = read_csr(mcause);
    MSG("mcause=%08x\r\n", (uint32_t)cause);
    epc = read_csr(mepc);
    MSG("mepc:%08x\r\n", (uint32_t)epc);
    tval = read_csr(mtval);
    MSG("mtval:%08x\r\n", (uint32_t)tval);

    cause = (cause & 0x3ff);

    switch (cause) {
        case 1:
            MSG("Instruction access fault\r\n");
            break;

        case 2:
            MSG("Illegal instruction\r\n");
            break;

        case 3:
            MSG("Breakpoint\r\n");
            break;

        case 4:
            MSG("Load address misaligned\r\n");
            break;

        case 5:
            MSG("Load access fault\r\n");
            break;

        case 6:
            MSG("Store/AMO address misaligned\r\n");
            break;

        case 7:
            MSG("Store/AMO access fault\r\n");
            break;

        case 8:
            MSG("Environment call from U-mode\r\n");
            epc += 4;
            write_csr(mepc, epc);
            break;

        case 11:
            MSG("Environment call from M-mode\r\n");
            epc += 4;
            write_csr(mepc, epc);
            isecall = 1;
            break;

        default:
            MSG("Cause num=%d\r\n", (uint32_t)cause);
            epc += 4;
            write_csr(mepc, epc);
            break;
    }

    if (!isecall) {
        while (1)
            ;
    }
}

void Interrupt_Handler(void)
{
    pFunc interruptFun;
    uint32_t num = 0;
    volatile uint32_t ulMEPC = 0UL, ulMCAUSE = 0UL;

    /* Store a few register values that might be useful when determining why this
    function was called. */
    __asm volatile("csrr %0, mepc"
                   : "=r"(ulMEPC));
    __asm volatile("csrr %0, mcause"
                   : "=r"(ulMCAUSE));

    if ((ulMCAUSE & 0x80000000) == 0) {
        /*Exception*/
        MSG("Exception should not be here\r\n");
    } else {
        num = ulMCAUSE & 0x3FF;

        if (num < IRQn_LAST) {
            interruptFun = __Interrupt_Handlers[num];

            if (NULL != interruptFun) {
                interruptFun();
            } else {
                MSG("Interrupt num:%d IRQHandler not installed\r\n", (unsigned int)num);

                if (num >= IRQ_NUM_BASE) {
                    MSG("Peripheral Interrupt num:%d \r\n", (unsigned int)num - IRQ_NUM_BASE);
                }

                while (1)
                    ;
            }
        } else {
            MSG("Unexpected interrupt num:%d\r\n", (unsigned int)num);
        }
    }
}

void handle_trap(void)
{
#define MCAUSE_INT_MASK  0x80000000 // [31]=1 interrupt, else exception
#define MCAUSE_CODE_MASK 0x7FFFFFFF // low bits show code

    unsigned long mcause_value = read_csr(mcause);
    if (mcause_value & MCAUSE_INT_MASK) {
        // Branch to interrupt handler here
        Interrupt_Handler();
    } else {
        // Branch to exception handle
        Trap_Handler();
    }
}

void __IRQ_ALIGN64 Trap_Handler_Stub(void)
{
    Trap_Handler();
}

void __IRQ Interrupt_Handler_Stub(void)
{
    Interrupt_Handler();
}

void FreeRTOS_Interrupt_Handler(void)
{
    Interrupt_Handler();
}

void Interrupt_Handler_Register(IRQn_Type irq, pFunc interruptFun)
{
    if (irq < IRQn_LAST) {
        __Interrupt_Handlers[irq] = interruptFun;
    }
}

void System_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
{
}

void clic_enable_interrupt(uint32_t source)
{
    *(volatile uint8_t *)(CLIC_HART0_ADDR + CLIC_INTIE + source) = 1;
}

void clic_disable_interrupt(uint32_t source)
{
    *(volatile uint8_t *)(CLIC_HART0_ADDR + CLIC_INTIE + source) = 0;
}

void clic_clear_pending(uint32_t source)
{
    *(volatile uint8_t *)(CLIC_HART0_ADDR + CLIC_INTIP + source) = 0;
}

void clic_set_pending(uint32_t source)
{
    *(volatile uint8_t *)(CLIC_HART0_ADDR + CLIC_INTIP + source) = 1;
}

void clic_set_intcfg(uint32_t source, uint32_t intcfg)
{
    *(volatile uint8_t *)(CLIC_HART0_ADDR + CLIC_INTCFG + source) = intcfg;
}

uint8_t clic_get_intcfg(uint32_t source)
{
    return *(volatile uint8_t *)(CLIC_HART0_ADDR + CLIC_INTCFG + source);
}

void clic_set_cliccfg(uint32_t cfg)
{
    *(volatile uint8_t *)(CLIC_HART0_ADDR + CLIC_CFG) = cfg;
}

uint8_t clic_get_cliccfg(void)
{
    return *(volatile uint8_t *)(CLIC_HART0_ADDR + CLIC_CFG);
}