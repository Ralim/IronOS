/*
 * FreeRTOS port for the WCH CH57x, context switch
 *
 * Based on the WCH Qingke V4A port (CH583 EVT, Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.)
 * SPDX-License-Identifier: Apache-2.0
 *
 * Task frame, 32 words: [0] mepc, [1] x1, [2..6] x5-x9, [7..14] x10-x17, [15..24] x18-x27, [25..28] x28-x31,
 * [29] x4 (holds the initial mstatus for a task that never ran), [30..31] unused.
 *
 * The switch happens in the software interrupt. SW_Handler saves the task, picks the next one, and then leaves the
 * interrupt (mret) into SW_Handler_Switch with interrupts still off, which restores the next task and mrets into it.
 * Leaving the interrupt first keeps this correct whether or not the core's hardware register stacking is enabled.
 * Handlers live in .highcode (RAM), the vector table jumps there with a plain j.
 */

#define portCONTEXT_SIZE (32 * 4)

.global xPortStartFirstTask
.global SW_Handler
.extern pxCurrentTCB
.extern vTaskSwitchContext

.section .highcode.SW_Handler_Switch,"ax",@progbits
.align 2
.func
SW_Handler_Switch:
	li t0, 0x1880				/* MPP = M, MPIE = 1: interrupts come back on with the mret below */
	csrw mstatus, t0

	lw t1, pxCurrentTCB
	lw sp, 0(t1)

	lw t0, 0(sp)
	csrw mepc, t0

	lw x1, 1 * 4(sp)
	lw x5, 2 * 4(sp)
	lw x6, 3 * 4(sp)
	lw x7, 4 * 4(sp)
	lw x8, 5 * 4(sp)
	lw x9, 6 * 4(sp)
	lw x10, 7 * 4(sp)
	lw x11, 8 * 4(sp)
	lw x12, 9 * 4(sp)
	lw x13, 10 * 4(sp)
	lw x14, 11 * 4(sp)
	lw x15, 12 * 4(sp)
	lw x16, 13 * 4(sp)
	lw x17, 14 * 4(sp)
	lw x18, 15 * 4(sp)
	lw x19, 16 * 4(sp)
	lw x20, 17 * 4(sp)
	lw x21, 18 * 4(sp)
	lw x22, 19 * 4(sp)
	lw x23, 20 * 4(sp)
	lw x24, 21 * 4(sp)
	lw x25, 22 * 4(sp)
	lw x26, 23 * 4(sp)
	lw x27, 24 * 4(sp)
	lw x28, 25 * 4(sp)
	lw x29, 26 * 4(sp)
	lw x30, 27 * 4(sp)
	lw x31, 28 * 4(sp)
	lw x4, 29 * 4(sp)
	addi sp, sp, portCONTEXT_SIZE
	mret
	.endfunc

.section .highcode.SW_Handler,"ax",@progbits
.align 2
.func
SW_Handler:
	addi sp, sp, -portCONTEXT_SIZE
	sw x1, 1 * 4(sp)
	sw x5, 2 * 4(sp)
	sw x6, 3 * 4(sp)
	sw x7, 4 * 4(sp)
	sw x8, 5 * 4(sp)
	sw x9, 6 * 4(sp)
	sw x10, 7 * 4(sp)
	sw x11, 8 * 4(sp)
	sw x12, 9 * 4(sp)
	sw x13, 10 * 4(sp)
	sw x14, 11 * 4(sp)
	sw x15, 12 * 4(sp)
	sw x16, 13 * 4(sp)
	sw x17, 14 * 4(sp)
	sw x18, 15 * 4(sp)
	sw x19, 16 * 4(sp)
	sw x20, 17 * 4(sp)
	sw x21, 18 * 4(sp)
	sw x22, 19 * 4(sp)
	sw x23, 20 * 4(sp)
	sw x24, 21 * 4(sp)
	sw x25, 22 * 4(sp)
	sw x26, 23 * 4(sp)
	sw x27, 24 * 4(sp)
	sw x28, 25 * 4(sp)
	sw x29, 26 * 4(sp)
	sw x30, 27 * 4(sp)
	sw x31, 28 * 4(sp)
	sw x4, 29 * 4(sp)

	lw t0, pxCurrentTCB
	sw sp, 0(t0)
	csrr a1, mepc
	sw a1, 0(sp)

	call vTaskSwitchContext

	li t0, 0x1800				/* MPP = M, MPIE = 0: stay masked until the next task is restored */
	csrw mstatus, t0
	la t0, SW_Handler_Switch
	csrw mepc, t0
	mret
	.endfunc

.section .text.xPortStartFirstTask,"ax",@progbits
.align 2
.func
xPortStartFirstTask:
	lw sp, pxCurrentTCB
	lw sp, 0(sp)

	lw x1, 0(sp)				/* The task entry point, entered with ret */
	lw x6, 3 * 4(sp)
	lw x7, 4 * 4(sp)
	lw x8, 5 * 4(sp)
	lw x9, 6 * 4(sp)
	lw x10, 7 * 4(sp)
	lw x11, 8 * 4(sp)
	lw x12, 9 * 4(sp)
	lw x13, 10 * 4(sp)
	lw x14, 11 * 4(sp)
	lw x15, 12 * 4(sp)
	lw x16, 13 * 4(sp)
	lw x17, 14 * 4(sp)
	lw x18, 15 * 4(sp)
	lw x19, 16 * 4(sp)
	lw x20, 17 * 4(sp)
	lw x21, 18 * 4(sp)
	lw x22, 19 * 4(sp)
	lw x23, 20 * 4(sp)
	lw x24, 21 * 4(sp)
	lw x25, 22 * 4(sp)
	lw x26, 23 * 4(sp)
	lw x27, 24 * 4(sp)
	lw x28, 25 * 4(sp)
	lw x29, 26 * 4(sp)
	lw x30, 27 * 4(sp)
	lw x31, 28 * 4(sp)
	lw x5, 29 * 4(sp)			/* Initial mstatus */
	ori x5, x5, 0x08			/* MIE: interrupts on from here */
	csrw mstatus, x5
	lw x5, 2 * 4(sp)
	addi sp, sp, portCONTEXT_SIZE
	ret
	.endfunc
