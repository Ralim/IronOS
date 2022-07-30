/*
 * QC.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */
#include "BSP.h"
#include "Pins.h"
#include "QC3.h"
#include "Settings.h"

#ifdef POW_QC
void QC_DPlusZero_Six() {
  // pull down D+
  gpio_write(QC_DP_LOW_Pin, 0);
}
void QC_DNegZero_Six() {
  gpio_write(QC_DM_HIGH_Pin, 0);
  gpio_write(QC_DM_LOW_Pin, 1);
}
void QC_DPlusThree_Three() {
  // pull up D+
  gpio_write(QC_DP_LOW_Pin, 1);
}
void QC_DNegThree_Three() {
  gpio_write(QC_DM_LOW_Pin, 1);
  gpio_write(QC_DM_HIGH_Pin, 1);
}
void QC_DM_PullDown() {
  // Turn on pulldown on D-
  gpio_set_mode(USB_DM_Pin, GPIO_INPUT_PD_MODE);
  gpio_set_mode(QC_DM_LOW_Pin, GPIO_INPUT_PD_MODE);
  gpio_set_mode(QC_DM_HIGH_Pin, GPIO_INPUT_PD_MODE);
}
void QC_DM_No_PullDown() {
  // Turn off pulldown on d-
  gpio_set_mode(USB_DM_Pin, GPIO_INPUT_MODE);
}
void QC_Init_GPIO() {
  // Setup any GPIO into the right states for QC
  // D+ pulldown as output
  gpio_set_mode(QC_DP_LOW_Pin, GPIO_OUTPUT_MODE);
  gpio_write(QC_DP_LOW_Pin, 0);
  // Make two D- pins floating
  gpio_set_mode(USB_DM_Pin, GPIO_INPUT_MODE);
  gpio_set_mode(QC_DM_LOW_Pin, GPIO_INPUT_MODE);
  gpio_set_mode(QC_DM_HIGH_Pin, GPIO_INPUT_MODE);
}
void QC_Post_Probe_En() {
  // Make two D- pins outputs
  gpio_set_mode(QC_DM_LOW_Pin, GPIO_OUTPUT_MODE);
  gpio_set_mode(QC_DM_HIGH_Pin, GPIO_OUTPUT_MODE);
}

uint8_t QC_DM_PulledDown() { return gpio_read(USB_DM_Pin) == 0; }
#endif
void QC_resync() {
#ifdef POW_QC
  seekQC(getSettingValue(SettingsOptions::QCIdealVoltage), getSettingValue(SettingsOptions::VoltageDiv)); // Run the QC seek again if we have drifted too much
#endif
}
