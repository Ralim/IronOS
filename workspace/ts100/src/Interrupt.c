#include "Interrupt.h"
#include "Bios.h"
#include "I2C.h"

volatile uint32_t system_Ticks;
volatile uint32_t lastKeyPress; //millis() at the last button event
volatile uint16_t keyState; //tracks the button status
volatile uint32_t lastMovement; //millis() at last movement event

//Delay in milliseconds using systemTick
void delayMs(uint32_t ticks) {
	uint32_t endtime = ticks + millis();
	while (millis() < endtime)
		;
}

void NMI_Handler(void) {
	;
}

void HardFault_Handler(void) {
	while (1)
		;
}

void MemManage_Handler(void) {
	while (1)
		;
}

void BusFault_Handler(void) {
	while (1)
		;
}

void UsageFault_Handler(void) {
	while (1)
		;
}

//Handles the tick of the sysTick events
void SysTick_Handler(void) {
	++system_Ticks;
}

/*Peripheral Interrupts				*/

void TIM3_IRQHandler(void) {
	TIM3_ISR();
}

//EXTI IRQ handler
//used for buttons and movement
void EXTI9_5_IRQHandler(void) {
//we are interested in line 9 and line 6 for buttons
	//Lien 5 == movement
	if (EXTI_GetITStatus(EXTI_Line9) != RESET) {
		if (GPIO_ReadInputDataBit(GPIOA, KEY_A) == SET)
			keyState &= ~(BUT_A);
		else
			keyState |= BUT_A;
		lastKeyPress = millis();
		EXTI_ClearITPendingBit(EXTI_Line9);
	} else if (EXTI_GetITStatus(EXTI_Line6) != RESET) {
		if (GPIO_ReadInputDataBit(GPIOA, KEY_B) == SET)
			keyState &= ~(BUT_B);
		else
			keyState |= BUT_B;
		lastKeyPress = millis();
		EXTI_ClearITPendingBit(EXTI_Line6);
	} else if (EXTI_GetITStatus(EXTI_Line5) != RESET) {	//Movement Event
		lastMovement = millis();
		EXTI_ClearITPendingBit(EXTI_Line5);
	}

}

/*********************** UNUSED IRQ *****************************************/
void WWDG_IRQHandler(void) {
}
void PVD_IRQHandler(void) {
}
void TAMPER_IRQHandler(void) {
}
void RTC_IRQHandler(void) {
}
void FLASH_IRQHandler(void) {
}
void RCC_IRQHandler(void) {
}
void EXTI0_IRQHandler(void) {
}
void EXTI1_IRQHandler(void) {
}
void EXTI2_IRQHandler(void) {
}
void EXTI3_IRQHandler(void) {
}
void EXTI4_IRQHandler(void) {
}
void DMA1_Channel1_IRQHandler(void) {
}
void DMA1_Channel2_IRQHandler(void) {
}
void DMA1_Channel3_IRQHandler(void) {
}
void DMA1_Channel4_IRQHandler(void) {
}
void DMA1_Channel5_IRQHandler(void) {
}
void DMA1_Channel6_IRQHandler(void) {
}
void DMA1_Channel7_IRQHandler(void) {
}
void ADC1_2_IRQHandler(void) {
}
void USB_HP_CAN1_TX_IRQHandler(void) {
}
void CAN1_RX1_IRQHandler(void) {
}
void CAN1_SCE_IRQHandler(void) {
}

void TIM1_BRK_IRQHandler(void) {
}
void TIM1_UP_IRQHandler(void) {
}
void TIM1_TRG_COM_IRQHandler(void) {
}
void TIM1_CC_IRQHandler(void) {
}
void TIM4_IRQHandler(void) {
}
void I2C1_EV_IRQHandler(void) {
}
void I2C1_ER_IRQHandler(void) {
}
void I2C2_EV_IRQHandler(void) {
}
void I2C2_ER_IRQHandler(void) {
}
void SPI1_IRQHandler(void) {
}
void SPI2_IRQHandler(void) {
}
void USART1_IRQHandler(void) {
}
void USART2_IRQHandler(void) {
}
void USART3_IRQHandler(void) {
}
void EXTI15_10_IRQHandler(void) {
}
void RTCAlarm_IRQHandler(void) {
}
void USBWakeUp_IRQHandler(void) {
}
void TIM8_BRK_IRQHandler(void) {
}
void TIM8_UP_IRQHandler(void) {
}
void TIM8_TRG_COM_IRQHandler(void) {
}
void TIM8_CC_IRQHandler(void) {
}
void ADC3_IRQHandler(void) {
}
void FSMC_IRQHandler(void) {
}
void SDIO_IRQHandler(void) {
}
void TIM5_IRQHandler(void) {
}
void SPI3_IRQHandler(void) {
}
void UART4_IRQHandler(void) {
}
void UART5_IRQHandler(void) {
}
void TIM6_IRQHandler(void) {
}
void TIM7_IRQHandler(void) {
}
void DMA2_Channel1_IRQHandler(void) {
}
void DMA2_Channel2_IRQHandler(void) {
}
void DMA2_Channel3_IRQHandler(void) {
}
void DMA2_Channel4_5_IRQHandler(void) {
}
void TIM2_IRQHandler(void) {

}
void SVC_Handler(void) {
}

void DebugMon_Handler(void) {
}

void PendSV_Handler(void) {
}

void USB_LP_CAN1_RX0_IRQHandler(void) {

}


