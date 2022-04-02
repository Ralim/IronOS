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

#include "dataStrs.h"
#include <stdarg.h>
#include <stdio.h>
#include "dataString.h"

Args* New_strBuff(void) {
    return New_args(NULL);
}

char* strsRemovePrefix(Args* buffs, char* inputStr, char* prefix) {
    int32_t size = strGetSize(inputStr);
    char* buff = args_getBuff(buffs, size);
    return strRemovePrefix(inputStr, prefix, buff);
}

char* strsGetDirectStr(Args* buffs, char* argPath) {
    char* directStr = NULL;
    directStr = strsCut(buffs, argPath, '"', '"');
    if (NULL != directStr) {
        return directStr;
    }
    directStr = strsCut(buffs, argPath, '\'', '\'');
    if (NULL != directStr) {
        return directStr;
    }
    return NULL;
}

char* strsAppend(Args* buffs, char* strOrigin, char* strToAppend) {
    int32_t size = strGetSize(strOrigin) + strGetSize(strToAppend);
    char* buff = args_getBuff(buffs, size);
    char* strOut = strCopy(buff, strOrigin);
    strAppend(strOut, strToAppend);
    return strOut;
}

char* strsGetLastToken(Args* buffs, char* argPath, char sign) {
    int32_t size = strGetSize(argPath);
    char* buff = args_getBuff(buffs, size);
    return strGetLastToken(buff, argPath, sign);
}

char* strsCut(Args* buffs, char* strIn, char startSign, char endSign) {
    int32_t size = strGetSize(strIn);
    char* buff = args_getBuff(buffs, size);
    return strCut(buff, strIn, startSign, endSign);
}

char* strsDeleteChar(Args* buffs, char* strIn, char ch) {
    int32_t size = strGetSize(strIn);
    return strDeleteChar(args_getBuff(buffs, size), strIn, ch);
}

static uint32_t getSizeOfFirstToken(char *str, char sign){
    uint32_t size = strGetSize(str);
    for( uint32_t i = 0; i < size ;i ++){
        if (str[i] == sign){
            return i;
        }
    }
    return size;
}

char* strsGetFirstToken(Args* buffs, char* strIn, char sign) {
    int32_t size = getSizeOfFirstToken(strIn, sign);
    return strGetFirstToken(args_getBuff(buffs, size), strIn, sign);
}

char* strsPopToken(Args* buffs, char* tokens, char sign) {
    int32_t size = strGetSize(tokens);
    char* buff = args_getBuff(buffs, size);
    return strPopToken(buff, tokens, sign);
}

char* strsCopy(Args* buffs, char* source) {
    int32_t size = strGetSize(source);
    char* buff = args_getBuff(buffs, size);
    return strCopy(buff, source);
}

char* strsFormat(Args* buffs, uint16_t buffSize, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char* res = args_getBuff(buffs, buffSize);
    vsnprintf(res, buffSize, fmt, args);
    va_end(args);
    return res;
}

