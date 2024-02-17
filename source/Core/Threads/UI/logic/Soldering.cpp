
#include "OperatingModes.h"
#include "SolderingCommon.h"
// State 1 = button locking
// State 2 = boost mode
// State 3 = buzzer timer

OperatingMode handleSolderingButtons(const ButtonState buttons, guiContext *cxt) {
  if (cxt->scratch_state.state1 == 1) {
    // Buttons are currently locked
    if (buttons == BUTTON_F_LONG) {
      if (getSettingValue(SettingsOptions::BoostTemp) && (getSettingValue(SettingsOptions::LockingMode) == 1)) {
        cxt->scratch_state.state2 = 1;
      }
    } else if (buttons == BUTTON_BOTH_LONG) {
      // Unlocking
      if (warnUser(translatedString(Tr->UnlockingKeysString), buttons)) {
        cxt->scratch_state.state1 = 0;
      }
    } else if (buttons != BUTTON_NONE) {
      // Do nothing and display a lock warning
      warnUser(translatedString(Tr->WarningKeysLockedString), buttons);
    }
    return OperatingMode::Soldering;
  }
  // otherwise we are unlocked
  switch (buttons) {
  case BUTTON_NONE:
    cxt->scratch_state.state2 = 0;
    break;
  case BUTTON_BOTH:
  /*Fall through*/
  case BUTTON_B_LONG:
    cxt->transitionMode = TransitionAnimation::Right;
    return OperatingMode::HomeScreen;
  case BUTTON_F_LONG:
    // if boost mode is enabled turn it on
    if (getSettingValue(SettingsOptions::BoostTemp)) {
      cxt->scratch_state.state2 = 1;
    }
    break;
  case BUTTON_F_SHORT:
  case BUTTON_B_SHORT:
    cxt->transitionMode = TransitionAnimation::Left;
    return OperatingMode::TemperatureAdjust;
  case BUTTON_BOTH_LONG:
    if (getSettingValue(SettingsOptions::LockingMode) != 0) {
      // Lock buttons
      if (warnUser(translatedString(Tr->LockingKeysString), buttons)) {
        cxt->scratch_state.state1 = 1;
      }
    }
    break;
  default:
    break;
  }
  return OperatingMode::Soldering;
}
OperatingMode gui_solderingMode(const ButtonState buttons, guiContext *cxt) {
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

  // Update the setpoints for the temperature
  if (cxt->scratch_state.state2) {
    if (getSettingValue(SettingsOptions::TemperatureInF)) {
      currentTempTargetDegC = TipThermoModel::convertFtoC(getSettingValue(SettingsOptions::BoostTemp));
    } else {
      currentTempTargetDegC = (getSettingValue(SettingsOptions::BoostTemp));
    }
  } else {
    if (getSettingValue(SettingsOptions::TemperatureInF)) {
      currentTempTargetDegC = TipThermoModel::convertFtoC(getSettingValue(SettingsOptions::SolderingTemp));
    } else {
      currentTempTargetDegC = (getSettingValue(SettingsOptions::SolderingTemp));
    }
  }

  // Update status
  int error = currentTempTargetDegC - TipThermoModel::getTipInC();
  if (error >= -10 && error <= 10) {
    // converged
    if (!cxt->scratch_state.state5) {
      setBuzzer(true);
      cxt->scratch_state.state3 = xTaskGetTickCount() + TICKS_SECOND / 3;
      cxt->scratch_state.state5 = true;
    }
    setStatusLED(LED_HOT);
  } else {
    setStatusLED(LED_HEATING);
    cxt->scratch_state.state5 = false;
  }
  if (cxt->scratch_state.state3 != 0 && xTaskGetTickCount() >= cxt->scratch_state.state3) {
    setBuzzer(false);
  }

  // Draw in the screen details
  if (getSettingValue(SettingsOptions::DetailedSoldering)) {
    if (OLED::getRotation()) {
      OLED::setCursor(50, 0);
    } else {
      OLED::setCursor(-1, 0);
    }

    gui_drawTipTemp(true, FontStyle::LARGE);

    if (cxt->scratch_state.state2) { // Boost mode is on
      if (OLED::getRotation()) {
        OLED::setCursor(34, 0);
      } else {
        OLED::setCursor(50, 0);
      }
      OLED::print(LargeSymbolPlus, FontStyle::LARGE);
    } else {
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
      if (OLED::getRotation()) {
        OLED::setCursor(32, 8);
      } else {
        OLED::setCursor(47, 8);
      }
      OLED::print(PowerSourceNames[getPowerSourceNumber()], FontStyle::SMALL, 2);
    }

    detailedPowerStatus();

  } else {
    basicSolderingStatus(cxt->scratch_state.state2);
  }
  // Check if we should bail due to undervoltage for example
  if (checkExitSoldering()) {
    setBuzzer(false);
    cxt->transitionMode = TransitionAnimation::Right;
    return OperatingMode::HomeScreen;
  }
#ifdef NO_SLEEP_MODE

  if (shouldShutdown()) {
    // shutdown
    currentTempTargetDegC = 0;
    cxt->transitionMode   = TransitionAnimation::Right;
    return OperatingMode::HomeScreen;
  }
#endif
  if (shouldBeSleeping()) {
    return OperatingMode::Sleeping;
  }

  if (heaterThermalRunaway) {
    currentTempTargetDegC = 0; // heater control off
    heaterThermalRunaway  = false;
    cxt->transitionMode   = TransitionAnimation::Right;
    return OperatingMode::ThermalRunaway;
  }
  return handleSolderingButtons(buttons, cxt);
}
