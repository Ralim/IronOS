//
// Created by laura on 24.04.23.
//

#include "SolderingCommon.h"
#include "OperatingModes.h"
#include "Types.h"
#include "configuration.h"
#include "history.hpp"

extern bool heaterThermalRunaway;

void detailedPowerStatus() {
  if (OLED::getRotation()) {
    OLED::setCursor(0, 0);
  } else {
    OLED::setCursor(67, 0);
  }
  // Print wattage
  {
    uint32_t x10Watt = x10WattHistory.average();
    if (x10Watt > 999) {
      // If we exceed 99.9W we drop the decimal place to keep it all fitting
      OLED::print(SmallSymbolSpace, FontStyle::SMALL);
      OLED::printNumber(x10WattHistory.average() / 10, 3, FontStyle::SMALL);
    } else {
      OLED::printNumber(x10WattHistory.average() / 10, 2, FontStyle::SMALL);
      OLED::print(SmallSymbolDot, FontStyle::SMALL);
      OLED::printNumber(x10WattHistory.average() % 10, 1, FontStyle::SMALL);
    }
    OLED::print(SmallSymbolWatts, FontStyle::SMALL);
  }

  if (OLED::getRotation()) {
    OLED::setCursor(0, 8);
  } else {
    OLED::setCursor(67, 8);
  }
  printVoltage();
  OLED::print(SmallSymbolVolts, FontStyle::SMALL);
}

void basicSolderingStatus(bool boostModeOn) {
  OLED::setCursor(0, 0);
  // We switch the layout direction depending on the orientation of the oled
  if (OLED::getRotation()) {
    // battery
    gui_drawBatteryIcon();
    // Space out gap between battery <-> temp
    OLED::print(LargeSymbolSpace, FontStyle::LARGE);
    // Draw current tip temp
    gui_drawTipTemp(true, FontStyle::LARGE);

    // We draw boost arrow if boosting,
    // or else gap temp <-> heat indicator
    if (boostModeOn) {
      OLED::drawSymbol(2);
    } else {
      OLED::print(LargeSymbolSpace, FontStyle::LARGE);
    }

    // Draw heating/cooling symbols
    OLED::drawHeatSymbol(X10WattsToPWM(x10WattHistory.average()));
  } else {
    // Draw heating/cooling symbols
    OLED::drawHeatSymbol(X10WattsToPWM(x10WattHistory.average()));
    // We draw boost arrow if boosting,
    // or else gap temp <-> heat indicator
    if (boostModeOn) {
      OLED::drawSymbol(2);
    } else {
      OLED::print(LargeSymbolSpace, FontStyle::LARGE);
    }
    // Draw current tip temp
    gui_drawTipTemp(true, FontStyle::LARGE);
    // Space out gap between battery <-> temp
    OLED::print(LargeSymbolSpace, FontStyle::LARGE);

    gui_drawBatteryIcon();
  }
}

bool checkExitSoldering(void) {
#ifdef POW_DC
  // Undervoltage test
  if (checkForUnderVoltage()) {
    lastButtonTime = xTaskGetTickCount();
    return true;
  }
#endif

#ifdef ACCEL_EXITS_ON_MOVEMENT
  // If the accel works in reverse where movement will cause exiting the soldering mode
  if (getSettingValue(Sensitivity)) {
    if (lastMovementTime) {
      if (lastMovementTime > TICKS_SECOND * 10) {
        // If we have moved recently; in the last second
        // Then exit soldering mode

        // Movement occurred in last update
        if (((TickType_t)(xTaskGetTickCount() - lastMovementTime)) < (TickType_t)(TICKS_SECOND / 5)) {
          currentTempTargetDegC = 0;
          lastMovementTime      = 0;
          return true;
        }
      }
    }
  }
#endif

  // If we have tripped thermal runaway, turn off heater and show warning

  return false;
}

int8_t getPowerSourceNumber(void) {
  int8_t sourceNumber = 0;
  if (getIsPoweredByDCIN()) {
    sourceNumber = 0;
  } else {
    // We are not powered via DC, so want to display the appropriate state for PD or QC
    bool poweredbyPD        = false;
    bool pdHasVBUSConnected = false;
#ifdef POW_PD
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
  return sourceNumber;
}

// Returns temperature of the tip in *C/*F (based on user settings)
TemperatureType_t getTipTemp(void) {
#ifdef FILTER_DISPLAYED_TIP_TEMP
  static history<TemperatureType_t, FILTER_DISPLAYED_TIP_TEMP> Filter_Temp;
  TemperatureType_t                                            reading = getSettingValue(SettingsOptions::TemperatureInF) ? TipThermoModel::getTipInF() : TipThermoModel::getTipInC();
  Filter_Temp.update(reading);
  return Filter_Temp.average();

#else
  return getSettingValue(SettingsOptions::TemperatureInF) ? TipThermoModel::getTipInF() : TipThermoModel::getTipInC();
#endif
}
