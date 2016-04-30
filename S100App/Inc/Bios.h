/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. ********************                       
File Name :      Bios.h
Version :        S100 APP Ver 2.11   
Description:
Author :         bure & Celery
Data:            2015/08/03
History:
2015/08/03   统一命名；
*******************************************************************************/

#ifndef __BIOS_H
#define __BIOS_H

#include "STM32F10x.h"

extern volatile u32 gTime[];

#define USB_DN_OUT()    GPIOA->CRH = (GPIOA->CRH & 0xFFFF3FFF) | 0x00003000
#define USB_DP_OUT()    GPIOA->CRH = (GPIOA->CRH & 0xFFF3FFFF) | 0x00030000

#define USB_DN_EN()     GPIOA->CRH = (GPIOA->CRH & 0xFFFFBFFF) | 0x0000B000
#define USB_DP_EN()     GPIOA->CRH = (GPIOA->CRH & 0xFFFBFFFF) | 0x000B0000

#define USB_DP_PD()     GPIOA->CRH = (GPIOA->CRH & 0xFFF3FFFF) | 0x00030000

#define USB_DN_HIGH()   GPIOA->BSRR  = GPIO_Pin_11
#define USB_DP_HIGH()   GPIOA->BSRR  = GPIO_Pin_12

#define USB_DN_LOW()    GPIOA->BRR  = GPIO_Pin_11
#define USB_DP_LOW()    GPIOA->BRR  = GPIO_Pin_12

#define LOW		0
#define HIGH		1

#define BLINK           1        // Bit0 : 0/1 显示/闪烁状态标志
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
void Ad_Init(void);
void Init_Timer2(void);
void Init_Timer3(void);
void TIM2_ISR(void);
void TIM3_ISR(void);
#endif
/********************************* END OF FILE ********************************/
