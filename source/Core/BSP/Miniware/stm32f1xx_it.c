// This is the stock standard STM interrupt file full of handlers
#include "stm32f1xx_it.h"
#include "Pins.h"
#include "Setup.h"
#include "cmsis_os.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"

extern TIM_HandleTypeDef htim1; // used for the systick

/******************************************************************************/
/*            Cortex-M3 Processor Interruption and Exception Handlers         */
/******************************************************************************/

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

// Timer 1 has overflowed, used for HAL ticks
void TIM1_UP_IRQHandler(void) { HAL_TIM_IRQHandler(&htim1); }

// Timer 3 is used for the PWM output to the tip
void TIM3_IRQHandler(void) {
  TIM_HandleTypeDef *handle = &htimADC;
  if (htimTip.Instance == TIM3) {
    handle = &htimTip;
  }
  HAL_TIM_IRQHandler(handle);
}

// Timer 2 is used for co-ordination of PWM & ADC
void TIM2_IRQHandler(void) {
  TIM_HandleTypeDef *handle = &htimADC;
  if (htimTip.Instance == TIM2) {
    handle = &htimTip;
  }
  HAL_TIM_IRQHandler(handle);
}

// Timer 2 is used for co-ordination of PWM & ADC
void TIM4_IRQHandler(void) {
  TIM_HandleTypeDef *handle = &htimADC;
  if (htimTip.Instance == TIM4) {
    handle = &htimTip;
  }
  HAL_TIM_IRQHandler(handle);
}

void I2C1_EV_IRQHandler(void) { HAL_I2C_EV_IRQHandler(&hi2c1); }
void I2C1_ER_IRQHandler(void) { HAL_I2C_ER_IRQHandler(&hi2c1); }

void DMA1_Channel6_IRQHandler(void) { HAL_DMA_IRQHandler(&hdma_i2c1_tx); }

void DMA1_Channel7_IRQHandler(void) { HAL_DMA_IRQHandler(&hdma_i2c1_rx); }

void EXTI9_5_IRQHandler(void) {
#ifdef INT_PD_Pin
  HAL_GPIO_EXTI_IRQHandler(INT_PD_Pin);
#endif
}
