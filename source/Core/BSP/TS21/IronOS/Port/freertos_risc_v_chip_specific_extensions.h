/*
 * FreeRTOS Kernel V10.4.6
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/*
 * The FreeRTOS kernel's RISC-V port is split between the the code that is
 * common across all currently supported RISC-V chips (implementations of the
 * RISC-V ISA), and code that tailors the port to a specific RISC-V chip:
 *
 * + FreeRTOS\Source\portable\GCC\RISC-V-RV32\portASM.S contains the code that
 *   is common to all currently supported RISC-V chips.  There is only one
 *   portASM.S file because the same file is built for all RISC-V target chips.
 *
 * + Header files called freertos_risc_v_chip_specific_extensions.h contain the
 *   code that tailors the FreeRTOS kernel's RISC-V port to a specific RISC-V
 *   chip.  There are multiple freertos_risc_v_chip_specific_extensions.h files
 *   as there are multiple RISC-V chip implementations.
 *
 * !!!NOTE!!!
 * TAKE CARE TO INCLUDE THE CORRECT freertos_risc_v_chip_specific_extensions.h
 * HEADER FILE FOR THE CHIP IN USE.  This is done using the assembler's (not the
 * compiler's!) include path.  For example, if the chip in use includes a core
 * local interrupter (CLINT) and does not include any chip specific register
 * extensions then add the path below to the assembler's include path:
 * FreeRTOS\Source\portable\GCC\RISC-V-RV32\chip_specific_extensions\RV32I_CLINT_no_extensions
 *
 */


#ifndef __FREERTOS_RISC_V_EXTENSIONS_H__
#define __FREERTOS_RISC_V_EXTENSIONS_H__

#define portasmHAS_SIFIVE_CLINT 0
#define portasmHAS_MTIME 0
/* if you want to use FPU, please define ARCH_FPU and enable float point and ABI of gcc */
#define ARCH_FPU 0


#if ARCH_FPU
#define portasmADDITIONAL_CONTEXT_SIZE 32 /* Must be even number on 32-bit cores. */
.macro portasmSAVE_ADDITIONAL_REGISTERS
    addi sp, sp, -(portasmADDITIONAL_CONTEXT_SIZE* portWORD_SIZE)
    fsw f0, 1*portWORD_SIZE(sp)
    fsw f1, 2*portWORD_SIZE(sp)
    fsw f2, 3*portWORD_SIZE(sp)
    fsw f3, 4*portWORD_SIZE(sp)
    fsw f4, 5*portWORD_SIZE(sp)
    fsw f5, 6*portWORD_SIZE(sp)
    fsw f6, 7*portWORD_SIZE(sp)
    fsw f7, 8*portWORD_SIZE(sp)
    fsw f8, 9*portWORD_SIZE(sp)
    fsw f9, 10*portWORD_SIZE(sp)
    fsw f10, 11*portWORD_SIZE(sp)
    fsw f11, 12*portWORD_SIZE(sp)
    fsw f12, 13*portWORD_SIZE(sp)
    fsw f13, 14*portWORD_SIZE(sp)
    fsw f14, 15*portWORD_SIZE(sp)
    fsw f15, 16*portWORD_SIZE(sp)
    fsw f16, 17*portWORD_SIZE(sp)
    fsw f17, 18*portWORD_SIZE(sp)
    fsw f18, 19*portWORD_SIZE(sp)
    fsw f19, 20*portWORD_SIZE(sp)
    fsw f20, 21*portWORD_SIZE(sp)
    fsw f21, 22*portWORD_SIZE(sp)
    fsw f22, 23*portWORD_SIZE(sp)
    fsw f23, 24*portWORD_SIZE(sp)
    fsw f24, 25*portWORD_SIZE(sp)
    fsw f25, 26*portWORD_SIZE(sp)
    fsw f26, 27*portWORD_SIZE(sp)
    fsw f27, 28*portWORD_SIZE(sp)
    fsw f28, 29*portWORD_SIZE(sp)
    fsw f29, 30*portWORD_SIZE(sp)
    fsw f30, 31*portWORD_SIZE(sp)
    fsw f31, 32*portWORD_SIZE(sp)
	.endm

.macro portasmRESTORE_ADDITIONAL_REGISTERS
    flw f0, 1*portWORD_SIZE(sp)
    flw f1, 2*portWORD_SIZE(sp)
    flw f2, 3*portWORD_SIZE(sp)
    flw f3, 4*portWORD_SIZE(sp)
    flw f4, 5*portWORD_SIZE(sp)
    flw f5, 6*portWORD_SIZE(sp)
    flw f6, 7*portWORD_SIZE(sp)
    flw f7, 8*portWORD_SIZE(sp)
    flw f8, 9*portWORD_SIZE(sp)
    flw f9, 10*portWORD_SIZE(sp)
    flw f10, 11*portWORD_SIZE(sp)
    flw f11, 12*portWORD_SIZE(sp)
    flw f12, 13*portWORD_SIZE(sp)
    flw f13, 14*portWORD_SIZE(sp)
    flw f14, 15*portWORD_SIZE(sp)
    flw f15, 16*portWORD_SIZE(sp)
    flw f16, 17*portWORD_SIZE(sp)
    flw f17, 18*portWORD_SIZE(sp)
    flw f18, 19*portWORD_SIZE(sp)
    flw f19, 20*portWORD_SIZE(sp)
    flw f20, 21*portWORD_SIZE(sp)
    flw f21, 22*portWORD_SIZE(sp)
    flw f22, 23*portWORD_SIZE(sp)
    flw f23, 24*portWORD_SIZE(sp)
    flw f24, 25*portWORD_SIZE(sp)
    flw f25, 26*portWORD_SIZE(sp)
    flw f26, 27*portWORD_SIZE(sp)
    flw f27, 28*portWORD_SIZE(sp)
    flw f28, 29*portWORD_SIZE(sp)
    flw f29, 30*portWORD_SIZE(sp)
    flw f30, 31*portWORD_SIZE(sp)
    flw f31, 32*portWORD_SIZE(sp)
    addi sp, sp, (portasmADDITIONAL_CONTEXT_SIZE* portWORD_SIZE)
	.endm
#else
#define portasmADDITIONAL_CONTEXT_SIZE 0 /* Must be even number on 32-bit cores. */


.macro portasmSAVE_ADDITIONAL_REGISTERS
    /* No additional registers to save, so this macro does nothing. */
    .endm

.macro portasmRESTORE_ADDITIONAL_REGISTERS
    /* No additional registers to restore, so this macro does nothing. */
    .endm

#endif

#endif /* __FREERTOS_RISC_V_EXTENSIONS_H__ */
