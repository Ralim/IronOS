/*
 * gui.cpp
 *
 *  Created on: 3Sep.,2017
 *      Author: Ben V. Brown
 */

#include "gui.hpp"
#include "../../configuration.h"
#include "Buttons.hpp"
#include "TipThermoModel.h"
#include "Translation.h"
#include "cmsis_os.h"
#include "main.hpp"
#include "string.h"

void gui_Menu(const menuitem *menu);

#ifdef POW_DC
static bool settings_setInputVRange(void);
static void settings_displayInputVRange(void);
static bool settings_setInputMinVRange(void);
static void settings_displayInputMinVRange(void);
#endif
#ifdef POW_QC
static bool settings_setQCInputV(void);
static void settings_displayQCInputV(void);
#endif
static bool settings_setSleepTemp(void);
static void settings_displaySleepTemp(void);
static bool settings_setSleepTime(void);
static void settings_displaySleepTime(void);
static bool settings_setShutdownTime(void);
static void settings_displayShutdownTime(void);
static bool settings_setSensitivity(void);
static void settings_displaySensitivity(void);
static bool settings_setTempF(void);
static void settings_displayTempF(void);
static bool settings_setAdvancedSolderingScreens(void);
static void settings_displayAdvancedSolderingScreens(void);
static bool settings_setAdvancedIDLEScreens(void);
static void settings_displayAdvancedIDLEScreens(void);
static bool settings_setScrollSpeed(void);
static void settings_displayScrollSpeed(void);
static bool settings_setPowerLimit(void);
static void settings_displayPowerLimit(void);
static bool settings_setDisplayRotation(void);
static void settings_displayDisplayRotation(void);
static bool settings_setBoostTemp(void);
static void settings_displayBoostTemp(void);
static bool settings_setAutomaticStartMode(void);
static void settings_displayAutomaticStartMode(void);
static bool settings_setLockingMode(void);
static void settings_displayLockingMode(void);
static bool settings_setCoolingBlinkEnabled(void);
static void settings_displayCoolingBlinkEnabled(void);
static bool settings_setResetSettings(void);
static void settings_displayResetSettings(void);
static bool settings_setCalibrate(void);
static void settings_displayCalibrate(void);
// static bool settings_setTipGain(void);
// static void settings_displayTipGain(void);
static bool settings_setCalibrateVIN(void);
static void settings_displayCalibrateVIN(void);
static void settings_displayReverseButtonTempChangeEnabled(void);
static bool settings_setReverseButtonTempChangeEnabled(void);
static void settings_displayTempChangeShortStep(void);
static bool settings_setTempChangeShortStep(void);
static void settings_displayTempChangeLongStep(void);
static bool settings_setTempChangeLongStep(void);
static void settings_displayPowerPulse(void);
static bool settings_setPowerPulse(void);
static void settings_displayAnimationSpeed(void);
static bool settings_setAnimationSpeed(void);
static void settings_displayAnimationLoop(void);
static bool settings_setAnimationLoop(void);
#ifdef HALL_SENSOR
static void settings_displayHallEffect(void);
static bool settings_setHallEffect(void);
#endif
// Menu functions
static void settings_displayPowerMenu(void);
static bool settings_enterPowerMenu(void);
static void settings_displaySolderingMenu(void);
static bool settings_enterSolderingMenu(void);
static void settings_displayPowerSavingMenu(void);
static bool settings_enterPowerSavingMenu(void);
static void settings_displayUIMenu(void);
static bool settings_enterUIMenu(void);
static void settings_displayAdvancedMenu(void);
static bool settings_enterAdvancedMenu(void);
/*
 * Root Settings Menu
 *
 * Power Menu
 *  Power Source
 *
 * Soldering
 * 	Boost Mode Enabled
 * 	Boost Mode Temp
 * 	Auto Start
 *  Temp change short step
 *  Temp change long step
 *	Locking Mode

 *
 * Power Saving
 * 	Sleep Temp
 * 	Sleep Time
 * 	Shutdown Time
 * 	Motion Sensitivity
 *
 * UI
 *  // Language
 *  Scrolling Speed
 *  Temperature Unit
 *  Display orientation
 *  Cooldown blink
 *  Reverse Temp change buttons + -
 *
 * Advanced
 *  Enable Power Limit
 *  Power Limit
 *  Detailed IDLE
 *  Detailed Soldering
 *  Logo Time
 *  Calibrate Temperature
 *  Calibrate Input V
 *  Reset Settings
 *
 */
const menuitem rootSettingsMenu[]{
/*
 * Power Menu
 * Soldering Menu
 * Power Saving Menu
 * UI Menu
 * Advanced Menu
 * Exit
 */
    {(const char *)NULL, settings_enterPowerMenu, settings_displayPowerMenu},         /*Power*/
    {(const char *)NULL, settings_enterSolderingMenu, settings_displaySolderingMenu}, /*Soldering*/
    {(const char *)NULL, settings_enterPowerSavingMenu, settings_displayPowerSavingMenu},         /*Sleep Options Menu*/
    {(const char *)NULL, settings_enterUIMenu, settings_displayUIMenu},               /*UI Menu*/
    {(const char *)NULL, settings_enterAdvancedMenu, settings_displayAdvancedMenu},   /*Advanced Menu*/
    {NULL, NULL, NULL}                                                                // end of menu marker. DO NOT REMOVE
};

const menuitem powerMenu[] = {
    /*
    * Power Source
     */
#ifdef POW_DC
    {(const char *)SettingsDescriptions[0], settings_setInputVRange, settings_displayInputVRange},          /*Voltage input*/
    {(const char *)SettingsDescriptions[28], settings_setInputMinVRange, settings_displayInputMinVRange},   /*Minimum voltage input*/
#endif
#ifdef POW_QC
    {(const char *)SettingsDescriptions[19], settings_setQCInputV, settings_displayQCInputV},               /*Voltage input*/
#endif
    {NULL, NULL, NULL}                                                                                              // end of menu marker. DO NOT REMOVE
};
const menuitem solderingMenu[] = {
    /*
     * Boost Mode Enabled
     * 	Boost Mode Temp
     * 	Auto Start
     *  Temp change short step
     *  Temp change long step
     */
    {(const char *)SettingsDescriptions[8], settings_setBoostTemp, settings_displayBoostTemp},                      /*Boost Temp*/
    {(const char *)SettingsDescriptions[9], settings_setAutomaticStartMode, settings_displayAutomaticStartMode},    /*Auto start*/
    {(const char *)SettingsDescriptions[22], settings_setTempChangeShortStep, settings_displayTempChangeShortStep}, /*Temp change short step*/
    {(const char *)SettingsDescriptions[23], settings_setTempChangeLongStep, settings_displayTempChangeLongStep},   /*Temp change long step*/
    {(const char *)SettingsDescriptions[27], settings_setLockingMode, settings_displayLockingMode},                 /*Locking Mode*/
    {NULL, NULL, NULL}                                                                                              // end of menu marker. DO NOT REMOVE
};
const menuitem UIMenu[] = {
    /*
     // Language
     *  Scrolling Speed
     *  Temperature Unit
     *  Display orientation
     *  Cooldown blink
     *  Reverse Temp change buttons + -
     */
    {(const char *)SettingsDescriptions[5], settings_setTempF,
     settings_displayTempF}, /* Temperature units, this has to be the first element in the array to work with the logic in settings_enterUIMenu() */
    {(const char *)SettingsDescriptions[7], settings_setDisplayRotation, settings_displayDisplayRotation},                                /*Display Rotation*/
    {(const char *)SettingsDescriptions[10], settings_setCoolingBlinkEnabled, settings_displayCoolingBlinkEnabled},                       /*Cooling blink warning*/
    {(const char *)SettingsDescriptions[15], settings_setScrollSpeed, settings_displayScrollSpeed},                                       /*Scroll Speed for descriptions*/
    {(const char *)SettingsDescriptions[21], settings_setReverseButtonTempChangeEnabled, settings_displayReverseButtonTempChangeEnabled}, /* Reverse Temp change buttons + - */
    {(const char *)SettingsDescriptions[30], settings_setAnimationSpeed, settings_displayAnimationSpeed},                                 /*Animation Speed adjustment */
    {(const char *)SettingsDescriptions[29], settings_setAnimationLoop, settings_displayAnimationLoop},                                   /*Animation Loop switch */
    {NULL, NULL, NULL}                                                                                                                    // end of menu marker. DO NOT REMOVE
};
const menuitem PowerSavingMenu[] = {
    /*
     * Sleep Temp
     * 	Sleep Time
     * 	Shutdown Time
     * 	Motion Sensitivity
     */
    {(const char *)SettingsDescriptions[1], settings_setSleepTemp, settings_displaySleepTemp},       /*Sleep Temp*/
    {(const char *)SettingsDescriptions[2], settings_setSleepTime, settings_displaySleepTime},       /*Sleep Time*/
    {(const char *)SettingsDescriptions[3], settings_setShutdownTime, settings_displayShutdownTime}, /*Shutdown Time*/
    {(const char *)SettingsDescriptions[4], settings_setSensitivity, settings_displaySensitivity},   /* Motion Sensitivity*/
#ifdef HALL_SENSOR
    {(const char *)SettingsDescriptions[26], settings_setHallEffect, settings_displayHallEffect}, /* HallEffect Sensitivity*/
#endif
    {NULL, NULL, NULL} // end of menu marker. DO NOT REMOVE
};
const menuitem advancedMenu[] = {

    /*
     *  Power limit
     *  Detailed IDLE
     *  Detailed Soldering
     *  Calibrate Temperature
     *  Calibrate Input V
     *  Reset Settings
     *  Power Pulse
     *  Animation Loop
     *  Animation Speed
     */
    {(const char *)SettingsDescriptions[20], settings_setPowerLimit, settings_displayPowerLimit},                             /*Power limit*/
    {(const char *)SettingsDescriptions[6], settings_setAdvancedIDLEScreens, settings_displayAdvancedIDLEScreens},            /* Advanced idle screen*/
    {(const char *)SettingsDescriptions[14], settings_setAdvancedSolderingScreens, settings_displayAdvancedSolderingScreens}, /* Advanced soldering screen*/
    {(const char *)SettingsDescriptions[12], settings_setResetSettings, settings_displayResetSettings},                       /*Resets settings*/
    {(const char *)SettingsDescriptions[11], settings_setCalibrate, settings_displayCalibrate},                               /*Calibrate tip*/
    {(const char *)SettingsDescriptions[13], settings_setCalibrateVIN, settings_displayCalibrateVIN},                         /*Voltage input cal*/
    {(const char *)SettingsDescriptions[24], settings_setPowerPulse, settings_displayPowerPulse},                             /*Power Pulse adjustment */
    //{ (const char *) SettingsDescriptions[25], settings_setTipGain, settings_displayTipGain }, /*TipGain*/
    {NULL, NULL, NULL} // end of menu marker. DO NOT REMOVE
};

static void printShortDescriptionDoubleLine(uint32_t shortDescIndex) {
  if (SettingsShortNames[shortDescIndex][0][0] == '\x00') {
    // Empty first line means that this uses large font (for CJK).
    OLED::setFont(0);
    OLED::setCharCursor(0, 0);
    OLED::print(SettingsShortNames[shortDescIndex][1]);
  } else {
    OLED::setFont(1);
    OLED::setCharCursor(0, 0);
    OLED::print(SettingsShortNames[shortDescIndex][0]);
    OLED::setCharCursor(0, 1);
    OLED::print(SettingsShortNames[shortDescIndex][1]);
  }
}

/**
 * Prints two small lines (or one line for CJK) of short description
 * and prepares cursor in big font after it.
 * @param shortDescIndex Index to of short description.
 * @param cursorCharPosition Custom cursor char position to set after printing
 * description.
 */
static void printShortDescription(uint32_t shortDescIndex, uint16_t cursorCharPosition) {
  // print short description (default single line, explicit double line)
  printShortDescriptionDoubleLine(shortDescIndex);

  // prepare cursor for value
  OLED::setFont(0);
  OLED::setCharCursor(cursorCharPosition, 0);
  // make room for scroll indicator
  OLED::setCursor(OLED::getCursorX() - 2, 0);
}

static int userConfirmation(const char *message) {
  uint16_t messageWidth = FONT_12_WIDTH * (strlen(message) + 7);
  uint32_t messageStart = xTaskGetTickCount();

  OLED::setFont(0);
  OLED::setCursor(0, 0);
  int16_t lastOffset = -1;
  bool    lcdRefresh = true;

  for (;;) {
    int16_t messageOffset = ((xTaskGetTickCount() - messageStart) / (systemSettings.descriptionScrollSpeed == 1 ? 10 : 20));
    messageOffset %= messageWidth; // Roll around at the end

    if (lastOffset != messageOffset) {
      OLED::clearScreen();

      //^ Rolling offset based on time
      OLED::setCursor((OLED_WIDTH - messageOffset), 0);
      OLED::print(message);
      lastOffset = messageOffset;
      lcdRefresh = true;
    }

    ButtonState buttons = getButtonState();
    switch (buttons) {
    case BUTTON_F_SHORT:
      // User confirmed
      return 1;

    case BUTTON_NONE:
      break;
    default:
    case BUTTON_BOTH:
    case BUTTON_B_SHORT:
    case BUTTON_F_LONG:
    case BUTTON_B_LONG:
      return 0;
    }

    if (lcdRefresh) {
      OLED::refresh();
      osDelay(40);
      lcdRefresh = false;
    }
  }
  return 0;
}
#ifdef POW_DC
static bool settings_setInputVRange(void) {
  systemSettings.minDCVoltageCells = (systemSettings.minDCVoltageCells + 1) % 5;
  if (systemSettings.minDCVoltageCells == 1 && systemSettings.minVoltageCells < 30)
    systemSettings.minVoltageCells = 30;
  return systemSettings.minDCVoltageCells == 4;
}

static void settings_displayInputVRange(void) {
  printShortDescription(0, 6);

  if (systemSettings.minDCVoltageCells) {
    OLED::printNumber(2 + systemSettings.minDCVoltageCells, 1);
    OLED::print(SymbolCellCount);
  } else {
    OLED::print(SymbolDC);
  }
}

static bool settings_setInputMinVRange(void) {
  systemSettings.minVoltageCells = (systemSettings.minVoltageCells + 1) % 38;
  if (systemSettings.minDCVoltageCells == 1 && systemSettings.minVoltageCells < 30)
    systemSettings.minVoltageCells = 30;
  else if(systemSettings.minVoltageCells < 24)
    systemSettings.minVoltageCells = 24;
  return systemSettings.minVoltageCells == 37;
}

static void settings_displayInputMinVRange(void) {
  if (systemSettings.minDCVoltageCells) {
    printShortDescription(28, 4);
    OLED::printNumber(systemSettings.minVoltageCells / 10, 2);
    OLED::print(SymbolDot);
    OLED::printNumber(systemSettings.minVoltageCells % 10, 1);
  } else {
    printShortDescription(28, 5);
    OLED::print(SettingNAChar);
  }
}
#endif
#ifdef POW_QC
static bool settings_setQCInputV(void) {
#ifdef POW_QC_20V
  systemSettings.QCIdealVoltage = (systemSettings.QCIdealVoltage + 1) % 3;
  return systemSettings.QCIdealVoltage == 2;
#else
  systemSettings.QCIdealVoltage = (systemSettings.QCIdealVoltage + 1) % 2;
  return systemSettings.QCIdealVoltage == 1;
#endif
}

static void settings_displayQCInputV(void) {
  printShortDescription(19, 5);
  // 0 = 9V, 1=12V, 2=20V (Fixed Voltages)
  // These are only used in QC modes
  switch (systemSettings.QCIdealVoltage) {
  case 0:
    OLED::printNumber(9, 2);
    OLED::print(SymbolVolts);
    break;
  case 1:
    OLED::printNumber(12, 2);
    OLED::print(SymbolVolts);
    break;
  case 2:
    OLED::printNumber(20, 2);
    OLED::print(SymbolVolts);
    break;
  default:
    break;
  }
}

#endif
static bool settings_setSleepTemp(void) {
  // If in C, 10 deg, if in F 20 deg
  if (systemSettings.temperatureInF) {
    systemSettings.SleepTemp += 20;
    if (systemSettings.SleepTemp > 580)
      systemSettings.SleepTemp = 60;
    return systemSettings.SleepTemp == 580;
  } else {
    systemSettings.SleepTemp += 10;
    if (systemSettings.SleepTemp > 300)
      systemSettings.SleepTemp = 10;
    return systemSettings.SleepTemp == 300;
  }
}

static void settings_displaySleepTemp(void) {
  printShortDescription(1, 5);
  OLED::printNumber(systemSettings.SleepTemp, 3);
}

static bool settings_setSleepTime(void) {
  systemSettings.SleepTime++; // Go up 1 minute at a time
  if (systemSettings.SleepTime >= 16) {
    systemSettings.SleepTime = 0; // can't set time over 10 mins
  }
  // Remember that ^ is the time of no movement
  if (DetectedAccelerometerVersion == NO_DETECTED_ACCELEROMETER)
    systemSettings.SleepTime = 0; // Disable sleep on no accel
  return systemSettings.SleepTime == 15;
}

static void settings_displaySleepTime(void) {
  printShortDescription(2, 5);
  if (systemSettings.SleepTime == 0) {
    OLED::print(OffString);
  } else if (systemSettings.SleepTime < 6) {
    OLED::printNumber(systemSettings.SleepTime * 10, 2);
    OLED::print(SymbolSeconds);
  } else {
    OLED::printNumber(systemSettings.SleepTime - 5, 2);
    OLED::print(SymbolMinutes);
  }
}

static bool settings_setShutdownTime(void) {
  systemSettings.ShutdownTime++;
  if (systemSettings.ShutdownTime > 60) {
    systemSettings.ShutdownTime = 0; // wrap to off
  }
  if (DetectedAccelerometerVersion == NO_DETECTED_ACCELEROMETER)
    systemSettings.ShutdownTime = 0; // Disable shutdown on no accel
  return systemSettings.ShutdownTime == 60;
}

static void settings_displayShutdownTime(void) {
  printShortDescription(3, 5);
  if (systemSettings.ShutdownTime == 0) {
    OLED::print(OffString);
  } else {
    OLED::printNumber(systemSettings.ShutdownTime, 2);
    OLED::print(SymbolMinutes);
  }
}
static bool settings_setTempF(void) {
  systemSettings.temperatureInF = !systemSettings.temperatureInF;
  if (systemSettings.temperatureInF) {
    // Change sleep, boost and soldering temps to the F equiv
    // C to F == F= ( (C*9) +160)/5
    systemSettings.BoostTemp     = ((systemSettings.BoostTemp * 9) + 160) / 5;
    systemSettings.SolderingTemp = ((systemSettings.SolderingTemp * 9) + 160) / 5;
    systemSettings.SleepTemp     = ((systemSettings.SleepTemp * 9) + 160) / 5;
  } else {
    // Change sleep, boost and soldering temps to the C equiv
    // F->C == C = ((F-32)*5)/9
    systemSettings.BoostTemp     = ((systemSettings.BoostTemp - 32) * 5) / 9;
    systemSettings.SolderingTemp = ((systemSettings.SolderingTemp - 32) * 5) / 9;
    systemSettings.SleepTemp     = ((systemSettings.SleepTemp - 32) * 5) / 9;
  }
  // Rescale both to be multiples of 10
  systemSettings.BoostTemp = systemSettings.BoostTemp / 10;
  systemSettings.BoostTemp *= 10;
  systemSettings.SolderingTemp = systemSettings.SolderingTemp / 10;
  systemSettings.SolderingTemp *= 10;
  systemSettings.SleepTemp = systemSettings.SleepTemp / 10;
  systemSettings.SleepTemp *= 10;
  return false;
}

static void settings_displayTempF(void) {
  printShortDescription(5, 7);

  OLED::print((systemSettings.temperatureInF) ? SymbolDegF : SymbolDegC);
}

static bool settings_setSensitivity(void) {
  systemSettings.sensitivity++;
  systemSettings.sensitivity = systemSettings.sensitivity % 10;
  return systemSettings.sensitivity == 9;
}

static void settings_displaySensitivity(void) {
  printShortDescription(4, 7);
  OLED::printNumber(systemSettings.sensitivity, 1, false);
}

static bool settings_setAdvancedSolderingScreens(void) {
  systemSettings.detailedSoldering = !systemSettings.detailedSoldering;
  return false;
}

static void settings_displayAdvancedSolderingScreens(void) {
  printShortDescription(14, 7);

  OLED::drawCheckbox(systemSettings.detailedSoldering);
}

static bool settings_setAdvancedIDLEScreens(void) {
  systemSettings.detailedIDLE = !systemSettings.detailedIDLE;
  return false;
}

static void settings_displayAdvancedIDLEScreens(void) {
  printShortDescription(6, 7);

  OLED::drawCheckbox(systemSettings.detailedIDLE);
}

static bool settings_setPowerLimit(void) {
  systemSettings.powerLimit += POWER_LIMIT_STEPS;
  if (systemSettings.powerLimit > MAX_POWER_LIMIT)
    systemSettings.powerLimit = 0;
  return systemSettings.powerLimit + POWER_LIMIT_STEPS > MAX_POWER_LIMIT;
}

static void settings_displayPowerLimit(void) {
  printShortDescription(20, 5);
  if (systemSettings.powerLimit == 0) {
    OLED::print(OffString);
  } else {
    OLED::printNumber(systemSettings.powerLimit, 2);
    OLED::print(SymbolWatts);
  }
}

static bool settings_setScrollSpeed(void) {
  if (systemSettings.descriptionScrollSpeed == 0)
    systemSettings.descriptionScrollSpeed = 1;
  else
    systemSettings.descriptionScrollSpeed = 0;
  return false;
}

static void settings_displayScrollSpeed(void) {
  printShortDescription(15, 7);
  OLED::print((systemSettings.descriptionScrollSpeed) ? SettingFastChar : SettingSlowChar);
}

static bool settings_setDisplayRotation(void) {
  systemSettings.OrientationMode++;
  systemSettings.OrientationMode = systemSettings.OrientationMode % 3;
  switch (systemSettings.OrientationMode) {
  case 0:
    OLED::setRotation(false);
    break;
  case 1:
    OLED::setRotation(true);
    break;
  case 2:
    // do nothing on auto
    break;
  default:
    break;
  }
  return systemSettings.OrientationMode == 2;
}

static void settings_displayDisplayRotation(void) {
  printShortDescription(7, 7);

  switch (systemSettings.OrientationMode) {
  case 0:
    OLED::print(SettingRightChar);
    break;
  case 1:
    OLED::print(SettingLeftChar);
    break;
  case 2:
    OLED::print(SettingAutoChar);
    break;
  default:
    OLED::print(SettingRightChar);
    break;
  }
}

static bool settings_setBoostTemp(void) {
  if (systemSettings.temperatureInF) {
    if (systemSettings.BoostTemp == 0) {
      systemSettings.BoostTemp = 480; // loop back at 480
    } else {
      systemSettings.BoostTemp += 20; // Go up 20F at a time
    }

    if (systemSettings.BoostTemp > 850) {
      systemSettings.BoostTemp = 0; // jump to off
    }
    return systemSettings.BoostTemp == 840;
  } else {
    if (systemSettings.BoostTemp == 0) {
      systemSettings.BoostTemp = 250; // loop back at 250
    } else {
      systemSettings.BoostTemp += 10; // Go up 10C at a time
    }
    if (systemSettings.BoostTemp > 450) {
      systemSettings.BoostTemp = 0; // Go to off state
    }
    return systemSettings.BoostTemp == 450;
  }
}

static void settings_displayBoostTemp(void) {
  printShortDescription(8, 5);
  if (systemSettings.BoostTemp) {
    OLED::printNumber(systemSettings.BoostTemp, 3);
  } else {
    OLED::print(OffString);
  }
}

static bool settings_setAutomaticStartMode(void) {
  systemSettings.autoStartMode++;
  systemSettings.autoStartMode %= 4;
  return systemSettings.autoStartMode == 3;
}

static void settings_displayAutomaticStartMode(void) {
  printShortDescription(9, 7);

  switch (systemSettings.autoStartMode) {
  case 0:
    OLED::print(SettingStartNoneChar);
    break;
  case 1:
    OLED::print(SettingStartSolderingChar);
    break;
  case 2:
    OLED::print(SettingStartSleepChar);
    break;
  case 3:
    OLED::print(SettingStartSleepOffChar);
    break;
  default:
    OLED::print(SettingStartNoneChar);
    break;
  }
}

static bool settings_setLockingMode(void) {
  systemSettings.lockingMode++;
  systemSettings.lockingMode %= 3;
  return systemSettings.lockingMode == 2;
}

static void settings_displayLockingMode(void) {
  printShortDescription(27, 7);

  switch (systemSettings.lockingMode) {
  case 0:
    OLED::print(SettingLockDisableChar);
    break;
  case 1:
    OLED::print(SettingLockBoostChar);
    break;
  case 2:
    OLED::print(SettingLockFullChar);
    break;
  default:
    OLED::print(SettingLockDisableChar);
    break;
  }
}

static bool settings_setCoolingBlinkEnabled(void) {
  systemSettings.coolingTempBlink = !systemSettings.coolingTempBlink;
  return false;
}

static void settings_displayCoolingBlinkEnabled(void) {
  printShortDescription(10, 7);

  OLED::drawCheckbox(systemSettings.coolingTempBlink);
}

static bool settings_setResetSettings(void) {
  if (userConfirmation(SettingsResetWarning)) {
    resetSettings();

    OLED::setFont(0);
    OLED::clearScreen();
    OLED::setCursor(0, 0);
    OLED::print(ResetOKMessage);
    OLED::refresh();

    waitForButtonPressOrTimeout(2000); // 2 second timeout
  }
  return false;
}

static void settings_displayResetSettings(void) { printShortDescription(12, 7); }

static void setTipOffset() {
  systemSettings.CalibrationOffset = 0;

  // If the thermo-couple at the end of the tip, and the handle are at
  // equilibrium, then the output should be zero, as there is no temperature
  // differential.
  while (systemSettings.CalibrationOffset == 0) {
    uint32_t offset = 0;
    for (uint8_t i = 0; i < 16; i++) {
      offset += getTipRawTemp(1);
      // cycle through the filter a fair bit to ensure we're stable.
      OLED::clearScreen();
      OLED::setCursor(0, 0);
      OLED::print(SymbolDot);
      for (uint8_t x = 0; x < (i / 4); x++)
        OLED::print(SymbolDot);
      OLED::refresh();
      osDelay(100);
    }
    systemSettings.CalibrationOffset = TipThermoModel::convertTipRawADCTouV(offset / 16);
  }
  OLED::clearScreen();
  OLED::setCursor(0, 0);
  OLED::drawCheckbox(true);
  OLED::printNumber(systemSettings.CalibrationOffset, 4);
  OLED::refresh();
  osDelay(1200);
}

// Provide the user the option to tune their own tip if custom is selected
// If not only do single point tuning as per usual
static bool settings_setCalibrate(void) {

  if (userConfirmation(SettingsCalibrationWarning)) {
    // User confirmed
    // So we now perform the actual calculation
    setTipOffset();
  }
  return false;
}

static void settings_displayCalibrate(void) { printShortDescription(11, 5); }

static bool settings_setCalibrateVIN(void) {
  // Jump to the voltage calibration subscreen
  OLED::setFont(0);
  OLED::clearScreen();

  for (;;) {
    OLED::setCursor(0, 0);
    OLED::printNumber(getInputVoltageX10(systemSettings.voltageDiv, 0) / 10, 2);
    OLED::print(SymbolDot);
    OLED::printNumber(getInputVoltageX10(systemSettings.voltageDiv, 0) % 10, 1, false);
    OLED::print(SymbolVolts);

    ButtonState buttons = getButtonState();
    switch (buttons) {
    case BUTTON_F_SHORT:
      systemSettings.voltageDiv++;
      break;

    case BUTTON_B_SHORT:
      systemSettings.voltageDiv--;
      break;

    case BUTTON_BOTH:
    case BUTTON_F_LONG:
    case BUTTON_B_LONG:
      saveSettings();
      OLED::setCursor(0, 0);
      OLED::printNumber(systemSettings.voltageDiv, 3);
      OLED::refresh();
      waitForButtonPressOrTimeout(1000);
      return false;
    case BUTTON_NONE:
    default:
      break;
    }

    OLED::refresh();
    osDelay(40);

    // Cap to sensible values
#if defined(MODEL_TS80) + defined(MODEL_TS80P) > 0
    if (systemSettings.voltageDiv < 500) {
      systemSettings.voltageDiv = 500;
    } else if (systemSettings.voltageDiv > 900) {
      systemSettings.voltageDiv = 900;
    }
#else
    if (systemSettings.voltageDiv < 360) {
      systemSettings.voltageDiv = 360;
    } else if (systemSettings.voltageDiv > 520) {
      systemSettings.voltageDiv = 520;
    }
#endif
  }
  return false;
}
//
// static bool settings_setTipGain(void) {
//	OLED::setFont(0);
//	OLED::clearScreen();
//
//	for (;;) {
//		OLED::setCursor(0, 0);
//		OLED::printNumber(systemSettings.TipGain / 10, 2);
//		OLED::print(SymbolDot);
//		OLED::printNumber(systemSettings.TipGain % 10, 1);
//
//		ButtonState buttons = getButtonState();
//		switch (buttons) {
//		case BUTTON_F_SHORT:
//			systemSettings.TipGain -= 1;
//			break;
//
//		case BUTTON_B_SHORT:
//			systemSettings.TipGain += 1;
//			break;
//
//		case BUTTON_BOTH:
//		case BUTTON_F_LONG:
//		case BUTTON_B_LONG:
//			saveSettings();
//			return false;
//		case BUTTON_NONE:
//		default:
//			break;
//		}
//
//		OLED::refresh();
//		osDelay(40);
//
//		// Cap to sensible values
//		if (systemSettings.TipGain < 150) {
//			systemSettings.TipGain = 150;
//		} else if (systemSettings.TipGain > 300) {
//			systemSettings.TipGain = 300;
//		}
//	}
//	return false;
//}
//
// static void settings_displayTipGain(void) {
//	printShortDescription(25, 5);
//}

static bool settings_setReverseButtonTempChangeEnabled(void) {
  systemSettings.ReverseButtonTempChangeEnabled = !systemSettings.ReverseButtonTempChangeEnabled;
  return false;
}

static void settings_displayReverseButtonTempChangeEnabled(void) {
  printShortDescription(21, 7);
  OLED::drawCheckbox(systemSettings.ReverseButtonTempChangeEnabled);
}

static bool settings_setTempChangeShortStep(void) {
  systemSettings.TempChangeShortStep += TEMP_CHANGE_SHORT_STEP;
  if (systemSettings.TempChangeShortStep > TEMP_CHANGE_SHORT_STEP_MAX) {
    systemSettings.TempChangeShortStep = TEMP_CHANGE_SHORT_STEP; // loop back at TEMP_CHANGE_SHORT_STEP_MAX
  }
  return systemSettings.TempChangeShortStep == TEMP_CHANGE_SHORT_STEP_MAX;
}

static void settings_displayTempChangeShortStep(void) {
  printShortDescription(22, 6);
  OLED::printNumber(systemSettings.TempChangeShortStep, 2);
}

static bool settings_setTempChangeLongStep(void) {
  if (systemSettings.TempChangeLongStep == TEMP_CHANGE_SHORT_STEP) {
    systemSettings.TempChangeLongStep = TEMP_CHANGE_LONG_STEP / 2;
  } else if (systemSettings.TempChangeLongStep == TEMP_CHANGE_LONG_STEP / 2) {
    systemSettings.TempChangeLongStep = TEMP_CHANGE_LONG_STEP;
  } else {
    systemSettings.TempChangeLongStep += TEMP_CHANGE_LONG_STEP;
  }
  if (systemSettings.TempChangeLongStep > TEMP_CHANGE_LONG_STEP_MAX) {
    systemSettings.TempChangeLongStep = TEMP_CHANGE_SHORT_STEP; // loop back at TEMP_CHANGE_LONG_STEP_MAX
  }
  return systemSettings.TempChangeLongStep == TEMP_CHANGE_LONG_STEP_MAX;
}

static void settings_displayTempChangeLongStep(void) {
  printShortDescription(23, 6);
  OLED::printNumber(systemSettings.TempChangeLongStep, 2);
}

static bool settings_setPowerPulse(void) {
  systemSettings.KeepAwakePulse += POWER_PULSE_INCREMENT;
  systemSettings.KeepAwakePulse %= POWER_PULSE_MAX;

  return systemSettings.KeepAwakePulse == POWER_PULSE_MAX - 1;
}
static void settings_displayPowerPulse(void) {
  printShortDescription(24, 5);
  if (systemSettings.KeepAwakePulse) {
    OLED::printNumber(systemSettings.KeepAwakePulse / 10, 1);
    OLED::print(SymbolDot);
    OLED::printNumber(systemSettings.KeepAwakePulse % 10, 1);
  } else {
    OLED::print(OffString);
  }
}

static bool settings_setAnimationLoop(void) {
  systemSettings.animationLoop = !systemSettings.animationLoop;
  return false;
}

static void settings_displayAnimationLoop(void) {
  printShortDescription(29, 7);
  OLED::drawCheckbox(systemSettings.animationLoop);
}

static bool settings_setAnimationSpeed(void) {
  systemSettings.animationSpeed += 50;
  systemSettings.animationSpeed = systemSettings.animationSpeed % 1000;
  if (systemSettings.animationSpeed < 250 && systemSettings.animationSpeed != 0)
    systemSettings.animationSpeed = 250;
  return systemSettings.animationSpeed == 950;
}

static void settings_displayAnimationSpeed(void) {
  printShortDescription(30, systemSettings.animationSpeed ? 5 : 7);
  if (systemSettings.animationSpeed)
    OLED::printNumber(systemSettings.animationSpeed, 3, false);
  else
    OLED::print(SettingSensitivityOff);
}
#ifdef HALL_SENSOR
static void settings_displayHallEffect(void) {
  printShortDescription(26, 7);
  switch (systemSettings.hallEffectSensitivity) {
  case 1:
    OLED::print(SettingSensitivityLow);
    break;
  case 2:
    OLED::print(SettingSensitivityMedium);
    break;
  case 3:
    OLED::print(SettingSensitivityHigh);
    break;
  case 0:
  default:
    OLED::print(SettingSensitivityOff);
    break;
  }
}
static bool settings_setHallEffect(void) {
  // To keep life simpler for now, we have a few preset sensitivity levels
  // Off, Low, Medium, High
  systemSettings.hallEffectSensitivity++;
  systemSettings.hallEffectSensitivity %= 4;
  return systemSettings.hallEffectSensitivity == 3;
}
#endif

static bool animOpenState = false;

static void displayMenu(size_t index) {
  // Call into the menu
  const char *textPtr = SettingsMenuEntries[index];
  if (textPtr[0] == '\x01') { // `\x01` is used as newline.
    // Empty first line means that this uses large font (for CJK).
    OLED::setFont(0);
    textPtr++;
  } else {
    OLED::setFont(1);
  }
  OLED::setCursor(0, 0);
  // Draw title
  OLED::print(textPtr);
  // Draw symbol
  // 16 pixel wide image
  // 2 pixel wide scrolling indicator
  static TickType_t menuSwitchLoopTick = 0;
  static size_t     menuCurrentIndex   = sizeof(rootSettingsMenu) + 1;
  static size_t     currentFrame       = 0;
  if (!animOpenState) {
    if (menuCurrentIndex != index) {
      menuCurrentIndex   = index;
      currentFrame       = systemSettings.animationSpeed ? 0 : 2;
      menuSwitchLoopTick = xTaskGetTickCount();
    }
    if (systemSettings.animationSpeed)
      if (systemSettings.animationLoop || currentFrame != 2)
        currentFrame = ((xTaskGetTickCount() - menuSwitchLoopTick) / systemSettings.animationSpeed) % 3;
    OLED::drawArea(OLED_WIDTH - 16 - 2, 0, 16, 16, (&SettingsMenuIcons[index][(16 * 2) * currentFrame]));
  }
}

static void settings_displayCalibrateVIN(void) { printShortDescription(13, 5); }
static void settings_displayPowerMenu(void) { displayMenu(0); }
static bool settings_enterPowerMenu(void) {
  gui_Menu(powerMenu);
  return false;
}
static void settings_displaySolderingMenu(void) { displayMenu(1); }
static bool settings_enterSolderingMenu(void) {
  gui_Menu(solderingMenu);
  return false;
}
static void settings_displayPowerSavingMenu(void) { displayMenu(2); }
static bool settings_enterPowerSavingMenu(void) {
  gui_Menu(PowerSavingMenu);
  return false;
}
static void settings_displayUIMenu(void) { displayMenu(3); }
static bool settings_enterUIMenu(void) {
  gui_Menu(HasFahrenheit ? UIMenu : UIMenu + 1);
  return false;
}
static void settings_displayAdvancedMenu(void) { displayMenu(4); }
static bool settings_enterAdvancedMenu(void) {
  gui_Menu(advancedMenu);
  return false;
}

void gui_Menu(const menuitem *menu) {
  // Draw the settings menu and provide iteration support etc
  uint8_t     currentScreen          = 0;
  uint32_t    autoRepeatTimer        = 0;
  uint8_t     autoRepeatAcceleration = 0;
  bool        earlyExit              = false;
  uint32_t    descriptionStart       = 0;
  int16_t     lastOffset             = -1;
  bool        lcdRefresh             = true;
  ButtonState lastButtonState        = BUTTON_NONE;
  static bool enterGUIMenu           = true;
  enterGUIMenu                       = true;
  uint8_t scrollContentSize          = 0;
  bool    scrollBlink                = false;
  bool    lastValue                  = false;

  for (uint8_t i = 0; menu[i].draw != NULL; i++) {
    scrollContentSize += 1;
  }

  // Animated menu opening.
  if (menu[currentScreen].draw != NULL) {
    // This menu is drawn in a secondary framebuffer.
    // Then we play a transition from the current primary
    // framebuffer to the new buffer.
    // The extra buffer is discarded at the end of the transition.
    animOpenState = true;
    OLED::useSecondaryFramebuffer(true);
    OLED::setFont(0);
    OLED::setCursor(0, 0);
    OLED::clearScreen();
    menu[currentScreen].draw();
    OLED::useSecondaryFramebuffer(false);
    OLED::transitionSecondaryFramebuffer(true);
    animOpenState = false;
  }

  while ((menu[currentScreen].draw != NULL) && earlyExit == false) {
    OLED::setFont(0);
    OLED::setCursor(0, 0);
    // If the user has hesitated for >=3 seconds, show the long text
    // Otherwise "draw" the option
    if ((xTaskGetTickCount() - lastButtonTime < 3000) || menu[currentScreen].description == NULL) {
      OLED::clearScreen();
      menu[currentScreen].draw();
      uint8_t indicatorHeight = OLED_HEIGHT / scrollContentSize;
      uint8_t position        = OLED_HEIGHT * currentScreen / scrollContentSize;
      if (lastValue)
        scrollBlink = !scrollBlink;
      if (!lastValue || !scrollBlink)
        OLED::drawScrollIndicator(position, indicatorHeight);
      lastOffset = -1;
      lcdRefresh = true;
    } else {
      // Draw description
      if (descriptionStart == 0)
        descriptionStart = xTaskGetTickCount();
      // lower the value - higher the speed
      int16_t descriptionWidth  = FONT_12_WIDTH * (strlen(menu[currentScreen].description) + 7);
      int16_t descriptionOffset = ((xTaskGetTickCount() - descriptionStart) / (systemSettings.descriptionScrollSpeed == 1 ? 10 : 20));
      descriptionOffset %= descriptionWidth; // Roll around at the end
      if (lastOffset != descriptionOffset) {
        OLED::clearScreen();
        OLED::setCursor((OLED_WIDTH - descriptionOffset), 0);
        OLED::print(menu[currentScreen].description);
        lastOffset = descriptionOffset;
        lcdRefresh = true;
      }
    }

    ButtonState buttons = getButtonState();

    if (buttons != lastButtonState) {
      autoRepeatAcceleration = 0;
      lastButtonState        = buttons;
    }

    switch (buttons) {
    case BUTTON_BOTH:
      earlyExit        = true; // will make us exit next loop
      descriptionStart = 0;
      break;
    case BUTTON_F_SHORT:
      // increment
      if (descriptionStart == 0) {
        if (menu[currentScreen].incrementHandler != NULL) {
          enterGUIMenu = false;
          lastValue    = menu[currentScreen].incrementHandler();

          if (enterGUIMenu) {
            OLED::useSecondaryFramebuffer(true);
            OLED::setFont(0);
            OLED::setCursor(0, 0);
            OLED::clearScreen();
            menu[currentScreen].draw();
            OLED::useSecondaryFramebuffer(false);
            OLED::transitionSecondaryFramebuffer(false);
          }
          enterGUIMenu = true;
        } else {
          earlyExit = true;
        }
      } else
        descriptionStart = 0;
      break;
    case BUTTON_B_SHORT:
      if (descriptionStart == 0) {
        currentScreen++;
        lastValue = false;
      } else
        descriptionStart = 0;
      break;
    case BUTTON_F_LONG:
      if ((int)(xTaskGetTickCount() - autoRepeatTimer + autoRepeatAcceleration) > PRESS_ACCEL_INTERVAL_MAX) {
        if ((lastValue = menu[currentScreen].incrementHandler()))
          autoRepeatTimer = 1000;
        else
          autoRepeatTimer = 0;

        autoRepeatTimer += xTaskGetTickCount();

        descriptionStart = 0;

        autoRepeatAcceleration += PRESS_ACCEL_STEP;
      }
      break;
    case BUTTON_B_LONG:
      if (xTaskGetTickCount() - autoRepeatTimer + autoRepeatAcceleration > PRESS_ACCEL_INTERVAL_MAX) {
        currentScreen++;
        autoRepeatTimer  = xTaskGetTickCount();
        descriptionStart = 0;

        autoRepeatAcceleration += PRESS_ACCEL_STEP;
      }
      break;
    case BUTTON_NONE:
    default:
      break;
    }

    if ((PRESS_ACCEL_INTERVAL_MAX - autoRepeatAcceleration) < PRESS_ACCEL_INTERVAL_MIN) {
      autoRepeatAcceleration = PRESS_ACCEL_INTERVAL_MAX - PRESS_ACCEL_INTERVAL_MIN;
    }

    if (lcdRefresh) {
      OLED::refresh(); // update the LCD
      osDelay(40);
      lcdRefresh = false;
    }
    if ((xTaskGetTickCount() - lastButtonTime) > (1000 * 30)) {
      // If user has not pressed any buttons in 30 seconds, exit back a menu layer
      // This will trickle the user back to the main screen eventually
      earlyExit        = true;
      descriptionStart = 0;
    }
  }
}

void enterSettingsMenu() {
  gui_Menu(rootSettingsMenu); // Call the root menu
  saveSettings();
}
