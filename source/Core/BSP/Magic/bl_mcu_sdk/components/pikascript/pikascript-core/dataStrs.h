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

#ifndef __STR_ARGS__H
#define __STR_ARGS__H
#include "dataArgs.h"
Args* New_strBuff(void);
char* strsGetFirstToken(Args* buffs, char* strIn, char sign);
char* strsGetLastToken(Args* buffs, char* arg_Path, char sign);
char* strsPopToken(Args* buffs, char* tokens, char sign);
char* strsCopy(Args* buffs, char* source);
char* strsDeleteChar(Args* buff, char* strIn, char ch);
char* strsCut(Args* buffs, char* strIn, char startSign, char endSign);
char* strsRemovePrefix(Args* buffs, char* inputStr, char* prefix);
char* strsAppend(Args* buffs, char* strOrigin, char* strAppend);
char* strsFormat(Args* buffs, uint16_t buffSize, const char* fmt, ...);
char* strsGetDirectStr(Args* buffs, char* argPath);
#endif
