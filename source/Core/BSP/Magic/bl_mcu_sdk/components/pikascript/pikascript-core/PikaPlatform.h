/*
 * This file is part of the PikaScript project.
 * http://github.com/pikastech/pikascript
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon 李昂 liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __PIKA_PALTFORM__H
#define __PIKA_PALTFORM__H
#include <stdint.h>
#include <stdlib.h>

#if defined(__CC_ARM) || defined(__CLANG_ARM) /* ARM Compiler */
#define PIKA_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__) /* for IAR Compiler */
#define PIKA_WEAK __weak
#elif defined(__GNUC__) /* GNU GCC Compiler */
#define PIKA_WEAK __attribute__((weak))
#endif

void __platformPrintf(char* fmt, ...);
void* __platformMalloc(size_t size);
void __platformFree(void* ptr);
void __platformEnableIrqHandle(void);
void __platformDisableIrqHandle(void);

void* __impl_pikaMalloc(size_t size);
void __impl_pikaFree(void* ptrm, size_t size);
uint8_t __isLocked_pikaMemory(void);
uint8_t __is_quick_malloc(void);
void __quick_malloc_enable(void);
void __quick_malloc_disable(void);
char* __platformLoadPikaAsm(void);
int32_t __platformSavePikaAsm(char* PikaAsm);
uint8_t __platformAsmIsToFlash(char* pyMultiLine);
int32_t __platformSavePikaAsmEOF(void);
PIKA_WEAK void __platformPoolInit(void);
void __platformWait(void);
#endif
