/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
File Name :      CTRL.h
Version :        S100 APP Ver 2.11   
Description:
Author :         Celery
Data:            2015/07/07
History:
2015/07/07   Í³Ò»ÃüÃû£»
*******************************************************************************/
#ifndef _HARDWARE_H
#define _HARDWARE_H

#include "STM32F10x.h"

#define SI_COE 8//56
#define SI_THRESHOLD   60

typedef enum VOL_CLASS {
    H_ALARM = 0,
    VOL_24,
    VOL_19,
    VOL_12,
    VOL_5,
    L_ALARM,
} VOL_CLASS;

extern s32 gZerop_ad;
extern u8 gCalib_flag;
extern u32 gTurn_offv;

u32 Get_gKey(void);
void Set_gKey(u32 key);
void Set_LongKeyFlag(u32 flag);
void Zero_Calibration(void);
int Read_Vb(u8 flag);
void Scan_Key(void);
u32 Get_SlAvg(u32 avg_data);
int Get_TempSlAvg(int avg_data);
u32 Get_AvgAd(void);
int Get_SensorTmp(void);
u16 Get_ThermometerTemp(void);
s16 Get_Temp(s16 wk_temp);
u32 Clear_Watchdog(void);
u32 Start_Watchdog(u32 ms);
u8 Get_AlarmType(void);
void Set_AlarmType(u8 type);
u32 Get_CalFlag(void);
#endif
/******************************** END OF FILE *********************************/
