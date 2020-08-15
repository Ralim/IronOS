#include "riscv_encoding.h"
#include "riscv_bits.h"
#include "n200_timer.h"
#include "n200_eclic.h"
#include "riscv_encoding.h"
#include "riscv_bits.h"
#include "n200_timer.h"
#include "n200_eclic.h"

#define USE_MSP    1                                       //启用中断栈


	.section .text.entry
	.align 4

	.global vPortYield
	.global vPortYield_from_ulSynchTrap
	.global xPortStartScheduler
	.global vPortEndScheduler
	.global vPortAsmAssertSP

/**
 * @brife 压栈通用寄存器
 * @param x 目标sp寄存器
 */
.macro pushREGFILE x
#ifdef __riscv_flen
	addi \x, \x, -REGBYTES * 68   //36+32
#else
	addi \x, \x, -REGBYTES * 36
#endif
	STORE x1, 1 * REGBYTES(\x)
	STORE x2, 2 * REGBYTES(\x)
	//STORE x3, 3 * REGBYTES(\x)
	//STORE	x4, 4 * REGBYTES(\x)
	STORE x5, 5 * REGBYTES(\x)
	STORE x6, 6 * REGBYTES(\x)
	STORE x7, 7 * REGBYTES(\x)
	STORE x8, 8 * REGBYTES(\x)
	STORE x9, 9 * REGBYTES(\x)
	STORE x10, 10 * REGBYTES(\x)
	STORE x11, 11 * REGBYTES(\x)
	STORE x12, 12 * REGBYTES(\x)
	STORE x13, 13 * REGBYTES(\x)
	STORE x14, 14 * REGBYTES(\x)
	STORE x15, 15 * REGBYTES(\x)
#ifndef __riscv_32e
	STORE x16, 16 * REGBYTES(\x)
	STORE x17, 17 * REGBYTES(\x)
	STORE x18, 18 * REGBYTES(\x)
	STORE x19, 19 * REGBYTES(\x)
	STORE x20, 20 * REGBYTES(\x)
	STORE x21, 21 * REGBYTES(\x)
	STORE x22, 22 * REGBYTES(\x)
	STORE x23, 23 * REGBYTES(\x)
	STORE x24, 24 * REGBYTES(\x)
	STORE x25, 25 * REGBYTES(\x)
	STORE x26, 26 * REGBYTES(\x)
	STORE x27, 27 * REGBYTES(\x)
	STORE x28, 28 * REGBYTES(\x)
	STORE x29, 29 * REGBYTES(\x)
	STORE x30, 30 * REGBYTES(\x)
	STORE x31, 31 * REGBYTES(\x)
#endif
.endm


/**
 * @brife 压栈csr寄存器(CSR_MSTATUS CSR_MEPC CSR_MSUBM CSR_MCAUSE)
 * @param x 目标sp寄存器
 */
.macro portSAVE_CONTEXT_EXCP x
	csrr t0, CSR_MSTATUS
	STORE t0, 32 * REGBYTES(\x)
	csrr t0, CSR_MEPC
	STORE t0, 33 * REGBYTES(\x)
	csrr t0, CSR_MSUBM
	STORE t0, 34 * REGBYTES(\x)
	csrr t0, CSR_MCAUSE
	STORE t0, 35 * REGBYTES(\x)
.endm


/**
 * @brife 压栈浮点寄存器
 * @param x 目标sp寄存器
 */
.macro pushVFPREGFILE x
	fsw	f0, 36 * REGBYTES(\x)
	fsw	f1, 37 * REGBYTES(\x)
	fsw	f2, 38 * REGBYTES(\x)
	fsw	f3, 39 * REGBYTES(\x)
	fsw	f4, 40 * REGBYTES(\x)
	fsw	f5, 41 * REGBYTES(\x)
	fsw	f6, 42 * REGBYTES(\x)
	fsw	f7, 43 * REGBYTES(\x)
	fsw	f8, 44 * REGBYTES(\x)
	fsw	f9, 45 * REGBYTES(\x)
	fsw	f10, 46 * REGBYTES(\x)
	fsw	f11, 47 * REGBYTES(\x)
	fsw	f12, 48 * REGBYTES(\x)
	fsw	f13, 49 * REGBYTES(\x)
	fsw	f14, 50 * REGBYTES(\x)
	fsw	f15, 51 * REGBYTES(\x)
	fsw	f16, 52 * REGBYTES(\x)
	fsw	f17, 53 * REGBYTES(\x)
	fsw	f18, 54 * REGBYTES(\x)
	fsw	f19, 55 * REGBYTES(\x)
	fsw	f20, 56 * REGBYTES(\x)
	fsw	f21, 57 * REGBYTES(\x)
	fsw	f22, 58 * REGBYTES(\x)
	fsw	f23, 59 * REGBYTES(\x)
	fsw	f24, 60 * REGBYTES(\x)
	fsw	f25, 61 * REGBYTES(\x)
	fsw	f26, 62 * REGBYTES(\x)
	fsw	f27, 63 * REGBYTES(\x)
	fsw	f28, 64 * REGBYTES(\x)
	fsw	f29, 65 * REGBYTES(\x)
	fsw	f30, 66 * REGBYTES(\x)
	fsw	f31, 67 * REGBYTES(\x)
.endm


/**
 * @brife 出栈通用寄存器
 * @param x 目标sp寄存器
 */
.macro popREGFILE x
	LOAD x1, 1 * REGBYTES(\x)
	//LOAD x2, 2 * REGBYTES(\x)
	//LOAD x3, 3 * REGBYTES(\x)
	//LOAD x4, 4 * REGBYTES(\x)
	LOAD x5, 5 * REGBYTES(\x)
	LOAD x6, 6 * REGBYTES(\x)
	LOAD x7, 7 * REGBYTES(\x)
	LOAD x8, 8 * REGBYTES(\x)
	LOAD x9, 9 * REGBYTES(\x)
	LOAD x10, 10 * REGBYTES(\x)
	LOAD x11, 11 * REGBYTES(\x)
	LOAD x12, 12 * REGBYTES(\x)
	LOAD x13, 13 * REGBYTES(\x)
	LOAD x14, 14 * REGBYTES(\x)
	LOAD x15, 15 * REGBYTES(\x)
#ifndef __riscv_32e
	LOAD x16, 16 * REGBYTES(\x)
	LOAD x17, 17 * REGBYTES(\x)
	LOAD x18, 18 * REGBYTES(\x)
	LOAD x19, 19 * REGBYTES(\x)
	LOAD x20, 20 * REGBYTES(\x)
	LOAD x21, 21 * REGBYTES(\x)
	LOAD x22, 22 * REGBYTES(\x)
	LOAD x23, 23 * REGBYTES(\x)
	LOAD x24, 24 * REGBYTES(\x)
	LOAD x25, 25 * REGBYTES(\x)
	LOAD x26, 26 * REGBYTES(\x)
	LOAD x27, 27 * REGBYTES(\x)
	LOAD x28, 28 * REGBYTES(\x)
	LOAD x29, 29 * REGBYTES(\x)
	LOAD x30, 30 * REGBYTES(\x)
	LOAD x31, 31 * REGBYTES(\x)
#endif
#ifdef __riscv_flen
	addi \x, \x, REGBYTES * 68   //36+32
#else
	addi \x, \x, REGBYTES * 36
#endif
.endm


/**
 * @brife 出栈csr寄存器(CSR_MSTATUS CSR_MEPC CSR_MSUBM CSR_MCAUSE)
 * @param x 目标sp寄存器
 */
.macro portRESTORE_CONTEXT_EXCP x
	LOAD t0, 35 * REGBYTES(\x)
	csrw CSR_MCAUSE, t0
	LOAD t0, 34 * REGBYTES(\x)
	csrw CSR_MSUBM, t0
	LOAD t0, 33 * REGBYTES(\x)
	csrw CSR_MEPC, t0
	LOAD t0, 32 * REGBYTES(\x)
	csrw CSR_MSTATUS, t0
.endm


/**
 * @brife 出栈浮点寄存器
 * @param x 目标sp寄存器
 */
.macro popVFPREGFILE x
	flw	f0, 36 * REGBYTES(\x)
	flw	f1, 37 * REGBYTES(\x)
	flw	f2, 38 * REGBYTES(\x)
	flw	f3, 39 * REGBYTES(\x)
	flw	f4, 40 * REGBYTES(\x)
	flw	f5, 41 * REGBYTES(\x)
	flw	f6, 42 * REGBYTES(\x)
	flw	f7, 43 * REGBYTES(\x)
	flw	f8, 44 * REGBYTES(\x)
	flw	f9, 45 * REGBYTES(\x)
	flw	f10, 46 * REGBYTES(\x)
	flw	f11, 47 * REGBYTES(\x)
	flw	f12, 48 * REGBYTES(\x)
	flw	f13, 49 * REGBYTES(\x)
	flw	f14, 50 * REGBYTES(\x)
	flw	f15, 51 * REGBYTES(\x)
	flw	f16, 52 * REGBYTES(\x)
	flw	f17, 53 * REGBYTES(\x)
	flw	f18, 54 * REGBYTES(\x)
	flw	f19, 55 * REGBYTES(\x)
	flw	f20, 56 * REGBYTES(\x)
	flw	f21, 57 * REGBYTES(\x)
	flw	f22, 58 * REGBYTES(\x)
	flw	f23, 59 * REGBYTES(\x)
	flw	f24, 60 * REGBYTES(\x)
	flw	f25, 61 * REGBYTES(\x)
	flw	f26, 62 * REGBYTES(\x)
	flw	f27, 63 * REGBYTES(\x)
	flw	f28, 64 * REGBYTES(\x)
	flw	f29, 65 * REGBYTES(\x)
	flw	f30, 66 * REGBYTES(\x)
	flw	f31, 67 * REGBYTES(\x)
.endm


/**
 * @brife 清理fpu状态寄存器
 */
.macro CONFIG_FS_CLEAN
	li t0, (0x1 << 13)
	csrc mstatus,t0
	li t0, (0x1 << 14)
	csrs mstatus,t0
.endm


/* -------------------------------------------------------------------------------------------------------- */
/**
 * @brife trap入口函数
 */
	.global trap_entry
	.align 6
trap_entry:
	pushREGFILE sp

	portSAVE_CONTEXT_EXCP sp

	csrr a0, mcause
	mv a1, sp
	jal ulSynchTrap
	mv sp,a0

	portRESTORE_CONTEXT_EXCP sp
	popREGFILE sp
	mret


/* -------------------------------------------------------------------------------------------------------- */
/**
 * @brife trq入口函数
 */
    .align 2
    .global irq_entry
irq_entry:
#if USE_MSP
	csrrw sp, CSR_MSCRATCHCSWL, sp
#endif
  	pushREGFILE sp
	portSAVE_CONTEXT_EXCP sp
#ifdef __riscv_flen
	csrr t2, mstatus
	li t0, (0x3 << 13)
	and t1,t2,t0
	bne t1,t0,1f
	pushVFPREGFILE sp
1:
	CONFIG_FS_CLEAN
#endif

int_loop:
  	csrrw ra, CSR_JALMNXTI, ra
  	csrrsi a0, CSR_MNXTI, MSTATUS_MIE
  	bnez a0, int_loop

  	csrc CSR_MSTATUS, MSTATUS_MIE

#ifdef __riscv_flen
	csrr t2, mstatus
	li t0, (0x3 << 13)
	and t1,t2, t0
	bne t1,t0, 2f
	popVFPREGFILE sp
2:
#endif
	portRESTORE_CONTEXT_EXCP sp
#ifdef __riscv_flen
	CONFIG_FS_CLEAN
#endif
  	popREGFILE sp
#if USE_MSP
	csrrw sp, CSR_MSCRATCHCSWL, sp
#endif
  	mret


/* -------------------------------------------------------------------------------------------------------- */
/**
 * @brife MTIME入口函数
 */
    .align 2
    .globl MTIME_HANDLER
MTIME_HANDLER:
#if USE_MSP
	csrrw sp, CSR_MSCRATCHCSWL, sp
#endif
  	pushREGFILE sp
	portSAVE_CONTEXT_EXCP sp

	csrs CSR_MSTATUS, MSTATUS_MIE
	jal	vPortSysTickHandler
	csrc CSR_MSTATUS, MSTATUS_MIE

	portRESTORE_CONTEXT_EXCP sp
	popREGFILE sp
#if USE_MSP
	csrrw sp, CSR_MSCRATCHCSWL, sp
#endif
	mret


/* -------------------------------------------------------------------------------------------------------- */
/**
 * @brife MSIP入口函数
 */
	.align 2
	.globl MSIP_HANDLER

MSIP_HANDLER:
	pushREGFILE sp
	portSAVE_CONTEXT_EXCP sp

	mv a0,sp
	call vPortClearMSIPInt
	jal taskswitch

	portRESTORE_CONTEXT_EXCP sp
	popREGFILE sp
	mret


/* -------------------------------------------------------------------------------------------------------- */
/**
 * @brife Trap模式请求切换任务函数
 */
    .align 6
vPortYield_from_ulSynchTrap:
	mv sp, a0
	portSAVE_CONTEXT_EXCP sp

	j _vPortYield


/* -------------------------------------------------------------------------------------------------------- */
/**
 * @brife MSIP模式请求切换任务函数
 */
    .align 6
vPortYield:
	mv sp, a0
_vPortYield:
	LOAD t0, pxCurrentTCB
	STORE sp, 0x0(t0)

#ifdef __riscv_flen
	csrr t2, mstatus
	li t0, (0x3 << 13)
	and t1,t2,t0
	bne t1,t0,1f
	pushVFPREGFILE sp
1:
	CONFIG_FS_CLEAN
#endif
	STORE a1, 33 * REGBYTES(sp)

#if USE_MSP
	csrr sp, CSR_MSCRATCH
#endif
	csrs CSR_MSTATUS, MSTATUS_MIE
	jal vDoTaskSwitchContext
	csrc CSR_MSTATUS, MSTATUS_MIE

	LOAD sp, pxCurrentTCB
	LOAD sp, 0x0(sp)

	portRESTORE_CONTEXT_EXCP sp
#ifdef __riscv_flen
	csrr t2, mstatus
	li t0, (0x3 << 13)
	and t1, t2, t0
	bne t1, t0, 2f
	popVFPREGFILE sp
2:
	CONFIG_FS_CLEAN
#endif
	popREGFILE sp
	mret


/* -------------------------------------------------------------------------------------------------------- */
/**
 * @brife freertos启动调度函数
 */
xPortStartScheduler:
	jal	vPortSetup
	csrc CSR_MSTATUS, MSTATUS_MIE

#if USE_MSP
	la t0, _sp
	csrw CSR_MSCRATCH, t0
#endif

	LOAD sp, pxCurrentTCB
	LOAD sp, 0x0(sp)

	portRESTORE_CONTEXT_EXCP sp

	popREGFILE sp
	mret


/* -------------------------------------------------------------------------------------------------------- */
/**
 * @brife MSIP模式请求切换任务函数
 */
vPortEndScheduler:
 	j vPortEndScheduler
