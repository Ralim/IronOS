/*
 * hardware.c
 *
 *  Created on: 2Sep.,2017
 *      Author: Ben V. Brown
 */

//These are all the functions for interacting with the hardware
#include "hardware.h"
volatile uint16_t PWMSafetyTimer = 0;
volatile int16_t CalibrationTempOffset = 0;
void setCalibrationOffset(int16_t offSet) {
	CalibrationTempOffset = offSet;
}
uint16_t getHandleTemperature() {
	// We return the current handle temperature in X10 C
	// TMP36 in handle, 0.5V offset and then 10mV per deg C (0.75V @ 25C for example)
	// STM32 = 4096 count @ 3.3V input -> But
	// We oversample by 32/(2^2) = 8 times oversampling
	// Therefore 32768 is the 3.3V input, so 0.201416015625 mV per count
	// So we need to subtract an offset of 0.5V to center on 0C (2482*2 counts)
	//
	uint16_t result = getADC(0);
	if (result < 4964)
		return 0;
	result -= 4964;    //remove 0.5V offset
	result /= 10;    //convert to X10 C
	return result;

}
uint16_t tipMeasurementToC(uint16_t raw) {
	return ((raw - 532) / 33) + (getHandleTemperature() / 10)
			- CalibrationTempOffset;
	//Surprisingly that appears to be a fairly good linear best fit
}
uint16_t ctoTipMeasurement(uint16_t temp) {
	//We need to compensate for cold junction temp
	return ((temp - (getHandleTemperature() / 10) + CalibrationTempOffset) * 33)
			+ 532;
}

uint16_t tipMeasurementToF(uint16_t raw) {
	return ((((raw - 532) / 33) + (getHandleTemperature() / 10)
			- CalibrationTempOffset) * 9) / 5 + 32;

}
uint16_t ftoTipMeasurement(uint16_t temp) {

	return (((((temp - 32) * 5) / 9) - (getHandleTemperature() / 10)
			+ CalibrationTempOffset) * 33) + 532;
}

uint16_t getTipInstantTemperature() {
	uint16_t sum;
	sum = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
	sum += HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
	sum += HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3);
	sum += HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_4);
	return sum;

}
uint16_t getTipRawTemp(uint8_t instant) {
	static int64_t filterFP = 0;
	const uint8_t filterBeta = 5; //higher values smooth out more, but reduce responsiveness

	if (instant == 1) {
		uint16_t itemp = getTipInstantTemperature();
		filterFP = (filterFP << filterBeta) - filterFP;
		filterFP += (itemp << 9);
		filterFP = filterFP >> filterBeta;
		return itemp;
	} else if (instant == 2) {
		filterFP = (getTipInstantTemperature() << 9);
		return filterFP >> 9;
	} else {
		return filterFP >> 9;
	}
}
uint16_t getInputVoltageX10(uint16_t divisor) {
	//ADC maximum is 16384 == 3.3V at input == 28V at VIN
	//Therefore we can divide down from there
	//Ideal term is 117
#define BATTFILTERDEPTH 64
	static uint8_t preFillneeded = 1;
	static uint32_t samples[BATTFILTERDEPTH];
	static uint8_t index = 0;
	if (preFillneeded) {
		for (uint8_t i = 0; i < BATTFILTERDEPTH; i++)
			samples[i] = getADC(1);
		preFillneeded = 0;
	}
	samples[index] = getADC(1);
	index = (index + 1) % BATTFILTERDEPTH;
	uint32_t sum = 0;

	for (uint8_t i = 0; i < BATTFILTERDEPTH; i++)
		sum += samples[i];

	sum /= BATTFILTERDEPTH;
	if (sum < 50)
		preFillneeded = 1;
	return sum / divisor;
}
uint8_t getTipPWM() {
	return htim2.Instance->CCR4;
}
void setTipPWM(uint8_t pulse) {
	PWMSafetyTimer = 2; //This is decremented in the handler for PWM so that the tip pwm is disabled if the PID task is not scheduled often enough.
	if (pulse > 100)
		pulse = 100;
	htim2.Instance->CCR4 = pulse;
}

//Thse are called by the HAL after the corresponding events from the system timers.

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	//Period has elapsed
	if (htim->Instance == TIM2) {
		//we want to turn on the output again
		PWMSafetyTimer--; //We decrement this safety value so that lockups in the scheduler will not cause the PWM to become locked in an active driving state.
		//While we could assume this could never happen, its a small price for increased safety
		if (htim2.Instance->CCR4 && PWMSafetyTimer) {
			htim3.Instance->CCR1 = 50;
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
		} else {
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
			htim3.Instance->CCR1 = 0;
		}
	} else if (htim->Instance == TIM1) {
		HAL_IncTick();
	}
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		//This was a pulse event
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
			htim3.Instance->CCR1 = 0;

		} /*else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
		 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_14, GPIO_PIN_RESET);
		 }*/
	}
}

