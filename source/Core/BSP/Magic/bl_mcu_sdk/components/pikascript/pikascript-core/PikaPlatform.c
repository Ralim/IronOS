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

#include "PikaPlatform.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

PIKA_WEAK void __platformDisableIrqHandle(void) {
    /* disable irq to support thread */
}
PIKA_WEAK void __platformEnableIrqHandle(void) {
    /* disable irq to support thread */
}
PIKA_WEAK void* __platformMalloc(size_t size) {
    return malloc(size);
}
PIKA_WEAK void __platformFree(void* ptr) {
    free(ptr);
}
PIKA_WEAK void* __impl_pikaMalloc(size_t size) {
    return __platformMalloc(size);
}
PIKA_WEAK void __impl_pikaFree(void* ptrm, size_t size) {
    __platformFree(ptrm);
}
PIKA_WEAK void __platformPoolInit(void) {}
PIKA_WEAK uint8_t __is_quick_malloc(void) {
    return 0;
}
PIKA_WEAK void __quick_malloc_enable(void) {}
PIKA_WEAK void __quick_malloc_disable(void) {}
PIKA_WEAK uint8_t __isLocked_pikaMemory(void) {
    return 0;
}
PIKA_WEAK void __platformPrintf(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}
PIKA_WEAK char* __platformLoadPikaAsm(void) {
    /* faild */
    return NULL;
}
PIKA_WEAK int32_t __platformSavePikaAsm(char* PikaAsm) {
    /* faild */
    return 1;
}
PIKA_WEAK uint8_t __platformAsmIsToFlash(char* pyMultiLine) {
    /* not to flash */
    return 0;
}
PIKA_WEAK int32_t __platformSavePikaAsmEOF(void) {
    return 1;
}
PIKA_WEAK void __platformWait(void){
    while(1){};
}