#include "BSP.h"
#include "BSP_Power.h"
#include "QC3.h"
#include "Settings.h"
#include "Pins.h"
#include "fusbpd.h"
bool FUSB302_present = false;
void power_probe() {
// If TS80 probe for QC
// If TS100 - noop
#ifdef defined(MODEL_TS80)+defined(MODEL_TS80P)>0

	startQC(systemSettings.voltageDiv);
	seekQC((systemSettings.cutoutSetting) ? 120 : 90,
			systemSettings.voltageDiv); // this will move the QC output to the preferred voltage to start with

#endif
}

void power_check() {
#ifdef defined(MODEL_TS80)+defined(MODEL_TS80P)>0
	QC_resync();
#endif
}
uint8_t usb_pd_detect() {
#ifdef MODEL_TS80P
	FUSB302_present = fusb302_detect();
	if (FUSB302_present) {
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
		GPIO_InitStruct.Pin = GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
	return FUSB302_present;
#endif
	return false;
}
uint8_t pd_irq_read() {
#ifdef MODEL_TS80P
	if (FUSB302_present) {
		return HAL_GPIO_ReadPin(INT_PD_GPIO_Port, INT_PD_Pin) == GPIO_PIN_SET ?
				1 : 0;
	}
#endif
	return 0;
}
