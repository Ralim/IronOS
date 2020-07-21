#include "BSP.h"
#include "BSP_Power.h"
#include "QC3.h"
#include "Settings.h"
#include "Pins.h"
#include "fusbpd.h"
#include "int_n.h"
bool FUSB302_present = false;
void power_probe() {
// If TS80 probe for QC
// If TS100 - noop
#if defined(MODEL_TS80)+defined(MODEL_TS80P)>0

	startQC(systemSettings.voltageDiv);
	seekQC((systemSettings.cutoutSetting) ? 120 : 90,
			systemSettings.voltageDiv); // this will move the QC output to the preferred voltage to start with

#endif
}

void power_check() {
#if defined(MODEL_TS80)+defined(MODEL_TS80P)>0
	QC_resync();
#endif
}
uint8_t usb_pd_detect() {
#ifdef MODEL_TS80P
	FUSB302_present = fusb302_detect();

	return FUSB302_present;
#endif
	return false;
}
uint8_t pd_irq_read() {
#ifdef MODEL_TS80P
	return HAL_GPIO_ReadPin(INT_PD_GPIO_Port, INT_PD_Pin) == GPIO_PIN_SET ?
			1 : 0;
#endif
	return 0;
}
