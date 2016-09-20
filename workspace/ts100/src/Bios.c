/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
 File Name :      Bios.c
 Version :        S100 APP Ver 2.11
 Description:
 Author :         Celery
 Data:            2015/07/07
 History:
 2015/07/07   ͳһ������
 *******************************************************************************/

#include "APP_Version.h"
#include "Bios.h"
#include "I2C.h"

/******************************************************************************/
#define ADC1_DR_Address    ((u32)0x4001244C)
volatile uint32_t gHeat_cnt = 0;

/*******************************************************************************
 Function:RCC_Config
 Description:Setup the system clocks to use internal HSE to run the system at 48Mhz
 *******************************************************************************/
void RCC_Config(void) {
	RCC_DeInit();
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	FLASH_SetLatency(FLASH_Latency_1);   // Flash 1 wait state for 48MHz
	RCC_CFGR_CFG()
	;
	RCC_PLL_EN()
	;
	RCC_HSICmd(ENABLE);
	RCC_PLLCmd(ENABLE);
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {
	}
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while (RCC_GetSYSCLKSource() != 0x08) {
	}

	RCC_AHBPeriphClockCmd(
	RCC_AHBPeriph_SRAM | RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2 |
	RCC_AHBPeriph_FLITF, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
	RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);

	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_Div1);       // USBCLK = 48MHz

	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000); //Enable the systick timer
}
/*******************************************************************************
 Function: NVIC_Config
 Description: Configures the NVIC table in the hardware
 Input: (tab_offset) the table offset for the NVIC
 *******************************************************************************/
void NVIC_Config(u16 tab_offset) {
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_SetVectorTable(NVIC_VectTab_FLASH, tab_offset);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);

}
/*******************************************************************************
 Function:GPIO_Config
 Description: Configures all the GPIO into required states
 *******************************************************************************/
void GPIO_Config(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // GPIOB & AFIO

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	GPIOA_OUTPUT()
	;
	GPIOA_L_DEF()
	;
	GPIOA_H_DEF()
	;

	GPIOB_OUTPUT()
	;
	GPIOB_L_DEF()
	;
	GPIOB_H_DEF()
	;

//------ PA7 TMP36 Analog input ----------------------------------------//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

//------ OLED_RST_PIN(PB9) ------------------------------------------------------------//
	GPIO_InitStructure.GPIO_Pin = OLED_RST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

//------- Heat_Pin - Iron enable output PB4--------------------------------------------------------//

	GPIO_InitStructure.GPIO_Pin = HEAT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

//------ PB0 Iron temp input---------------------------------------//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

//---------- INPUT Voltage Detection Pin VB PB1(Ai9) ---------------------------------------//
	GPIO_InitStructure.GPIO_Pin = VB_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

//-------- K1 = PA9, K2 = PA6 ----------------------------------------------------------//
	GPIO_InitStructure.GPIO_Pin = KEY1_PIN | KEY2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/*******************************************************************************
 Function: Adc_Init
 Description:Enable the ADC's and setup the DMA as well to automatically read them to system ram.
 *******************************************************************************/
void Adc_Init(void) {
	u32 timeout = 10 * 0x1000;
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	/* DMA1 channel1 configuration ---------------------------------------------*/
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) ADC1ConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 2;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize =
	DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	/* Enable DMA1 channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	// ADC1 configuration ------------------------------------------------------//
	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 2;
	ADC_Init(ADC1, &ADC_InitStructure);

	// ADC2 configuration ------------------------------------------------------//
	ADC_DeInit(ADC2);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC2, &ADC_InitStructure);

	// ADC1,2 regular channel7  channel9 and channel8 configuration ----------//
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 2,
	ADC_SampleTime_239Cycles5); //28 or 55
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1,
	ADC_SampleTime_239Cycles5); //28 or 55
	ADC_RegularChannelConfig(ADC2, ADC_Channel_9, 1,
	ADC_SampleTime_55Cycles5); //28 or 55

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	ADC_Cmd(ADC1, ENABLE); /* Enable ADC1 */
	ADC_Cmd(ADC2, ENABLE); /* Enable ADC2 */

	ADC_ResetCalibration(ADC1); /* Enable ADC1 reset calibaration register */
	while (ADC_GetResetCalibrationStatus(ADC1))
		if (!timeout--)
			return; /* Check the end of ADC1 reset calibration register */

	ADC_ResetCalibration(ADC2); /* Enable ADC2 reset calibaration register */
	timeout = 10 * 0x1000;
	while (ADC_GetResetCalibrationStatus(ADC2))
		if (!timeout--)
			return; /* Check the end of ADC2 reset calibration register */

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);
}

/*******************************************************************************
 Function:
 Description: Setup Timer2 to fire every 10ms
 *******************************************************************************/
void Init_Timer2(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1;   // (48MHz)/48 = 1MHz
	TIM_TimeBaseStructure.TIM_Period = 10000 - 1;  // Interrupt per 10mS
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
 Function:
 Description: Init Timer3 to fire every 50us to be used to control the irons software PWM
 This needs to be really fast as there is a cap used between this and the driver circuitry
 That prevents astuck mcu heating the tip
 *******************************************************************************/
void Init_Timer3(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1;    //(48MHz)/48 = 1MHz
	TIM_TimeBaseStructure.TIM_Period = 50 - 1;     // Interrupt per 50us
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV2;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM3, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
//We want to enable the EXTI IRQ for the two buttons on PA6 and PA9
void Init_EXTI(void) {
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,
	GPIO_PinSource6 | GPIO_PinSource9);

	/* Configure EXTI0 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line6 | EXTI_Line9;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //trigger on up and down
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}
/*******************************************************************************
 Function:Start_Watchdog
 Description: Starts the system watchdog timer
 *******************************************************************************/
u32 Start_Watchdog(u32 ms) {
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	/* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz (min:0.8ms -- max:3276.8ms */
	IWDG_SetPrescaler(IWDG_Prescaler_32);

	/* Set counter reload value to XXms */
	IWDG_SetReload(ms * 10 / 8);

	/* Reload IWDG counter */
	IWDG_ReloadCounter();

	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	IWDG_Enable();
	return 1;
}
/*******************************************************************************
 Function:Clear_Watchdog
 Description:Resets the watchdog timer
 *******************************************************************************/
void Clear_Watchdog(void) {
	IWDG_ReloadCounter();

}
/*******************************************************************************
 Function:TIM2_ISR
 Description:Handles Timer 2 tick. (10mS)
 Automatically decrements all >0 values in gTime.
 Also reads the buttons every 4 ticks
 *******************************************************************************/
void TIM2_ISR(void) {

	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  // Clear interrupt flag

}
/*******************************************************************************
 Function: TIM3_ISR
 Description:Sets the output pin as appropriate
 If the Heat_cnt >0 then heater on, otherwise off.
 *******************************************************************************/
void TIM3_ISR(void) {
	volatile static u8 heat_flag = 0; //heat flag == used to make the pin toggle

	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  // Clear interrupt flag

	if (gHeat_cnt > 0) {
		--gHeat_cnt;
		if (heat_flag)
			HEAT_OFF();
		else
			HEAT_ON();
		heat_flag = !heat_flag;
	} else {
		HEAT_OFF();
		heat_flag = 0;
	}

}
/******************************** END OF FILE *********************************/
