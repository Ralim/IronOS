
#include "OperatingModes.h"

extern bool          heaterThermalRunaway;
extern OperatingMode currentMode;

void gui_solderingMode(uint8_t jumpToSleep) {
  /*
   * * Soldering (gui_solderingMode)
   * -> Main loop where we draw temp, and animations
   * --> User presses buttons and they goto the temperature adjust screen
   * ---> Display the current setpoint temperature
   * ---> Use buttons to change forward and back on temperature
   * ---> Both buttons or timeout for exiting
   * --> Long hold front button to enter boost mode
   * ---> Just temporarily sets the system into the alternate temperature for
   * PID control
   * --> Long hold back button to exit
   * --> Double button to exit
   * --> Long hold double button to toggle key lock
   */
  bool boostModeOn   = false;
  bool buttonsLocked = false;
  currentMode        = OperatingMode::soldering;

  if (jumpToSleep) {
    if (gui_SolderingSleepingMode(jumpToSleep == 2, true) == 1) {
      lastButtonTime = xTaskGetTickCount();
      return; // If the function returns non-0 then exit
    }
  }
  for (;;) {
    ButtonState buttons = getButtonState();
    if (buttonsLocked && (getSettingValue(SettingsOptions::LockingMode) != 0)) { // If buttons locked
      switch (buttons) {
      case BUTTON_NONE:
        boostModeOn = false;
        break;
      case BUTTON_BOTH_LONG:
        // Unlock buttons
        buttonsLocked = false;
        warnUser(translatedString(Tr->UnlockingKeysString), TICKS_SECOND);
        break;
      case BUTTON_F_LONG:
        // if boost mode is enabled turn it on
        if (getSettingValue(SettingsOptions::BoostTemp) && (getSettingValue(SettingsOptions::LockingMode) == 1)) {
          boostModeOn = true;
          currentMode = OperatingMode::boost;
        }
        break;
        // fall through
      case BUTTON_BOTH:
      case BUTTON_B_LONG:
      case BUTTON_F_SHORT:
      case BUTTON_B_SHORT:
        // Do nothing and display a lock warning
        warnUser(translatedString(Tr->WarningKeysLockedString), TICKS_SECOND / 2);
        break;
      default:
        break;
      }
    } else { // Button not locked
      switch (buttons) {
      case BUTTON_NONE:
        // stay
        boostModeOn = false;
        currentMode = OperatingMode::soldering;
        break;
      case BUTTON_BOTH:
      case BUTTON_B_LONG:
        return; // exit on back long hold
      case BUTTON_F_LONG:
        // if boost mode is enabled turn it on
        if (getSettingValue(SettingsOptions::BoostTemp)) {
          boostModeOn = true;
          currentMode = OperatingMode::boost;
        }
        break;
      case BUTTON_F_SHORT:
      case BUTTON_B_SHORT: {
        uint16_t oldTemp = getSettingValue(SettingsOptions::SolderingTemp);
        gui_solderingTempAdjust(); // goto adjust temp mode
        if (oldTemp != getSettingValue(SettingsOptions::SolderingTemp)) {
          saveSettings(); // only save on change
        }
      } break;
      case BUTTON_BOTH_LONG:
        if (getSettingValue(SettingsOptions::LockingMode) != 0) {
          // Lock buttons
          buttonsLocked = true;
          warnUser(translatedString(Tr->LockingKeysString), TICKS_SECOND);
        }
        break;
      default:
        break;
      }
    }
    // else we update the screen information

    OLED::clearScreen();

    // Draw in the screen details
    if (getSettingValue(SettingsOptions::DetailedSoldering)) {
      if (OLED::getRotation()) {
        OLED::setCursor(50, 0);
      } else {
        OLED::setCursor(-1, 0);
      }
      gui_drawTipTemp(true, FontStyle::LARGE);

#ifndef NO_SLEEP_MODE
      if (getSettingValue(SettingsOptions::Sensitivity) && getSettingValue(SettingsOptions::SleepTime)) {
        if (OLED::getRotation()) {
          OLED::setCursor(32, 0);
        } else {
          OLED::setCursor(47, 0);
        }
        printCountdownUntilSleep(getSleepTimeout());
      }
#endif

      if (boostModeOn) {
        if (OLED::getRotation()) {
          OLED::setCursor(38, 8);
        } else {
          OLED::setCursor(55, 8);
        }
        OLED::print(SmallSymbolPlus, FontStyle::SMALL);
      }

      if (OLED::getRotation()) {
        OLED::setCursor(0, 0);
      } else {
        OLED::setCursor(67, 0);
      }
      // Print wattage
      {
        uint32_t x10Watt = x10WattHistory.average();
        if (x10Watt > 999) { // If we exceed 99.9W we drop the decimal place to keep it all fitting
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
    } else {
      OLED::setCursor(0, 0);
      // We switch the layout direction depending on the orientation of the oled
      if (OLED::getRotation()) {
        // battery
        gui_drawBatteryIcon();
        OLED::print(LargeSymbolSpace, FontStyle::LARGE); // Space out gap between battery <-> temp
        gui_drawTipTemp(true, FontStyle::LARGE);         // Draw current tip temp

        // We draw boost arrow if boosting, or else gap temp <-> heat
        // indicator
        if (boostModeOn)
          OLED::drawSymbol(2);
        else
          OLED::print(LargeSymbolSpace, FontStyle::LARGE);

        // Draw heating/cooling symbols
        OLED::drawHeatSymbol(X10WattsToPWM(x10WattHistory.average()));
      } else {
        // Draw heating/cooling symbols
        OLED::drawHeatSymbol(X10WattsToPWM(x10WattHistory.average()));
        // We draw boost arrow if boosting, or else gap temp <-> heat
        // indicator
        if (boostModeOn)
          OLED::drawSymbol(2);
        else
          OLED::print(LargeSymbolSpace, FontStyle::LARGE);
        gui_drawTipTemp(true, FontStyle::LARGE); // Draw current tip temp

        OLED::print(LargeSymbolSpace, FontStyle::LARGE); // Space out gap between battery <-> temp

        gui_drawBatteryIcon();
      }
    }
    OLED::refresh();
    // Update the setpoints for the temperature
    if (boostModeOn) {
      if (getSettingValue(SettingsOptions::TemperatureInF))
        currentTempTargetDegC = TipThermoModel::convertFtoC(getSettingValue(SettingsOptions::BoostTemp));
      else {
        currentTempTargetDegC = (getSettingValue(SettingsOptions::BoostTemp));
      }
    } else {
      if (getSettingValue(SettingsOptions::TemperatureInF))
        currentTempTargetDegC = TipThermoModel::convertFtoC(getSettingValue(SettingsOptions::SolderingTemp));
      else {
        currentTempTargetDegC = (getSettingValue(SettingsOptions::SolderingTemp));
      }
    }

#ifdef POW_DC
    // Undervoltage test
    if (checkForUnderVoltage()) {
      lastButtonTime = xTaskGetTickCount();
      return;
    }
#endif

#ifdef ACCEL_EXITS_ON_MOVEMENT
    // If the accel works in reverse where movement will cause exiting the soldering mode
    if (getSettingValue(SettingsOptions::Sensitivity)) {
      if (lastMovementTime) {
        if (lastMovementTime > TICKS_SECOND * 10) {
          // If we have moved recently; in the last second
          // Then exit soldering mode

          if (((TickType_t)(xTaskGetTickCount() - lastMovementTime)) < (TickType_t)(TICKS_SECOND)) {
            currentTempTargetDegC = 0;
            return;
          }
        }
      }
    }
#endif
#ifdef NO_SLEEP_MODE
    // No sleep mode, but still want shutdown timeout

    if (shouldShutdown()) {
      // shutdown
      currentTempTargetDegC = 0;
      return; // we want to exit soldering mode
    }
#endif
    if (shouldBeSleeping(false)) {
      if (gui_SolderingSleepingMode(false, false)) {
        return; // If the function returns non-0 then exit
      }
    }
    // Update LED status
    int error = currentTempTargetDegC - TipThermoModel::getTipInC();
    if (error >= -10 && error <= 10) {
      // converged
      setStatusLED(LED_HOT);
    } else {
      setStatusLED(LED_HEATING);
    }
    // If we have tripped thermal runaway, turn off heater and show warning
    if (heaterThermalRunaway) {
      currentTempTargetDegC = 0; // heater control off
      warnUser(translatedString(Tr->WarningThermalRunaway), 10 * TICKS_SECOND);
      heaterThermalRunaway = false;
      return;
    }
    // slow down ui update rate
    GUIDelay();
  }
}
