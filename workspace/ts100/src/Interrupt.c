#include "Interrupt.h"
#include "Bios.h"
#include "I2C.h"

volatile uint32_t system_Ticks;
volatile uint32_t BkeyChange; //millis() at the last button event
volatile uint32_t AkeyChange;
volatile uint8_t rawKeys;
volatile uint8_t LongKeys;
volatile uint32_t lastMovement; //millis() at last movement event
volatile uint8_t RotationChangedFlag;
volatile uint8_t InterruptMask;

//Delay in milliseconds using systemTick
void delayMs(uint32_t ticks) {
	uint32_t endtime = ticks + millis();
	while (millis() < endtime)
		;
}
uint32_t getLastButtonPress() {
	if (BkeyChange > AkeyChange)
		return BkeyChange;
	return AkeyChange;
}
uint8_t getButtons() {
	//We want to check the times for the lat buttons & also the rawKeys state
	//If a key has just gone down, rawKeys & KEY ==1
	//This is effectively just a big switch statement to handle a small delay on triggering to stop bounce, and also handle key repeat
	uint8_t out = 0;
	//Special handling case for two keys down at once
	//This is here to allow for people who do not manage to get both keys down within 50ms of each other.
	if ((rawKeys & (BUT_A | BUT_B)) == (BUT_A | BUT_B)) {
		//both keys are probably down
		if (millis() - AkeyChange > 50)
			if (millis() - BkeyChange > 50) {
				out = (BUT_A | BUT_B);
				AkeyChange = millis();
				BkeyChange = millis();
				rawKeys = 0;
			}
		LongKeys = 0;
	} else {
		if (millis() - AkeyChange > 80) {
			if (LongKeys & BUT_A) {
				if (rawKeys & BUT_A) {
					if (millis() - AkeyChange > 800) {
						out |= BUT_A;
						AkeyChange = millis();
						LongKeys &= ~BUT_A;

						LongKeys |= (BUT_A << 2);
					}
				} else {
					LongKeys &= ~BUT_A;
					LongKeys &= ~(BUT_A << 2);
				}

			} else if (LongKeys & (BUT_A << 2)) {
				if (rawKeys & BUT_A) {
					if (millis() - AkeyChange > 100) {
						out |= BUT_A;
						AkeyChange = millis();
					}
				} else {
					LongKeys &= ~BUT_A;
					LongKeys &= ~(BUT_A << 2);
				}
			} else {
				if (rawKeys & BUT_A) {
					//The key is down
					out |= BUT_A;
					LongKeys |= BUT_A;
				} else {
					//The key has been lifted
					LongKeys &= ~BUT_A;
					LongKeys &= ~(BUT_A << 2);
				}
			}
		}
		if (millis() - BkeyChange > 80) {
			if (LongKeys & BUT_B) {
				if (rawKeys & BUT_B) {
					if (millis() - BkeyChange > 800) {
						out |= BUT_B;
						BkeyChange = millis();
						LongKeys |= (BUT_B << 2);
						LongKeys &= ~BUT_B;
					}
				} else {
					LongKeys &= ~BUT_B;
					LongKeys &= ~(BUT_B << 2);
				}
			} else if (LongKeys & (BUT_B << 2)) {
				if (rawKeys & BUT_B) {
					if (millis() - BkeyChange > 100) {
						out |= BUT_B;
						BkeyChange = millis();
					}
				} else {
					LongKeys &= ~BUT_B;
					LongKeys &= ~(BUT_B << 2);
				}
			} else {
				if (rawKeys & BUT_B) {
					//The key is down
					out |= BUT_B;
					LongKeys |= BUT_B;
				} else {
					//The key has been lifted
					LongKeys &= ~BUT_B;
					LongKeys &= ~(BUT_B << 2);
				}
			}
		}
	}
	return out;
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
	//Line 5 == movement
	if (EXTI_GetITStatus(EXTI_Line9) != RESET) {
		if (GPIO_ReadInputDataBit(GPIOA, KEY_A) == SET) {
			rawKeys &= ~BUT_A;
		} else {
			rawKeys |= BUT_A;
		}
		AkeyChange = millis();
		EXTI_ClearITPendingBit(EXTI_Line9);
	} else if (EXTI_GetITStatus(EXTI_Line6) != RESET) {
		if (GPIO_ReadInputDataBit(GPIOA, KEY_B) == SET) {
			rawKeys &= ~BUT_B;
		} else {
			rawKeys |= BUT_B;
		}
		BkeyChange = millis();
		EXTI_ClearITPendingBit(EXTI_Line6);
	} else if (EXTI_GetITStatus(EXTI_Line5) != RESET) {	//Movement Event
		if (!InterruptMask)
			lastMovement = millis();
		EXTI_ClearITPendingBit(EXTI_Line5);
	}

}
void EXTI3_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line3) != RESET) {	//Orientation Change
		RotationChangedFlag = 1;
		EXTI_ClearITPendingBit(EXTI_Line3);
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

