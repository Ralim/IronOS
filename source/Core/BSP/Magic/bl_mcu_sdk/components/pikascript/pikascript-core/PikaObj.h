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

#ifndef _Process__H
#define _Process__H
#include "dataArgs.h"
#include "dataLink.h"
#include "dataMemory.h"
typedef struct PikaObj_t PikaObj;
typedef PikaObj* (*NewFun)(Args* args);
typedef PikaObj* (*InitFun)(PikaObj* self, Args* args);
typedef PikaObj Parameters;
typedef void (*Method)(PikaObj* self, Args* args);
struct PikaObj_t {
    /* list */
    Args* list;
};

/* operation */
int32_t obj_deinit(PikaObj* self);
int32_t obj_init(PikaObj* self, Args* args);
int32_t obj_update(PikaObj* self);
int32_t obj_enable(PikaObj* self);
int32_t obj_disable(PikaObj* self);

// arg type operations
int32_t obj_setInt(PikaObj* self, char* argPath, int64_t val);
int32_t obj_setPtr(PikaObj* self, char* argPath, void* pointer);
int32_t obj_setFloat(PikaObj* self, char* argPath, float value);
int32_t obj_setStr(PikaObj* self, char* argPath, char* str);
int32_t obj_setArg(PikaObj* self, char* argPath, Arg* arg);

void* obj_getPtr(PikaObj* self, char* argPath);
float obj_getFloat(PikaObj* self, char* argPath);
char* obj_getStr(PikaObj* self, char* argPath);
int64_t obj_getInt(PikaObj* self, char* argPath);
Arg* obj_getArg(PikaObj* self, char* argPath);

// arg general operations
int32_t obj_bind(PikaObj* self, char* type, char* name, void* pointer);
int32_t obj_set(PikaObj* self, char* name, char* valStr);

int32_t obj_bindInt(PikaObj* self, char* name, int32_t* valPtr);
int32_t obj_bindFloat(PikaObj* self, char* name, float* valPtr);
int32_t obj_bindString(PikaObj* self, char* name, char** valPtr);

char* obj_print(PikaObj* self, char* name);

// args operations
int32_t obj_load(PikaObj* self, Args* args, char* name);

// subObject
int32_t obj_addOther(PikaObj* self, char* subObjectName, void* new_projcetFun);
int32_t obj_setObjWithoutClass(PikaObj* self,
                               char* subObjectName,
                               void* new_projcetFun);
PikaObj* obj_getObjDirect(PikaObj* self, char* name);
PikaObj* obj_getObj(PikaObj* self, char* processPathectory, int32_t deepth);

// subProcess
int32_t obj_freeObj(PikaObj* self, char* subObjectName);

/* method */
int32_t class_defineMethod(PikaObj* self, char* declearation, Method methodPtr);

int32_t obj_removeArg(PikaObj* self, char* argPath);
int32_t obj_isArgExist(PikaObj* self, char* argPath);
PikaObj* obj_getClassObjByNewFun(PikaObj* self, char* name, NewFun newClassFun);
PikaObj* newRootObj(char* name, NewFun newObjFun);
PikaObj* obj_getClassObj(PikaObj* obj);
Arg* obj_getMethod(PikaObj* obj, char* methodPath);

void obj_setErrorCode(PikaObj* self, int32_t errCode);
int32_t obj_getErrorCode(PikaObj* self);
void obj_setSysOut(PikaObj* self, char* str);
char* args_getSysOut(Args* args);
void args_setErrorCode(Args* args, int32_t errCode);
int32_t args_getErrorCode(Args* args);
void args_setSysOut(Args* args, char* str);
char* obj_getSysOut(PikaObj* self);
void obj_sysPrintf(PikaObj* self, char* fmt, ...);
uint8_t obj_getRefArg(PikaObj* self,
                      char* targetArgName,
                      char* sourceArgPath,
                      Args* targetArgs);
uint8_t obj_getAnyArg(PikaObj* self,
                      char* targetArgName,
                      char* sourceArgPath,
                      Args* targetArgs);

void method_returnStr(Args* args, char* val);
void method_returnInt(Args* args, int32_t val);
void method_returnFloat(Args* args, float val);
void method_returnPtr(Args* args, void* val);
int32_t method_getInt(Args* args, char* argName);
float method_getFloat(Args* args, char* argName);
char* method_getStr(Args* args, char* argName);
void method_returnArg(Args* args, Arg* arg);
char* methodArg_getDec(Arg* method_arg);
void* methodArg_getPtr(Arg* method_arg);

void obj_runNoRes(PikaObj* slef, char* cmd);
void obj_run(PikaObj* self, char* cmd);
Parameters* obj_runDirect(PikaObj* self, char* cmd);
PikaObj* New_PikaObj(void);

/* tools */
int fast_atoi(char* src);
char* fast_itoa(char* buf, uint32_t val);

#endif
