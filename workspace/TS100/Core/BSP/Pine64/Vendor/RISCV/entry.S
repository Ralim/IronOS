#include "riscv_encoding.h"
#include "riscv_bits.h"
#include "n200_eclic.h"
#include "n200_timer.h"

#define USE_MSP     1                                      //启用中断栈


/**
 * @brief 压栈通用寄存器
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
	#STORE x3, 3 * REGBYTES(\x)
	#STORE x4, 4 * REGBYTES(\x)
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
 * @brief 压栈csr寄存器（CSR_MSTATUS、CSR_MEPC、CSR_MSUBM、CSR_MCAUSE）
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
 * @brief 压栈浮点寄存器
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
	flw	f10,46 * REGBYTES(\x)
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
 * @brief 出栈通用寄存器
 * @param x 目标sp寄存器
 */
.macro popREGFILE x
	LOAD x1, 1 * REGBYTES(\x)
	#LOAD x2, 2 * REGBYTES(\x)
	#LOAD x3, 3 * REGBYTES(\x)
	#LOAD x4, 4 * REGBYTES(\x)
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
 * @brief 出栈csr寄存器（CSR_MSTATUS、CSR_MEPC、CSR_MSUBM、CSR_MCAUSE）
 * @param x 目标sp寄存器
 */
.macro portRESTORE_CONTEXT_EXCP x
	LOAD t0, 35*REGBYTES(\x)
  	csrw CSR_MCAUSE, t0
	LOAD t0, 34*REGBYTES(\x)
  	csrw CSR_MSUBM, t0
	LOAD t0, 33*REGBYTES(\x)
  	csrw CSR_MEPC, t0
  	LOAD t0, 32*REGBYTES(\x)
  	csrw CSR_MSTATUS, t0
.endm


/**
 * @brief 出栈浮点寄存器
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
 * @brief 清理fpu状态寄存器
 */
.macro CONFIG_FS_CLEAN
	li t0, (0x1 << 13)                                     //配置FS为clean状态
	csrc mstatus, t0
	li t0, (0x1 << 14)
	csrs mstatus, t0
.endm


/* -------------------------------------------------------------------------------------------------------- */
/**
 * @brief trap入口函数
 */
    .section .text.trap
    .align 6// In CLIC mode, the trap entry must be 64bytes aligned
    .global trap_entry
    .weak trap_entry
trap_entry:
	pushREGFILE sp                                         //trap这里就直接使用当前栈，方便对当前位置进行异常分析，
	                                                       //同时不担心(任务栈/中断栈/主栈)溢出，因为程序进入这里便不会返回了
	portSAVE_CONTEXT_EXCP sp

	csrr a0, mcause
	mv a1, sp
	jal ulSynchTrap
	mv sp, a0

	portRESTORE_CONTEXT_EXCP sp
	popREGFILE sp
	mret


/* -------------------------------------------------------------------------------------------------------- */
/**
 * @brief irq入口函数
 */
    .section .text.irq
    .align 2
    .global irq_entry
    .weak irq_entry
irq_entry:
#if USE_MSP
	csrrw sp, CSR_MSCRATCHCSWL, sp
#endif
  	pushREGFILE sp                                         //保存通用寄存器
	portSAVE_CONTEXT_EXCP sp
#ifdef __riscv_flen
	csrr t2, mstatus
	li t0, (0x3 << 13)
	and t1, t2, t0
	bne t1, t0, 1f                                         //浮点寄存器状态为Dirty状态，则保存浮点寄存器, 否则不用保存
	pushVFPREGFILE sp
1:
	CONFIG_FS_CLEAN
#endif

  	csrrw ra, CSR_JALMNXTI, ra                             //跳转到中断向量表入口地址，中断处理返回时继续回到此处

  	csrc CSR_MSTATUS, MSTATUS_MIE                          //此时中断处理完毕，中断关闭，注意mret退出中断时将恢复从mpie恢复mie，
	  									                   //因此在中断内部修改mie仅会保持中断mret前，退出中断将恢复为进中断前状态
#ifdef __riscv_flen
	csrr t2, mstatus
	li t0, (0x3 << 13)
	and t1, t2, t0
	bne t1, t0, 2f                                         //浮点寄存器状态为Dirty状态，则恢复浮点寄存器, 否则不用恢复
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
