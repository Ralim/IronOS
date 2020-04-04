/*
 * hardware.c
 *
 *  Created on: 2Sep.,2017
 *      Author: Ben V. Brown
 */

// These are all the functions for interacting with the hardware
#include "hardware.h"
#include "history.hpp"
volatile uint16_t PWMSafetyTimer = 0;

uint16_t getHandleTemperature() {
	// We return the current handle temperature in X10 C
	// TMP36 in handle, 0.5V offset and then 10mV per deg C (0.75V @ 25C for
	// example) STM32 = 4096 count @ 3.3V input -> But We oversample by 32/(2^2) =
	// 8 times oversampling Therefore 32768 is the 3.3V input, so 0.1007080078125
	// mV per count So we need to subtract an offset of 0.5V to center on 0C
	// (4964.8 counts)
	//
	int32_t result = getADC(0);
	result -= 4965;  // remove 0.5V offset
	// 10mV per C
	// 99.29 counts per Deg C above 0C
	result *= 100;
	result /= 993;
	return result;
}

uint16_t getTipInstantTemperature() {
	uint16_t sum = 0;	// 12 bit readings * 8 -> 15 bits
	uint16_t readings[8];
	//Looking to reject the highest outlier readings.
	//As on some hardware these samples can run into the op-amp recovery time
	//Once this time is up the signal stabilises quickly, so no need to reject minimums
	readings[0] = hadc1.Instance->JDR1;
	readings[1] = hadc1.Instance->JDR2;
	readings[2] = hadc1.Instance->JDR3;
	readings[3] = hadc1.Instance->JDR4;
	readings[4] = hadc2.Instance->JDR1;
	readings[5] = hadc2.Instance->JDR2;
	readings[6] = hadc2.Instance->JDR3;
	readings[7] = hadc2.Instance->JDR4;

	for (int i = 0; i < 8; i++) {
		sum += readings[i];
	}
	return sum;  // 8x over sample
}

//2 second filter (ADC is PID_TIM_HZ Hz)
history<uint16_t, PID_TIM_HZ> rawTempFilter = { { 0 }, 0, 0 };

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
#ifdef MODEL_TS100
#define BATTFILTERDEPTH 32
#else
#define BATTFILTERDEPTH 8

#endif
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
	return sum * 4 / divisor;
}
#ifdef MODEL_TS80
void DPlusZero_Six() {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);		// pull down D+
}
void DNegZero_Six() {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
}
void DPlusThree_Three() {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);			// pull up D+
}
void DNegThree_Three() {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
}

void QC_Seek9V() {
	DNegZero_Six();
	DPlusThree_Three();
}
void QC_Seek12V() {
	DNegZero_Six();
	DPlusZero_Six();
}
void QC_Seek20V() {
	DNegThree_Three();
	DPlusThree_Three();
}
void QC_SeekContMode() {
	DNegThree_Three();
	DPlusZero_Six();
}
void QC_SeekContPlus() {
	QC_SeekContMode();
	vTaskDelay(3);
	QC_Seek20V();
	vTaskDelay(1);
	QC_SeekContMode();
}
void QC_SeekContNeg() {
	QC_SeekContMode();
	vTaskDelay(3);
	QC_Seek12V();
	vTaskDelay(1);
	QC_SeekContMode();
}
uint8_t QCMode = 0;
uint8_t QCTries = 0;
void seekQC(int16_t Vx10, uint16_t divisor) {
	if (QCMode == 5)
		startQC(divisor);
	if (QCMode == 0)
		return;  // NOT connected to a QC Charger

	if (Vx10 < 45)
		return;
	if (xTaskGetTickCount() < 100)
		return;
	if (Vx10 > 130)
		Vx10 = 130;  //Cap max value at 13V
	// Seek the QC to the Voltage given if this adapter supports continuous mode
	// try and step towards the wanted value

	// 1. Measure current voltage
	int16_t vStart = getInputVoltageX10(divisor, 1);
	int difference = Vx10 - vStart;

	// 2. calculate ideal steps (0.2V changes)

	int steps = difference / 2;
	if (QCMode == 3) {
		if (steps > -2 && steps < 2)
			return; // dont bother with small steps
		while (steps < 0) {
			QC_SeekContNeg();
			vTaskDelay(3);
			steps++;
		}
		while (steps > 0) {
			QC_SeekContPlus();
			vTaskDelay(3);
			steps--;
		}
		vTaskDelay(10);
	}
#ifdef ENABLE_QC2
	// Re-measure
	/* Disabled due to nothing to test and code space of around 1k*/
	steps = vStart - getInputVoltageX10(divisor, 1);
	if (steps < 0)
		steps = -steps;
	if (steps > 4) {
		// No continuous mode, so QC2
		QCMode = 2;
		// Goto nearest
		if (Vx10 > 110) {
			// request 12V
			// D- = 0.6V, D+ = 0.6V
			// Clamp PB3
			QC_Seek12V();

		} else {
			// request 9V
			QC_Seek9V();
		}
	}
#endif
}
// Must be called after FreeRToS Starts
void startQC(uint16_t divisor) {
	// Pre check that the input could be >5V already, and if so, dont both
	// negotiating as someone is feeding in hv
	uint16_t vin = getInputVoltageX10(divisor, 1);
	if (vin > 100) {
		QCMode = 1;  // Already at 12V, user has probably over-ridden this
		return;
	}
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_10;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	//Turn off output mode on pins that we can
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_13;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// Tries to negotiate QC for 9V
	// This is a multiple step process.
	// 1. Set around 0.6V on D+ for 1.25 Seconds or so
	// 2. After this It should un-short D+->D- and instead add a 20k pulldown on
	// D-
	DPlusZero_Six();

	// Delay 1.25 seconds
	uint8_t enteredQC = 0;
	for (uint16_t i = 0; i < 200 && enteredQC == 0; i++) {
		vTaskDelay(1);	//10mS pause
		if (i > 130) {
			if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_RESET) {
				enteredQC = 1;
			}
			if (i == 140) {
				//For some marginal QC chargers, we try adding a pulldown
				GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
				GPIO_InitStruct.Pull = GPIO_PULLDOWN;
				GPIO_InitStruct.Pin = GPIO_PIN_11;
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
			}
		}
	}
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	if (enteredQC) {
		// We have a QC capable charger
		QC_Seek9V();
		GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		QC_Seek9V();
		// Wait for frontend ADC to stabilise
		QCMode = 4;
		for (uint8_t i = 0; i < 10; i++) {
			if (getInputVoltageX10(divisor, 1) > 80) {
				// yay we have at least QC2.0 or QC3.0
				QCMode = 3;	// We have at least QC2, pray for 3
				return;
			}
			vTaskDelay(10);  // 100mS
		}
		QCMode = 5;
		QCTries++;
		if (QCTries > 10) // 10 goes to get it going
			QCMode = 0;
	} else {
		// no QC
		QCMode = 0;
	}
	if (QCTries > 10)
		QCMode = 0;
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
int16_t calculateMaxVoltage(uint8_t useHP) {
// This measures the tip resistance, then it calculates the appropriate
// voltage To stay under ~18W. Mosfet is "9A", so no issues there
// QC3.0 supports up to 18W, which is 2A @9V and 1.5A @12V
	uint32_t milliOhms = 4500;
// Check no tip
	if (milliOhms > 10000)
		return -1;
//Because of tolerance, if a user has asked for the higher power mode, then just goto 12V and call it a day
	if (useHP)
		return 120;
//
// V = sqrt(18W*R)
// Convert this to sqrt(18W)*sqrt(milli ohms)*sqrt(1/1000)

	uint32_t Vx = sqrt32(milliOhms);
	if (useHP)
		Vx *= 1549;	//sqrt(24)*sqrt(1/1000)*10000
	else
		Vx *= 1342;	// sqrt(18) * sqrt(1/1000)*10000

// Round to nearest 200mV,
// So divide by 100 to start, to get in Vxx
	Vx /= 100;
	if (Vx % 10 >= 5)
		Vx += 10;
	Vx /= 10;
// Round to nearest increment of 2
	if (Vx % 2 == 1)
		Vx++;
//Because of how bad the tolerance is on detecting the tip resistance is
//Its more functional to bin this
	if (Vx < 90)
		Vx = 90;
	else if (Vx >= 105)
		Vx = 120;
	return Vx;
}

#endif
volatile uint8_t pendingPWM = 0;

void setTipPWM(uint8_t pulse) {
	PWMSafetyTimer = 10; // This is decremented in the handler for PWM so that the tip pwm is
						 // disabled if the PID task is not scheduled often enough.

	pendingPWM = pulse;
}

// These are called by the HAL after the corresponding events from the system
// timers.

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
// Period has elapsed
	if (htim->Instance == TIM2) {
		// we want to turn on the output again
		PWMSafetyTimer--;
		// We decrement this safety value so that lockups in the
		// scheduler will not cause the PWM to become locked in an
		// active driving state.
		// While we could assume this could never happen, its a small price for
		// increased safety
		htim2.Instance->CCR4 = pendingPWM;
		if (htim2.Instance->CCR4 && PWMSafetyTimer) {
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
		} else {
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
		}
	} else if (htim->Instance == TIM1) {
		// STM uses this for internal functions as a counter for timeouts
		HAL_IncTick();
	}
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
// This was a when the PWM for the output has timed out
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
	}
}

void vApplicationIdleHook(void) {
	HAL_IWDG_Refresh(&hiwdg);
}

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
		StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
	*ppxIdleTaskStackBuffer = &xIdleStack[0];
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
	/* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */
