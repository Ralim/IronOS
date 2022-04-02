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

#ifndef _arg__H
#define _arg__H
#include "dataMemory.h"

typedef uint8_t Arg;
typedef uint32_t Hash;
typedef enum {
    TYPE_NONE,
    TYPE_OBJECT,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_METHOD,
    TYPE_POINTER,
    TYPE_MATE_OBJECT,
    TYPE_STRING
} ArgType;

uint16_t content_typeOffset(uint8_t* content);
uint16_t content_contentOffset(uint8_t* content);
uint16_t content_sizeOffset(uint8_t* self);
uint16_t content_contentOffset(uint8_t* self);
uint16_t content_nextOffset(uint8_t* self);
uint8_t content_nameOffset(uint8_t* self);

uint32_t content_getNameHash(uint8_t* content);
ArgType content_getType(uint8_t* self);
uint8_t* content_getNext(uint8_t* self);
uint16_t content_getSize(uint8_t* self);
uint8_t* content_getContent(uint8_t* content);

uint16_t content_totleSize(uint8_t* self);

uint8_t* content_init(char* name,
                      ArgType type,
                      uint8_t* content,
                      uint16_t size,
                      uint8_t* next);
uint8_t* content_deinit(uint8_t* self);
uint8_t* content_setName(uint8_t* self, char* name);
uint8_t* content_setType(uint8_t* self, ArgType type);
uint8_t* content_setContent(uint8_t* self, uint8_t* content, uint16_t size);
void content_setNext(uint8_t* self, uint8_t* next);

uint16_t arg_getTotleSize(Arg* self);
void arg_freeContent(Arg* self);

Arg* arg_setName(Arg* self, char* name);
Arg* arg_setContent(Arg* self, uint8_t* content, uint32_t size);
Arg* arg_newContent(Arg* self, uint32_t size);
Arg* arg_setType(Arg* self, ArgType type);
Hash arg_getNameHash(Arg* self);
ArgType arg_getType(Arg* self);
uint8_t* arg_getContent(Arg* self);
uint16_t arg_getContentSize(Arg* self);
Hash hash_time33(char* str);

Arg* arg_setInt(Arg* self, char* name, int64_t val);
Arg* arg_setFloat(Arg* self, char* name, float val);
Arg* arg_setPtr(Arg* self, char* name, ArgType type, void* pointer);
Arg* arg_setStr(Arg* self, char* name, char* string);

int64_t arg_getInt(Arg* self);
float arg_getFloat(Arg* self);
void* arg_getPtr(Arg* self);
char* arg_getStr(Arg* self);
Arg* arg_copy(Arg* argToBeCopy);

Arg* arg_init(Arg* self, void* voidPointer);
void arg_deinit(Arg* self);

Arg* New_arg(void* voidPointer);
#endif
