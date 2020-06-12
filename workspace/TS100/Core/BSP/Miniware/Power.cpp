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
           systemSettings.voltageDiv);  // this will move the QC output to the preferred voltage to start with

#endif
}

void power_check() {
#ifdef MODEL_TS80
    QC_resync();
#endif
}
uint8_t usb_pd_detect(){
#ifdef MODEL_TS80
	FUSB302_present=fusb302_detect();
	return FUSB302_present;
#endif
	return false;
}
