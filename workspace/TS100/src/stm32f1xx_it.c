// This is the stock standard STM interrupt file full of handlers
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"
#include "cmsis_os.h"
#include "Setup.h"

extern TIM_HandleTypeDef htim1; //used for the systick

/******************************************************************************/
/*            Cortex-M3 Processor Interruption and Exception Handlers         */
/******************************************************************************/

void NMI_Handler(void) {
	NVIC_SystemReset();
}

//We have the assembly for a breakpoint trigger here to halt the system when a debugger is connected
// Hardfault handler, often a screwup in the code
void HardFault_Handler(void) {
	NVIC_SystemReset();
}

// Memory management unit had an error
void MemManage_Handler(void) {
	NVIC_SystemReset();
}

// Prefetcher or busfault occured
void BusFault_Handler(void) {
	NVIC_SystemReset();
}

void UsageFault_Handler(void) {
	NVIC_SystemReset();
}

void DebugMon_Handler(void) {
}

// Systick is used by FreeRTOS tick
void SysTick_Handler(void) {
	osSystickHandler();
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file.					                      */
/******************************************************************************/

// DMA used to move the ADC readings into system ram
void DMA1_Channel1_IRQHandler(void) {
	HAL_DMA_IRQHandler(&hdma_adc1);
}

//ADC interrupt used for DMA
void ADC1_2_IRQHandler(void) {
	HAL_ADC_IRQHandler(&hadc1);
}

//Timer 1 has overflowed, used for HAL ticks
void TIM1_UP_IRQHandler(void) {
	HAL_TIM_IRQHandler(&htim1);

}
//Timer 3 is used for the PWM output to the tip
void TIM3_IRQHandler(void) {
	HAL_TIM_IRQHandler(&htim3);
}

//Timer 2 is used for co-ordination of PWM & ADC
void TIM2_IRQHandler(void) {
	HAL_TIM_IRQHandler(&htim2);
}
//EXTI 3 is triggered via the accelerometer on orientation change
void EXTI3_IRQHandler(void) {

	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}
//EXTI 5 is triggered via the accelerometer on movement
void EXTI9_5_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);

}
