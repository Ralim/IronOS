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

#ifndef __DATA_QUEUE__H
#define __DATA_QUEUE__H
#include "dataArgs.h"

typedef Args Queue;
Queue* New_queue(void);

int32_t queue_deinit(Queue* queue);
int32_t queue_pushInt(Queue* queue, int val);
int32_t queue_pushFloat(Queue* queue, float val);
int32_t queue_pushStr(Queue* queue, char* str);
int32_t queue_pushArg(Queue* queue, Arg* arg);
char *fast_itoa(char *buf, uint32_t val);

int64_t queue_popInt(Queue* queue);
float queue_popFloat(Queue* queue);
char* queue_popStr(Queue* queue);
Arg* queue_popArg(Queue* queue);
#endif
