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
static void displayInputVRange(void);
static bool showInputVOptions(void);
static void displayInputMinVRange(void);
#endif
#ifdef POW_QC
static void displayQCInputV(void);
#endif
#if POW_PD
static void displayPDNegTimeout(void);
static void displayPDVpdo(void);
#endif
static void displaySensitivity(void);
static void displayShutdownTime(void);
static bool showSleepOptions(void);
#ifndef NO_SLEEP_MODE
static bool setSleepTemp(void);
static void displaySleepTemp(void);
static void displaySleepTime(void);
#endif
static bool setTempF(void);
static void displayTempF(void);
static void displayAdvancedSolderingScreens(void);
static void displayAdvancedIDLEScreens(void);
static void displayScrollSpeed(void);
static void displayPowerLimit(void);
#ifdef BLE_ENABLED
static void displayBluetoothLE(void);
#endif
#ifndef NO_DISPLAY_ROTATE
static bool setDisplayRotation(void);
static void displayDisplayRotation(void);
#endif

static bool setBoostTemp(void);
static void displayBoostTemp(void);
static void displayAutomaticStartMode(void);
static void displayLockingMode(void);
static void displayCoolingBlinkEnabled(void);
static bool setResetSettings(void);
static void displayResetSettings(void);
static bool setCalibrate(void);
static void displayCalibrate(void);
static bool setCalibrateVIN(void);
static void displayCalibrateVIN(void);
static void displayReverseButtonTempChangeEnabled(void);
static void displayTempChangeShortStep(void);
static void displayTempChangeLongStep(void);
static void displayPowerPulse(void);
static bool displayAnimationOptions(void);
static void displayAnimationSpeed(void);
static void displayAnimationLoop(void);
static void displayPowerPulseWait(void);
static bool showPowerPulseOptions(void);
static void displayPowerPulseDuration(void);
static void displayBrightnessLevel(void);
static void displayInvertColor(void);
static void displayLogoTime(void);

#ifdef HALL_SENSOR
static void displayHallEffect(void);
static bool showHallEffect(void);
#endif
// Menu functions

#if defined(POW_DC) || defined(POW_QC)
static void displayPowerMenu(void);
static bool enterPowerMenu(void);
#endif
static void displaySolderingMenu(void);
static bool enterSolderingMenu(void);
static void displayPowerSavingMenu(void);
static bool enterPowerSavingMenu(void);
static void displayUIMenu(void);
static bool enterUIMenu(void);
static void displayAdvancedMenu(void);
static bool enterAdvancedMenu(void);
/*
 * Root Settings Menu
 *
 * Power Menu
 *  Power Source
 *  -Minimum Voltage
 *  QC Voltage
 *  PD Timeout
 *  PDVpdo
 *
 * Soldering
 *  Boost Mode Temp
 *  Auto Start
 *  Temp Change Short Step
 *  Temp Change Long Step
 *  Locking Mode
 *
 * Power Saving
 *  Motion Sensitivity
 *  -Sleep Temp
 *  -Sleep Time
 *  -Shutdown Time
 *  Hall Sensor Sensitivity
 *
 * UI
 *  Temperature Unit
 *  Display Orientation
 *  Cooldown Blink
 *  Scrolling Speed
 *  Swap Temp Change Buttons + -
 *  Animation Speed
 *  -Animation Loop
 *  OLED Brightness
 *  Invert Screen
 *  Logo Timeout
 *  Detailed IDLE
 *  Detailed Soldering
 *
 * Advanced
 *  BluetoothLE
 *  Power Limit
 *  Calibrate CJC At Next Boot
 *  Calibrate Input V
 *  Power Pulse
 *  -Power Pulse Delay
 *  -Power Pulse Duration
 *  Factory Reset
 *
 */
const menuitem rootSettingsMenu[] {
  /*
   * Power Menu
   * Soldering Menu
   * Power Saving Menu
   * UI Menu
   * Advanced Menu
   * // Language
   * Exit
   */
#if defined(POW_DC) || defined(POW_QC)
  {0, enterPowerMenu, displayPowerMenu, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0}, /*Power*/
#endif
      {0, enterSolderingMenu, displaySolderingMenu, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0},                                       /*Soldering*/
      {0, enterPowerSavingMenu, displayPowerSavingMenu, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0},                                   /*Sleep Options Menu*/
      {0, enterUIMenu, displayUIMenu, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0},                                                     /*UI Menu*/
      {0, enterAdvancedMenu, displayAdvancedMenu, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0},                                         /*Advanced Menu*/
      {0, settings_setLanguageSwitch, settings_displayLanguageSwitch, settings_showLanguageSwitch, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0}, /*Language Switch*/
  {
    0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0
  } // end of menu marker. DO NOT REMOVE
};

#if defined(POW_DC) || defined(POW_QC) || defined(POW_PD)
const menuitem powerMenu[] = {
/*
 *  Power Source
 *  -Minimum Voltage
 *  QC Voltage
 *  PD Timeout
 *  PDVpdo
 */
#ifdef POW_DC
    {SETTINGS_DESC(SettingsItemIndex::DCInCutoff), nullptr, displayInputVRange, nullptr, SettingsOptions::MinDCVoltageCells, SettingsItemIndex::DCInCutoff, 6},            /*Voltage input*/
    {SETTINGS_DESC(SettingsItemIndex::MinVolCell), nullptr, displayInputMinVRange, showInputVOptions, SettingsOptions::MinVoltageCells, SettingsItemIndex::MinVolCell, 5}, /*Minimum voltage input*/
#endif
#ifdef POW_QC
    {SETTINGS_DESC(SettingsItemIndex::QCMaxVoltage), nullptr, displayQCInputV, nullptr, SettingsOptions::QCIdealVoltage, SettingsItemIndex::QCMaxVoltage, 4}, /*Voltage input*/
#endif
#if POW_PD
    {SETTINGS_DESC(SettingsItemIndex::PDNegTimeout), nullptr, displayPDNegTimeout, nullptr, SettingsOptions::PDNegTimeout, SettingsItemIndex::PDNegTimeout, 5}, /*PD timeout setup*/
    {SETTINGS_DESC(SettingsItemIndex::PDVpdo), nullptr, displayPDVpdo, nullptr, SettingsOptions::PDVpdo, SettingsItemIndex::PDVpdo, 7 }, /*Toggle PPS & EPR*/
#endif
    {0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0} // end of menu marker. DO NOT REMOVE
};
#endif
const menuitem solderingMenu[] = {
    /*
     *  Boost Mode Temp
     *  Auto Start
     *  Temp Change Short Step
     *  Temp Change Long Step
     *  Locking Mode
     */
    {SETTINGS_DESC(SettingsItemIndex::BoostTemperature), setBoostTemp, displayBoostTemp, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::BoostTemperature, 5}, /*Boost Temp*/
    {SETTINGS_DESC(SettingsItemIndex::AutoStart), nullptr, displayAutomaticStartMode, nullptr, SettingsOptions::AutoStartMode, SettingsItemIndex::AutoStart, 7},                   /*Auto start*/
    {SETTINGS_DESC(SettingsItemIndex::TempChangeShortStep), nullptr, displayTempChangeShortStep, nullptr, SettingsOptions::TempChangeShortStep, SettingsItemIndex::TempChangeShortStep,
     6}, /*Temp change short step*/
    {SETTINGS_DESC(SettingsItemIndex::TempChangeLongStep), nullptr, displayTempChangeLongStep, nullptr, SettingsOptions::TempChangeLongStep, SettingsItemIndex::TempChangeLongStep,
     6},                                                                                                                                                    /*Temp change long step*/
    {SETTINGS_DESC(SettingsItemIndex::LockingMode), nullptr, displayLockingMode, nullptr, SettingsOptions::LockingMode, SettingsItemIndex::LockingMode, 7}, /*Locking Mode*/
    {0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0}                                                 // end of menu marker. DO NOT REMOVE
};
const menuitem PowerSavingMenu[] = {
    /*
     *  Motion Sensitivity
     *  -Sleep Temp
     *  -Sleep Time
     *  -Shutdown Time
     *  Hall Sensor Sensitivity
     */
    {SETTINGS_DESC(SettingsItemIndex::MotionSensitivity), nullptr, displaySensitivity, nullptr, SettingsOptions::Sensitivity, SettingsItemIndex::MotionSensitivity, 7}, /* Motion Sensitivity*/
#ifndef NO_SLEEP_MODE
    {SETTINGS_DESC(SettingsItemIndex::SleepTemperature), setSleepTemp, displaySleepTemp, showSleepOptions, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::SleepTemperature,
     5},                                                                                                                                                           /*Sleep Temp*/
    {SETTINGS_DESC(SettingsItemIndex::SleepTimeout), nullptr, displaySleepTime, showSleepOptions, SettingsOptions::SleepTime, SettingsItemIndex::SleepTimeout, 5}, /*Sleep Time*/
#endif
    {SETTINGS_DESC(SettingsItemIndex::ShutdownTimeout), nullptr, displayShutdownTime, showSleepOptions, SettingsOptions::ShutdownTime, SettingsItemIndex::ShutdownTimeout, 5}, /*Shutdown Time*/
#ifdef HALL_SENSOR
    {SETTINGS_DESC(SettingsItemIndex::HallEffSensitivity), nullptr, displayHallEffect, showHallEffect, SettingsOptions::HallEffectSensitivity, SettingsItemIndex::HallEffSensitivity,
     7}, /* HallEffect Sensitivity*/
#endif
    {0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0} // end of menu marker. DO NOT REMOVE
};
const menuitem UIMenu[] = {
    /*
     *  Temperature Unit
     *  Display Orientation
     *  Cooldown Blink
     *  Scrolling Speed
     *  Swap Temp Change Buttons + -
     *  Animation Speed
     *  -Animation Loop
     *  OLED Brightness
     *  Invert Screen
     *  Logo Timeout
     *  Detailed IDLE
     *  Detailed Soldering
     */
    {SETTINGS_DESC(SettingsItemIndex::TemperatureUnit), setTempF, displayTempF, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::TemperatureUnit,
     7}, /* Temperature units, this has to be the first element in the array to work with the logic in enterUIMenu() */
#ifndef NO_DISPLAY_ROTATE
    {SETTINGS_DESC(SettingsItemIndex::DisplayRotation), setDisplayRotation, displayDisplayRotation, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::DisplayRotation,
     7}, /*Display Rotation*/
#endif
    {SETTINGS_DESC(SettingsItemIndex::CooldownBlink), nullptr, displayCoolingBlinkEnabled, nullptr, SettingsOptions::CoolingTempBlink, SettingsItemIndex::CooldownBlink, 7}, /*Cooling blink warning*/
    {SETTINGS_DESC(SettingsItemIndex::ScrollingSpeed), nullptr, displayScrollSpeed, nullptr, SettingsOptions::DescriptionScrollSpeed, SettingsItemIndex::ScrollingSpeed,
     7}, /*Scroll Speed for descriptions*/
    {SETTINGS_DESC(SettingsItemIndex::ReverseButtonTempChange), nullptr, displayReverseButtonTempChangeEnabled, nullptr, SettingsOptions::ReverseButtonTempChangeEnabled,
     SettingsItemIndex::ReverseButtonTempChange, 7},                                                                                                          /*Reverse Temp change buttons + - */
    {SETTINGS_DESC(SettingsItemIndex::AnimSpeed), nullptr, displayAnimationSpeed, nullptr, SettingsOptions::AnimationSpeed, SettingsItemIndex::AnimSpeed, 7}, /*Animation Speed adjustment */
    {SETTINGS_DESC(SettingsItemIndex::AnimLoop), nullptr, displayAnimationLoop, displayAnimationOptions, SettingsOptions::AnimationLoop, SettingsItemIndex::AnimLoop, 7}, /*Animation Loop switch */
    {SETTINGS_DESC(SettingsItemIndex::Brightness), nullptr, displayBrightnessLevel, nullptr, SettingsOptions::OLEDBrightness, SettingsItemIndex::Brightness, 6},          /*Brightness Level*/
    {SETTINGS_DESC(SettingsItemIndex::ColourInversion), nullptr, displayInvertColor, nullptr, SettingsOptions::OLEDInversion, SettingsItemIndex::ColourInversion, 7},     /*Invert screen colour*/
    {SETTINGS_DESC(SettingsItemIndex::LOGOTime), nullptr, displayLogoTime, nullptr, SettingsOptions::LOGOTime, SettingsItemIndex::LOGOTime, 5},                           /*Set logo duration*/
    {SETTINGS_DESC(SettingsItemIndex::AdvancedIdle), nullptr, displayAdvancedIDLEScreens, nullptr, SettingsOptions::DetailedIDLE, SettingsItemIndex::AdvancedIdle, 7},    /*Advanced idle screen*/
    {SETTINGS_DESC(SettingsItemIndex::AdvancedSoldering), nullptr, displayAdvancedSolderingScreens, nullptr, SettingsOptions::DetailedSoldering, SettingsItemIndex::AdvancedSoldering,
     7},                                                                                                    /*Advanced soldering screen*/
    {0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0} // end of menu marker. DO NOT REMOVE
};
const menuitem advancedMenu[] = {
/*
 *  BluetoothLE
 *  Power Limit
 *  Calibrate CJC At Next Boot
 *  Calibrate Input V
 *  Power Pulse
 *  -Power Pulse Delay
 *  -Power Pulse Duration
 *  Factory Reset
 */
#ifdef BLE_ENABLED
    {SETTINGS_DESC(SettingsItemIndex::BluetoothLE), nullptr, displayBluetoothLE, nullptr, SettingsOptions::BluetoothLE, SettingsItemIndex::BluetoothLE, 7}, /*Toggle BLE*/
#endif
    {SETTINGS_DESC(SettingsItemIndex::PowerLimit), nullptr, displayPowerLimit, nullptr, SettingsOptions::PowerLimit, SettingsItemIndex::PowerLimit, 4}, /*Power limit*/
    {SETTINGS_DESC(SettingsItemIndex::CalibrateCJC), setCalibrate, displayCalibrate, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::CalibrateCJC,
     7}, /*Calibrate Cold Junktion Compensation at next boot*/
    {SETTINGS_DESC(SettingsItemIndex::VoltageCalibration), setCalibrateVIN, displayCalibrateVIN, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::VoltageCalibration,
     5},                                                                                                                                                              /*Voltage input cal*/
    {SETTINGS_DESC(SettingsItemIndex::PowerPulsePower), nullptr, displayPowerPulse, nullptr, SettingsOptions::KeepAwakePulse, SettingsItemIndex::PowerPulsePower, 5}, /*Power Pulse adjustment */
    {SETTINGS_DESC(SettingsItemIndex::PowerPulseWait), nullptr, displayPowerPulseWait, showPowerPulseOptions, SettingsOptions::KeepAwakePulseWait, SettingsItemIndex::PowerPulseWait,
     7}, /*Power Pulse Wait adjustment*/
    {SETTINGS_DESC(SettingsItemIndex::PowerPulseDuration), nullptr, displayPowerPulseDuration, showPowerPulseOptions, SettingsOptions::KeepAwakePulseDuration, SettingsItemIndex::PowerPulseDuration,
     7}, /*Power Pulse Duration adjustment*/
    {SETTINGS_DESC(SettingsItemIndex::SettingsReset), setResetSettings, displayResetSettings, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::SettingsReset, 7}, /*Resets settings*/
    {0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0} // end of menu marker. DO NOT REMOVE
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

static void displayInputVRange(void) {

  if (getSettingValue(SettingsOptions::MinDCVoltageCells)) {
    OLED::printNumber(2 + getSettingValue(SettingsOptions::MinDCVoltageCells), 1, FontStyle::LARGE);
    OLED::print(LargeSymbolCellCount, FontStyle::LARGE);
  } else {
    OLED::print(LargeSymbolDC, FontStyle::LARGE);
  }
}

static bool showInputVOptions(void) { return getSettingValue(SettingsOptions::MinDCVoltageCells) > 0; }
static void displayInputMinVRange(void) {

  OLED::printNumber(getSettingValue(SettingsOptions::MinVoltageCells) / 10, 1, FontStyle::LARGE);
  OLED::print(LargeSymbolDot, FontStyle::LARGE);
  OLED::printNumber(getSettingValue(SettingsOptions::MinVoltageCells) % 10, 1, FontStyle::LARGE);
}
#endif
#ifdef POW_QC

static void displayQCInputV(void) {

  // These are only used in QC modes
  // Allows setting the voltage negotiated for QC
  auto voltage = getSettingValue(SettingsOptions::QCIdealVoltage);
  OLED::printNumber(voltage / 10, 2, FontStyle::LARGE);
  OLED::print(LargeSymbolDot, FontStyle::LARGE);
  OLED::printNumber(voltage % 10, 1, FontStyle::LARGE);
}

#endif

#if POW_PD

static void displayPDNegTimeout(void) {

  auto value = getSettingValue(SettingsOptions::PDNegTimeout);
  if (value == 0) {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  } else {
    OLED::printNumber(value, 3, FontStyle::LARGE);
  }
}
static void displayPDVpdo(void) { OLED::drawCheckbox(getSettingValue(SettingsOptions::PDVpdo)); }
#endif

static bool setBoostTemp(void) {
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

static void displayBoostTemp(void) {

  if (getSettingValue(SettingsOptions::BoostTemp)) {
    OLED::printNumber(getSettingValue(SettingsOptions::BoostTemp), 3, FontStyle::LARGE);
  } else {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  }
}

static void displayAutomaticStartMode(void) {

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

static void displayTempChangeShortStep(void) { OLED::printNumber(getSettingValue(SettingsOptions::TempChangeShortStep), 2, FontStyle::LARGE); }

static void displayTempChangeLongStep(void) { OLED::printNumber(getSettingValue(SettingsOptions::TempChangeLongStep), 2, FontStyle::LARGE); }

static void displayLockingMode(void) {

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

static void displaySensitivity(void) { OLED::printNumber(getSettingValue(SettingsOptions::Sensitivity), 1, FontStyle::LARGE, false); }
static bool showSleepOptions(void) { return getSettingValue(SettingsOptions::Sensitivity) > 0; }

#ifndef NO_SLEEP_MODE

static bool setSleepTemp(void) {
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

static void displaySleepTemp(void) { OLED::printNumber(getSettingValue(SettingsOptions::SleepTemp), 3, FontStyle::LARGE); }

static void displaySleepTime(void) {

  if (getSettingValue(SettingsOptions::SleepTime) == 0) {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  } else if (getSettingValue(SettingsOptions::SleepTime) < 6) {
    OLED::printNumber(getSettingValue(SettingsOptions::SleepTime) * 10, 2, FontStyle::LARGE);
    OLED::print(LargeSymbolSeconds, FontStyle::LARGE);
  } else {
    OLED::printNumber(getSettingValue(SettingsOptions::SleepTime) - 5, 2, FontStyle::LARGE);
    OLED::print(LargeSymbolMinutes, FontStyle::LARGE);
  }
}
#endif

static void displayShutdownTime(void) {

  if (getSettingValue(SettingsOptions::ShutdownTime) == 0) {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  } else {
    OLED::printNumber(getSettingValue(SettingsOptions::ShutdownTime), 2, FontStyle::LARGE);
    OLED::print(LargeSymbolMinutes, FontStyle::LARGE);
  }
}

#ifdef HALL_SENSOR
static void displayHallEffect(void) { OLED::printNumber(getSettingValue(SettingsOptions::HallEffectSensitivity), 1, FontStyle::LARGE, false); }
static bool showHallEffect(void) { return getHallSensorFitted(); }
#endif

static bool setTempF(void) {
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

static void displayTempF(void) { OLED::print((getSettingValue(SettingsOptions::TemperatureInF)) ? LargeSymbolDegF : LargeSymbolDegC, FontStyle::LARGE); }

#ifndef NO_DISPLAY_ROTATE
static bool setDisplayRotation(void) {
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

static void displayDisplayRotation(void) {

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

static void displayCoolingBlinkEnabled(void) { OLED::drawCheckbox(getSettingValue(SettingsOptions::CoolingTempBlink)); }

static void displayScrollSpeed(void) { OLED::print(translatedString((getSettingValue(SettingsOptions::DescriptionScrollSpeed)) ? Tr->SettingFastChar : Tr->SettingSlowChar), FontStyle::LARGE); }

static void displayReverseButtonTempChangeEnabled(void) { OLED::drawCheckbox(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled)); }

static void displayAnimationSpeed(void) {

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

static bool displayAnimationOptions(void) { return getSettingValue(SettingsOptions::AnimationSpeed) > 0; }
static void displayAnimationLoop(void) { OLED::drawCheckbox(getSettingValue(SettingsOptions::AnimationLoop)); }

static void displayBrightnessLevel(void) {

  OLED::printNumber((getSettingValue(SettingsOptions::OLEDBrightness) / BRIGHTNESS_STEP + 1), 2, FontStyle::LARGE);
  // While not optimal to apply this here, it is _very_ convienient
  OLED::setBrightness(getSettingValue(SettingsOptions::OLEDBrightness));
}

static void displayInvertColor(void) {

  OLED::drawCheckbox(getSettingValue(SettingsOptions::OLEDInversion));
  // While not optimal to apply this here, it is _very_ convienient
  OLED::setInverseDisplay(getSettingValue(SettingsOptions::OLEDInversion));
}

static void displayLogoTime(void) {

  if (getSettingValue(SettingsOptions::LOGOTime) == 0) {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  } else if (getSettingValue(SettingsOptions::LOGOTime) == 5) {
    OLED::drawArea(OLED_WIDTH - 24 - 2, 0, 24, 16, infinityIcon);
  } else {
    OLED::printNumber(getSettingValue(SettingsOptions::LOGOTime), 2, FontStyle::LARGE);
    OLED::print(LargeSymbolSeconds, FontStyle::LARGE);
  }
}

static void displayAdvancedIDLEScreens(void) { OLED::drawCheckbox(getSettingValue(SettingsOptions::DetailedIDLE)); }

static void displayAdvancedSolderingScreens(void) { OLED::drawCheckbox(getSettingValue(SettingsOptions::DetailedSoldering)); }
#ifdef BLE_ENABLED
static void displayBluetoothLE(void) { OLED::drawCheckbox(getSettingValue(SettingsOptions::BluetoothLE)); }
#endif
static void displayPowerLimit(void) {

  if (getSettingValue(SettingsOptions::PowerLimit) == 0) {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  } else {
    OLED::printNumber(getSettingValue(SettingsOptions::PowerLimit), 3, FontStyle::LARGE);
    OLED::print(LargeSymbolWatts, FontStyle::LARGE);
  }
}

static bool setCalibrate(void) {
  if (getSettingValue(SettingsOptions::CalibrateCJC) < 1) {
    if (userConfirmation(translatedString(Tr->SettingsCalibrationWarning))) {
      // User confirmed
      // So we now set the tick
      setSettingValue(SettingsOptions::CalibrateCJC, 1);
    }
  } else {
    setSettingValue(SettingsOptions::CalibrateCJC, 0);
  }
  return false;
}

static void displayCalibrate(void) { OLED::drawCheckbox(getSettingValue(SettingsOptions::CalibrateCJC)); }

static bool setCalibrateVIN(void) {
  // Jump to the voltage calibration subscreen
  OLED::clearScreen();

  for (;;) {
    OLED::setCursor(0, 0);
    uint16_t voltage = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);
    OLED::printNumber(voltage / 10, 2, FontStyle::LARGE);
    OLED::print(LargeSymbolDot, FontStyle::LARGE);
    OLED::printNumber(voltage % 10, 1, FontStyle::LARGE, false);
    OLED::print(LargeSymbolVolts, FontStyle::LARGE);

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
      OLED::clearScreen();
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

static void displayCalibrateVIN(void) {}

static void displayPowerPulse(void) {

  if (getSettingValue(SettingsOptions::KeepAwakePulse)) {
    OLED::printNumber(getSettingValue(SettingsOptions::KeepAwakePulse) / 10, 1, FontStyle::LARGE);
    OLED::print(LargeSymbolDot, FontStyle::LARGE);
    OLED::printNumber(getSettingValue(SettingsOptions::KeepAwakePulse) % 10, 1, FontStyle::LARGE);
  } else {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  }
}

static bool showPowerPulseOptions(void) { return getSettingValue(SettingsOptions::KeepAwakePulse) > 0; }
static void displayPowerPulseWait(void) { OLED::printNumber(getSettingValue(SettingsOptions::KeepAwakePulseWait), 1, FontStyle::LARGE); }

static void displayPowerPulseDuration(void) { OLED::printNumber(getSettingValue(SettingsOptions::KeepAwakePulseDuration), 1, FontStyle::LARGE); }

static bool setResetSettings(void) {
  if (userConfirmation(translatedString(Tr->SettingsResetWarning))) {
    resetSettings();
    warnUser(translatedString(Tr->ResetOKMessage), 3 * TICKS_SECOND);
    reboot();
  }
  return false;
}

static void displayResetSettings(void) {}

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

#if defined(POW_DC) || defined(POW_QC)
static void displayPowerMenu(void) { displayMenu(0); }
static bool enterPowerMenu(void) {
  gui_Menu(powerMenu);
  return false;
}
#endif
static void displaySolderingMenu(void) { displayMenu(1); }
static bool enterSolderingMenu(void) {
  gui_Menu(solderingMenu);
  return false;
}
static void displayPowerSavingMenu(void) { displayMenu(2); }
static bool enterPowerSavingMenu(void) {
  gui_Menu(PowerSavingMenu);
  return false;
}
static void displayUIMenu(void) { displayMenu(3); }
static bool enterUIMenu(void) {
  gui_Menu(HasFahrenheit ? UIMenu : UIMenu + 1);
  return false;
}
static void displayAdvancedMenu(void) { displayMenu(4); }
static bool enterAdvancedMenu(void) {
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
      if (menu[currentScreen].shortDescriptionSize > 0) {
        printShortDescription(menu[currentScreen].shortDescriptionIndex, menu[currentScreen].shortDescriptionSize);
      }
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
      if (menu[currentScreen].shortDescriptionSize > 0) {
        printShortDescription(menu[currentScreen].shortDescriptionIndex, menu[currentScreen].shortDescriptionSize);
      }
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

    if ((xTaskGetTickCount() - lastButtonTime) > (TICKS_SECOND * 2 * 60)) {
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
