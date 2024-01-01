/*
 * GUIThread.cpp
 *
 *  Created on: 19 Aug 2019
 *      Author: ralim
 */
extern "C" {
#include "FreeRTOSConfig.h"
}
#include "BootLogo.h"
#include "Buttons.hpp"
#include "I2CBB2.hpp"
#include "LIS2DH12.hpp"
#include "MMA8652FC.hpp"
#include "OLED.hpp"
#include "OperatingModeUtilities.h"
#include "OperatingModes.h"
#include "Settings.h"
#include "TipThermoModel.h"
#include "Translation.h"
#include "cmsis_os.h"
#include "configuration.h"
#include "history.hpp"
#include "main.hpp"
#include "power.hpp"
#include "settingsGUI.hpp"
#include "stdlib.h"
#include "string.h"
#ifdef POW_PD
#include "USBPD.h"
#include "pd.h"
#endif

// File local variables

extern bool heaterThermalRunaway;

/* StartGUITask function */
void startGUITask(void const *argument) {
  (void)argument;
  prepareTranslations();

  OLED::initialize(); // start up the LCD
  OLED::setBrightness(getSettingValue(SettingsOptions::OLEDBrightness));
  OLED::setInverseDisplay(getSettingValue(SettingsOptions::OLEDInversion));

  bool buttonLockout = false;
  renderHomeScreenAssets();
  getTipRawTemp(1); // reset filter

  OLED::setRotation(getSettingValue(SettingsOptions::OrientationMode) & 1);
  // If the front button is held down, on supported devices, show PD debugging metrics
#ifdef HAS_POWER_DEBUG_MENU
#ifdef DEBUG_POWER_MENU_BUTTON_B
  if (getButtonB()) {
#else
  if (getButtonA()) {
#endif
    showPDDebug();
  }
#endif

  if (getSettingValue(SettingsOptions::CalibrateCJC) > 0) {
    performCJCC();
  }

  uint16_t logoMode  = getSettingValue(SettingsOptions::LOGOTime);
  uint16_t startMode = getSettingValue(SettingsOptions::AutoStartMode);
  // If the boot logo is enabled (but it times out) and the autostart mode is enabled (but not set to sleep w/o heat), start heating during boot logo
  if (logoMode && logoMode < logoMode_t::ONETIME && startMode && startMode < autoStartMode_t::ZERO) {
    uint16_t sleepTempDegC = getSettingValue(SettingsOptions::SleepTemp);
    if (getSettingValue(SettingsOptions::TemperatureInF)) {
      sleepTempDegC = TipThermoModel::convertFtoC(sleepTempDegC);
    }
    // Only heat to sleep temperature (but no higher than 75°C for safety)
    currentTempTargetDegC = min(sleepTempDegC, 75);
  }

  showBootLogo();
  showWarnings();
  if (getSettingValue(SettingsOptions::AutoStartMode)) {
    // jump directly to the autostart mode
    gui_solderingMode(getSettingValue(SettingsOptions::AutoStartMode) - 1);
    buttonLockout = true;
  }

  drawHomeScreen(buttonLockout);
}
