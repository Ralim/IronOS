/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. ********************                       
 File Name :      Bios.h
 Version :        S100 APP Ver 2.11
 Description:
 Author :         bure & Celery
 Data:            2015/08/03
 History:
 2015/08/03   ͳһ������
 *******************************************************************************/

#ifndef __BIOS_H
#define __BIOS_H

#include "stm32f10x.h"
#include "S100V0_1.h"
#include "Analog.h"
extern volatile u32 gTime[];
extern volatile uint32_t gHeat_cnt;
inline void setIronTimer(uint32_t time) {
	gHeat_cnt = time;
}


#define LOW		0
#define HIGH		1

#define BLINK           1        // Bit0 : 0/1 ��ʾ/��˸״̬��־
#define WAIT_TIMES      100000

#define SECTOR_SIZE     512
#define SECTOR_CNT      4096
#define HEAT_T          200

u32 Get_HeatingTime(void);
void Set_HeatingTime(u32 heating_time);
u16 Get_AdcValue(u8 i);
void Init_Gtime(void);
void Delay_Ms(u32 ms);
void Delay_HalfMs(u32 ms);
void USB_Port(u8 state);
void NVIC_Config(u16 tab_offset);
void RCC_Config(void);
void GPIO_Config(void);
void Adc_Init(void);
void Init_Timer2(void);
void Init_Timer3(void);
void TIM2_ISR(void);
void TIM3_ISR(void);
#endif
/********************************* END OF FILE ********************************/
