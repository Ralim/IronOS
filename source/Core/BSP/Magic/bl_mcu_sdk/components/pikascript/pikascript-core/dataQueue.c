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

#include "dataQueue.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dataArgs.h"



Queue* New_queue(void) {
    Args* args = New_args(NULL);
    args_setInt(args, "top", 0);
    args_setInt(args, "bottom", 0);
    Queue* queue = args;
    return queue;
}

int32_t queue_deinit(Queue* queue) {
    Args* args = queue;
    args_deinit(args);
    return 0;
}

int32_t queue_pushInt(Queue* queue, int val) {
    Args* args = queue;
    uint64_t top = args_getInt(args, "top");
    char buff[11];
    char* topStr = fast_itoa(buff, top);
    /* add top */
    args_setInt(args, "top", top + 1);
    return args_setInt(args, topStr, val);
}

int64_t queue_popInt(Queue* queue) {
    Args* args = queue;
    uint64_t bottom = args_getInt(args, "bottom");
    char buff[11];
    char* bottomStr = fast_itoa(buff, bottom);
    /* add bottom */
    args_setInt(args, "bottom", bottom + 1);
    int64_t res = args_getInt(args, bottomStr);
    args_removeArg(args, args_getArg(args, bottomStr));
    return res;
}

int32_t queue_pushFloat(Queue* queue, float val) {
    Args* args = queue;
    uint64_t top = args_getInt(args, "top");
    char buff[11];
    char* topStr = fast_itoa(buff, top);
    /* add top */
    args_setInt(args, "top", top + 1);
    return args_setFloat(args, topStr, val);
}

float queue_popFloat(Queue* queue) {
    Args* args = queue;
    uint64_t bottom = args_getInt(args, "bottom");
    char buff[11];
    char* bottomStr = fast_itoa(buff, bottom);
    /* add bottom */
    args_setInt(args, "bottom", bottom + 1);
    float res = args_getFloat(args, bottomStr);
    args_removeArg(args, args_getArg(args, bottomStr));
    return res;
}

int32_t queue_pushStr(Queue* queue, char* str) {
    Args* args = queue;
    uint64_t top = args_getInt(args, "top");
    char buff[11];
    /* add top */
    char* topStr = fast_itoa(buff, top);
    args_setInt(args, "top", top + 1);
    return args_setStr(args, topStr, str);
}

char* queue_popStr(Queue* queue) {
    Args* args = queue;
    uint64_t bottom = args_getInt(args, "bottom");
    char buff[11];
    /* add bottom */
    args_setInt(args, "bottom", bottom + 1);
    return args_getStr(args, fast_itoa(buff, bottom));
}

int32_t queue_pushArg(Queue* queue, Arg* arg) {
    Args* args = queue;
    uint64_t top = args_getInt(args, "top");
    /* add top */
    args_setInt(args, "top", top + 1);
    char buff[11];
    arg = arg_setName(arg, fast_itoa(buff, top));
    return args_setArg(args, arg);
}

Arg* queue_popArg(Queue* queue) {
    Args* args = queue;
    uint64_t bottom = args_getInt(args, "bottom");
    /* add bottom */
    args_setInt(args, "bottom", bottom + 1);
    char buff[11];
    Arg* res = args_getArg(args, fast_itoa(buff, bottom));
    return res;
}
