// BSP mapping functions

#include "BSP.h"
#include "I2C_Wrapper.hpp"
#include "Model_Config.h"
#include "Pins.h"
#include "Setup.h"
#include "history.hpp"
#include "main.hpp"
#include <IRQ.h>
volatile uint16_t PWMSafetyTimer = 0;
volatile uint8_t pendingPWM = 0;
uint16_t totalPWM = 255;
const uint16_t powerPWM = 255;

history<uint16_t, PID_TIM_HZ> rawTempFilter = { { 0 }, 0, 0 };
void resetWatchdog() {
	HAL_IWDG_Refresh(&hiwdg);
}

#ifdef TEMP_NTC
// Lookup table for the NTC
// Stored as ADCReading,Temp in degC
static const uint16_t NTCHandleLookup[] = {
// ADC Reading , Temp in C
		29189, 0,  //
		29014, 1,  //
		28832, 2,  //
		28644, 3,  //
		28450, 4,  //
		28249, 5,  //
		28042, 6,  //
		27828, 7,  //
		27607, 8,  //
		27380, 9,  //
		27146, 10, //
		26906, 11, //
		26660, 12, //
		26407, 13, //
		26147, 14, //
		25882, 15, //
		25610, 16, //
		25332, 17, //
		25049, 18, //
		24759, 19, //
		24465, 20, //
		24164, 21, //
		23859, 22, //
		23549, 23, //
		23234, 24, //
		22915, 25, //
		22591, 26, //
		22264, 27, //
		21933, 28, //
		21599, 29, //
		21261, 30, //
		20921, 31, //
		20579, 32, //
		20234, 33, //
		19888, 34, //
		19541, 35, //
		19192, 36, //
		18843, 37, //
		18493, 38, //
		18143, 39, //
		17793, 40, //
		17444, 41, //
		17096, 42, //
		16750, 43, //
		16404, 44, //
		16061, 45, //
					//		15719, 46, //
					//		15380, 47, //
					//		15044, 48, //
					//		14710, 49, //
					//		14380, 50, //
					//		14053, 51, //
					//		13729, 52, //
					//		13410, 53, //
					//		13094, 54, //
					//		12782, 55, //
					//		12475, 56, //
					//		12172, 57, //
					//		11874, 58, //
					//		11580, 59, //
					//		11292, 60, //
		};
#endif

// These are called by the HAL after the corresponding events from the system
// timers.

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	// Period has elapsed
	if (htim->Instance == TIM1) {
		// STM uses this for internal functions as a counter for timeouts
		HAL_IncTick();
	}
}
uint16_t getHandleTemperature() {
	return 250; //TODO
}

uint16_t getTipInstantTemperature() {
	return getADC(2);
}

uint16_t getTipRawTemp(uint8_t refresh) {
	if (refresh) {
		uint16_t lastSample = getTipInstantTemperature();
		rawTempFilter.update(lastSample);
		return lastSample;
	} else {
		return rawTempFilter.average();
	}
}

uint16_t getInputVoltageX10(uint16_t divisor, uint8_t sample) {
// ADC maximum is 32767 == 3.3V at input == 28.05V at VIN
// Therefore we can divide down from there
// Multiplying ADC max by 4 for additional calibration options,
// ideal term is 467
	static uint8_t preFillneeded = 10;
	static uint32_t samples[BATTFILTERDEPTH];
	static uint8_t index = 0;
	if (preFillneeded) {
		for (uint8_t i = 0; i < BATTFILTERDEPTH; i++)
			samples[i] = getADC(1);
		preFillneeded--;
	}
	if (sample) {
		samples[index] = getADC(1);
		index = (index + 1) % BATTFILTERDEPTH;
	}
	uint32_t sum = 0;

	for (uint8_t i = 0; i < BATTFILTERDEPTH; i++)
		sum += samples[i];

	sum /= BATTFILTERDEPTH;
	if (divisor == 0) {
		divisor = 1;
	}
	return sum * 4 / divisor;
}
bool tryBetterPWM(uint8_t pwm) {
	//We dont need this for the MHP30
	return false;
}
void setTipPWM(uint8_t pulse) {
	//We can just set the timer directly
	htim3.Instance->CCR1 = pulse;
}

void unstick_I2C() {
	GPIO_InitTypeDef GPIO_InitStruct;
	int timeout = 100;
	int timeout_cnt = 0;

	// 1. Clear PE bit.
	hi2c1.Instance->CR1 &= ~(0x0001);
	/**I2C1 GPIO Configuration
	 PB6     ------> I2C1_SCL
	 PB7     ------> I2C1_SDA
	 */
	//  2. Configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	GPIO_InitStruct.Pin = SCL_Pin;
	HAL_GPIO_Init(SCL_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = SDA_Pin;
	HAL_GPIO_Init(SDA_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET);

	while (GPIO_PIN_SET != HAL_GPIO_ReadPin(SDA_GPIO_Port, SDA_Pin)) {
		// Move clock to release I2C
		HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_RESET);
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET);

		timeout_cnt++;
		if (timeout_cnt > timeout)
			return;
	}

	// 12. Configure the SCL and SDA I/Os as Alternate function Open-Drain.
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	GPIO_InitStruct.Pin = SCL_Pin;
	HAL_GPIO_Init(SCL_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SDA_Pin;
	HAL_GPIO_Init(SDA_GPIO_Port, &GPIO_InitStruct);

	HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET);

	// 13. Set SWRST bit in I2Cx_CR1 register.
	hi2c1.Instance->CR1 |= 0x8000;

	asm("nop");

	// 14. Clear SWRST bit in I2Cx_CR1 register.
	hi2c1.Instance->CR1 &= ~0x8000;

	asm("nop");

	// 15. Enable the I2C peripheral by setting the PE bit in I2Cx_CR1 register
	hi2c1.Instance->CR1 |= 0x0001;

	// Call initialization function.
	HAL_I2C_Init(&hi2c1);
}

uint8_t getButtonA() {
	return HAL_GPIO_ReadPin(KEY_A_GPIO_Port, KEY_A_Pin) == GPIO_PIN_RESET ?
			1 : 0;
}
uint8_t getButtonB() {
	return HAL_GPIO_ReadPin(KEY_B_GPIO_Port, KEY_B_Pin) == GPIO_PIN_RESET ?
			1 : 0;
}

void BSPInit(void) {
}

void reboot() {
	NVIC_SystemReset();
}

void delay_ms(uint16_t count) {
	HAL_Delay(count);
}
