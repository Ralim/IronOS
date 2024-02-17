#include "OperatingModes.h"
extern osThreadId GUITaskHandle;
extern osThreadId MOVTaskHandle;
extern osThreadId PIDTaskHandle;

OperatingMode showDebugMenu(const ButtonState buttons, guiContext *cxt) {
  OLED::setCursor(0, 0);                                   // Position the cursor at the 0,0 (top left)
  OLED::print(SmallSymbolVersionNumber, FontStyle::SMALL); // Print version number
  OLED::setCursor(0, 8);                                   // second line
  OLED::print(DebugMenu[cxt->scratch_state.state1], FontStyle::SMALL);
  switch (cxt->scratch_state.state1) {
  case 0: // Build Date
    break;
  case 1: // Device ID
  {
    uint64_t id = getDeviceID();
#ifdef DEVICE_HAS_VALIDATION_CODE
    // If device has validation code; then we want to take over both lines of the screen
    OLED::clearScreen();   // Ensure the buffer starts clean
    OLED::setCursor(0, 0); // Position the cursor at the 0,0 (top left)
    OLED::print(DebugMenu[cxt->scratch_state.state1], FontStyle::SMALL);
    OLED::drawHex(getDeviceValidation(), FontStyle::SMALL, 8);
    OLED::setCursor(0, 8); // second line
#endif
    OLED::drawHex((uint32_t)(id >> 32), FontStyle::SMALL, 8);
    OLED::drawHex((uint32_t)(id & 0xFFFFFFFF), FontStyle::SMALL, 8);
  } break;
  case 2: // ACC Type
    OLED::print(AccelTypeNames[(int)DetectedAccelerometerVersion], FontStyle::SMALL);
    break;
  case 3: // Power Negotiation Status
    OLED::print(PowerSourceNames[getPowerSourceNumber()], FontStyle::SMALL);
    break;
  case 4: // Input Voltage
    printVoltage();
    break;
  case 5: // Temp in °C
    OLED::printNumber(TipThermoModel::getTipInC(), 6, FontStyle::SMALL);
    break;
  case 6: // Handle Temp in °C
    OLED::printNumber(getHandleTemperature(0) / 10, 6, FontStyle::SMALL);
    OLED::print(SmallSymbolDot, FontStyle::SMALL);
    OLED::printNumber(getHandleTemperature(0) % 10, 1, FontStyle::SMALL);
    break;
  case 7: // Max Temp Limit in °C
    OLED::printNumber(TipThermoModel::getTipMaxInC(), 6, FontStyle::SMALL);
    break;
  case 8: // System Uptime
    OLED::printNumber(xTaskGetTickCount() / TICKS_100MS, 8, FontStyle::SMALL);
    break;
  case 9: // Movement Timestamp
    OLED::printNumber(lastMovementTime / TICKS_100MS, 8, FontStyle::SMALL);
    break;
  case 10:                                                              // Tip Resistance in Ω
    OLED::printNumber(getTipResistanceX10() / 10, 6, FontStyle::SMALL); // large to pad over so that we cover ID left overs
    OLED::print(SmallSymbolDot, FontStyle::SMALL);
    OLED::printNumber(getTipResistanceX10() % 10, 1, FontStyle::SMALL);
    break;
  case 11: // Raw Tip in µV
    OLED::printNumber(TipThermoModel::convertTipRawADCTouV(getTipRawTemp(0), true), 8, FontStyle::SMALL);
    break;
  case 12: // Tip Cold Junction Compensation Offset in µV
    OLED::printNumber(getSettingValue(SettingsOptions::CalibrationOffset), 8, FontStyle::SMALL);
    break;
  case 13: // High Water Mark for GUI
    OLED::printNumber(uxTaskGetStackHighWaterMark(GUITaskHandle), 8, FontStyle::SMALL);
    break;
  case 14: // High Water Mark for Movement Task
    OLED::printNumber(uxTaskGetStackHighWaterMark(MOVTaskHandle), 8, FontStyle::SMALL);
    break;
  case 15: // High Water Mark for PID Task
    OLED::printNumber(uxTaskGetStackHighWaterMark(PIDTaskHandle), 8, FontStyle::SMALL);
    break;
    break;
#ifdef HALL_SENSOR
  case 16: // Raw Hall Effect Value
  {
    int16_t hallEffectStrength = getRawHallEffect();
    if (hallEffectStrength < 0) {
      hallEffectStrength = -hallEffectStrength;
    }
    OLED::printNumber(hallEffectStrength, 6, FontStyle::SMALL);
  } break;
#endif

  default:
    break;
  }

  if (buttons == BUTTON_B_SHORT) {
    cxt->transitionMode = TransitionAnimation::Down;
    return OperatingMode::HomeScreen;
  } else if (buttons == BUTTON_F_SHORT) {
    cxt->scratch_state.state1++;
#ifdef HALL_SENSOR
    cxt->scratch_state.state1 = cxt->scratch_state.state1 % 17;
#else
    cxt->scratch_state.state1 = cxt->scratch_state.state1 % 16;
#endif
  }
  return OperatingMode::DebugMenuReadout; // Stay in debug menu
}
