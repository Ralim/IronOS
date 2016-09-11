/********************* (C) COPYRIGHT 2015 e-Design Co.,Ltd. **********************
 File Name :      Bios.c
 Version :        S100 APP Ver 2.11
 Description:
 Author :         Celery
 Data:            2015/07/07
 History:
 2015/07/07   ͳһ������
 *******************************************************************************/

#include <Hardware.h>
#include <usb_lib.h>
#include "APP_Version.h"
#include "Bios.h"
#include "I2C.h"
#include "CTRL.h"

/******************************************************************************/
#define ADC1_DR_Address    ((u32)0x4001244C)

vu8 gSk = 0; //
vu32 gTimeOut, gMs_timeout;
volatile u32 gTime[8];
vu16 ADC1ConvertedValue[2];
vu32 gHeat_cnt = 0;

/*******************************************************************************
 ������: Get_AdcValue
 ��������:��ȡADC ת����Ķ���
 �������:ת�����AD
 ���ز���:NULL
 *******************************************************************************/
u16 Get_AdcValue(u8 i) {
	return ADC1ConvertedValue[i];
}
/*******************************************************************************
 ������: Set_HeatingTime
 ��������:���ü���ʱ��
 �������:heating_time ����ʱ��
 ���ز���:NULL
 *******************************************************************************/
void Set_HeatingTime(u32 heating_time) {
	gHeat_cnt = heating_time;
}
/*******************************************************************************
 ������: Get_HeatingTime
 ��������:��ȡ����ʱ��
 �������:NULL
 ���ز���:����ʱ��
 *******************************************************************************/
u32 Get_HeatingTime(void) {
	return gHeat_cnt;
}
/*******************************************************************************
 ������: Init_GTIME
 ��������:��ʼ����ʱ��
 �������:NULL
 ���ز���:NULL
 *******************************************************************************/
void Init_Gtime(void) {
	u8 i;

	for (i = 0; i < 8; i++)
		gTime[i] = 0;
}

/*******************************************************************************
 ������: Delay_Ms
 ��������:��ʱ����
 �������:��ʱ�ȴ��ĺ�����ֵ
 ���ز���:NULL
 *******************************************************************************/
void Delay_Ms(u32 ms) {
	gMs_timeout = ms * 20;
	while (gMs_timeout)
		; // {if(Scan_key()!=0)break;}
}
/*******************************************************************************
 ������: Delay_HalfMs
 ��������:ÿ��λΪ0.5�������ʱ����
 �������:��ʱ�ȴ���0.5������
 ���ز���:NULL
 *******************************************************************************/
void Delay_HalfMs(u32 ms) {
	gMs_timeout = ms * 10;
	while (gMs_timeout)
		; // {if(Scan_key()!=0)break;}
}

/*******************************************************************************
 ������: USB_Port
 ��������:���� USB �豸 IO �˿�
 �������:State = ENABLE / DISABLE
 ���ز���:NULL
 *******************************************************************************/
void USB_Port(u8 state) {
	USB_DN_LOW();
	USB_DP_LOW();
	if (state == DISABLE) {
		USB_DN_OUT();
		USB_DP_OUT();
	} else {
		USB_DN_EN();
		USB_DP_EN();
	}
}
/*******************************************************************************
 ������: RCC_Config
 ��������:ʱ�ӳ�ʼ��
 �������:NULL
 ���ز���:NULL
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
	RCC_AHBPeriph_FLITF,          // Enable DMA1 clock ???
			ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
	RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2, //| RCC_APB2Periph_ADC3, //RCC_APB2Periph_TIM1,
			ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);

	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_Div1);       // USBCLK = 48MHz
}
/*******************************************************************************
 ������: NVIC_Config
 ��������:�жϳ�ʼ��
 �������:tab_offset
 ���ز���:NULL
 *******************************************************************************/
void NVIC_Config(u16 tab_offset) {
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_SetVectorTable(NVIC_VectTab_FLASH, tab_offset);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
 ������: GPIO_Config
 ��������:����GPIO
 �������:NULL
 ���ز���:NULL
 *******************************************************************************/
void GPIO_Config(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

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

//------ PA7��Ϊģ��ͨ��Ai7�������� ----------------------------------------//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

//------ OLED_RST_PIN(PB9) ------------------------------------------------------------//
	GPIO_InitStructure.GPIO_Pin = OLED_RST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

//------- ���ȿ���λ PB4--------------------------------------------------------//
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE); // PB4=JNTRST

	GPIO_InitStructure.GPIO_Pin = HEAT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

//------ PB0��Ϊģ��ͨ��Ai8�������� ---------------------------------------//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

//---------- INPUT Voltage Detection Pin VB PB1(Ai9) ---------------------------------------//
	GPIO_InitStructure.GPIO_Pin = VB_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

//-------- K1 = PA8, K2 = PA6 ----------------------------------------------------------//
	GPIO_InitStructure.GPIO_Pin = KEY1_PIN | KEY2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/*******************************************************************************
 ������: Ad_Init
 ��������: ��ʼ�� AD
 �������:NULL
 ���ز���:NULL
 *******************************************************************************/
void Ad_Init(void) {
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
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
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
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 2, ADC_SampleTime_239Cycles5); //28 or 55
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5); //28 or 55
	ADC_RegularChannelConfig(ADC2, ADC_Channel_9, 1, ADC_SampleTime_55Cycles5); //28 or 55

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
 ������: Init_Timer2
 ��������: ��ʼ�� ��ʱ��2
 �������:NULL
 ���ز���:NULL
 *******************************************************************************/
void Init_Timer2(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1;      // (48MHz)/48 = 1MHz
	TIM_TimeBaseStructure.TIM_Period = 10000 - 1;    // Interrupt per 10mS
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
 ������: Init_Timer3
 ��������: ��ʼ�� ��ʱ��3
 �������:NULL
 ���ز���:NULL
 *******************************************************************************/
void Init_Timer3(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	TIM_TimeBaseStructure.TIM_Prescaler = 48 - 1;      //(48MHz)/48 = 1MHz
	TIM_TimeBaseStructure.TIM_Period = 50 - 1;      // Interrupt per 50us
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
/*******************************************************************************
 ������: TIM2_ISR
 ��������: ��ʱ��2�жϺ���
 �������:NULL
 ���ز���:NULL
 *******************************************************************************/
void TIM2_ISR(void) {
	u8 i;

	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);       // Clear interrupt flag
	for (i = 0; i < 8; i++)
		if (gTime[i] > 0)
			gTime[i]--;

	if (++gSk % 4 == 0)
		Scan_Key();
}
/*******************************************************************************
 Function: TIM3_ISR
 Description:Sets the output pin as appropriate
 If the Heat_cnt >0 then heater on, otherwise off.
 *******************************************************************************/
void TIM3_ISR(void) {
	static u8 heat_flag = 0;

	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);       // Clear interrupt flag

	if (gTimeOut > 0)
		gTimeOut--;
	if (gMs_timeout > 0)
		gMs_timeout--;

	if (gHeat_cnt > 0) {
		gHeat_cnt--;
		if (heat_flag)
			HEAT_OFF();
		else
			HEAT_ON();

		heat_flag = ~heat_flag;
	}
	if (gHeat_cnt == 0) {
		HEAT_OFF();
		heat_flag = 0;
	}
}
/******************************** END OF FILE *********************************/
