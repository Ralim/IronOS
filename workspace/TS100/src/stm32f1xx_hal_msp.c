#include <hardware.h>
#include "stm32f1xx_hal.h"
#include "Setup.h"
/**
 * Initializes the Global MSP.
 */
void HAL_MspInit(void) {
	__HAL_RCC_AFIO_CLK_ENABLE()
	;

	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	/* System interrupt init*/
	/* MemoryManagement_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	/* BusFault_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	/* UsageFault_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
	/* SVCall_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
	/* DebugMonitor_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
	/* PendSV_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);

	/**NOJTAG: JTAG-DP Disabled and SW-DP Enabled
	 */
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
//	__HAL_AFIO_REMAP_SWJ_DISABLE(); /*Disable swd for debug io use*/


}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {

	GPIO_InitTypeDef GPIO_InitStruct;
	if (hadc->Instance == ADC1) {
		__HAL_RCC_ADC1_CLK_ENABLE()
		;
		/**ADC1 GPIO Configuration
		 PA7     ------> ADC1_IN7
		 PB0     ------> ADC1_IN8
		 PB1     ------> ADC1_IN9
		 */
		GPIO_InitStruct.Pin = TMP36_INPUT_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		HAL_GPIO_Init(TMP36_INPUT_GPIO_Port, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = TIP_TEMP_Pin | VIN_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* ADC1 DMA Init */
		/* ADC1 Init */
		hdma_adc1.Instance = DMA1_Channel1;
		hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
		hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		hdma_adc1.Init.Mode = DMA_CIRCULAR;
		hdma_adc1.Init.Priority = DMA_PRIORITY_VERY_HIGH;
		HAL_DMA_Init(&hdma_adc1);

		__HAL_LINKDMA(hadc, DMA_Handle, hdma_adc1);

		/* ADC1 interrupt Init */
		HAL_NVIC_SetPriority(ADC1_2_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
	}

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c) {

	GPIO_InitTypeDef GPIO_InitStruct;
	if (hi2c->Instance == I2C1) {
		/**I2C1 GPIO Configuration
		 PB6     ------> I2C1_SCL
		 PB7     ------> I2C1_SDA
		 */
		GPIO_InitStruct.Pin = SCL_Pin | SDA_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* Peripheral clock enable */
		__HAL_RCC_I2C1_CLK_ENABLE()
		;

	}

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base) {
	if (htim_base->Instance == TIM3) {
		/* Peripheral clock enable */
		__HAL_RCC_TIM3_CLK_ENABLE()
		;
	} else if (htim_base->Instance == TIM2) {
		/* Peripheral clock enable */
		__HAL_RCC_TIM2_CLK_ENABLE()
		;
	}
}
