
#include "OperatingModes.h"
#include "SolderingCommon.h"

OperatingMode gui_solderingProfileMode(const ButtonState buttons, guiContext *cxt) {
  /*
   * * Soldering
   * -> Main loop where we draw temp, and animations
   * --> Long hold back button to exit
   * --> Double button to exit
   */

  uint16_t tipTemp = 0;

  // If this is during init, start at preheat
  if (cxt->scratch_state.state1 == 0) {
    cxt->scratch_state.state5 = getSettingValue(SettingsOptions::ProfilePreheatTemp);
  }
  uint16_t phaseTicksPerDegree      = TICKS_SECOND / getSettingValue(SettingsOptions::ProfilePreheatSpeed);
  uint16_t profileCurrentTargetTemp = 0;

  switch (buttons) {
  case BUTTON_BOTH:
  case BUTTON_B_LONG:
    cxt->transitionMode = TransitionAnimation::Right;
    return OperatingMode::HomeScreen; // exit on back long hold
  case BUTTON_F_LONG:
  case BUTTON_F_SHORT:
  case BUTTON_B_SHORT:
  case BUTTON_NONE:
    // Not used yet
    break;
  default:
    break;
  }

  if (getSettingValue(SettingsOptions::TemperatureInF)) {
    tipTemp = TipThermoModel::getTipInF();
  } else {
    tipTemp = TipThermoModel::getTipInC();
  }
  // If time of entering is unknown; then we start now
  if (cxt->scratch_state.state3 == 0) {
    cxt->scratch_state.state3 = xTaskGetTickCount();
  }

  // if start temp is unknown (preheat), we're setting it now
  if (cxt->scratch_state.state6 == 0) {
    cxt->scratch_state.state6 = tipTemp;
    // if this is hotter than the preheat temperature, we should fail
    if (cxt->scratch_state.state6 >= 55) {
      warnUser(translatedString(Tr->TooHotToStartProfileWarning), buttons);
      return OperatingMode::HomeScreen;
    }
  }
  uint16_t phaseElapsedSeconds = (xTaskGetTickCount() - cxt->scratch_state.state3) / TICKS_SECOND;

  // have we finished this phase?
  if (phaseElapsedSeconds >= cxt->scratch_state.state2 && tipTemp == cxt->scratch_state.state5) {
    cxt->scratch_state.state1++;
    cxt->scratch_state.state6 = cxt->scratch_state.state5;
    cxt->scratch_state.state3 = xTaskGetTickCount();
    phaseElapsedSeconds       = 0;
    if (cxt->scratch_state.state1 > getSettingValue(SettingsOptions::ProfilePhases)) {
      // done with all phases, lets go to cooldown
      cxt->scratch_state.state2 = 0;
      cxt->scratch_state.state5 = 0;
      phaseTicksPerDegree       = TICKS_SECOND / getSettingValue(SettingsOptions::ProfileCooldownSpeed);
    } else {
      // set up next phase
      switch (cxt->scratch_state.state1) {
      case 1:
        cxt->scratch_state.state2 = getSettingValue(SettingsOptions::ProfilePhase1Duration);
        cxt->scratch_state.state5 = getSettingValue(SettingsOptions::ProfilePhase1Temp);
        break;
      case 2:
        cxt->scratch_state.state2 = getSettingValue(SettingsOptions::ProfilePhase2Duration);
        cxt->scratch_state.state5 = getSettingValue(SettingsOptions::ProfilePhase2Temp);
        break;
      case 3:
        cxt->scratch_state.state2 = getSettingValue(SettingsOptions::ProfilePhase3Duration);
        cxt->scratch_state.state5 = getSettingValue(SettingsOptions::ProfilePhase3Temp);
        break;
      case 4:
        cxt->scratch_state.state2 = getSettingValue(SettingsOptions::ProfilePhase4Duration);
        cxt->scratch_state.state5 = getSettingValue(SettingsOptions::ProfilePhase4Temp);
        break;
      case 5:
        cxt->scratch_state.state2 = getSettingValue(SettingsOptions::ProfilePhase5Duration);
        cxt->scratch_state.state5 = getSettingValue(SettingsOptions::ProfilePhase5Temp);
        break;
      default:
        break;
      }
      if (cxt->scratch_state.state6 < cxt->scratch_state.state5) {
        phaseTicksPerDegree = (cxt->scratch_state.state2 * TICKS_SECOND) / (cxt->scratch_state.state5 - cxt->scratch_state.state6);
      } else {
        phaseTicksPerDegree = (cxt->scratch_state.state2 * TICKS_SECOND) / (cxt->scratch_state.state6 - cxt->scratch_state.state5);
      }
    }
  }

  // cooldown phase done?
  if (cxt->scratch_state.state1 > getSettingValue(SettingsOptions::ProfilePhases)) {
    if (TipThermoModel::getTipInC() < 55) {
      // we're done, let the buzzer beep too
      setStatusLED(LED_STANDBY);
      if (cxt->scratch_state.state4 == 0) {
        setBuzzer(true);
        cxt->scratch_state.state4 = xTaskGetTickCount() + TICKS_SECOND / 3;
      }
    }
  }

  // determine current target temp
  if (cxt->scratch_state.state6 < cxt->scratch_state.state5) {
    if (profileCurrentTargetTemp < cxt->scratch_state.state5) {
      profileCurrentTargetTemp = cxt->scratch_state.state6 + ((xTaskGetTickCount() - cxt->viewEnterTime) / phaseTicksPerDegree);
    }
  } else {
    if (profileCurrentTargetTemp > cxt->scratch_state.state5) {
      profileCurrentTargetTemp = cxt->scratch_state.state6 - ((xTaskGetTickCount() - cxt->viewEnterTime) / phaseTicksPerDegree);
    }
  }

  // Draw in the screen details
  if (getSettingValue(SettingsOptions::DetailedSoldering)) {
    // print temperature
    if (OLED::getRotation()) {
      OLED::setCursor(48, 0);
    } else {
      OLED::setCursor(0, 0);
    }

    OLED::printNumber(tipTemp, 3, FontStyle::SMALL);
    OLED::print(SmallSymbolSlash, FontStyle::SMALL);
    OLED::printNumber(profileCurrentTargetTemp, 3, FontStyle::SMALL);

    if (getSettingValue(SettingsOptions::TemperatureInF)) {
      OLED::print(SmallSymbolDegF, FontStyle::SMALL);
    } else {
      OLED::print(SmallSymbolDegC, FontStyle::SMALL);
    }

    // print phase
    if (cxt->scratch_state.state1 > 0 && cxt->scratch_state.state1 <= getSettingValue(SettingsOptions::ProfilePhases)) {
      if (OLED::getRotation()) {
        OLED::setCursor(36, 0);
      } else {
        OLED::setCursor(55, 0);
      }
      OLED::printNumber(cxt->scratch_state.state1, 1, FontStyle::SMALL);
    }

    // print time progress / preheat / cooldown
    if (OLED::getRotation()) {
      OLED::setCursor(42, 8);
    } else {
      OLED::setCursor(0, 8);
    }

    if (cxt->scratch_state.state1 == 0) {
      OLED::print(translatedString(Tr->ProfilePreheatString), FontStyle::SMALL);
    } else if (cxt->scratch_state.state1 > getSettingValue(SettingsOptions::ProfilePhases)) {
      OLED::print(translatedString(Tr->ProfileCooldownString), FontStyle::SMALL);
    } else {
      OLED::printNumber(phaseElapsedSeconds / 60, 1, FontStyle::SMALL);
      OLED::print(SmallSymbolColon, FontStyle::SMALL);
      OLED::printNumber(phaseElapsedSeconds % 60, 2, FontStyle::SMALL, false);

      OLED::print(SmallSymbolSlash, FontStyle::SMALL);

      // blink if we can't keep up with the time goal
      if (phaseElapsedSeconds < cxt->scratch_state.state2 + 2 || (xTaskGetTickCount() / TICKS_SECOND) % 2 == 0) {
        OLED::printNumber(cxt->scratch_state.state2 / 60, 1, FontStyle::SMALL);
        OLED::print(SmallSymbolColon, FontStyle::SMALL);
        OLED::printNumber(cxt->scratch_state.state2 % 60, 2, FontStyle::SMALL, false);
      }
    }

    detailedPowerStatus();

  } else {
    basicSolderingStatus(false);
  }

  // Update the setpoints for the temperature
  if (getSettingValue(SettingsOptions::TemperatureInF)) {
    currentTempTargetDegC = TipThermoModel::convertFtoC(profileCurrentTargetTemp);
  } else {
    currentTempTargetDegC = profileCurrentTargetTemp;
  }

  if (checkExitSoldering() || (cxt->scratch_state.state4 != 0 && xTaskGetTickCount() >= cxt->scratch_state.state4)) {
    setBuzzer(false);
    return OperatingMode::HomeScreen;
  }
  if (heaterThermalRunaway) {
    currentTempTargetDegC = 0; // heater control off
    heaterThermalRunaway  = false;
    return OperatingMode::ThermalRunaway;
  }

  // Update LED status
  if (cxt->scratch_state.state1 == 0) {
    setStatusLED(LED_HEATING);
  } else if (cxt->scratch_state.state1 > getSettingValue(SettingsOptions::ProfilePhases)) {
    setStatusLED(LED_COOLING_STILL_HOT);
  } else {
    setStatusLED(LED_HOT);
  }
  return OperatingMode::SolderingProfile;
}
