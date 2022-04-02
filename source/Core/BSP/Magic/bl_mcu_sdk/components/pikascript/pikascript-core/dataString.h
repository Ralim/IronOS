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

#ifndef __MY_TEST_TOOLS_H
#define __MY_TEST_TOOLS_H
#include <stdint.h>

/* size */
uint32_t strGetSize(char* pData);
/* append */
char* strAppend(char* strOut, char* pData);
char* strAppend_unlimited(char* strOut, char* pData);
char* strAppendWithSize(char* strOut, char* pData, int32_t Size);
/* cut */
char* strCut(char* strOut, char* strIn, char startSign, char endSign);
/* assert */
int32_t strIsStartWith(char* str, char* strStart);
int32_t strEqu(char* str1, char* str2);
/* delete */
char* strDeleteEnter(char* str);
char* strDeleteChar(char* strOut, char* strIn, char ch);
/* prefix */
char* strRemovePrefix(char* inputStr, char* prefix, char* outputStr);
/* token */
int32_t strGetToken(char* string, char** argv, char sign);
char* strPopToken(char* strOut, char* strIn, char sign);
int32_t strCountSign(char* strIn, char sign);
int32_t strGetTokenNum(char* strIn, char sign);
char* strGetFirstToken(char* strOut, char* strIn, char sign);
char* strGetLastToken(char* strOut, char* strIn, char sign);
char* strClear(char* str);
int32_t strIsContain(char* str, char ch);
char* strCopy(char* strBuff, char* strIn);
char* strGetLastLine(char *strOut, char *strIn);

#endif
