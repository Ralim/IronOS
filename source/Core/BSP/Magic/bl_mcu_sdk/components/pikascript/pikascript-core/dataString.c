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

#include "dataString.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* strAppendWithSize_unlimited(char* strOut, char* pData, int32_t Size) {
    int32_t strOut_i = strGetSize(strOut);
    for (int32_t i = 0; i < Size; i++) {
        strOut[strOut_i + i] = pData[i];
    }
    strOut_i += Size;
    // add \0 to the end of strOut
    strOut[strOut_i] = 0;

    return strOut;
}

char* strCut(char* strOut, char* strIn, char startSign, char endSign) {
    int32_t Size = strGetSize(strIn);
    int32_t iStart = 0;
    int32_t iEnd = Size;
    uint8_t isStart = 0;
    uint8_t isEnd = 0;
    for (int32_t i = 0; i < Size; i++) {
        if (strIn[i] == startSign) {
            iStart = i;
            isStart = 1;
            break;
        }
    }
    for (int32_t i = Size - 1; i >= 0; i--) {
        if (strIn[i] == endSign) {
            iEnd = i;
            isEnd = 1;
            break;
        }
    }
    int outi = 0;
    for (int32_t i = iStart + 1; i < iEnd; i++) {
        strOut[outi] = strIn[i];
        outi++;
    }
    /* add \0 */
    strOut[outi] = 0;
    if (isStart && isEnd) {
        /* succeed */
        return strOut;
    }
    /* faild */
    return NULL;
}

char* strDeleteChar(char* strOut, char* strIn, char ch) {
    int32_t iOut = 0;
    for (int32_t i = 0; i < strGetSize(strIn); i++) {
        if (ch == strIn[i]) {
            continue;
        }
        strOut[iOut] = strIn[i];
        iOut++;
    }
    /* add \0 */
    strOut[iOut] = 0;
    return strOut;
}

char* strDeleteEnter(char* str) {
    return strDeleteChar(str, str, '\n');
}

char* strAppendWithSize(char* strOut, char* pData, int32_t Size) {
    int32_t strOut_i = strGetSize(strOut);
    for (int32_t i = 0; i < Size; i++) {
        strOut[strOut_i + i] = pData[i];
    }
    strOut_i += Size;
    // add \0 to the end of strOut
    strOut[strOut_i] = 0;

    return strOut;
}

int32_t strCountSign(char* strIn, char sign) {
    int32_t count = 0;
    for (int32_t i = 0; i < strGetSize(strIn); i++) {
        if (sign == strIn[i]) {
            count++;
        }
    }
    return count;
}

int32_t strGetTokenNum(char* strIn, char sign) {
    return strCountSign(strIn, sign) + 1;
}

uint32_t strGetSize(char* pData) {
    return strlen(pData);
}

char* strAppend_unlimited(char* strOut, char* pData) {
    uint32_t Size = 0;
    Size = strGetSize(pData);
    return strAppendWithSize_unlimited(strOut, pData, Size);
}

char* strGetLastLine(char* strOut, char* strIn) {
    int32_t size = strGetSize(strIn);
    char sign = '\n';
    uint32_t beginIndex = 0;

    /* skip the latest '\n' */
    for (int32_t i = size - 2; i > -1; i--) {
        if (strIn[i] == sign) {
            beginIndex = i + 1;
            break;
        }
    }

    memcpy(strOut, strIn + beginIndex, size - beginIndex);
    strOut[size - beginIndex + 1] = 0;
    return strOut;
}

char* strGetLastToken(char* strOut, char* strIn, char sign) {
    int32_t size = strGetSize(strIn);
    int32_t buffSize = 0;
    for (int32_t i = size - 1; i > -1; i--) {
        if (strIn[i] != sign) {
            strOut[size - i - 1] = strIn[i];
            buffSize++;
        }
        if (strIn[i] == sign) {
            break;
        }
    }
    int32_t i = 0;
    for (i = 0; i < buffSize / 2; i++) {
        char buff = strOut[i];
        strOut[i] = strOut[buffSize - i - 1];
        strOut[buffSize - i - 1] = buff;
    }
    strOut[buffSize] = 0;
    return strOut;
}

char* strPopToken(char* strOut, char* strIn, char sign) {
    int32_t getSign = 0;
    int32_t iPoped = 0;
    int32_t iOut = 0;
    int32_t size = strGetSize(strIn);
    int32_t i = 0;
    for (i = 0; i < size; i++) {
        if (getSign) {
            strIn[iPoped] = strIn[i];
            iPoped++;
            continue;
        }
        if (strIn[i] != sign) {
            strOut[iOut++] = strIn[i];
            continue;
        }
        if (strIn[i] == sign) {
            getSign = 1;
            continue;
        }
    }
    strOut[iOut] = 0;
    strIn[iPoped] = 0;
    return strOut;
}

char* strGetFirstToken(char* strOut, char* strIn, char sign) {
    int32_t size = strGetSize(strIn);
    for (int32_t i = 0; i < size; i++) {
        if (strIn[i] != sign) {
            strOut[i] = strIn[i];
        }
        if (strIn[i] == sign) {
            break;
        }
    }
    return strOut;
}

int32_t strGetToken(char* string, char** argv, char sign) {
    int32_t argc = 0;
    int32_t i = 0;
    // arg_i point32_t to the arg operated now
    int32_t arg_i = 0;
    // if not found ' ', then put chars from CMD to argv_tem
    int32_t char_i = 0;
    for (i = 0; (i < strGetSize(string)); i++) {
        if (string[i] != sign) {
            argv[arg_i][char_i] = string[i];
            char_i++;
        }
        if (string[i] == sign) {
            // write '\0' to the end of argv
            argv[arg_i][char_i] = 0;
            arg_i++;
            char_i = 0;
        }
        // write '\0' to the end of last argv
        argv[arg_i][char_i] = 0;
    }
    argc = arg_i + 1;
    return argc;
}

char* strAppend(char* strOut, char* pData) {
    uint32_t Size = 0;
    Size = strGetSize(pData);
    return strAppendWithSize(strOut, pData, Size);
}

int32_t strIsStartWith(char* str, char* strStart) {
    if (NULL == str || NULL == strStart) {
        /* input is null */
        return 0;
    }
    uint32_t size = strGetSize(strStart);
    uint32_t CMDName_get = 1;
    for (int32_t i = 0; i < size; i++) {
        if (str[i] != strStart[i]) {
            CMDName_get = 0;
        }
    }
    return CMDName_get;
}

int32_t strEqu(char* str1, char* str2) {
    if (NULL == str1 || NULL == str2) {
        return 0;
    }
    return !strcmp(str1, str2);
}

char* strRemovePrefix(char* inputStr, char* prefix, char* outputStr) {
    if (!strIsStartWith(inputStr, prefix)) {
        return NULL;
    }

    for (int32_t i = strGetSize(prefix); i < strGetSize(inputStr); i++) {
        outputStr[i - strGetSize(prefix)] = inputStr[i];
    }
    return outputStr;
}

char* strClear(char* str) {
    for (int32_t i = 0; i < sizeof(str); i++) {
        str[i] = 0;
    }
    return str;
}

int32_t strIsContain(char* str, char ch) {
    for (int32_t i = 0; i < strGetSize(str); i++) {
        if (str[i] == ch) {
            return 1;
        }
    }
    return 0;
}

char* strCopy(char* strBuff, char* strIn) {
    memcpy(strBuff, strIn, strGetSize(strIn));
    return strBuff;
}
