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

#include "dataStack.h"
#include "dataQueue.h"

Stack* New_Stack(void) {
    Args* args = New_args(NULL);
    args_setInt(args, "top", 0);
    Stack* stack = args;
    return stack;
}
int32_t stack_deinit(Stack* stack) {
    Args* args = stack;
    args_deinit(args);
    return 0;
}
int32_t stack_pushStr(Stack* stack, char* str) {
    Args* args = stack;
    uint64_t top = args_getInt(args, "top");
    char buff[11];
    /* add top */
    char* topStr = fast_itoa(buff, top);
    args_setInt(args, "top", top + 1);
    return args_setStr(args, topStr, str);
}
char* stack_popStr(Stack* queue, char* outBuff) {
    Args* args = queue;
    uint64_t top = args_getInt(args, "top") - 1;
    char buff[11];
    /* sub top */
    args_setInt(args, "top", top);
    char* topStr = fast_itoa(buff, top);
    strcpy(outBuff, args_getStr(args, topStr));
    args_removeArg(args, args_getArg(args, topStr));
    return outBuff;
}
