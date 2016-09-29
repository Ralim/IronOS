/*
 * Setup functions for the basic hardware present in the system
 */
#ifndef __BIOS_H
#define __BIOS_H

#include "stm32f10x.h"
#include "S100V0_1.h"/*For pin definitions*/
#include "Analog.h"/*So that we can attach the DMA to the output array*/
#include "stm32f10x_flash.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_iwdg.h"
#include "misc.h"
extern volatile uint32_t gHeat_cnt;

inline void setIronTimer(uint32_t time) {
	gHeat_cnt = time;
}
inline uint32_t getIronTimer() {
	return gHeat_cnt;
}
/*Get set the remaining toggles of the heater output*/
u32 Get_HeatingTime(void);
void Set_HeatingTime(u32 heating_time);

void Init_Gtime(void);
void USB_Port(u8 state);
void NVIC_Config(u16 tab_offset);
void RCC_Config(void);
void GPIO_Config(void);
void Adc_Init(void);
void Init_Timer3(void);
/*Interrupts*/
void TIM3_ISR(void);
void Init_EXTI(void);
/*Watchdog*/
void Start_Watchdog(uint32_t ms);
void Clear_Watchdog(void);
#endif

