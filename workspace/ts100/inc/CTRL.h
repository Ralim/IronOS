/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
 File Name :      CTRL.h
 Version :        S100 APP Ver 2.11
 Description:
 Author :         Celery
 Data:            2015/07/07
 History:
 2015/07/07   ͳһ������
 *******************************************************************************/
#ifndef _CTRL_H
#define _CTRL_H

#include "stm32f10x.h"
#include "Bios.h"

#define TEMPSHOW_TIMER            gTime[0]/*TEMPSHOW_TIMER*/
#define HEATING_TIMER             gTime[1]/*HEATING_TIMER*/
#define ENTER_WAIT_TIMER          gTime[2]/*ENTER_WAIT_TIMER*/
#define EFFECTIVE_KEY_TIMER       gTime[3]/*EFFECTIVE_KEY_TIMER*/
#define LEAVE_WAIT_TIMER          gTime[4]/*LEAVE_WAIT_TIMER*/
#define G6_TIMER                  gTime[5]/*SWITCH_SHOW_TIMER*/
#define UI_TIMER                  gTime[6]/*UI_TIMER ͼ���������ʱ�����*/
#define KD_TIMER                  gTime[7]/*������ʱ*/
//------------------------------ ��������------------------------------------//
#define   KEY_ST(KEY_PIN)      GPIO_ReadInputDataBit(GPIOA, KEY_PIN)
#define   NO_KEY    0x0	/*NO Keys pressed*/
#define   KEY_V1    0x0100/*V1 key pressed*/
#define   KEY_V2    0x0040/*V2 Key pressed*/
#define   KEY_CN    0X8000/*(Long key press i think)*/
#define   KEY_V3    (KEY_V1|KEY_V2)/*Both Keys pressed*/

typedef enum WORK_STATUS {
	IDLE = 1, //System is idle
	THERMOMETER, //Thermometer mode, basically reads sensor and shows temp
	TEMP_CTR, //Soldering Mode (Temp Controlled)
	WAIT, //System in wait state
	TEMP_SET, //Setting the soldering temp
	CONFIG, //system config mode
	MODE_CNG, //DFU mode i think??
	ALARM, //An alarm has been fired
} WORK_STATUS;

typedef enum WARNING_STATUS {
	NORMAL_TEMP = 1, HIGH_TEMP, SEN_ERR, HIGH_VOLTAGE, LOW_VOLTAGE,
} WARNING_STATUS;

typedef struct {
	u8 ver[16];
	s16 t_standby;          // 200��C=1800  2520,�����¶�
	s16 t_work;             // 350��C=3362, �����¶�
	s16 t_step;             //���ò���
	u32 wait_time;          //3*60*100   3  mintute
	u32 idle_time;          //6*60*100   6 minute
} DEVICE_INFO_SYS;

extern DEVICE_INFO_SYS device_info;
void Set_PrevTemp(s16 Temp);
u8 Get_CtrlStatus(void);
void Set_CtrlStatus(u8 status);
s16 Get_TempVal(void);
u16 Get_HtFlag(void);
void System_Init(void);
void Pid_Init(void);
u16 Pid_Realize(s16 temp);
u32 Heating_Time(s16 temp, s16 wk_temp);
void Status_Tran(void);
#endif
/******************************** END OF FILE *********************************/
