#include "BSP.h"
#include "BSP_Power.h"
#include "QC3.h"
#include "Settings.h"
#include "Pins.h"
#include "fusbpd.h"
#include "Model_Config.h"
#include "int_n.h"
bool FUSB302_present = false;
void power_probe() {
// If TS80(p) probe for QC
// If TS100 - noop
#ifdef POW_QC

	startQC(systemSettings.voltageDiv);
	seekQC((systemSettings.cutoutSetting) ? 120 : 90,
			systemSettings.voltageDiv); // this will move the QC output to the preferred voltage to start with

#endif
}

void power_check() {
#ifdef POW_QC
	QC_resync();
#endif
}
uint8_t usb_pd_detect() {
#ifdef POW_PD
	FUSB302_present = fusb302_detect();

	return FUSB302_present;
#endif
	return false;
}
uint8_t pd_irq_read() {
#ifdef POW_PD
	return HAL_GPIO_ReadPin(INT_PD_GPIO_Port, INT_PD_Pin) == GPIO_PIN_SET ?
			1 : 0;
#endif
	return 0;
}
