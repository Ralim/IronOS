
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

#ifdef PROFILE_SUPPORT
  bool        waitForRelease   = false;
  TickType_t  profileStartTime = 0;
  TickType_t  phaseStartTime   = 0;

  uint16_t tipTemp     = 0;
  uint8_t profilePhase = 0;

  uint16_t phaseElapsedSeconds      = 0;
  uint16_t phaseTotalSeconds        = 0;
  uint16_t phaseStartTemp           = 0;
  uint16_t phaseEndTemp             = 0;
  uint16_t phaseTicksPerDegree      = 0;
  uint16_t profileCurrentTargetTemp = 0;

  ButtonState buttons  = getButtonState();

  if (buttons != BUTTON_NONE) {
    // Soldering entered by long-pressing F button, profile mode
    waitForRelease = true;
    profileStartTime = xTaskGetTickCount();
    phaseStartTime = xTaskGetTickCount();
    phaseEndTemp = getSettingValue(SettingsOptions::ProfilePreheatTemp);
    phaseTicksPerDegree = TICKS_SECOND / getSettingValue(SettingsOptions::ProfilePreheatSpeed);
  }
#else
  ButtonState buttons;
#endif

  if (jumpToSleep) {
    if (gui_SolderingSleepingMode(jumpToSleep == 2, true) == 1) {
      lastButtonTime = xTaskGetTickCount();
      return; // If the function returns non-0 then exit
    }
  }
  for (;;) {
    buttons = getButtonState();
#ifdef PROFILE_SUPPORT
    if (buttons) {
      if (waitForRelease) buttons = BUTTON_NONE;
    } else {
      waitForRelease = false;
    }
#endif
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
#ifdef PROFILE_SUPPORT
        if (profileStartTime != 0) break; // no boost in profile mode
#endif
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
#ifdef PROFILE_SUPPORT
        if (profileStartTime != 0) break; // no manual temp change in profile mode
#endif
        // if boost mode is enabled turn it on
        if (getSettingValue(SettingsOptions::BoostTemp)) {
          boostModeOn = true;
          currentMode = OperatingMode::boost;
        }
        break;
      case BUTTON_F_SHORT:
      case BUTTON_B_SHORT: {
#ifdef PROFILE_SUPPORT
        if (profileStartTime != 0) break; // no manual temp change in profile mode
#endif
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

#ifdef PROFILE_SUPPORT
    if (profileStartTime != 0) {
      if (getSettingValue(SettingsOptions::TemperatureInF)) {
        tipTemp = TipThermoModel::getTipInF();
      } else {
        tipTemp = TipThermoModel::getTipInC();
      }

      // if start temp is unknown (preheat), we're setting it now
      if (phaseStartTemp == 0) {
        phaseStartTemp = tipTemp;
        // if this is hotter than the preheat temperature, we should fail
        if (phaseStartTemp >= 55) {
          warnUser(translatedString(Tr->TooHotToStartProfileWarning), 10 * TICKS_SECOND);
          return;
        }
      }

      phaseElapsedSeconds = (xTaskGetTickCount() - phaseStartTime) / TICKS_SECOND;

      // have we finished this phase?
      if (phaseElapsedSeconds >= phaseTotalSeconds && tipTemp == phaseEndTemp) {
        profilePhase++;
        phaseStartTemp = phaseEndTemp;
        phaseStartTime = xTaskGetTickCount();
        phaseElapsedSeconds = 0;
        if (profilePhase > getSettingValue(SettingsOptions::ProfilePhases)) {
          // done with all phases, lets go to cooldown
          phaseTotalSeconds = 0;
          phaseEndTemp = 0;
          phaseTicksPerDegree = TICKS_SECOND / getSettingValue(SettingsOptions::ProfileCooldownSpeed);
        } else {
          // set up next phase
          switch(profilePhase) {
            case 1:
              phaseTotalSeconds = getSettingValue(SettingsOptions::ProfilePhase1Duration);
              phaseEndTemp = getSettingValue(SettingsOptions::ProfilePhase1Temp);
              break;
            case 2:
              phaseTotalSeconds = getSettingValue(SettingsOptions::ProfilePhase2Duration);
              phaseEndTemp = getSettingValue(SettingsOptions::ProfilePhase2Temp);
              break;
            case 3:
              phaseTotalSeconds = getSettingValue(SettingsOptions::ProfilePhase3Duration);
              phaseEndTemp = getSettingValue(SettingsOptions::ProfilePhase3Temp);
              break;
            case 4:
              phaseTotalSeconds = getSettingValue(SettingsOptions::ProfilePhase4Duration);
              phaseEndTemp = getSettingValue(SettingsOptions::ProfilePhase4Temp);
              break;
            case 5:
              phaseTotalSeconds = getSettingValue(SettingsOptions::ProfilePhase5Duration);
              phaseEndTemp = getSettingValue(SettingsOptions::ProfilePhase5Temp);
              break;
          }
          if (phaseStartTemp < phaseEndTemp) {
            phaseTicksPerDegree = (phaseTotalSeconds * TICKS_SECOND) / (phaseEndTemp - phaseStartTemp);
          } else {
            phaseTicksPerDegree = (phaseTotalSeconds * TICKS_SECOND) / (phaseStartTemp - phaseEndTemp);
          }
        }
      }

      // cooldown phase done?
      if (profilePhase > getSettingValue(SettingsOptions::ProfilePhases)) {
        if (TipThermoModel::getTipInC() < 55) {
          // we're done, let the buzzer beep too
          setStatusLED(LED_STANDBY, true);
          return;
        }
      }

      // determine current target temp
      if (phaseStartTemp < phaseEndTemp) {
        if (profileCurrentTargetTemp < phaseEndTemp) {
          profileCurrentTargetTemp = phaseStartTemp + ((xTaskGetTickCount() - phaseStartTime) / phaseTicksPerDegree);
        }
      } else {
        if (profileCurrentTargetTemp > phaseEndTemp) {
          profileCurrentTargetTemp = phaseStartTemp - ((xTaskGetTickCount() - phaseStartTime) / phaseTicksPerDegree);
        }
      }
    }
#endif

    OLED::clearScreen();
    // Draw in the screen details
    if (getSettingValue(SettingsOptions::DetailedSoldering)) {
      if (OLED::getRotation()) {
        OLED::setCursor(50, 0);
      } else {
        OLED::setCursor(-1, 0);
      }

#ifdef PROFILE_SUPPORT
      if (profileStartTime != 0) {
        // print temperature
        if (OLED::getRotation()) {
          OLED::setCursor(48, 0);
        } else {
          OLED::setCursor(0, 0);
        }

        OLED::printNumber(tipTemp, 3, FontStyle::SMALL);
        OLED::print(SmallSymbolSlash, FontStyle::SMALL);
        OLED::printNumber(profileCurrentTargetTemp, 3, FontStyle::SMALL);

        if (getSettingValue(SettingsOptions::TemperatureInF))
          OLED::print(SmallSymbolDegF, FontStyle::SMALL);
        else
          OLED::print(SmallSymbolDegC, FontStyle::SMALL);

        // print phase
        if (profilePhase > 0 && profilePhase <= getSettingValue(SettingsOptions::ProfilePhases)) {
          if (OLED::getRotation()) {
            OLED::setCursor(36, 0);
          } else {
            OLED::setCursor(55, 0);
          }
          OLED::printNumber(profilePhase, 1, FontStyle::SMALL);
        }

        // print time progress / preheat / cooldown
        if (OLED::getRotation()) {
          OLED::setCursor(42, 8);
        } else {
          OLED::setCursor(0, 8);
        }

        if (profilePhase == 0) {
          OLED::print(translatedString(Tr->ProfilePreheatString), FontStyle::SMALL);
        } else if (profilePhase > getSettingValue(SettingsOptions::ProfilePhases)) {
          OLED::print(translatedString(Tr->ProfileCooldownString), FontStyle::SMALL);
        } else {
          OLED::printNumber(phaseElapsedSeconds / 60, 1, FontStyle::SMALL);
          OLED::print(SmallSymbolColon, FontStyle::SMALL);
          OLED::printNumber(phaseElapsedSeconds % 60, 2, FontStyle::SMALL, false);

          OLED::print(SmallSymbolSlash, FontStyle::SMALL);

          // blink if we can't keep up with the time goal
          if (phaseElapsedSeconds < phaseTotalSeconds+2 || (xTaskGetTickCount() / TICKS_SECOND) % 2 == 0) {
            OLED::printNumber(phaseTotalSeconds / 60, 1, FontStyle::SMALL);
            OLED::print(SmallSymbolColon, FontStyle::SMALL);
            OLED::printNumber(phaseTotalSeconds % 60, 2, FontStyle::SMALL, false);
          }
        }

      } else {
#endif
      gui_drawTipTemp(true, FontStyle::LARGE);
#ifdef PROFILE_SUPPORT
      }
#endif

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
#ifdef PROFILE_SUPPORT
    if (profileStartTime != 0) {
      if (getSettingValue(SettingsOptions::TemperatureInF)) {
        currentTempTargetDegC = TipThermoModel::convertFtoC(profileCurrentTargetTemp);
      } else {
        currentTempTargetDegC = profileCurrentTargetTemp;
      }
    } else
#endif
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
#ifdef PROFILE_SUPPORT
    if (profileStartTime == 0) {
#endif
      // Update LED status (manual mode)
      int error = currentTempTargetDegC - TipThermoModel::getTipInC();
      if (error >= -10 && error <= 10) {
        // converged
        setStatusLED(LED_HOT, true);
      } else {
        setStatusLED(LED_HEATING, false);
      }
#ifdef PROFILE_SUPPORT
    } else {
      // Update LED status (profile mode)
      if (profilePhase == 0) {
        setStatusLED(LED_HEATING, false);
      } else if (profilePhase > getSettingValue(SettingsOptions::ProfilePhases)) {
        setStatusLED(LED_COOLING_STILL_HOT, false);
      } else {
        setStatusLED(LED_HOT, false);
      }
    }
#endif
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
