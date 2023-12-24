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
#endif /* POW_DC */

#ifdef POW_QC
static void displayQCInputV(void);
#endif /* POW_QC */

#ifdef POW_PD
static void displayPDNegTimeout(void);
static void displayPDVpdo(void);
#endif /* POW_PD */

static void displaySensitivity(void);
static void displayShutdownTime(void);
static bool showSleepOptions(void);

#ifndef NO_SLEEP_MODE
static bool setSleepTemp(void);
static void displaySleepTemp(void);
static void displaySleepTime(void);
#endif /* *not* NO_SLEEP_MODE */

static bool setTempF(void);
static void displayTempF(void);
static void displayAdvancedSolderingScreens(void);
static void displayAdvancedIDLEScreens(void);
static void displayScrollSpeed(void);
static void displayPowerLimit(void);

#ifdef BLE_ENABLED
static void displayBluetoothLE(void);
#endif /* BLE_ENABLED */

#ifndef NO_DISPLAY_ROTATE
static bool setDisplayRotation(void);
static void displayDisplayRotation(void);
#endif /* *not* NO_DISPLAY_ROTATE */

static bool setBoostTemp(void);
static void displayBoostTemp(void);

#ifdef PROFILE_SUPPORT
static bool setProfilePreheatTemp();
static bool setProfilePhase1Temp();
static bool setProfilePhase2Temp();
static bool setProfilePhase3Temp();
static bool setProfilePhase4Temp();
static bool setProfilePhase5Temp();
static void displayProfilePhases(void);
static void displayProfilePreheatTemp(void);
static void displayProfilePreheatSpeed(void);
static void displayProfilePhase1Temp(void);
static void displayProfilePhase1Duration(void);
static void displayProfilePhase2Temp(void);
static void displayProfilePhase2Duration(void);
static void displayProfilePhase3Temp(void);
static void displayProfilePhase3Duration(void);
static void displayProfilePhase4Temp(void);
static void displayProfilePhase4Duration(void);
static void displayProfilePhase5Temp(void);
static void displayProfilePhase5Duration(void);
static void displayProfileCooldownSpeed(void);
static bool showProfileOptions(void);
static bool showProfilePhase2Options(void);
static bool showProfilePhase3Options(void);
static bool showProfilePhase4Options(void);
static bool showProfilePhase5Options(void);
#endif /* PROFILE_SUPPORT */

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
#endif /* HALL_SENSOR */

// Menu functions

#if defined(POW_DC) || defined(POW_QC)
static void displayPowerMenu(void);
#endif /* POW_DC or POW_QC */

static void displaySolderingMenu(void);
static void displayPowerSavingMenu(void);
static void displayUIMenu(void);
static void displayAdvancedMenu(void);

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
 *  Profile Phases
 *  Profile Preheat Temperature
 *  Profile Preheat Max Temperature Change Per Second
 *  Profile Phase 1 Temperature
 *  Profile Phase 1 Duration (s)
 *  Profile Phase 2 Temperature
 *  Profile Phase 2 Duration (s)
 *  Profile Phase 3 Temperature
 *  Profile Phase 3 Duration (s)
 *  Profile Phase 4 Temperature
 *  Profile Phase 4 Duration (s)
 *  Profile Phase 5 Temperature
 *  Profile Phase 5 Duration (s)
 *  Profile Cooldown Max Temperature Change Per Second
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
 *  Swap Temp Change Buttons +/-
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

/* vvv !!!DISABLE CLANG-FORMAT for menuitems initialization!!! vvv */

/* clang-format off */

/* A lot of suggestions by clang-format can be useful
 * but not when dealing with such menuitems declarations.
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
  /* Power */
  {0, nullptr, displayPowerMenu, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0},
#endif
  /* Soldering */
  {0, nullptr, displaySolderingMenu, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0},
  /* Sleep Options Menu */
  {0, nullptr, displayPowerSavingMenu, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0},
  /* UI Menu */
  {0, nullptr, displayUIMenu, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0},
  /* Advanced Menu */
  {0, nullptr, displayAdvancedMenu, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0},
  /* Language Switch */
  {0, settings_setLanguageSwitch, settings_displayLanguageSwitch, settings_showLanguageSwitch, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0},
  /* vvvv end of menu marker. DO NOT REMOVE vvvv */
  {0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0}
  /* ^^^^ end of menu marker. DO NOT REMOVE ^^^^ */
};

#if defined(POW_DC) || defined(POW_QC) || defined(POW_PD)
const menuitem powerMenu[] = {
  /*
   * Power Source
   * -Minimum Voltage
   * QC Voltage
   * PD Timeout
   * PDVpdo
   */
#ifdef POW_DC
  /* Voltage input */
  {SETTINGS_DESC(SettingsItemIndex::DCInCutoff), nullptr, displayInputVRange, nullptr, SettingsOptions::MinDCVoltageCells, SettingsItemIndex::DCInCutoff, 6},
  /* Minimum voltage input */
  {SETTINGS_DESC(SettingsItemIndex::MinVolCell), nullptr, displayInputMinVRange, showInputVOptions, SettingsOptions::MinVoltageCells, SettingsItemIndex::MinVolCell, 5},
#endif
#ifdef POW_QC
  /* Voltage input */
  {SETTINGS_DESC(SettingsItemIndex::QCMaxVoltage), nullptr, displayQCInputV, nullptr, SettingsOptions::QCIdealVoltage, SettingsItemIndex::QCMaxVoltage, 4},
#endif
#ifdef POW_PD
  /* PD timeout setup */
  {SETTINGS_DESC(SettingsItemIndex::PDNegTimeout), nullptr, displayPDNegTimeout, nullptr, SettingsOptions::PDNegTimeout, SettingsItemIndex::PDNegTimeout, 5},
  /* Toggle PPS & EPR */
  {SETTINGS_DESC(SettingsItemIndex::PDVpdo), nullptr, displayPDVpdo, nullptr, SettingsOptions::PDVpdo, SettingsItemIndex::PDVpdo, 7},
#endif
  /* vvvv end of menu marker. DO NOT REMOVE vvvv */
  {0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0}
  /* ^^^^ end of menu marker. DO NOT REMOVE ^^^^ */
};
#endif /* POW_DC or POW_QC or POW_PD */

const menuitem solderingMenu[] = {
  /*
   *  Boost Mode Temp
   *  Auto Start
   *  Temp Change Short Step
   *  Temp Change Long Step
   *  Locking Mode
   *  Profile Phases
   *  Profile Preheat Temperature
   *  Profile Preheat Max Temperature Change Per Second
   *  Profile Phase 1 Temperature
   *  Profile Phase 1 Duration (s)
   *  Profile Phase 2 Temperature
   *  Profile Phase 2 Duration (s)
   *  Profile Phase 3 Temperature
   *  Profile Phase 3 Duration (s)
   *  Profile Phase 4 Temperature
   *  Profile Phase 4 Duration (s)
   *  Profile Phase 5 Temperature
   *  Profile Phase 5 Duration (s)
   *  Profile Cooldown Max Temperature Change Per Second
   */
  /* Boost Temp */
  {SETTINGS_DESC(SettingsItemIndex::BoostTemperature), setBoostTemp, displayBoostTemp, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::BoostTemperature, 5},
  /* Auto start */
  {SETTINGS_DESC(SettingsItemIndex::AutoStart), nullptr, displayAutomaticStartMode, nullptr, SettingsOptions::AutoStartMode, SettingsItemIndex::AutoStart, 7},
  /* Temp change short step */
  {SETTINGS_DESC(SettingsItemIndex::TempChangeShortStep), nullptr, displayTempChangeShortStep, nullptr, SettingsOptions::TempChangeShortStep, SettingsItemIndex::TempChangeShortStep, 6},
  /* Temp change long step */
  {SETTINGS_DESC(SettingsItemIndex::TempChangeLongStep), nullptr, displayTempChangeLongStep, nullptr, SettingsOptions::TempChangeLongStep, SettingsItemIndex::TempChangeLongStep, 6},
  /* Locking Mode */
  {SETTINGS_DESC(SettingsItemIndex::LockingMode), nullptr, displayLockingMode, nullptr, SettingsOptions::LockingMode, SettingsItemIndex::LockingMode, 7},
#ifdef PROFILE_SUPPORT
  /* Profile Phases */
  {SETTINGS_DESC(SettingsItemIndex::ProfilePhases), nullptr, displayProfilePhases, nullptr, SettingsOptions::ProfilePhases, SettingsItemIndex::ProfilePhases, 7},
  /* Profile Preheat Temp */
  {SETTINGS_DESC(SettingsItemIndex::ProfilePreheatTemp), setProfilePreheatTemp, displayProfilePreheatTemp, showProfileOptions, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::ProfilePreheatTemp, 5},
  /* Profile Preheat Speed */
  {SETTINGS_DESC(SettingsItemIndex::ProfilePreheatSpeed), nullptr, displayProfilePreheatSpeed, showProfileOptions, SettingsOptions::ProfilePreheatSpeed, SettingsItemIndex::ProfilePreheatSpeed, 5},
  /* Phase 1 Temp */
  {SETTINGS_DESC(SettingsItemIndex::ProfilePhase1Temp), setProfilePhase1Temp, displayProfilePhase1Temp, showProfileOptions, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::ProfilePhase1Temp, 5},
  /* Phase 1 Duration */
  {SETTINGS_DESC(SettingsItemIndex::ProfilePhase1Duration), nullptr, displayProfilePhase1Duration, showProfileOptions, SettingsOptions::ProfilePhase1Duration, SettingsItemIndex::ProfilePhase1Duration, 5},
  /* Phase 2 Temp */
  {SETTINGS_DESC(SettingsItemIndex::ProfilePhase1Temp), setProfilePhase2Temp, displayProfilePhase2Temp, showProfilePhase2Options, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::ProfilePhase2Temp, 5},
  /* Phase 2 Duration */
  {SETTINGS_DESC(SettingsItemIndex::ProfilePhase1Duration), nullptr, displayProfilePhase2Duration, showProfilePhase2Options, SettingsOptions::ProfilePhase2Duration, SettingsItemIndex::ProfilePhase2Duration, 5},
  /* Phase 3 Temp */
  {SETTINGS_DESC(SettingsItemIndex::ProfilePhase1Temp), setProfilePhase3Temp, displayProfilePhase3Temp, showProfilePhase3Options, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::ProfilePhase3Temp, 5},
  /* Phase 3 Duration */
  {SETTINGS_DESC(SettingsItemIndex::ProfilePhase1Duration), nullptr, displayProfilePhase3Duration, showProfilePhase3Options, SettingsOptions::ProfilePhase3Duration, SettingsItemIndex::ProfilePhase3Duration, 5},
  /* Phase 4 Temp */
  {SETTINGS_DESC(SettingsItemIndex::ProfilePhase1Temp), setProfilePhase4Temp, displayProfilePhase4Temp, showProfilePhase4Options, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::ProfilePhase4Temp, 5},
  /* Phase 4 Duration */
  {SETTINGS_DESC(SettingsItemIndex::ProfilePhase1Duration), nullptr, displayProfilePhase4Duration, showProfilePhase4Options, SettingsOptions::ProfilePhase4Duration, SettingsItemIndex::ProfilePhase4Duration, 5},
  /* Phase 5 Temp */
  {SETTINGS_DESC(SettingsItemIndex::ProfilePhase1Temp), setProfilePhase5Temp, displayProfilePhase5Temp, showProfilePhase5Options, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::ProfilePhase5Temp, 5},
  /* Phase 5 Duration */
  {SETTINGS_DESC(SettingsItemIndex::ProfilePhase1Duration), nullptr, displayProfilePhase5Duration, showProfilePhase5Options, SettingsOptions::ProfilePhase5Duration, SettingsItemIndex::ProfilePhase5Duration, 5},
  /* Profile Cooldown Speed */
  {SETTINGS_DESC(SettingsItemIndex::ProfileCooldownSpeed), nullptr, displayProfileCooldownSpeed, showProfileOptions, SettingsOptions::ProfileCooldownSpeed, SettingsItemIndex::ProfileCooldownSpeed, 5},
#endif /* PROFILE_SUPPORT */
  /* vvvv end of menu marker. DO NOT REMOVE vvvv */
  {0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0}
  /* ^^^^ end of menu marker. DO NOT REMOVE ^^^^ */
};

const menuitem PowerSavingMenu[] = {
  /*
   *  Motion Sensitivity
   *  -Sleep Temp
   *  -Sleep Time
   *  -Shutdown Time
   *  Hall Sensor Sensitivity
   */
  /* Motion Sensitivity */
  {SETTINGS_DESC(SettingsItemIndex::MotionSensitivity), nullptr, displaySensitivity, nullptr, SettingsOptions::Sensitivity, SettingsItemIndex::MotionSensitivity, 7},
#ifndef NO_SLEEP_MODE
  /* Sleep Temp */
  {SETTINGS_DESC(SettingsItemIndex::SleepTemperature), setSleepTemp, displaySleepTemp, showSleepOptions, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::SleepTemperature, 5},
  /* Sleep Time */
  {SETTINGS_DESC(SettingsItemIndex::SleepTimeout), nullptr, displaySleepTime, showSleepOptions, SettingsOptions::SleepTime, SettingsItemIndex::SleepTimeout, 5},
#endif /* *not* NO_SLEEP_MODE */
  /* Shutdown Time */
  {SETTINGS_DESC(SettingsItemIndex::ShutdownTimeout), nullptr, displayShutdownTime, showSleepOptions, SettingsOptions::ShutdownTime, SettingsItemIndex::ShutdownTimeout, 5},
#ifdef HALL_SENSOR
  /* Hall Effect Sensitivity */
  {SETTINGS_DESC(SettingsItemIndex::HallEffSensitivity), nullptr, displayHallEffect, showHallEffect, SettingsOptions::HallEffectSensitivity, SettingsItemIndex::HallEffSensitivity, 7},
#endif /* HALL_SENSOR */
  /* vvvv end of menu marker. DO NOT REMOVE vvvv */
  {0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0}
  /* ^^^^ end of menu marker. DO NOT REMOVE ^^^^ */
};

const menuitem UIMenu[] = {
  /*
   *  Temperature Unit
   *  Display Orientation
   *  Cooldown Blink
   *  Scrolling Speed
   *  Swap Temp Change Buttons +/-
   *  Animation Speed
   *  -Animation Loop
   *  OLED Brightness
   *  Invert Screen
   *  Logo Timeout
   *  Detailed IDLE
   *  Detailed Soldering
   */
  /* Temperature units, this has to be the first element in the array to work with the logic in enterUIMenu() */
  {SETTINGS_DESC(SettingsItemIndex::TemperatureUnit), setTempF, displayTempF, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::TemperatureUnit, 7},
#ifndef NO_DISPLAY_ROTATE
  /* Display Rotation */
  {SETTINGS_DESC(SettingsItemIndex::DisplayRotation), setDisplayRotation, displayDisplayRotation, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::DisplayRotation, 7},
#endif /* *not* NO_DISPLAY_ROTATE */
  /* Cooling blink warning */
  {SETTINGS_DESC(SettingsItemIndex::CooldownBlink), nullptr, displayCoolingBlinkEnabled, nullptr, SettingsOptions::CoolingTempBlink, SettingsItemIndex::CooldownBlink, 7},
  /* Scroll Speed for descriptions */
  {SETTINGS_DESC(SettingsItemIndex::ScrollingSpeed), nullptr, displayScrollSpeed, nullptr, SettingsOptions::DescriptionScrollSpeed, SettingsItemIndex::ScrollingSpeed, 7},
  /* Reverse Temp change buttons +/- */
  {SETTINGS_DESC(SettingsItemIndex::ReverseButtonTempChange), nullptr, displayReverseButtonTempChangeEnabled, nullptr, SettingsOptions::ReverseButtonTempChangeEnabled, SettingsItemIndex::ReverseButtonTempChange, 7},
  /* Animation Speed adjustment */
  {SETTINGS_DESC(SettingsItemIndex::AnimSpeed), nullptr, displayAnimationSpeed, nullptr, SettingsOptions::AnimationSpeed, SettingsItemIndex::AnimSpeed, 7},
  /* Animation Loop switch */
  {SETTINGS_DESC(SettingsItemIndex::AnimLoop), nullptr, displayAnimationLoop, displayAnimationOptions, SettingsOptions::AnimationLoop, SettingsItemIndex::AnimLoop, 7},
  /* Brightness Level */
  {SETTINGS_DESC(SettingsItemIndex::Brightness), nullptr, displayBrightnessLevel, nullptr, SettingsOptions::OLEDBrightness, SettingsItemIndex::Brightness, 7},
  /* Invert screen colour */
  {SETTINGS_DESC(SettingsItemIndex::ColourInversion), nullptr, displayInvertColor, nullptr, SettingsOptions::OLEDInversion, SettingsItemIndex::ColourInversion, 7},
  /* Set logo duration */
  {SETTINGS_DESC(SettingsItemIndex::LOGOTime), nullptr, displayLogoTime, nullptr, SettingsOptions::LOGOTime, SettingsItemIndex::LOGOTime, 5},
  /* Advanced idle screen */
  {SETTINGS_DESC(SettingsItemIndex::AdvancedIdle), nullptr, displayAdvancedIDLEScreens, nullptr, SettingsOptions::DetailedIDLE, SettingsItemIndex::AdvancedIdle, 7},
  /* Advanced soldering screen */
  {SETTINGS_DESC(SettingsItemIndex::AdvancedSoldering), nullptr, displayAdvancedSolderingScreens, nullptr, SettingsOptions::DetailedSoldering, SettingsItemIndex::AdvancedSoldering, 7},
  /* vvvv end of menu marker. DO NOT REMOVE vvvv */
  {0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0}
  /* ^^^^ end of menu marker. DO NOT REMOVE ^^^^ */
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
  /* Toggle BLE */
  {SETTINGS_DESC(SettingsItemIndex::BluetoothLE), nullptr, displayBluetoothLE, nullptr, SettingsOptions::BluetoothLE, SettingsItemIndex::BluetoothLE, 7},
#endif /* BLE_ENABLED */
  /* Power limit */
  {SETTINGS_DESC(SettingsItemIndex::PowerLimit), nullptr, displayPowerLimit, nullptr, SettingsOptions::PowerLimit, SettingsItemIndex::PowerLimit, 4},
  /* Calibrate Cold Junktion Compensation at next boot */
  {SETTINGS_DESC(SettingsItemIndex::CalibrateCJC), setCalibrate, displayCalibrate, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::CalibrateCJC, 7},
  /* Voltage input cal */
  {SETTINGS_DESC(SettingsItemIndex::VoltageCalibration), setCalibrateVIN, displayCalibrateVIN, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::VoltageCalibration, 5},
  /* Power Pulse adjustment */
  {SETTINGS_DESC(SettingsItemIndex::PowerPulsePower), nullptr, displayPowerPulse, nullptr, SettingsOptions::KeepAwakePulse, SettingsItemIndex::PowerPulsePower, 5},
  /* Power Pulse Wait adjustment */
  {SETTINGS_DESC(SettingsItemIndex::PowerPulseWait), nullptr, displayPowerPulseWait, showPowerPulseOptions, SettingsOptions::KeepAwakePulseWait, SettingsItemIndex::PowerPulseWait, 7},
  /* Power Pulse Duration adjustment */
  {SETTINGS_DESC(SettingsItemIndex::PowerPulseDuration), nullptr, displayPowerPulseDuration, showPowerPulseOptions, SettingsOptions::KeepAwakePulseDuration, SettingsItemIndex::PowerPulseDuration, 7},
  /* Resets settings */
  {SETTINGS_DESC(SettingsItemIndex::SettingsReset), setResetSettings, displayResetSettings, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::SettingsReset, 7},
  /* vvvv end of menu marker. DO NOT REMOVE vvvv */
  {0, nullptr, nullptr, nullptr, SettingsOptions::SettingsOptionsLength, SettingsItemIndex::NUM_ITEMS, 0}
  /* ^^^^ end of menu marker. DO NOT REMOVE ^^^^ */
};

/* clang-format on */

const menuitem *subSettingsMenus[] {
#if defined(POW_DC) || defined(POW_QC) || defined(POW_PD)
  powerMenu,
#endif
      solderingMenu, PowerSavingMenu, UIMenu, advancedMenu,
};
/* ^^^ !!!ENABLE CLANG-FORMAT back!!! ^^^ */

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
  TickType_t tickStart = xTaskGetTickCount();
  for (;;) {
    drawScrollingText(message, xTaskGetTickCount() - tickStart);

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

    OLED::refresh();
    osDelay(40);
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

#endif /* POW_DC */

#ifdef POW_QC

static void displayQCInputV(void) {
  // These are only used in QC modes
  // Allows setting the voltage negotiated for QC
  auto voltage = getSettingValue(SettingsOptions::QCIdealVoltage);
  OLED::printNumber(voltage / 10, 2, FontStyle::LARGE);
  OLED::print(LargeSymbolDot, FontStyle::LARGE);
  OLED::printNumber(voltage % 10, 1, FontStyle::LARGE);
}

#endif /* POW_QC */

#ifdef POW_PD /* POW_PD */

static void displayPDNegTimeout(void) {
  auto value = getSettingValue(SettingsOptions::PDNegTimeout);
  if (value == 0) {
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
  } else {
    OLED::printNumber(value, 3, FontStyle::LARGE);
  }
}

static void displayPDVpdo(void) { OLED::drawCheckbox(getSettingValue(SettingsOptions::PDVpdo)); }

#endif /* POW_PD */

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
  case autoStartMode_t::NO:
    OLED::print(translatedString(Tr->SettingStartNoneChar), FontStyle::LARGE);
    break;
  case autoStartMode_t::SOLDER:
    OLED::print(translatedString(Tr->SettingStartSolderingChar), FontStyle::LARGE);
    break;
  case autoStartMode_t::SLEEP:
    OLED::print(translatedString(Tr->SettingStartSleepChar), FontStyle::LARGE);
    break;
  case autoStartMode_t::ZERO:
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

#ifdef PROFILE_SUPPORT

static void displayProfilePhases(void) { OLED::printNumber(getSettingValue(SettingsOptions::ProfilePhases), 1, FontStyle::LARGE); }

static bool setProfileTemp(const enum SettingsOptions option) {
  // If in C, 5 deg, if in F 10 deg
  uint16_t temp = getSettingValue(option);
  if (getSettingValue(SettingsOptions::TemperatureInF)) {
    temp += 10;
    if (temp > MAX_TEMP_F) {
      temp = MIN_TEMP_F;
    }
    setSettingValue(option, temp);
    return temp == MAX_TEMP_F;
  } else {
    temp += 5;
    if (temp > MAX_TEMP_C) {
      temp = MIN_TEMP_C;
    }
    setSettingValue(option, temp);
    return temp == MAX_TEMP_C;
  }
}

static bool setProfilePreheatTemp(void) { return setProfileTemp(SettingsOptions::ProfilePreheatTemp); }
static bool setProfilePhase1Temp(void) { return setProfileTemp(SettingsOptions::ProfilePhase1Temp); }
static bool setProfilePhase2Temp(void) { return setProfileTemp(SettingsOptions::ProfilePhase2Temp); }
static bool setProfilePhase3Temp(void) { return setProfileTemp(SettingsOptions::ProfilePhase3Temp); }
static bool setProfilePhase4Temp(void) { return setProfileTemp(SettingsOptions::ProfilePhase4Temp); }
static bool setProfilePhase5Temp(void) { return setProfileTemp(SettingsOptions::ProfilePhase5Temp); }

static void displayProfilePreheatTemp(void) { OLED::printNumber(getSettingValue(SettingsOptions::ProfilePreheatTemp), 3, FontStyle::LARGE); }
static void displayProfilePhase1Temp(void) { OLED::printNumber(getSettingValue(SettingsOptions::ProfilePhase1Temp), 3, FontStyle::LARGE); }
static void displayProfilePhase2Temp(void) { OLED::printNumber(getSettingValue(SettingsOptions::ProfilePhase2Temp), 3, FontStyle::LARGE); }
static void displayProfilePhase3Temp(void) { OLED::printNumber(getSettingValue(SettingsOptions::ProfilePhase3Temp), 3, FontStyle::LARGE); }
static void displayProfilePhase4Temp(void) { OLED::printNumber(getSettingValue(SettingsOptions::ProfilePhase4Temp), 3, FontStyle::LARGE); }
static void displayProfilePhase5Temp(void) { OLED::printNumber(getSettingValue(SettingsOptions::ProfilePhase5Temp), 3, FontStyle::LARGE); }
static void displayProfilePreheatSpeed(void) { OLED::printNumber(getSettingValue(SettingsOptions::ProfilePreheatSpeed), 2, FontStyle::LARGE); }
static void displayProfileCooldownSpeed(void) { OLED::printNumber(getSettingValue(SettingsOptions::ProfileCooldownSpeed), 2, FontStyle::LARGE); }
static void displayProfilePhase1Duration(void) { OLED::printNumber(getSettingValue(SettingsOptions::ProfilePhase1Duration), 3, FontStyle::LARGE); }
static void displayProfilePhase2Duration(void) { OLED::printNumber(getSettingValue(SettingsOptions::ProfilePhase2Duration), 3, FontStyle::LARGE); }
static void displayProfilePhase3Duration(void) { OLED::printNumber(getSettingValue(SettingsOptions::ProfilePhase3Duration), 3, FontStyle::LARGE); }
static void displayProfilePhase4Duration(void) { OLED::printNumber(getSettingValue(SettingsOptions::ProfilePhase4Duration), 3, FontStyle::LARGE); }
static void displayProfilePhase5Duration(void) { OLED::printNumber(getSettingValue(SettingsOptions::ProfilePhase5Duration), 3, FontStyle::LARGE); }

static bool showProfileOptions(void) { return getSettingValue(SettingsOptions::ProfilePhases); }
static bool showProfilePhase2Options(void) { return getSettingValue(SettingsOptions::ProfilePhases) >= 2; }
static bool showProfilePhase3Options(void) { return getSettingValue(SettingsOptions::ProfilePhases) >= 3; }
static bool showProfilePhase4Options(void) { return getSettingValue(SettingsOptions::ProfilePhases) >= 4; }
static bool showProfilePhase5Options(void) { return getSettingValue(SettingsOptions::ProfilePhases) >= 5; }

#endif /* PROFILE_SUPPORT */

static void displaySensitivity(void) { OLED::printNumber(getSettingValue(SettingsOptions::Sensitivity), 1, FontStyle::LARGE, false); }
static bool showSleepOptions(void) { return getSettingValue(SettingsOptions::Sensitivity) > 0; }

#ifndef NO_SLEEP_MODE

static bool setSleepTemp(void) {
  // If in C, 10 deg, if in F 20 deg
  uint16_t temp = getSettingValue(SettingsOptions::SleepTemp);
  if (getSettingValue(SettingsOptions::TemperatureInF)) {
    temp += 20;
    if (temp > 580) {
      temp = 60;
    }
    setSettingValue(SettingsOptions::SleepTemp, temp);
    return temp == 580;
  } else {
    temp += 10;
    if (temp > 300) {
      temp = 10;
    }
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

#endif /* *not* NO_SLEEP_MODE */

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
#endif /* HALL_SENSOR */

static void setTempF(const enum SettingsOptions option) {
  uint16_t Temp = getSettingValue(option);
  if (getSettingValue(SettingsOptions::TemperatureInF)) {
    // Change temp to the F equiv
    // C to F == F= ( (C*9) +160)/5
    Temp = ((Temp * 9) + 160) / 5;
  } else {
    // Change temp to the C equiv
    // F->C == C = ((F-32)*5)/9
    Temp = ((Temp - 32) * 5) / 9;
  }
  // Rescale to be multiples of 10
  Temp = Temp / 10;
  Temp *= 10;
  setSettingValue(option, Temp);
}

static bool setTempF(void) {
  bool res = nextSettingValue(SettingsOptions::TemperatureInF);
  setTempF(SettingsOptions::BoostTemp);
  setTempF(SettingsOptions::SolderingTemp);
#ifndef NO_SLEEP_MODE
  setTempF(SettingsOptions::SleepTemp);
#endif /* *not* NO_SLEEP_MODE */
#ifdef PROFILE_SUPPORT
  setTempF(SettingsOptions::ProfilePreheatTemp);
  setTempF(SettingsOptions::ProfilePhase1Temp);
  setTempF(SettingsOptions::ProfilePhase2Temp);
  setTempF(SettingsOptions::ProfilePhase3Temp);
  setTempF(SettingsOptions::ProfilePhase4Temp);
  setTempF(SettingsOptions::ProfilePhase5Temp);
#endif /* PROFILE_SUPPORT */
  return res;
}

static void displayTempF(void) { OLED::printSymbolDeg(FontStyle::LARGE); }

#ifndef NO_DISPLAY_ROTATE

static bool setDisplayRotation(void) {
  bool res = nextSettingValue(SettingsOptions::OrientationMode);
  switch (getSettingValue(SettingsOptions::OrientationMode)) {
  case orientationMode_t::RIGHT:
    OLED::setRotation(false);
    break;
  case orientationMode_t::LEFT:
    OLED::setRotation(true);
    break;
  case orientationMode_t::AUTO:
    // do nothing on auto
    break;
  default:
    break;
  }
  return res;
}

static void displayDisplayRotation(void) {
  switch (getSettingValue(SettingsOptions::OrientationMode)) {
  case orientationMode_t::RIGHT:
    OLED::print(translatedString(Tr->SettingRightChar), FontStyle::LARGE);
    break;
  case orientationMode_t::LEFT:
    OLED::print(translatedString(Tr->SettingLeftChar), FontStyle::LARGE);
    break;
  case orientationMode_t::AUTO:
    OLED::print(translatedString(Tr->SettingAutoChar), FontStyle::LARGE);
    break;
  default:
    OLED::print(translatedString(Tr->SettingRightChar), FontStyle::LARGE);
    break;
  }
}

#endif /* NO_DISPLAY_ROTATE */

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
  OLED::printNumber((getSettingValue(SettingsOptions::OLEDBrightness) / BRIGHTNESS_STEP + 1), 1, FontStyle::LARGE);
  // While not optimal to apply this here, it is _very_ convenient
  OLED::setBrightness(getSettingValue(SettingsOptions::OLEDBrightness));
}

static void displayInvertColor(void) {
  OLED::drawCheckbox(getSettingValue(SettingsOptions::OLEDInversion));
  // While not optimal to apply this here, it is _very_ convenient
  OLED::setInverseDisplay(getSettingValue(SettingsOptions::OLEDInversion));
}

static void displayLogoTime(void) {
  switch (getSettingValue(SettingsOptions::LOGOTime)) {
  case logoMode_t::SKIP:
    OLED::print(translatedString(Tr->OffString), FontStyle::LARGE);
    break;
  case logoMode_t::ONETIME:
    OLED::drawArea(OLED_WIDTH - 16 - 2, 0, 16, 16, RepeatOnce);
    break;
  case logoMode_t::INFINITY:
    OLED::drawArea(OLED_WIDTH - 16 - 2, 0, 16, 16, RepeatInf);
    break;
  default:
    OLED::printNumber(getSettingValue(SettingsOptions::LOGOTime), 2, FontStyle::LARGE);
    OLED::print(LargeSymbolSeconds, FontStyle::LARGE);
    break;
  }
}

static void displayAdvancedIDLEScreens(void) { OLED::drawCheckbox(getSettingValue(SettingsOptions::DetailedIDLE)); }

static void displayAdvancedSolderingScreens(void) { OLED::drawCheckbox(getSettingValue(SettingsOptions::DetailedSoldering)); }

#ifdef BLE_ENABLED
static void displayBluetoothLE(void) { OLED::drawCheckbox(getSettingValue(SettingsOptions::BluetoothLE)); }
#endif /* BLE_ENABLED */

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
    OLED::setCursor(25, 0);
    uint16_t voltage = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);
    OLED::printNumber(voltage / 10, 2, FontStyle::LARGE);
    OLED::print(LargeSymbolDot, FontStyle::LARGE);
    OLED::printNumber(voltage % 10, 1, FontStyle::LARGE, false);
    OLED::print(LargeSymbolVolts, FontStyle::LARGE);
    OLED::setCursor(0, 8);
    OLED::printNumber(getSettingValue(SettingsOptions::VoltageDiv), 3, FontStyle::SMALL);

    switch (getButtonState()) {
    case BUTTON_F_SHORT:
      prevSettingValue(SettingsOptions::VoltageDiv);
      break;
    case BUTTON_B_SHORT:
      nextSettingValue(SettingsOptions::VoltageDiv);
      break;
    case BUTTON_BOTH:
    case BUTTON_F_LONG:
    case BUTTON_B_LONG:
      saveSettings();
      OLED::clearScreen();
      OLED::setCursor(0, 0);
      warnUser(translatedString(Tr->CalibrationDone), getButtonState());
      OLED::refresh();
      waitForButtonPressOrTimeout(0.5 * TICKS_SECOND);
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
    warnUser(translatedString(Tr->ResetOKMessage), getButtonState());
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
#endif /* POW_DC or POW_QC */

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
