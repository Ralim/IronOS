/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdLib_SysObj__H
#define __PikaStdLib_SysObj__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdLib_SysObj(Args *args);

void PikaStdLib_SysObj_remove(PikaObj *self, char * argPath);
void PikaStdLib_SysObj_type(PikaObj *self, char * argPath);

#endif
