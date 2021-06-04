/*
 * QC3.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */

// Quick charge 3.0 supporting functions
#include "QC3.h"
#include "BSP.h"
#include "Settings.h"
#include "cmsis_os.h"
#include "stdint.h"
enum QCState {
  NOT_STARTED = 0, // Have not checked
  QC_3        = 1,
  QC_2        = 2,
  NO_QC       = 3,

};
void QC_Seek9V() {
  QC_DNegZero_Six();
  QC_DPlusThree_Three();
}
void QC_Seek12V() {
  QC_DNegZero_Six();
  QC_DPlusZero_Six();
}
void QC_Seek20V() {
  QC_DNegThree_Three();
  QC_DPlusThree_Three();
}
void QC_SeekContMode() {
  QC_DNegThree_Three();
  QC_DPlusZero_Six();
}
void QC_SeekContPlus() {
  QC_SeekContMode();
  osDelay(30);
  QC_Seek20V();
  osDelay(10);
  QC_SeekContMode();
}
void QC_SeekContNeg() {
  QC_SeekContMode();
  osDelay(30);
  QC_Seek12V();
  osDelay(10);
  QC_SeekContMode();
}
QCState QCMode  = QCState::NOT_STARTED;
uint8_t QCTries = 0;
void    seekQC(int16_t Vx10, uint16_t divisor) {
  if (QCMode == QCState::NOT_STARTED)
    startQC(divisor);

  if (Vx10 < 45)
    return;
  if (xTaskGetTickCount() < TICKS_SECOND)
    return;
#ifdef POW_QC_20V
  if (Vx10 > 200)
    Vx10 = 200; // Cap max value at 20V
#else
  if (Vx10 > 130)
    Vx10 = 130; // Cap max value at 13V

#endif
  // Seek the QC to the Voltage given if this adapter supports continuous mode
  // try and step towards the wanted value

  // 1. Measure current voltage
  int16_t vStart     = getInputVoltageX10(divisor, 1);
  int     difference = Vx10 - vStart;

  // 2. calculate ideal steps (0.2V changes)

  int steps = difference / 2;
  if (QCMode == QCState::QC_3) {
    if (steps > -2 && steps < 2)
      return; // dont bother with small steps
    while (steps < 0) {
      QC_SeekContNeg();
      vTaskDelay(3 * TICKS_10MS);
      steps++;
    }
    while (steps > 0) {
      QC_SeekContPlus();
      vTaskDelay(3 * TICKS_10MS);
      steps--;
    }
    osDelay(100);
  }
#ifdef ENABLE_QC2
  // Re-measure
  /* Disabled due to nothing to test and code space of around 1k*/
  steps = vStart - getInputVoltageX10(divisor, 1);
  if (steps < 0)
    steps = -steps;
  if (steps > 4) {
    // No continuous mode, so QC2
    QCMode = QCState::QC_2;
    // Goto nearest
    if (Vx10 > 190) {
      // request 20V
      QC_Seek20V();
    } else if (Vx10 > 110) {
      // request 12V
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
  if (getInputVoltageX10(divisor, 1) > 80) {
    QCTries = 11;
    QCMode  = QCState::NO_QC;
    return;
  }
  if (QCTries > 10) {
    QCMode = QCState::NO_QC;
    return;
  }
  QCMode = QCState::NOT_STARTED;
  QC_Init_GPIO();

  // Tries to negotiate QC for 9V
  // This is a multiple step process.
  // 1. Set around 0.6V on D+ for 1.25 Seconds or so
  // 2. After this It should un-short D+->D- and instead add a 20k pulldown on
  // D-
  QC_DPlusZero_Six();

  // Delay 1.25 seconds
  uint8_t  enteredQC = 0;
  uint16_t exitTime  = systemSettings.QCNegotiationMode & 0x02 ? 130 : 200;
  for (uint16_t i = 0; i < exitTime && enteredQC == 0; i++) {
    vTaskDelay(TICKS_10MS); // 10mS pause
    if (i > 130) {
      if (QC_DM_PulledDown()) {
        enteredQC = 1;
      }
      if ((i == 140) && ((systemSettings.QCNegotiationMode & 0x01) == 0)) {
        // For some marginal QC chargers, we try adding a pulldown
        QC_DM_PullDown();
      }
    }
  }
  QC_DM_No_PullDown();
  if (enteredQC) {
    // We have a QC capable charger
    QC_Seek9V();
    QC_Post_Probe_En();
    QC_Seek9V();
    // Wait for frontend ADC to stabilise
    QCMode = QCState::QC_2;
    for (uint8_t i = 0; i < 10; i++) {
      if (getInputVoltageX10(divisor, 1) > 80) {
        // yay we have at least QC2.0 or QC3.0
        QCMode = QCState::QC_3; // We have at least QC2, pray for 3
        return;
      }
      vTaskDelay(TICKS_100MS); // 100mS
    }
    QCMode = QCState::NOT_STARTED;
    QCTries++;

  } else {
    // no QC
    QCTries++;
    QCMode = QCState::NO_QC;
  }
}

bool hasQCNegotiated() { return QCMode == QCState::QC_3 || QCMode == QCState::QC_2; }
