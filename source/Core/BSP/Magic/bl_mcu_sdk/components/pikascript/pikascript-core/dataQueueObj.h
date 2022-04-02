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

#ifndef __DATA_QUEUE_OBJ__H
#define __DATA_QUEUE_OBJ__H
#include "PikaObj.h"
typedef PikaObj QueueObj;

QueueObj* New_queueObj(void);
int32_t queueObj_init(QueueObj* self);

int32_t queueObj_pushInt(QueueObj* self, int val);
int32_t queueObj_pushFloat(QueueObj* self, float val);
int32_t queueObj_pushStr(QueueObj* self, char* str);
int32_t queueObj_pushObj(QueueObj* self, char* className);

int64_t queueObj_popInt(QueueObj* self);
float queueObj_popFloat(QueueObj* self);
char* queueObj_popStr(QueueObj* self);
PikaObj* queueObj_popObj(QueueObj* self);

PikaObj* queueObj_getCurrentObj(QueueObj* self);

#endif
