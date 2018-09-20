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
uint16_t tipGainCalValue = 0;
uint16_t lookupTipDefaultCalValue(enum TipType tipID);
void setTipType(enum TipType tipType, uint8_t manualCalGain) {
	if (manualCalGain)
		tipGainCalValue = manualCalGain;
	else
		tipGainCalValue = lookupTipDefaultCalValue(tipType);
}
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

	//((Raw Tip-RawOffset) * calibrationgain) / 1000 = tip delta in CX10
	// tip delta in CX10 + handleTemp in CX10 = tip absolute temp in CX10
	//Div answer by 10 to get final result

	uint32_t tipDelta = ((raw - CalibrationTempOffset) * tipGainCalValue)
			/ 1000;
	tipDelta += getHandleTemperature();

	return tipDelta / 10;

}
uint16_t ctoTipMeasurement(uint16_t temp) {
	//[ (temp-handle/10) * 10000 ]/calibrationgain = tip raw delta
	// tip raw delta + tip offset = tip ADC reading
	int32_t TipRaw = ((temp - (getHandleTemperature() / 10)) * 10000)
			/ tipGainCalValue;
	TipRaw += CalibrationTempOffset;
	return TipRaw;
}

uint16_t tipMeasurementToF(uint16_t raw) {
	//Convert result from C to F
	return (tipMeasurementToC(raw) * 9) / 5 + 32;

}
uint16_t ftoTipMeasurement(uint16_t temp) {
//Convert the temp back to C from F
	return ctoTipMeasurement(((temp - 32) * 5) / 9);
}

uint16_t __attribute__ ((long_call, section (".data.ramfuncs"))) getTipInstantTemperature() {
	uint16_t sum;
	sum = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
	sum += HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
	sum += HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3);
	sum += HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_4);
	sum += HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_1);
	sum += HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_2);
	sum += HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_3);
	sum += HAL_ADCEx_InjectedGetValue(&hadc2, ADC_INJECTED_RANK_4);

	return sum; // 8x over sample

}
/*
 * Loopup table for the tip calibration values for
 * the gain of the tip's
 * This can be found by line of best fit of TipRaw on X, and TipTemp-handle on Y.
 * Then take the m term * 10000
 * */
uint16_t lookupTipDefaultCalValue(enum TipType tipID) {

	switch (tipID) {
	case TS_D24:
		return 141;
		break;
	case TS_BC2:
		return (133 + 129) / 2;
		break;
	case TS_C1:
		return 133;
		break;
	case TS_B2:
		return 133;
	default:
		return 132; // make this the average of all
		break;
	}
}

uint16_t __attribute__ ((long_call, section (".data.ramfuncs"))) getTipRawTemp(
		uint8_t instant) {
	static int64_t filterFP = 0;
	static uint16_t lastSample = 0;
	const uint8_t filterBeta = 7; //higher values smooth out more, but reduce responsiveness

	if (instant == 1) {
		uint16_t itemp = getTipInstantTemperature();
		filterFP = (filterFP << filterBeta) - filterFP;
		filterFP += (itemp << 9);
		filterFP = filterFP >> filterBeta;
		uint16_t temp = itemp;
		itemp += lastSample;
		itemp /= 2;
		lastSample = temp;
		return itemp;
	} else if (instant == 2) {
		filterFP = (getTipInstantTemperature() << 8);
		return filterFP >> 9;
	} else {
		return filterFP >> 9;
	}
}
uint16_t getInputVoltageX10(uint16_t divisor) {
	//ADC maximum is 32767 == 3.3V at input == 28.05V at VIN
	//Therefore we can divide down from there
	//Multiplying ADC max by 4 for additional calibration options,
	//ideal term is 467
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
	return sum * 4 / divisor;
}
volatile uint32_t pendingPWM = 0;
uint8_t getTipPWM() {
	return pendingPWM;
}
void __attribute__ ((long_call, section (".data.ramfuncs"))) setTipPWM(uint8_t pulse) {
	PWMSafetyTimer = 2; //This is decremented in the handler for PWM so that the tip pwm is disabled if the PID task is not scheduled often enough.
	if (pulse > 100)
		pulse = 100;

	pendingPWM = pulse;
}

//These are called by the HAL after the corresponding events from the system timers.

void __attribute__ ((long_call, section (".data.ramfuncs"))) HAL_TIM_PeriodElapsedCallback(
		TIM_HandleTypeDef *htim) {
	//Period has elapsed
	if (htim->Instance == TIM2) {
		//we want to turn on the output again
		PWMSafetyTimer--; //We decrement this safety value so that lockups in the scheduler will not cause the PWM to become locked in an active driving state.
		//While we could assume this could never happen, its a small price for increased safety
		htim2.Instance->CCR4 = pendingPWM;
		if (htim2.Instance->CCR4 && PWMSafetyTimer) {
			htim3.Instance->CCR1 = 50;
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
		} else {
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
			htim3.Instance->CCR1 = 0;
		}
	} else if (htim->Instance == TIM1) {
		// STM uses this for internal functions as a counter for timeouts
		HAL_IncTick();
	}
}

void __attribute__ ((long_call, section (".data.ramfuncs"))) HAL_TIM_PWM_PulseFinishedCallback(
		TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		//This was a when the PWM for the output has timed out
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
			htim3.Instance->CCR1 = 0;

		}
	}
}

