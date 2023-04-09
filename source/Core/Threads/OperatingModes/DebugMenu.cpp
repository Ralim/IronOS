#include "OperatingModes.h"
extern osThreadId    GUITaskHandle;
extern osThreadId    MOVTaskHandle;
extern osThreadId    PIDTaskHandle;
extern OperatingMode currentMode;

void showDebugMenu(void) {
  currentMode        = OperatingMode::debug;
  uint8_t     screen = 0;
  ButtonState b;
  for (;;) {
    OLED::clearScreen();                                     // Ensure the buffer starts clean
    OLED::setCursor(0, 0);                                   // Position the cursor at the 0,0 (top left)
    OLED::print(SmallSymbolVersionNumber, FontStyle::SMALL); // Print version number
    OLED::setCursor(0, 8);                                   // second line
    OLED::print(DebugMenu[screen], FontStyle::SMALL);
    switch (screen) {
    case 0: // Build Date
      break;
    case 1: // Device ID
    {
      uint64_t id = getDeviceID();
#ifdef DEVICE_HAS_VALIDATION_CODE
      // If device has validation code; then we want to take over both lines of the screen
      OLED::clearScreen();   // Ensure the buffer starts clean
      OLED::setCursor(0, 0); // Position the cursor at the 0,0 (top left)
      OLED::print(DebugMenu[screen], FontStyle::SMALL);
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
    {
      int sourceNumber = 0;
      if (getIsPoweredByDCIN()) {
        sourceNumber = 0;
      } else {
        // We are not powered via DC, so want to display the appropriate state for PD or QC
        bool poweredbyPD        = false;
        bool pdHasVBUSConnected = false;
#if POW_PD
        if (USBPowerDelivery::fusbPresent()) {
          // We are PD capable
          if (USBPowerDelivery::negotiationComplete()) {
            // We are powered via PD
            poweredbyPD = true;
#ifdef VBUS_MOD_TEST
            pdHasVBUSConnected = USBPowerDelivery::isVBUSConnected();
#endif
          }
        }
#endif
        if (poweredbyPD) {

          if (pdHasVBUSConnected) {
            sourceNumber = 2;
          } else {
            sourceNumber = 3;
          }
        } else {
          sourceNumber = 1;
        }
      }
      OLED::print(PowerSourceNames[sourceNumber], FontStyle::SMALL);
    } break;
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
      if (hallEffectStrength < 0)
        hallEffectStrength = -hallEffectStrength;
      OLED::printNumber(hallEffectStrength, 6, FontStyle::SMALL);
    } break;
#endif

    default:
      break;
    }

    OLED::refresh();
    b = getButtonState();
    if (b == BUTTON_B_SHORT)
      return;
    else if (b == BUTTON_F_SHORT) {
      screen++;
#ifdef HALL_SENSOR
      screen = screen % 17;
#else
      screen = screen % 16;
#endif
    }
    GUIDelay();
  }
}
