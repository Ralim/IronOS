
#include "OperatingModes.h"
#include "SolderingCommon.h"

extern OperatingMode currentMode;

void gui_solderingProfileMode() {
  /*
   * * Soldering (gui_solderingMode)
   * -> Main loop where we draw temp, and animations
   * PID control
   * --> Long hold back button to exit
   * --> Double button to exit
   */
  currentMode = OperatingMode::soldering;

  TickType_t buzzerEnd = 0;

  bool        waitForRelease = true;
  TickType_t  phaseStartTime = xTaskGetTickCount();

  uint16_t tipTemp      = 0;
  uint8_t  profilePhase = 0;

  uint16_t phaseElapsedSeconds      = 0;
  uint16_t phaseTotalSeconds        = 0;
  uint16_t phaseStartTemp           = 0;
  uint16_t phaseEndTemp             = getSettingValue(SettingsOptions::ProfilePreheatTemp);
  uint16_t phaseTicksPerDegree      = TICKS_SECOND / getSettingValue(SettingsOptions::ProfilePreheatSpeed);
  uint16_t profileCurrentTargetTemp = 0;

  for (;;) {
    ButtonState buttons = getButtonState();
    if (buttons) {
      if (waitForRelease) {
        buttons = BUTTON_NONE;
      }
    } else {
      waitForRelease = false;
    }

    switch (buttons) {
    case BUTTON_NONE:
      break;
    case BUTTON_BOTH:
    case BUTTON_B_LONG:
      return; // exit on back long hold
    case BUTTON_F_LONG:
    case BUTTON_F_SHORT:
    case BUTTON_B_SHORT:
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

      phaseStartTemp      = phaseEndTemp;
      phaseStartTime      = xTaskGetTickCount();
      phaseElapsedSeconds = 0;

      if (profilePhase > getSettingValue(SettingsOptions::ProfilePhases)) {
        // done with all phases, lets go to cooldown
        phaseTotalSeconds   = 0;
        phaseEndTemp        = 0;
        phaseTicksPerDegree = TICKS_SECOND / getSettingValue(SettingsOptions::ProfileCooldownSpeed);
      } else {
        // set up next phase
        switch(profilePhase) {
        case 1:
          phaseTotalSeconds = getSettingValue(SettingsOptions::ProfilePhase1Duration);
          phaseEndTemp      = getSettingValue(SettingsOptions::ProfilePhase1Temp);
          break;
        case 2:
          phaseTotalSeconds = getSettingValue(SettingsOptions::ProfilePhase2Duration);
          phaseEndTemp      = getSettingValue(SettingsOptions::ProfilePhase2Temp);
          break;
        case 3:
          phaseTotalSeconds = getSettingValue(SettingsOptions::ProfilePhase3Duration);
          phaseEndTemp      = getSettingValue(SettingsOptions::ProfilePhase3Temp);
          break;
        case 4:
          phaseTotalSeconds = getSettingValue(SettingsOptions::ProfilePhase4Duration);
          phaseEndTemp      = getSettingValue(SettingsOptions::ProfilePhase4Temp);
          break;
        case 5:
          phaseTotalSeconds = getSettingValue(SettingsOptions::ProfilePhase5Duration);
          phaseEndTemp      = getSettingValue(SettingsOptions::ProfilePhase5Temp);
          break;
        default:
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
        setStatusLED(LED_STANDBY);
        if (buzzerEnd == 0) {
          setBuzzer(true);
          buzzerEnd = xTaskGetTickCount() + TICKS_SECOND / 3;
        }
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

    OLED::clearScreen();
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
        if (phaseElapsedSeconds < phaseTotalSeconds + 2 || (xTaskGetTickCount() / TICKS_SECOND) % 2 == 0) {
          OLED::printNumber(phaseTotalSeconds / 60, 1, FontStyle::SMALL);
          OLED::print(SmallSymbolColon, FontStyle::SMALL);
          OLED::printNumber(phaseTotalSeconds % 60, 2, FontStyle::SMALL, false);
        }
      }

      detailedPowerStatus();

    } else {
      basicSolderingStatus(false);
    }

    OLED::refresh();
    // Update the setpoints for the temperature
    if (getSettingValue(SettingsOptions::TemperatureInF)) {
      currentTempTargetDegC = TipThermoModel::convertFtoC(profileCurrentTargetTemp);
    } else {
      currentTempTargetDegC = profileCurrentTargetTemp;
    }

    if (checkExitSoldering() || (buzzerEnd != 0 && xTaskGetTickCount() >= buzzerEnd)) {
      setBuzzer(false);
      return;
    }

    // Update LED status
    if (profilePhase == 0) {
      setStatusLED(LED_HEATING);
    } else if (profilePhase > getSettingValue(SettingsOptions::ProfilePhases)) {
      setStatusLED(LED_COOLING_STILL_HOT);
    } else {
      setStatusLED(LED_HOT);
    }

    // slow down ui update rate
    GUIDelay();
  }
}
