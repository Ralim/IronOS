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
#include "gd32vf103_libopt.h"

#ifdef POW_QC
void QC_DPlusZero_Six() {
  // pull down D+
  gpio_bit_reset(QC_DP_LOW_GPIO_Port, QC_DP_LOW_Pin);
}
void QC_DNegZero_Six() {
  gpio_bit_set(QC_DM_HIGH_GPIO_Port, QC_DM_HIGH_Pin);
  gpio_bit_reset(QC_DM_LOW_GPIO_Port, QC_DM_LOW_Pin);
}
void QC_DPlusThree_Three() {
  // pull up D+
  gpio_bit_set(QC_DP_LOW_GPIO_Port, QC_DP_LOW_Pin);
}
void QC_DNegThree_Three() {
  gpio_bit_set(QC_DM_LOW_GPIO_Port, QC_DM_LOW_Pin);
  gpio_bit_set(QC_DM_HIGH_GPIO_Port, QC_DM_HIGH_Pin);
}
void QC_DM_PullDown() { gpio_init(USB_DM_LOW_GPIO_Port, GPIO_MODE_IPD, GPIO_OSPEED_2MHZ, USB_DM_Pin); }
void QC_DM_No_PullDown() { gpio_init(USB_DM_LOW_GPIO_Port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, USB_DM_Pin); }
void QC_Init_GPIO() {
  // Setup any GPIO into the right states for QC
  // D+ pulldown as output
  gpio_init(QC_DP_LOW_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, QC_DP_LOW_Pin);
  // Make two D- pins floating
  QC_DM_PullDown();
}
void QC_Post_Probe_En() {
  // Make two D- pins outputs
  gpio_init(QC_DM_LOW_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, QC_DM_LOW_Pin);
  gpio_init(QC_DM_HIGH_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, QC_DM_HIGH_Pin);
}

uint8_t QC_DM_PulledDown() { return gpio_input_bit_get(USB_DM_LOW_GPIO_Port, USB_DM_Pin) == RESET ? 1 : 0; }
#endif
void QC_resync() {
#ifdef POW_QC
  seekQC(getSettingValue(SettingsOptions::QCIdealVoltage), getSettingValue(SettingsOptions::VoltageDiv)); // Run the QC seek again if we have drifted too much
#endif
}
