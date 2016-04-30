/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
File Name :      UI.h
Version :        S100 APP Ver 2.11
Description:
Author :         Celery
Data:            2015/07/07
History:
2015/07/07   Í³Ò»ÃüÃû£»
*******************************************************************************/
#ifndef _UI_H
#define _UI_H

#include "STM32F10x.h"

u32 Get_UpdataFlag(void);
void Set_UpdataFlag(u32 Cont);
void APP_Init(void);
u32 Calculation_TWork(u8 Flag);
void Temp_SetProc(void);
void Display_Temp(u8 x,s16 Temp);
void Show_Notice(void);
void Show_Warning(void);
void Show_MiniTS(void);
void Show_TempDown(s16 Temp,s16 Dst_Temp);
void Set_TemperatureShowFlag(u8 flag);
s16 TemperatureShow_Change(u8 flag ,s16 Tmp);
u8 Get_TemperatureShowFlag(void);
void Show_Ver(u8 ver[],u8 flag);
void Show_Triangle(u8 empty,u8 fill);
void Shift_Char(u8* ptr,u8 pos);
void Show_Set(void);
void Show_OrderChar(u8* ptr,u8 num,u8 width);
u8 Reverse_Bin8(u8 data);
void Show_ReverseChar(u8* ptr,u8 num,u8 width,u8 direction);
u8 Show_TempReverse(u8 num,u8 width,u8 direction);
void Show_HeatingIcon(u32 ht_flag,u16 active);
void Display_Str(u8 x, char* str);
void Display_Str10(u8 x, char* str);
void Clear_Pervious(u16 data);
void Print_Integer(s32 data,u8 posi);
u8 Roll_Num(u16 Step,u8 Flag);
void OLed_Display(void);
void Show_Cal(u8 flag);
void Show_Config(void);
#endif
/******************************** END OF FILE *********************************/
