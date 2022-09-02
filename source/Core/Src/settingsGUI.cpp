/*
 * settingsGUI.cpp
 *
 *  Created on: 3Sep.,2017
 *      Author: Ben V. Brown
 */

#include "settingsGUI.hpp"
#include "Buttons.hpp"
#include "ScrollMessage.hpp"
#include "TipThermoModel.h"
#include "Translation.h"
#include "cmsis_os.h"
#include "configuration.h"
#include "main.hpp"

void gui_Menu(const menuitem *menu);

#ifdef POW_DC
static void settings_displayInputVRange(void);
static bool settings_showInputVOptions(void);
static void settings_displayInputMinVRange(void);
#endif
#ifdef POW_QC
static void settings_displayQCInputV(void);
#endif
#if POW_PD
static void settings_displayPDNegTimeout(void);
#endif
static void settings_displaySensitivity(void);
static void settings_displayShutdownTime(void);
static bool settings_showSleepOptions(void);
#ifndef NO_SLEEP_MODE
static bool settings_setSleepTemp(void);
static void settings_displaySleepTemp(void);
static void settings_displaySleepTime(void);
#endif
static bool settings_setTempF(void);
static void settings_displayTempF(void);
static void settings_displayAdvancedSolderingScreens(void);
static void settings_displayAdvancedIDLEScreens(void);
static void settings_displayScrollSpeed(void);
static void settings_displayPowerLimit(void);
#ifndef NO_DISPLAY_ROTATE
static bool settings_setDisplayRotation(void);
static void settings_displayDisplayRotation(void);
#endif

static bool settings_setBoostTemp(void);
static void settings_displayBoostTemp(void);
static void settings_displayAutomaticStartMode(void);
static void settings_displayLockingMode(void);
static void settings_displayCoolingBlinkEnabled(void);
static bool settings_setResetSettings(void);
static void settings_displayResetSettings(void);
static bool settings_setCalibrate(void);
static void settings_displayCalibrate(void);
static bool settings_setCalibrateVIN(void);
static void settings_displayCalibrateVIN(void);
static void settings_displayReverseButtonTempChangeEnabled(void);
static void settings_displayTempChangeShortStep(void);
static void settings_displayTempChangeLongStep(void);
static void settings_displayPowerPulse(void);
static bool settings_displayAnimationOptions(void);
static void settings_displayAnimationSpeed(void);
static void settings_displayAnimationLoop(void);
static void settings_displayPowerPulseWait(void);
static bool settings_showPowerPulseOptions(void);
static void settings_displayPowerPulseDuration(void);
static void settings_displayBrightnessLevel(void);
static void settings_displayInvertColor(void);
static void settings_displayLogoTime(void);

#ifdef HALL_SENSOR
static void settings_displayHallEffect(void);
static bool settings_showHallEffect(void);
#endif
// Menu functions

#if defined(POW_DC) || defined(POW_QC)
static void settings_displayPowerMenu(void);
static bool settings_enterPowerMenu(void);
#endif
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
 *  -Minimum Voltage
 *  QC Voltage
 *  PD Timeout
 *
 * Soldering
 * 	Boost Mode Temp
 * 	Auto Start
 *  Temp change short step
 *  Temp change long step
 *	Locking Mode
 *
 * Power Saving
 * 	Motion Sensitivity
 * 	-Sleep Temp
 * 	-Sleep Time
 * 	-Shutdown Time
 *
 * UI
 *  // Language
 *  Temperature Unit
 *  Display orientation
 *  Cooldown blink
 *  Scrolling Speed
 *  Reverse Temp change buttons + -
 *  Animation Speed
 *  -Animation Loop
 *  OLED Brightnes
 *  Invert Screen
 *  Detailed IDLE
 *  Detailed Soldering
 *
 * Advanced
 *  Power Limit
 *  Factory Reset
 *  Calibrate Temperature
 *  Calibrate Input V
 *  Power Pulse
 *  -Power Pulse Delay
 *  -Power Pulse duration
 *
 */
const menuitem rootSettingsMenu[] {
  /*
   * Power Menu
   * Soldering Menu
   * Power Saving Menu
   * UI Menu
   * Advanced Menu
   * Exit
   */

#if defined(POW_DC) || defined(POW_QC)
  {0, settings_enterPowerMenu, settings_displayPowerMenu, nullptr, SettingsOptions::SettingsOptionsLength}, /*Power*/
#endif
      {0, settings_enterSolderingMenu, settings_displaySolderingMenu, nullptr, SettingsOptions::SettingsOptionsLength},                     /*Soldering*/
      {0, settings_enterPowerSavingMenu, settings_displayPowerSavingMenu, nullptr, SettingsOptions::SettingsOptionsLength},                 /*Sleep Options Menu*/
      {0, settings_enterUIMenu, settings_displayUIMenu, nullptr, SettingsOptions::SettingsOptionsLength},                                   /*UI Menu*/
      {0, settings_enterAdvancedMenu, settings_displayAdvancedMenu, nullptr, SettingsOptions::SettingsOptionsLength},                       /*Advanced Menu*/
      {0, settings_setLanguageSwitch, settings_displayLanguageSwitch, settings_showLanguageSwitch, SettingsOptions::SettingsOptionsLength}, /*Language Switch*/
  {
    0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength
  } // end of menu marker. DO NOT REMOVE
};

#if defined(POW_DC) || defined(POW_QC) || defined(POW_PD)
const menuitem powerMenu[] = {
/*
 *  Power Source
 *  -Minimum Voltage
 *  QC Voltage
 *  PD Timeout
 */
#ifdef POW_DC
    {SETTINGS_DESC(SettingsItemIndex::DCInCutoff), nullptr, settings_displayInputVRange, nullptr, SettingsOptions::MinDCVoltageCells},                     /*Voltage input*/
    {SETTINGS_DESC(SettingsItemIndex::MinVolCell), nullptr, settings_displayInputMinVRange, settings_showInputVOptions, SettingsOptions::MinVoltageCells}, /*Minimum voltage input*/
#endif
#ifdef POW_QC
    {SETTINGS_DESC(SettingsItemIndex::QCMaxVoltage), nullptr, settings_displayQCInputV, nullptr, SettingsOptions::QCIdealVoltage}, /*Voltage input*/
#endif
#if POW_PD
    {SETTINGS_DESC(SettingsItemIndex::PDNegTimeout), nullptr, settings_displayPDNegTimeout, nullptr, SettingsOptions::PDNegTimeout}, /*PD timeout setup*/
#endif
    {0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength} // end of menu marker. DO NOT REMOVE
};
#endif
const menuitem solderingMenu[] = {
    /*
     * 	Boost Temp
     * 	Auto Start
     *  Temp change short step
     *  Temp change long step
     *	Locking Mode
     */
    {SETTINGS_DESC(SettingsItemIndex::BoostTemperature), settings_setBoostTemp, settings_displayBoostTemp, nullptr, SettingsOptions::SettingsOptionsLength}, /*Boost Temp*/
    {SETTINGS_DESC(SettingsItemIndex::AutoStart), nullptr, settings_displayAutomaticStartMode, nullptr, SettingsOptions::AutoStartMode},                     /*Auto start*/
    {SETTINGS_DESC(SettingsItemIndex::TempChangeShortStep), nullptr, settings_displayTempChangeShortStep, nullptr, SettingsOptions::TempChangeShortStep},    /*Temp change short step*/
    {SETTINGS_DESC(SettingsItemIndex::TempChangeLongStep), nullptr, settings_displayTempChangeLongStep, nullptr, SettingsOptions::TempChangeLongStep},       /*Temp change long step*/
    {SETTINGS_DESC(SettingsItemIndex::LockingMode), nullptr, settings_displayLockingMode, nullptr, SettingsOptions::LockingMode},                            /*Locking Mode*/
    {0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength}                                                                                   // end of menu marker. DO NOT REMOVE
};
const menuitem PowerSavingMenu[] = {
    /*
     * 	Motion Sensitivity
     * 	-Sleep Temp
     * 	-Sleep Time
     * 	-Shutdown Time
     */
    {SETTINGS_DESC(SettingsItemIndex::MotionSensitivity), nullptr, settings_displaySensitivity, nullptr, SettingsOptions::Sensitivity}, /* Motion Sensitivity*/
#ifndef NO_SLEEP_MODE
    {SETTINGS_DESC(SettingsItemIndex::SleepTemperature), settings_setSleepTemp, settings_displaySleepTemp, settings_showSleepOptions, SettingsOptions::SettingsOptionsLength}, /*Sleep Temp*/
    {SETTINGS_DESC(SettingsItemIndex::SleepTimeout), nullptr, settings_displaySleepTime, settings_showSleepOptions, SettingsOptions::SleepTime},                               /*Sleep Time*/
#endif
    {SETTINGS_DESC(SettingsItemIndex::ShutdownTimeout), nullptr, settings_displayShutdownTime, settings_showSleepOptions, SettingsOptions::ShutdownTime}, /*Shutdown Time*/
#ifdef HALL_SENSOR
    {SETTINGS_DESC(SettingsItemIndex::HallEffSensitivity), nullptr, settings_displayHallEffect, settings_showHallEffect, SettingsOptions::HallEffectSensitivity}, /* HallEffect Sensitivity*/
#endif
    {0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength} // end of menu marker. DO NOT REMOVE
};
const menuitem UIMenu[] = {
    /*
     *  // Language
     *  Temperature Unit
     *  Display orientation
     *  Cooldown blink
     *  Scrolling Speed
     *  Reverse Temp change buttons + -
     *  Animation Speed
     *  -Animation Loop
     *  OLED Brightnes
     *  Invert Screen
     *  Logo Timeout
     *  Detailed IDLE
     *  Detailed Soldering
     */
    {SETTINGS_DESC(SettingsItemIndex::TemperatureUnit), settings_setTempF, settings_displayTempF, nullptr,
     SettingsOptions::SettingsOptionsLength}, /* Temperature units, this has to be the first element in the array to work with the logic in settings_enterUIMenu() */
#ifndef NO_DISPLAY_ROTATE
    {SETTINGS_DESC(SettingsItemIndex::DisplayRotation), settings_setDisplayRotation, settings_displayDisplayRotation, nullptr, SettingsOptions::SettingsOptionsLength}, /*Display Rotation*/
#endif
    {SETTINGS_DESC(SettingsItemIndex::CooldownBlink), nullptr, settings_displayCoolingBlinkEnabled, nullptr, SettingsOptions::CoolingTempBlink}, /*Cooling blink warning*/
    {SETTINGS_DESC(SettingsItemIndex::ScrollingSpeed), nullptr, settings_displayScrollSpeed, nullptr, SettingsOptions::DescriptionScrollSpeed},  /*Scroll Speed for descriptions*/
    {SETTINGS_DESC(SettingsItemIndex::ReverseButtonTempChange), nullptr, settings_displayReverseButtonTempChangeEnabled, nullptr,
     SettingsOptions::ReverseButtonTempChangeEnabled},                                                                                                      /*Reverse Temp change buttons + - */
    {SETTINGS_DESC(SettingsItemIndex::AnimSpeed), nullptr, settings_displayAnimationSpeed, nullptr, SettingsOptions::AnimationSpeed},                       /*Animation Speed adjustment */
    {SETTINGS_DESC(SettingsItemIndex::AnimLoop), nullptr, settings_displayAnimationLoop, settings_displayAnimationOptions, SettingsOptions::AnimationLoop}, /*Animation Loop switch */
    {SETTINGS_DESC(SettingsItemIndex::Brightness), nullptr, settings_displayBrightnessLevel, nullptr, SettingsOptions::OLEDBrightness},                     /*Brightness Level*/
    {SETTINGS_DESC(SettingsItemIndex::ColourInversion), nullptr, settings_displayInvertColor, nullptr, SettingsOptions::OLEDInversion},                     /*Invert screen colour*/
    {SETTINGS_DESC(SettingsItemIndex::LOGOTime), nullptr, settings_displayLogoTime, nullptr, SettingsOptions::LOGOTime},                                    /*Set logo duration*/
    {SETTINGS_DESC(SettingsItemIndex::AdvancedIdle), nullptr, settings_displayAdvancedIDLEScreens, nullptr, SettingsOptions::DetailedIDLE},                 /*Advanced idle screen*/
    {SETTINGS_DESC(SettingsItemIndex::AdvancedSoldering), nullptr, settings_displayAdvancedSolderingScreens, nullptr, SettingsOptions::DetailedSoldering},  /*Advanced soldering screen*/
    {0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength}                                                                                  // end of menu marker. DO NOT REMOVE
};
const menuitem advancedMenu[] = {

    /*
     *  Power Limit
     *  Factory Reset
     *  Calibrate Temperature
     *  Calibrate Input V
     *  Power Pulse
     *  -Power Pulse Delay
     *  -Power Pulse duration
     */
    {SETTINGS_DESC(SettingsItemIndex::PowerLimit), nullptr, settings_displayPowerLimit, nullptr, SettingsOptions::PowerLimit},                                        /*Power limit*/
    {SETTINGS_DESC(SettingsItemIndex::SettingsReset), settings_setResetSettings, settings_displayResetSettings, nullptr, SettingsOptions::SettingsOptionsLength},     /*Resets settings*/
    {SETTINGS_DESC(SettingsItemIndex::TemperatureCalibration), settings_setCalibrate, settings_displayCalibrate, nullptr, SettingsOptions::SettingsOptionsLength},    /*Calibrate tip*/
    {SETTINGS_DESC(SettingsItemIndex::VoltageCalibration), settings_setCalibrateVIN, settings_displayCalibrateVIN, nullptr, SettingsOptions::SettingsOptionsLength},  /*Voltage input cal*/
    {SETTINGS_DESC(SettingsItemIndex::PowerPulsePower), nullptr, settings_displayPowerPulse, nullptr, SettingsOptions::KeepAwakePulse},                               /*Power Pulse adjustment */
    {SETTINGS_DESC(SettingsItemIndex::PowerPulseWait), nullptr, settings_displayPowerPulseWait, settings_showPowerPulseOptions, SettingsOptions::KeepAwakePulseWait}, /*Power Pulse Wait adjustment*/
    {SETTINGS_DESC(SettingsItemIndex::PowerPulseDuration), nullptr, settings_displayPowerPulseDuration, settings_showPowerPulseOptions,
     SettingsOptions::KeepAwakePulseDuration},                             /*Power Pulse Duration adjustment*/
    {0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength} // end of menu marker. DO NOT REMOVE
};

/**
 * Prints two small lines (or one line for CJK) of short description for
 * setting items and prepares cursor after it.
 * @param settingsItemIndex Index of the setting item.
 * @param cursorCharPosition Custom cursor char position to set after printing
 * description.
 */
static void printShortDescription(SettingsItemIndex settingsItemIndex, uint16_t cursorCharPosition) {
  // print short description (default single line, explicit double line)
  uint8_t shortDescIndex = static_cast<uint8_t>(settingsItemIndex);
  OLED::printWholeScreen(translatedString(Tr->SettingsShortNames[shortDescIndex]));

  // prepare cursor for value
  // make room for scroll indicator
  OLED::setCursor(cursorCharPosition * FONT_12_WIDTH - 2, 0);
}

static int userConfirmation(const char *message) {
  ScrollMessage scrollMessage;

  for (;;) {
    bool lcdRefresh = scrollMessage.drawUpdate(message, xTaskGetTickCount());

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
    }
  }
  return 0;
}
#ifdef POW_DC

static void settings_displayInputVRange(void) {
  printShortDescription(SettingsItemIndex::DCInCutoff, 6);

  if (getSettingValue(SettingsOptions::MinDCVoltageCells)) {
    OLED::printNumber(2 + getSettingValue(SettingsOptions::MinDCVoltageCells), 1, FontStyle::LARGE);
    OLED::print(SymbolCellCount, FontStyle::LARGE);
  } else {
    OLED::print(SymbolDC, FontStyle::LARGE);
  }
}

static bool settings_showInputVOptions(void) { return getSettingValue(SettingsOptions::MinDCVoltageCells) > 0; }
static void settings_displayInputMinVRange(void) {
  printShortDescription(SettingsItemIndex::MinVolCell, 5);
  OLED::printNumber(getSettingValue(SettingsOptions::MinVoltageCells) / 10, 1, FontStyle::LARGE);
  OLED::print(SymbolDot, FontStyle::LARGE);
  OLED::printNumber(getSettingValue(SettingsOptions::MinVoltageCells) % 10, 1, FontStyle::LARGE);
}
#endif
#ifdef POW_QC

static void settings_displayQCInputV(void) {
  printShortDescription(SettingsItemIndex::QCMaxVoltage, 4);
  // These are only used in QC modes
  // Allows setting the voltage negotiated for QC
  auto voltage = getSettingValue(SettingsOptions::QCIdealVoltage);
  OLED::printNumber(voltage / 10, 2, FontStyle::LARGE);
  OLED::print(SymbolDot, FontStyle::LARGE);
  OLED::printNumber(voltage % 10, 1, FontStyle::LARGE);
}

#endif

#if POW_PD

static void settings_displayPDNegTimeout(void) {
  printShortDescription(SettingsItemIndex::PDNegTimeout, 5);
  auto value = getSettingValue(SettingsOptions::PDNegTimeout);
  if (value == 0) {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  } else {
    OLED::printNumber(value, 3, FontStyle::LARGE);
  }
}
#endif

static void settings_displayShutdownTime(void) {
  printShortDescription(SettingsItemIndex::ShutdownTimeout, 5);
  if (getSettingValue(SettingsOptions::ShutdownTime) == 0) {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  } else {
    OLED::printNumber(getSettingValue(SettingsOptions::ShutdownTime), 2, FontStyle::LARGE);
    OLED::print(SymbolMinutes, FontStyle::LARGE);
  }
}

static void settings_displaySensitivity(void) {
  printShortDescription(SettingsItemIndex::MotionSensitivity, 7);
  OLED::printNumber(getSettingValue(SettingsOptions::Sensitivity), 1, FontStyle::LARGE, false);
}
static bool settings_showSleepOptions(void) { return getSettingValue(SettingsOptions::Sensitivity) > 0; }

#ifndef NO_SLEEP_MODE

static bool settings_setSleepTemp(void) {
  // If in C, 10 deg, if in F 20 deg
  uint16_t temp = getSettingValue(SettingsOptions::SleepTemp);
  if (getSettingValue(SettingsOptions::TemperatureInF)) {
    temp += 20;
    if (temp > 580)
      temp = 60;
    setSettingValue(SettingsOptions::SleepTemp, temp);
    return temp == 580;
  } else {
    temp += 10;
    if (temp > 300)
      temp = 10;
    setSettingValue(SettingsOptions::SleepTemp, temp);
    return temp == 300;
  }
}

static void settings_displaySleepTemp(void) {
  printShortDescription(SettingsItemIndex::SleepTemperature, 5);
  OLED::printNumber(getSettingValue(SettingsOptions::SleepTemp), 3, FontStyle::LARGE);
}

static void settings_displaySleepTime(void) {
  printShortDescription(SettingsItemIndex::SleepTimeout, 5);
  if (getSettingValue(SettingsOptions::SleepTime) == 0) {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  } else if (getSettingValue(SettingsOptions::SleepTime) < 6) {
    OLED::printNumber(getSettingValue(SettingsOptions::SleepTime) * 10, 2, FontStyle::LARGE);
    OLED::print(SymbolSeconds, FontStyle::LARGE);
  } else {
    OLED::printNumber(getSettingValue(SettingsOptions::SleepTime) - 5, 2, FontStyle::LARGE);
    OLED::print(SymbolMinutes, FontStyle::LARGE);
  }
}
#endif

static bool settings_setTempF(void) {
  bool     res           = nextSettingValue(SettingsOptions::TemperatureInF);
  uint16_t BoostTemp     = getSettingValue(SettingsOptions::BoostTemp);
  uint16_t SolderingTemp = getSettingValue(SettingsOptions::SolderingTemp);
  uint16_t SleepTemp     = getSettingValue(SettingsOptions::SleepTemp);

  if (getSettingValue(SettingsOptions::TemperatureInF)) {
    // Change sleep, boost and soldering temps to the F equiv
    // C to F == F= ( (C*9) +160)/5
    BoostTemp     = ((BoostTemp * 9) + 160) / 5;
    SolderingTemp = ((SolderingTemp * 9) + 160) / 5;
    SleepTemp     = ((SleepTemp * 9) + 160) / 5;
  } else {
    // Change sleep, boost and soldering temps to the C equiv
    // F->C == C = ((F-32)*5)/9
    BoostTemp     = ((BoostTemp - 32) * 5) / 9;
    SolderingTemp = ((SolderingTemp - 32) * 5) / 9;
    SleepTemp     = ((SleepTemp - 32) * 5) / 9;
  }
  // Rescale both to be multiples of 10
  BoostTemp = BoostTemp / 10;
  BoostTemp *= 10;
  SolderingTemp = SolderingTemp / 10;
  SolderingTemp *= 10;
  SleepTemp = SleepTemp / 10;
  SleepTemp *= 10;
  setSettingValue(SettingsOptions::BoostTemp, BoostTemp);
  setSettingValue(SettingsOptions::SolderingTemp, SolderingTemp);
  setSettingValue(SettingsOptions::SleepTemp, SleepTemp);

  return res;
}

static void settings_displayTempF(void) {
  printShortDescription(SettingsItemIndex::TemperatureUnit, 7);
  OLED::print((getSettingValue(SettingsOptions::TemperatureInF)) ? SymbolDegF : SymbolDegC, FontStyle::LARGE);
}

static void settings_displayAdvancedSolderingScreens(void) {
  printShortDescription(SettingsItemIndex::AdvancedSoldering, 7);
  OLED::drawCheckbox(getSettingValue(SettingsOptions::DetailedSoldering));
}

static void settings_displayAdvancedIDLEScreens(void) {
  printShortDescription(SettingsItemIndex::AdvancedIdle, 7);
  OLED::drawCheckbox(getSettingValue(SettingsOptions::DetailedIDLE));
}

static void settings_displayPowerLimit(void) {
  printShortDescription(SettingsItemIndex::PowerLimit, 5);
  if (getSettingValue(SettingsOptions::PowerLimit) == 0) {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  } else {
    OLED::printNumber(getSettingValue(SettingsOptions::PowerLimit), 2, FontStyle::LARGE);
    OLED::print(SymbolWatts, FontStyle::LARGE);
  }
}

static void settings_displayScrollSpeed(void) {
  printShortDescription(SettingsItemIndex::ScrollingSpeed, 7);
  OLED::print(translatedString((getSettingValue(SettingsOptions::DescriptionScrollSpeed)) ? Tr->SettingFastChar : Tr->SettingSlowChar), FontStyle::LARGE);
}

#ifndef NO_DISPLAY_ROTATE
static bool settings_setDisplayRotation(void) {
  bool res = nextSettingValue(SettingsOptions::OrientationMode);
  switch (getSettingValue(SettingsOptions::OrientationMode)) {
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
  return res;
}

static void settings_displayDisplayRotation(void) {
  printShortDescription(SettingsItemIndex::DisplayRotation, 7);

  switch (getSettingValue(SettingsOptions::OrientationMode)) {
  case 0:
    OLED::print(translatedString(Tr->SettingRightChar), FontStyle::LARGE);
    break;
  case 1:
    OLED::print(translatedString(Tr->SettingLeftChar), FontStyle::LARGE);
    break;
  case 2:
    OLED::print(translatedString(Tr->SettingAutoChar), FontStyle::LARGE);
    break;
  default:
    OLED::print(translatedString(Tr->SettingRightChar), FontStyle::LARGE);
    break;
  }
}
#endif
static bool settings_setBoostTemp(void) {
  uint16_t value = getSettingValue(SettingsOptions::BoostTemp);
  if (getSettingValue(SettingsOptions::TemperatureInF)) {
    if (value == 0) {
      value = MIN_BOOST_TEMP_F; // loop back at 480
    } else {
      value += 20; // Go up 20F at a time
    }

    if (value >= MAX_TEMP_F) {
      value = 0; // jump to off
    }
    setSettingValue(SettingsOptions::BoostTemp, value);
    return value >= (MAX_TEMP_F - 10);
  }
  if (value == 0) {
    value = MIN_BOOST_TEMP_C; // loop back at 250
  } else {
    value += 10; // Go up 10C at a time
  }
  if (value > MAX_TEMP_C) {
    value = 0; // Go to off state
  }
  setSettingValue(SettingsOptions::BoostTemp, value);
  return value >= MAX_TEMP_C;
}

static void settings_displayBoostTemp(void) {
  printShortDescription(SettingsItemIndex::BoostTemperature, 5);
  if (getSettingValue(SettingsOptions::BoostTemp)) {
    OLED::printNumber(getSettingValue(SettingsOptions::BoostTemp), 3, FontStyle::LARGE);
  } else {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  }
}

static void settings_displayAutomaticStartMode(void) {
  printShortDescription(SettingsItemIndex::AutoStart, 7);

  switch (getSettingValue(SettingsOptions::AutoStartMode)) {
  case 0:
    OLED::print(translatedString(Tr->SettingStartNoneChar), FontStyle::LARGE);
    break;
  case 1:
    OLED::print(translatedString(Tr->SettingStartSolderingChar), FontStyle::LARGE);
    break;
  case 2:
    OLED::print(translatedString(Tr->SettingStartSleepChar), FontStyle::LARGE);
    break;
  case 3:
    OLED::print(translatedString(Tr->SettingStartSleepOffChar), FontStyle::LARGE);
    break;
  default:
    OLED::print(translatedString(Tr->SettingStartNoneChar), FontStyle::LARGE);
    break;
  }
}

static void settings_displayLockingMode(void) {
  printShortDescription(SettingsItemIndex::LockingMode, 7);

  switch (getSettingValue(SettingsOptions::LockingMode)) {
  case 0:
    OLED::print(translatedString(Tr->SettingLockDisableChar), FontStyle::LARGE);
    break;
  case 1:
    OLED::print(translatedString(Tr->SettingLockBoostChar), FontStyle::LARGE);
    break;
  case 2:
    OLED::print(translatedString(Tr->SettingLockFullChar), FontStyle::LARGE);
    break;
  default:
    OLED::print(translatedString(Tr->SettingLockDisableChar), FontStyle::LARGE);
    break;
  }
}

static void settings_displayCoolingBlinkEnabled(void) {
  printShortDescription(SettingsItemIndex::CooldownBlink, 7);
  OLED::drawCheckbox(getSettingValue(SettingsOptions::CoolingTempBlink));
}

static bool settings_setResetSettings(void) {
  if (userConfirmation(translatedString(Tr->SettingsResetWarning))) {
    resetSettings();
    warnUser(translatedString(Tr->SettingsResetMessage), 10 * TICKS_SECOND);
  }
  return false;
}

static void settings_displayResetSettings(void) { printShortDescription(SettingsItemIndex::SettingsReset, 7); }

static void setTipOffset() {
  uint16_t setoffset = 0;

  // If the thermo-couple at the end of the tip, and the handle are at
  // equilibrium, then the output should be zero, as there is no temperature
  // differential.
  while (setoffset == 0) {
    uint32_t offset = 0;
    for (uint8_t i = 0; i < 16; i++) {
      offset += getTipRawTemp(1);
      // cycle through the filter a fair bit to ensure we're stable.
      OLED::clearScreen();
      OLED::setCursor(0, 0);
      OLED::print(SymbolDot, FontStyle::LARGE);
      for (uint8_t x = 0; x < (i / 4); x++)
        OLED::print(SymbolDot, FontStyle::LARGE);
      OLED::refresh();
      osDelay(100);
    }
    setoffset = TipThermoModel::convertTipRawADCTouV(offset / 16, true);
  }
  setSettingValue(SettingsOptions::CalibrationOffset, setoffset);
  OLED::clearScreen();
  OLED::setCursor(0, 0);
  OLED::drawCheckbox(true);
  OLED::printNumber(setoffset, 4, FontStyle::LARGE);
  OLED::refresh();
  osDelay(1200);
}

// Provide the user the option to tune their own tip if custom is selected
// If not only do single point tuning as per usual
static bool settings_setCalibrate(void) {

  if (userConfirmation(translatedString(Tr->SettingsCalibrationWarning))) {
    // User confirmed
    // So we now perform the actual calculation
    setTipOffset();
  }
  return false;
}

static void settings_displayCalibrate(void) { printShortDescription(SettingsItemIndex::TemperatureCalibration, 5); }

static bool settings_setCalibrateVIN(void) {
  // Jump to the voltage calibration subscreen
  OLED::clearScreen();

  for (;;) {
    OLED::setCursor(0, 0);
    uint16_t voltage = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);
    OLED::printNumber(voltage / 10, 2, FontStyle::LARGE);
    OLED::print(SymbolDot, FontStyle::LARGE);
    OLED::printNumber(voltage % 10, 1, FontStyle::LARGE, false);
    OLED::print(SymbolVolts, FontStyle::LARGE);

    switch (getButtonState()) {
    case BUTTON_F_SHORT:
      nextSettingValue(SettingsOptions::VoltageDiv);
      break;
    case BUTTON_B_SHORT:
      prevSettingValue(SettingsOptions::VoltageDiv);
      break;
    case BUTTON_BOTH:
    case BUTTON_F_LONG:
    case BUTTON_B_LONG:
      saveSettings();
      OLED::setCursor(0, 0);
      OLED::printNumber(getSettingValue(SettingsOptions::VoltageDiv), 3, FontStyle::LARGE);
      OLED::refresh();
      waitForButtonPressOrTimeout(1 * TICKS_SECOND);
      return false;
    case BUTTON_NONE:
    default:
      break;
    }

    OLED::refresh();
    osDelay(40);
  }
  return false;
}

static void settings_displayReverseButtonTempChangeEnabled(void) {
  printShortDescription(SettingsItemIndex::ReverseButtonTempChange, 7);
  OLED::drawCheckbox(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled));
}

static void settings_displayTempChangeShortStep(void) {
  printShortDescription(SettingsItemIndex::TempChangeShortStep, 6);
  OLED::printNumber(getSettingValue(SettingsOptions::TempChangeShortStep), 2, FontStyle::LARGE);
}

static void settings_displayTempChangeLongStep(void) {
  printShortDescription(SettingsItemIndex::TempChangeLongStep, 6);
  OLED::printNumber(getSettingValue(SettingsOptions::TempChangeLongStep), 2, FontStyle::LARGE);
}

static void settings_displayPowerPulse(void) {
  printShortDescription(SettingsItemIndex::PowerPulsePower, 5);
  if (getSettingValue(SettingsOptions::KeepAwakePulse)) {
    OLED::printNumber(getSettingValue(SettingsOptions::KeepAwakePulse) / 10, 1, FontStyle::LARGE);
    OLED::print(SymbolDot, FontStyle::LARGE);
    OLED::printNumber(getSettingValue(SettingsOptions::KeepAwakePulse) % 10, 1, FontStyle::LARGE);
  } else {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  }
}

static bool settings_displayAnimationOptions(void) { return getSettingValue(SettingsOptions::AnimationSpeed) > 0; }
static void settings_displayAnimationLoop(void) {
  printShortDescription(SettingsItemIndex::AnimLoop, 7);
  OLED::drawCheckbox(getSettingValue(SettingsOptions::AnimationLoop));
}

static void settings_displayAnimationSpeed(void) {
  printShortDescription(SettingsItemIndex::AnimSpeed, 7);
  switch (getSettingValue(SettingsOptions::AnimationSpeed)) {
  case settingOffSpeed_t::SLOW:
    OLED::print(translatedString(Tr->SettingSlowChar), FontStyle::LARGE);
    break;
  case settingOffSpeed_t::MEDIUM:
    OLED::print(translatedString(Tr->SettingMediumChar), FontStyle::LARGE);
    break;
  case settingOffSpeed_t::FAST:
    OLED::print(translatedString(Tr->SettingFastChar), FontStyle::LARGE);
    break;
  default:
    OLED::print(translatedString(Tr->SettingOffChar), FontStyle::LARGE);
    break;
  }
}

static bool settings_showPowerPulseOptions(void) { return getSettingValue(SettingsOptions::KeepAwakePulse) > 0; }
static void settings_displayPowerPulseWait(void) {
  printShortDescription(SettingsItemIndex::PowerPulseWait, 7);
  OLED::printNumber(getSettingValue(SettingsOptions::KeepAwakePulseWait), 1, FontStyle::LARGE);
}

static void settings_displayPowerPulseDuration(void) {
  printShortDescription(SettingsItemIndex::PowerPulseDuration, 7);
  OLED::printNumber(getSettingValue(SettingsOptions::KeepAwakePulseDuration), 1, FontStyle::LARGE);
}

static void settings_displayBrightnessLevel(void) {
  OLED::drawArea(0, 0, 16, 16, brightnessIcon);
  OLED::setCursor(6 * FONT_12_WIDTH - 2, 0);
  // printShortDescription(SettingsItemIndex::Brightness, 7);
  OLED::printNumber((getSettingValue(SettingsOptions::OLEDBrightness) / 11 + 1), 2, FontStyle::LARGE);
  // While not optimal to apply this here, it is _very_ convienient
  OLED::setBrightness(getSettingValue(SettingsOptions::OLEDBrightness));
}

static void settings_displayInvertColor(void) {
  OLED::drawArea(0, 0, 24, 16, invertDisplayIcon);
  OLED::setCursor(7 * FONT_12_WIDTH - 2, 0);
  // printShortDescription(SettingsItemIndex::ColourInversion, 7);
  OLED::drawCheckbox(getSettingValue(SettingsOptions::OLEDInversion));
  // While not optimal to apply this here, it is _very_ convienient
  OLED::setInverseDisplay(getSettingValue(SettingsOptions::OLEDInversion));
}

static void settings_displayLogoTime(void) {
  printShortDescription(SettingsItemIndex::LOGOTime, 5);
  if (getSettingValue(SettingsOptions::LOGOTime) == 0) {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  } else if (getSettingValue(SettingsOptions::LOGOTime) == 5) {
    OLED::drawArea(OLED_WIDTH - 24 - 2, 0, 24, 16, infinityIcon);
  } else {
    OLED::printNumber(getSettingValue(SettingsOptions::LOGOTime), 2, FontStyle::LARGE);
    OLED::print(SymbolSeconds, FontStyle::LARGE);
  }
}

#ifdef HALL_SENSOR
static void settings_displayHallEffect(void) {
  printShortDescription(SettingsItemIndex::HallEffSensitivity, 7);
  OLED::printNumber(getSettingValue(SettingsOptions::HallEffectSensitivity), 1, FontStyle::LARGE, false);
}
static bool settings_showHallEffect(void) { return getHallSensorFitted(); }
#endif

// Indicates whether a menu transition is in progress, so that the menu icon
// animation is paused during the transition.
static bool animOpenState = false;

static void displayMenu(size_t index) {
  // Call into the menu
  // Draw title
  OLED::printWholeScreen(translatedString(Tr->SettingsMenuEntries[index]));
  static TickType_t menuSwitchLoopTick = 0;
  static size_t     menuCurrentIndex   = sizeof(rootSettingsMenu) + 1;
  TickType_t        step               = TICKS_100MS * 5;
  switch (getSettingValue(SettingsOptions::AnimationSpeed)) {
  case settingOffSpeed_t::FAST:
    step = TICKS_100MS * 3;
    break;
  case settingOffSpeed_t::MEDIUM:
    step = TICKS_100MS * 4;
    break;
  default: // SLOW or off - defaulted above
    break;
  }
  size_t currentFrame;
  if (!animOpenState && (getSettingValue(SettingsOptions::AnimationSpeed) != settingOffSpeed_t::OFF)) {
    if (menuCurrentIndex != index) {
      menuCurrentIndex   = index;
      menuSwitchLoopTick = xTaskGetTickCount();
    }
    currentFrame = ((xTaskGetTickCount() - menuSwitchLoopTick) / step);
    if (getSettingValue(SettingsOptions::AnimationLoop)) {
      currentFrame %= 3;
    } else if (currentFrame > 2) {
      currentFrame = 2;
    }
  } else {
    // We want the animation to restart after completing the transition.
    menuCurrentIndex = sizeof(rootSettingsMenu) + 1;
    // Always draw the last frame if icon animation is disabled.
    currentFrame = getSettingValue(SettingsOptions::AnimationSpeed) == settingOffSpeed_t::OFF ? 2 : 0;
  }
  // Draw symbol
  // 16 pixel wide image
  // less 2 pixel wide scrolling indicator
  OLED::drawArea(OLED_WIDTH - 16 - 2, 0, 16, 16, (&SettingsMenuIcons[index][(16 * 2) * currentFrame]));
}

static void settings_displayCalibrateVIN(void) { printShortDescription(SettingsItemIndex::VoltageCalibration, 5); }

#if defined(POW_DC) || defined(POW_QC)
static void settings_displayPowerMenu(void) { displayMenu(0); }
static bool settings_enterPowerMenu(void) {
  gui_Menu(powerMenu);
  return false;
}
#endif
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

uint8_t gui_getMenuLength(const menuitem *menu) {
  uint8_t scrollContentSize = 0;
  for (uint8_t i = 0; menu[i].draw != nullptr; i++) {
    if (menu[i].isVisible == nullptr) {
      scrollContentSize += 1; // Always visible
    } else if (menu[i].isVisible()) {
      scrollContentSize += 1; // Selectively visible and chosen to show
    }
  }
  return scrollContentSize;
}
void gui_Menu(const menuitem *menu) {
  // Draw the settings menu and provide iteration support etc

  // This is used to detect whether a menu-exit transition should be played.
  static bool wasInGuiMenu;
  wasInGuiMenu = true;

  enum class NavState {
    Idle,
    Entering,
    ScrollingDown,
    Exiting,
  };

  uint8_t    currentScreen          = 0; // Current screen index in the menu struct
  uint8_t    screensSkipped         = 0; // Number of screens skipped due to being disabled
  TickType_t autoRepeatTimer        = 0;
  TickType_t autoRepeatAcceleration = 0;
  bool       earlyExit              = false;
  bool       lcdRefresh             = true;

  ButtonState lastButtonState   = BUTTON_NONE;
  uint8_t     scrollContentSize = gui_getMenuLength(menu);

  bool     scrollBlink = false;
  bool     lastValue   = false;
  NavState navState    = NavState::Entering;

  ScrollMessage scrollMessage;

  while ((menu[currentScreen].draw != nullptr) && earlyExit == false) {
    bool valueChanged = false;
    // Handle menu transition:
    if (navState != NavState::Idle) {
      // Check if this menu item shall be skipped. If it shall be skipped,
      // `draw()` returns true. Draw on the secondary framebuffer as we want
      // to keep the primary framebuffer intact for the upcoming transition
      // animation.
      OLED::useSecondaryFramebuffer(true);
      if (menu[currentScreen].isVisible != nullptr) {
        if (!menu[currentScreen].isVisible()) {
          currentScreen++;
          screensSkipped++;
          OLED::useSecondaryFramebuffer(false);
          continue;
        }
      }

      animOpenState = true;
      // The menu entering/exiting transition uses the secondary framebuffer,
      // but the scroll down transition does not.
      if (navState == NavState::ScrollingDown) {
        OLED::useSecondaryFramebuffer(false);
      }
      OLED::setCursor(0, 0);
      OLED::clearScreen();
      menu[currentScreen].draw();
      if (navState == NavState::ScrollingDown) {
        // Play the scroll down animation.
        OLED::maskScrollIndicatorOnOLED();
        OLED::transitionScrollDown();
      } else {
        // The menu was drawn in a secondary framebuffer.
        // Now we play a transition from the pre-drawn primary
        // framebuffer to the new buffer.
        // The extra buffer is discarded at the end of the transition.
        OLED::useSecondaryFramebuffer(false);
        OLED::transitionSecondaryFramebuffer(navState == NavState::Entering);
      }
      animOpenState = false;
      navState      = NavState::Idle;
    }

    // If the user has hesitated for >=3 seconds, show the long text
    // Otherwise "draw" the option
    if ((xTaskGetTickCount() - lastButtonTime < (TICKS_SECOND * 3)) || menu[currentScreen].description == 0) {
      lcdRefresh = true;
      OLED::setCursor(0, 0);
      OLED::clearScreen();
      menu[currentScreen].draw();
      uint8_t indicatorHeight = OLED_HEIGHT / scrollContentSize;
      uint8_t position        = OLED_HEIGHT * (currentScreen - screensSkipped) / scrollContentSize;
      if (lastValue)
        scrollBlink = !scrollBlink;
      if (!lastValue || !scrollBlink)
        OLED::drawScrollIndicator(position, indicatorHeight);
    } else {
      // Draw description
      const char *description = translatedString(Tr->SettingsDescriptions[menu[currentScreen].description - 1]);
      lcdRefresh |= scrollMessage.drawUpdate(description, xTaskGetTickCount());
    }

    if (lcdRefresh) {
      OLED::refresh(); // update the LCD
      osDelay(40);
      lcdRefresh = false;
    }

    ButtonState buttons = getButtonState();

    if (buttons != lastButtonState) {
      autoRepeatAcceleration = 0;
      lastButtonState        = buttons;
    }

    auto callIncrementHandler = [&]() {
      wasInGuiMenu = false;
      valueChanged = true;
      bool res     = false;
      if ((int)menu[currentScreen].autoSettingOption < (int)SettingsOptions::SettingsOptionsLength) {
        res = nextSettingValue(menu[currentScreen].autoSettingOption);
      } else if (menu[currentScreen].incrementHandler != nullptr) {
        res = menu[currentScreen].incrementHandler();
      } else {
        earlyExit = true;
      }
      if (wasInGuiMenu) {
        navState = NavState::Exiting;
      }
      wasInGuiMenu = true;
      return res;
    };

    switch (buttons) {
    case BUTTON_BOTH:
      earlyExit = true; // will make us exit next loop
      scrollMessage.reset();
      break;
    case BUTTON_F_SHORT:
      // increment
      if (scrollMessage.isReset()) {
        lastValue = callIncrementHandler();
      } else
        scrollMessage.reset();
      break;
    case BUTTON_B_SHORT:
      if (scrollMessage.isReset()) {
        currentScreen++;
        navState  = NavState::ScrollingDown;
        lastValue = false;
      } else
        scrollMessage.reset();
      break;
    case BUTTON_F_LONG:
      if (xTaskGetTickCount() + autoRepeatAcceleration > autoRepeatTimer + PRESS_ACCEL_INTERVAL_MAX) {

        if ((lastValue = callIncrementHandler()))
          autoRepeatTimer = 1000;
        else
          autoRepeatTimer = 0;

        autoRepeatTimer += xTaskGetTickCount();

        scrollMessage.reset();

        autoRepeatAcceleration += PRESS_ACCEL_STEP;
      }
      break;
    case BUTTON_B_LONG:
      if (xTaskGetTickCount() - autoRepeatTimer + autoRepeatAcceleration > PRESS_ACCEL_INTERVAL_MAX) {
        currentScreen++;
        navState        = NavState::ScrollingDown;
        autoRepeatTimer = xTaskGetTickCount();
        scrollMessage.reset();

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

    if ((xTaskGetTickCount() - lastButtonTime) > (TICKS_SECOND * 30)) {
      // If user has not pressed any buttons in 30 seconds, exit back a menu layer
      // This will trickle the user back to the main screen eventually
      earlyExit = true;
      scrollMessage.reset();
    }
    if (valueChanged) {
      // If user changed value, update the scroll content size
      scrollContentSize = gui_getMenuLength(menu);
    }
  }
}

void enterSettingsMenu() {
  gui_Menu(rootSettingsMenu); // Call the root menu
  saveSettings();
}
