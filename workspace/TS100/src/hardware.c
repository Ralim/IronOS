/*
 * hardware.c
 *
 *  Created on: 2Sep.,2017
 *      Author: Ben V. Brown
 */

//These are all the functions for interacting with the hardware
#include "hardware.h"
#include "FreeRTos.h"
#include "stm32f1xx_hal.h"
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

uint16_t getTipInstantTemperature() {
	uint16_t sum;
	sum = hadc1.Instance->JDR1;
	sum += hadc1.Instance->JDR2;
	sum += hadc1.Instance->JDR3;
	sum += hadc1.Instance->JDR4;
	sum += hadc2.Instance->JDR1;
	sum += hadc2.Instance->JDR2;
	sum += hadc2.Instance->JDR3;
	sum += hadc2.Instance->JDR4;
	return sum; // 8x over sample

}
/*
 * Loopup table for the tip calibration values for
 * the gain of the tip's
 * This can be found by line of best fit of TipRaw on X, and TipTemp-handle on Y.
 * Then take the m term * 10000
 * */
uint16_t lookupTipDefaultCalValue(enum TipType tipID) {
#ifdef MODEL_TS100
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
#else
	return 132;
#endif
}

uint16_t getTipRawTemp(uint8_t instant) {
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
uint16_t getInputVoltageX10(uint8_t divisor) {
	//ADC maximum is 16384 == 3.3V at input == 28V at VIN
	//Therefore we can divide down from there
	//Ideal term is 117
	//For TS80 input range is up to 16V, so ideal diviser is then going to be 205

#define BATTFILTERDEPTH 48
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
#ifdef MODEL_TS80
uint8_t QCMode = 0;
void seekQC(int16_t Vx10) {
	if (QCMode <= 1)
		return;	//NOT connected to a QC Charger

	//Seek the QC to the Voltage given if this adapter supports continuous mode
	//try and step towards the wanted value

	//1. Measure current voltage
	int16_t vStart = getInputVoltageX10(195);
	int difference = Vx10 - vStart;

	//2. calculate ideal steps (0.2V changes)

	int steps = difference / 2;
	if (QCMode == 3) {
		while (steps < 0) {
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
			vTaskDelay(3);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
			vTaskDelay(3);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_IWDG_Refresh(&hiwdg);
			vTaskDelay(3);
			steps++;
		}
		while (steps > 0) {
			//step once up
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
			vTaskDelay(3);

			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
			vTaskDelay(3);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
			HAL_IWDG_Refresh(&hiwdg);
			vTaskDelay(3);
			steps--;
		}
	}
	//Re-measure
	/* Disabled due to nothing to test and code space of around 1k*/
#ifdef QC2_ROUND_DOWN
	steps = vStart - getInputVoltageX10(195);
	if (steps < 0)
		steps = -steps;
	if (steps > (difference / 2)) {
		//No continuous mode, so QC2
		QCMode = 2;
		//Goto nearest
		if (Vx10 > 10.5) {
			//request 12V
			//D- = 0.6V, D+ = 0.6V
			//Clamp PB3
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);//pull down D+
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

		} else {
			//request 9V
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		}
	}
#endif

}

//Must be called after FreeRToS Starts
void startQC() {
	//Pre check that the input could be >5V already, and if so, dont both negotiating as someone is feeding in hv
	uint16_t vin = getInputVoltageX10(205);
	if (vin > 150)
		return; // Over voltage
	if (vin > 100) {
		QCMode = 1; // ALready at ~12V
		return;
	}
	GPIO_InitTypeDef GPIO_InitStruct;

	// Tries to negotiate QC for 9V
	// This is a multiple step process.
	// 1. Set around 0.6V on D+ for 1.25 Seconds or so
	//2. After this It should un-short D+->D- and instead add a 20k pulldown on D-
	// 3. Now set D+ to 3.3V and D- to 0.6V to request 9V
// OR both at 0.6V for 12V request (if the adapter can do it).
	//If 12V is implimented then should fallback to 9V after validation
	//Step 1. We want to pull D+ to 0.6V
	// Pull PB3 donwn to ground
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);//pull low to put 0.6V on D+
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);//pull low to put 0.6V on D+
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_13;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//Delay 1.25 seconds
	uint8_t enteredQC = 0;
	for (uint16_t i = 0; i < 130 && enteredQC == 0; i++) {
//		HAL_Delay(10);
		vTaskDelay(1);
		HAL_IWDG_Refresh(&hiwdg);
	}
	//Check if D- is low to spot a QC charger
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_RESET)
		enteredQC = 1;
	if (enteredQC) {
		//We have a QC capable charger
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_8;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
		HAL_IWDG_Refresh(&hiwdg);

		//Wait for frontend ADC to stabilise
		QCMode = 4;
		for (uint8_t i = 0; i < 10; i++) {
			if (getInputVoltageX10(195) > 80) {
				//yay we have at least QC2.0 or QC3.0
				QCMode = 3;	//We have at least QC2, pray for 3
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
				return;
			}
			vTaskDelay(10); //100mS
		}
		QCMode = 0;
		//No QC / not working  to us

	} else {
		// no QC
	}
}
//Get tip resistance in milliohms
uint32_t calculateTipR(uint8_t useFilter) {
	// We inject a small current into the front end of the iron,
	// By measuring the Vdrop over the tip we can calculate the resistance
	// Turn PA0 into an output and drive high to inject (3.3V-0.6)/(6K8+Rtip) current
	// PA0->Diode -> 6K8 -> Tip -> GND
	// So the op-amp will amplify the small signal across the tip and convert this into an easily read voltage
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);	//Set low first
	setTipPWM(0);
	vTaskDelay(1);
	uint32_t offReading = getTipInstantTemperature();
	for (uint8_t i = 0; i < 24; i++) {
		if (useFilter == 0) {
			vTaskDelay(1);	//delay to allow it too stabilize
			offReading += getTipInstantTemperature();
		} else {
			offReading += getTipRawTemp(0);
		}
	}

	//Turn on
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);	//Set low first
	vTaskDelay(1);	//delay to allow it too stabilize
	uint32_t onReading = getTipInstantTemperature();
	for (uint8_t i = 0; i < 24; i++) {
		if (useFilter == 0) {
			vTaskDelay(1);	//delay to allow it too stabilize
			onReading += getTipInstantTemperature();
		} else {
			onReading += getTipRawTemp(0);
		}
	}

	uint32_t difference = onReading - offReading;
	// V = IR, therefore I = V/R
	//We can divide this reading by a known "gain" to get the resulting resistance
	//This was determined emperically
	//This tip is 4.688444162 ohms, 4688 milliohms (Measured using 4 terminal measurement)
	//25x oversampling reads this as around 47490
	//Almost perfectly 10x the milliohms value
	//This will drift massively with tip temp
	//However we really only need 10x ohms
	return (difference / 10) + 1;	//ceil
}
static unsigned int sqrt32(unsigned long n) {
	unsigned int c = 0x8000;
	unsigned int g = 0x8000;

	for (;;) {
		if (g * g > n)
			g ^= c;
		c >>= 1;
		if (c == 0)
			return g;
		g |= c;
	}
}
int16_t calculateMaxVoltage(uint8_t useFilter) {
	// This measures the tip resistance, then it calculates the appropriate voltage
	// To stay under ~18W. Mosfet is "9A", so no issues there
	//QC3.0 supports up to 18W, which is 2A @9V and 1.5A @12V
	uint32_t milliOhms = calculateTipR(useFilter);
	//Check no tip
	if (milliOhms > 10000)
		return -1;
	//
	//V = sqrt(18W*R)
	//Convert this to sqrt(18W)*sqrt(milli ohms)*sqrt(1/1000)

	uint32_t Vx = sqrt32(milliOhms);
	//Round to nearest 200mV,
	//So divide by 100 to start, to get in Vxx
	Vx *= 1342;
	Vx /= 100;
	if (Vx % 10 >= 5)
		Vx += 10;
	Vx /= 10;
	//Round to nearest increment of 2
	if (Vx % 2 == 1)
		Vx++;
	return Vx;
}

#endif
volatile uint32_t pendingPWM = 0;
uint8_t getTipPWM() {
	return pendingPWM;
}
void setTipPWM(uint8_t pulse) {
	PWMSafetyTimer = 2; //This is decremented in the handler for PWM so that the tip pwm is disabled if the PID task is not scheduled often enough.
	if (pulse > 100)
		pulse = 100;

	pendingPWM = pulse;
}

//These are called by the HAL after the corresponding events from the system timers.

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
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

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		//This was a when the PWM for the output has timed out
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
			htim3.Instance->CCR1 = 0;

		}
	}
}

