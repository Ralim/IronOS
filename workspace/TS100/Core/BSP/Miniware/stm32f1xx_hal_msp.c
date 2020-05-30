#include "Pins.h"
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


}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {

	GPIO_InitTypeDef GPIO_InitStruct;
	if (hadc->Instance == ADC1) {
		__HAL_RCC_ADC1_CLK_ENABLE()
		;

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
		HAL_NVIC_SetPriority(ADC1_2_IRQn, 15, 0);
		HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
	} else {
		__HAL_RCC_ADC2_CLK_ENABLE()
		;

		/**ADC2 GPIO Configuration
		 PB0     ------> ADC2_IN8
		 PB1     ------> ADC2_IN9
		 */
		GPIO_InitStruct.Pin = TIP_TEMP_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* ADC2 interrupt Init */
		HAL_NVIC_SetPriority(ADC1_2_IRQn, 15, 0);
		HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
	}

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c) {

	GPIO_InitTypeDef GPIO_InitStruct;
	/**I2C1 GPIO Configuration
	 PB6     ------> I2C1_SCL
	 PB7     ------> I2C1_SDA
	 */
	GPIO_InitStruct.Pin = SCL_Pin | SDA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* Peripheral clock enable */
	__HAL_RCC_I2C1_CLK_ENABLE()
	;
	/* I2C1 DMA Init */
	/* I2C1_RX Init */
	hdma_i2c1_rx.Instance = DMA1_Channel7;
	hdma_i2c1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_i2c1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_i2c1_rx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_i2c1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_i2c1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hdma_i2c1_rx.Init.Mode = DMA_NORMAL;
	hdma_i2c1_rx.Init.Priority = DMA_PRIORITY_LOW;
	HAL_DMA_Init(&hdma_i2c1_rx);

	__HAL_LINKDMA(hi2c, hdmarx, hdma_i2c1_rx);

	/* I2C1_TX Init */
	hdma_i2c1_tx.Instance = DMA1_Channel6;
	hdma_i2c1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
	hdma_i2c1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_i2c1_tx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_i2c1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_i2c1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hdma_i2c1_tx.Init.Mode = DMA_NORMAL;
	hdma_i2c1_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
	HAL_DMA_Init(&hdma_i2c1_tx);

	__HAL_LINKDMA(hi2c, hdmatx, hdma_i2c1_tx);

	/* I2C1 interrupt Init */
	HAL_NVIC_SetPriority(I2C1_EV_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
	HAL_NVIC_SetPriority(I2C1_ER_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);

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
