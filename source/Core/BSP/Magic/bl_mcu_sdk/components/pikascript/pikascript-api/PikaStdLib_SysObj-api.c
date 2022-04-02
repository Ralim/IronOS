/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaStdLib_SysObj.h"
#include "BaseObj.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

void PikaStdLib_SysObj_removeMethod(PikaObj *self, Args *args){
    char * argPath = args_getStr(args, "argPath");
    PikaStdLib_SysObj_remove(self, argPath);
}

void PikaStdLib_SysObj_typeMethod(PikaObj *self, Args *args){
    char * argPath = args_getStr(args, "argPath");
    PikaStdLib_SysObj_type(self, argPath);
}

PikaObj *New_PikaStdLib_SysObj(Args *args){
    PikaObj *self = New_BaseObj(args);
    class_defineMethod(self, "remove(argPath:str)", PikaStdLib_SysObj_removeMethod);
    class_defineMethod(self, "type(argPath:str)", PikaStdLib_SysObj_typeMethod);
    return self;
}

Arg *PikaStdLib_SysObj(PikaObj *self){
    return arg_setMetaObj("", "PikaStdLib_SysObj", New_PikaStdLib_SysObj);
}
