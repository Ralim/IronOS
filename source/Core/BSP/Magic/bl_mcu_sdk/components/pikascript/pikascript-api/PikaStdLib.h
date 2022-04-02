/* ******************************** */
/* Warning! Don't modify this file! */
/* ******************************** */
#ifndef __PikaStdLib__H
#define __PikaStdLib__H
#include <stdio.h>
#include <stdlib.h>
#include "PikaObj.h"

PikaObj *New_PikaStdLib(Args *args);

Arg * PikaStdLib_MemChecker(PikaObj *self);
Arg * PikaStdLib_SysObj(PikaObj *self);

#endif
