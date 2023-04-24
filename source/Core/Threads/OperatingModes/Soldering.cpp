
#include "OperatingModes.h"
#include "SolderingCommon.h"

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
  bool converged     = false;
  currentMode        = OperatingMode::soldering;

  TickType_t buzzerEnd = 0;

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

      detailedPowerStatus();

    } else {
      basicSolderingStatus(boostModeOn);
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

    if (checkExitSoldering()) {
      setBuzzer(false);
      return;
    }

    // Update status
    int error = currentTempTargetDegC - TipThermoModel::getTipInC();
    if (error >= -10 && error <= 10) {
      // converged
      if (!converged) {
        setBuzzer(true);
        buzzerEnd = xTaskGetTickCount() + TICKS_SECOND / 3;
        converged = true;
      }
      setStatusLED(LED_HOT);
    } else {
      setStatusLED(LED_HEATING);
      converged = false;
    }
    if (buzzerEnd != 0 && xTaskGetTickCount() >= buzzerEnd) {
      setBuzzer(false);
    }

    // slow down ui update rate
    GUIDelay();
  }
}

