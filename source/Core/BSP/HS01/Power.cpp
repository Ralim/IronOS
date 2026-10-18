#include "BSP.h"
#include "BSP_Power.h"
#include "FreeRTOS.h"
#include "Settings.h"
#include "Setup.h"
#include "configuration.h"
#include "task.h"

// The CH224K reports nothing back, so step 20 -> 15 -> 12 -> 9 V until VBUS matches the request
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
  // Within 15% of the request means the charger gave it to us
  if (inputX10 * 100 >= requestedX10 * 85 || CH224KLevel == sizeof(CH224KLevels) - 1) {
    CH224KSettled = true;
    return;
  }
  CH224KLevel++;
  setCH224KVoltage(CH224KLevels[CH224KLevel]);
  CH224KRequestedTick = now;
}

// Treated as a DC input, the PD side is invisible to us
bool getIsPoweredByDCIN() { return true; }
