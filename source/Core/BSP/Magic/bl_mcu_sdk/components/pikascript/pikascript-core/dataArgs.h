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

#ifndef _dataArgs__H
#define _dataArgs__H
#include "dataArg.h"
#include "dataLink.h"
#include "dataMemory.h"
#include "dataString.h"

typedef Link Args;

/* operation */
void args_deinit(Args* self);
void args_init(Args* self, Args* args);
int32_t args_getSize(Args* self);
LinkNode* args_getNode(Args* self, char* name);

Arg* args_getArgByIndex(Args* self, int32_t index);
Arg* args_getArg(Args* self, char* name);
int32_t args_removeArg(Args* self, Arg* argNow);
Arg* args_getArg_hash(Args* self, Hash nameHash);

int32_t args_setArg(Args* self, Arg* arg);

int32_t args_copyArgByName(Args* self, char* name, Args* directList);
int32_t args_copyArg(Args* self, Arg* argToBeCopy);

ArgType args_getType(Args* self, char* name);
int32_t args_isArgExist_hash(Args* self, Hash nameHash);
int32_t args_isArgExist(Args* self, char* name);

int32_t args_setStr(Args* self, char* name, char* strIn);
int32_t args_setStrWithDefaultName(Args* self, char* strIn);
char* args_getStr(Args* self, char* name);

int32_t args_setFloatWithDefaultName(Args* self, float argFloat);
int32_t args_setFloat(Args* self, char* name, float argFloat);
float args_getFloat(Args* self, char* name);

int32_t args_setPtr(Args* self, char* name, void* argPointer);
void* args_getPtr(Args* self, char* name);

int32_t args_setInt(Args* self, char* name, int64_t int64In);
int64_t args_getInt(Args* self, char* name);

void args_bindInt(Args* self, char* name, int32_t* intPtr);
void args_bindFloat(Args* self, char* name, float* floatPtr);
void args_bindStr(Args* self, char* name, char** stringPtr);
/* arg general opeartion */
void args_bind(Args* self, char* type, char* name, void* pointer);
char* args_print(Args* self, char* name);

int32_t args_setStruct(Args* self,
                       char* name,
                       void* struct_ptr,
                       uint32_t struct_size);
void* args_getStruct(Args* self, char* name, void* struct_out);

int32_t args_set(Args* self, char* name, char* valueStr);
int32_t args_setObjectWithClass(Args* self,
                                char* objectName,
                                char* className,
                                void* objectPtr);
int32_t args_setPtrWithType(Args* self,
                            char* name,
                            ArgType type,
                            void* objPtr);
int32_t args_foreach(Args* self,
                     int32_t (*eachHandle)(Arg* argEach, Args* handleArgs),
                     Args* handleArgs);

char* args_getBuff(Args* self, int32_t size);
uint8_t args_setLiteral(Args* self, char* targetArgName, char* literal);

Args* New_args(Args* args);
#endif
