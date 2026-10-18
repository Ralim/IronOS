#include "BSP.h"
#include "BSP_Power.h"
#include "FreeRTOS.h"
#include "Settings.h"
#include "Setup.h"
#include "configuration.h"
#include "task.h"

// The CH224K gives no status, so try 20, 15, 12 and 9 V until VBUS agrees with the request
static const uint8_t  CH224KLevels[]      = {20, 15, 12, 9};
static uint8_t        CH224KLevel         = 0;
static bool           CH224KSettled       = false;
static TickType_t     CH224KRequestedTick = 0;
static const uint16_t CH224KSettleTicks   = TICKS_SECOND;

bool CH224KVoltageSettled() { return CH224KSettled; }

void power_check() {
  if (CH224KSettled) {
    return;
  }
  const TickType_t now = xTaskGetTickCount();
  if (now - CH224KRequestedTick < CH224KSettleTicks) {
    return;
  }
  const uint16_t requestedX10 = CH224KLevels[CH224KLevel] * 10;
  const uint16_t inputX10     = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 1);
  // Accept the voltage if VBUS is within 15% of the request
  if (inputX10 * 100 >= requestedX10 * 85 || CH224KLevel == sizeof(CH224KLevels) - 1) {
    CH224KSettled = true;
    return;
  }
  CH224KLevel++;
  setCH224KVoltage(CH224KLevels[CH224KLevel]);
  CH224KRequestedTick = now;
}

// The CH224K does the PD negotiation, so IronOS sees a DC input
bool getIsPoweredByDCIN() { return true; }
