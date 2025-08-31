// This is the stock standard STM interrupt file full of handlers
#include "stm32f1xx_it.h"
#include "Pins.h"
#include "Setup.h"
#include "cmsis_os.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
extern TIM_HandleTypeDef htim4; // used for the systick

/******************************************************************************/
/*            Cortex-M3 Processor Interruption and Exception Handlers         */
/******************************************************************************/

void NMI_Handler(void) {}

// We have the assembly for a breakpoint trigger here to halt the system when a debugger is connected
// Hardfault handler, often a screwup in the code
void HardFault_Handler(void) {}

// Memory management unit had an error
void MemManage_Handler(void) {}

// Prefetcher or busfault occured
void BusFault_Handler(void) {}

void UsageFault_Handler(void) {}

void DebugMon_Handler(void) {}

// Systick is used by FreeRTOS tick
void SysTick_Handler(void) { osSystickHandler(); }

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file.					                      */
/******************************************************************************/

// DMA used to move the ADC readings into system ram
void DMA1_Channel1_IRQHandler(void) { HAL_DMA_IRQHandler(&hdma_adc1); }
// ADC interrupt used for DMA
void ADC1_2_IRQHandler(void) { HAL_ADC_IRQHandler(&hadc1); }

// used for hal ticks
void TIM4_IRQHandler(void) { HAL_TIM_IRQHandler(&htim4); }

void EXTI9_5_IRQHandler(void) { HAL_GPIO_EXTI_IRQHandler(INT_PD_Pin); }
