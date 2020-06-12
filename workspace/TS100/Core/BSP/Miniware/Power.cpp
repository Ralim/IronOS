#include "BSP.h"
#include "BSP_Power.h"
#include "QC3.h"
#include "Settings.h"
#include "FUSB302.h"
bool FUSB302_present = false;
void power_probe() {
// If TS80 probe for QC
// If TS100 - noop
#ifdef MODEL_TS80

	startQC(systemSettings.voltageDiv);

	seekQC((systemSettings.cutoutSetting) ? 120 : 90,
			systemSettings.voltageDiv); // this will move the QC output to the preferred voltage to start with

#endif
}

void power_check() {
#ifdef MODEL_TS80
	QC_resync();
	if (FUSB302_present) {
		pd_run_state_machine();
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_RESET) {
			tcpc_alert();
		}
	}

#endif
}
uint8_t usb_pd_detect() {
#ifdef MODEL_TS80
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
