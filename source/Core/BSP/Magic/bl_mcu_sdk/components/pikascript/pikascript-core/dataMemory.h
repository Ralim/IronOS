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

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "PikaPlatform.h"

typedef struct {
    uint32_t heapUsed;
    uint32_t heapUsedMax;
} PikaMemInfo;

typedef uint8_t* BitMap;

typedef struct {
    BitMap bitmap;
    uint8_t* mem;
    uint8_t aline;
    uint32_t size;
    uint32_t first_free_block;
    uint32_t purl_free_block_start;
} Pool;

void pikaFree(void* mem, uint32_t size);
void* pikaMalloc(uint32_t size);
uint16_t pikaMemNow(void);
uint16_t pikaMemMax(void);
void pikaMemMaxReset(void);

uint32_t aline_by(uint32_t size, uint32_t aline);

BitMap bitmap_init(uint32_t size);
void bitmap_set(BitMap bitmap, uint32_t index, uint8_t bit);
uint8_t bitmap_get(BitMap bitmap, uint32_t index);
uint8_t bitmap_getByte(BitMap bitmap, uint32_t index);
void bitmap_deinit(BitMap bitmap);

Pool pool_init(uint32_t size, uint8_t aline);
void* pool_malloc(Pool* pool, uint32_t size);
void pool_free(Pool* pool, void* mem, uint32_t size);
void pool_deinit(Pool* pool);
void pool_printBlocks(Pool* pool, uint32_t block_min, uint32_t block_max);
#endif
