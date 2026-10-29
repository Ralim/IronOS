/********************************** (C) COPYRIGHT *******************************
 * File Name          : startup_ch57x.s
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2020/04/30
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

	.section	.init,"ax",@progbits
	.global	_start
	.align	1
_start:
	j	handle_reset

    .section    .vector,"ax",@progbits
    .align  1
_vector_base:
    .option norvc;
    .word   0
    .word   0
        j   NMI_Handler                 /* NMI Handler */
        j   HardFault_Handler           /* Hard Fault Handler */
    .word   0
    .word   0
    .word   0
    .word   0
    .word   0
    .word   0
    .word   0
    .word   0
        j   SysTick_Handler            /* SysTick Handler */
    .word   0
        j   SW_Handler                 /* SW Handler */
    .word   0
    /* External Interrupts */
        j   TMR0_IRQHandler            /* 0:  TMR0 */
        j   GPIOA_IRQHandler           /* GPIOA */
        j   GPIOB_IRQHandler           /* GPIOB */
        j   SPI0_IRQHandler            /* SPI0 */
        j   BB_IRQHandler              /* BLEB */
        j   LLE_IRQHandler             /* BLEL */
        j   USB_IRQHandler             /* USB */
        .word   0
        j   TMR1_IRQHandler            /* TMR1 */
        j   TMR2_IRQHandler            /* TMR2 */
        j   UART0_IRQHandler           /* UART0 */
        j   UART1_IRQHandler           /* UART1 */
        j   RTC_IRQHandler             /* RTC */
        j   ADC_IRQHandler             /* ADC */
        .word   0
        j   PWMX_IRQHandler            /* PWMX */
        j   TMR3_IRQHandler            /* TMR3 */
        j   UART2_IRQHandler           /* UART2 */
        j   UART3_IRQHandler           /* UART3 */
        j   WDOG_BAT_IRQHandler        /* WDOG_BAT */

    .option rvc;

    .section    .vector_handler, "ax", @progbits
    .weak   NMI_Handler
    .weak   HardFault_Handler
    .weak   SysTick_Handler
    .weak   SW_Handler
    .weak   TMR0_IRQHandler
    .weak   GPIOA_IRQHandler
    .weak   GPIOB_IRQHandler
    .weak   SPI0_IRQHandler
    .weak   BB_IRQHandler
    .weak   LLE_IRQHandler
    .weak   USB_IRQHandler
    .weak   TMR1_IRQHandler
    .weak   TMR2_IRQHandler
    .weak   UART0_IRQHandler
    .weak   UART1_IRQHandler
    .weak   RTC_IRQHandler
    .weak   ADC_IRQHandler
    .weak   PWMX_IRQHandler
    .weak   TMR3_IRQHandler
    .weak   UART2_IRQHandler
    .weak   UART3_IRQHandler
    .weak   WDOG_BAT_IRQHandler

NMI_Handler:  j fault_halt
HardFault_Handler:  j fault_halt
SysTick_Handler:  j fault_halt
SW_Handler:  j fault_halt
TMR0_IRQHandler:  j fault_halt
GPIOA_IRQHandler:  j fault_halt
GPIOB_IRQHandler:  j fault_halt
SPI0_IRQHandler:  j fault_halt
BB_IRQHandler:  j fault_halt
LLE_IRQHandler:  j fault_halt
USB_IRQHandler:  j fault_halt
TMR1_IRQHandler:  j fault_halt
TMR2_IRQHandler:  j fault_halt
UART0_IRQHandler:  j fault_halt
UART1_IRQHandler:  j fault_halt
RTC_IRQHandler:  j fault_halt
ADC_IRQHandler:  j fault_halt
PWMX_IRQHandler:  j fault_halt
TMR3_IRQHandler:  j fault_halt
UART2_IRQHandler:  j fault_halt
UART3_IRQHandler:  j fault_halt
WDOG_BAT_IRQHandler:  j fault_halt

	/* Faults and stray interrupts: heater off, then wait for the watchdog */
	.global	fault_halt
fault_halt:
	csrci mstatus, 0x8
	li t0, 0x40002000		/* TMR0 */
	sb zero, 0(t0)			/* R8_TMR0_CTRL_MOD: output off */
	sw zero, 0x10(t0)		/* R32_TMR0_FIFO */
	li t0, 0x400010C0		/* R32_PB_DIR */
	li t1, 0x00800000		/* PB23 */
	sw t1, 0x0C(t0)			/* R32_PB_CLR */
	lw t2, 0(t0)
	or t2, t2, t1
	sw t2, 0(t0)
1:	j 1b

	.section	.handle_reset,"ax",@progbits
	.weak	handle_reset
	.align	1
handle_reset:
	/* Heater off before anything else: PB23 (TMR0 output, high = heater on) is an input after reset */
	li t0, 0x400010C0		/* R32_PB_DIR */
	li t1, 0x00800000		/* PB23 */
	sw t1, 0x0C(t0)			/* R32_PB_CLR */
	lw t2, 0(t0)
	or t2, t2, t1
	sw t2, 0(t0)
.option push
.option	norelax
	la gp, __global_pointer$
.option	pop
1:
	la sp, _eusrstack

/* Load highcode code  section from flash to RAM */
2:
    la a0, _highcode_lma
    la a1, _highcode_vma_start
    la a2, _highcode_vma_end
    bgeu a1, a2, 2f
1:
    lw t0, (a0)
    sw t0, (a1)
    addi a0, a0, 4
    addi a1, a1, 4
    bltu a1, a2, 1b

/* Load data section from flash to RAM */
2:
	la a0, _data_lma
	la a1, _data_vma
	la a2, _edata
	bgeu a1, a2, 2f
1:
	lw t0, (a0)
	sw t0, (a1)
	addi a0, a0, 4
	addi a1, a1, 4
	bltu a1, a2, 1b
2:
	/* clear bss section */
	la a0, _sbss
	la a1, _ebss
	bgeu a0, a1, 2f
1:
	sw zero, (a0)
	addi a0, a0, 4
	bltu a0, a1, 1b
2:
	/* PFIC_CFGR = KEY1 | NESTCTRL | HWSTKCTRL: interrupt nesting and hardware register stacking off (both on
	   after reset), every handler saves its own registers */
	li t0, 0xE000E048
	li t1, 0xFA050003
	sw t1, 0(t0)
	la t0, _vector_base
    ori t0, t0, 1
	csrw mtvec, t0
	call __libc_init_array
	/* main runs in machine mode with interrupts on */
    li t0, 0x1888
    csrs mstatus, t0
	la t0, main
	csrw mepc, t0
	mret


