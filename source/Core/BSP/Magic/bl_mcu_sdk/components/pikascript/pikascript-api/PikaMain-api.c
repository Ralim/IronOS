/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#include "PikaMain.h"
#include "PikaStdLib_SysObj.h"
#include "PikaStdLib.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseObj.h"

PikaObj *New_PikaMain(Args *args){
    PikaObj *self = New_PikaStdLib_SysObj(args);
    obj_newObj(self, "PikaStdLib", "PikaStdLib", New_PikaStdLib);
    return self;
}

Arg *PikaMain(PikaObj *self){
    return arg_setMetaObj("", "PikaMain", New_PikaMain);
}
