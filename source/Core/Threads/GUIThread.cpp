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
#define MOVEMENT_INACTIVITY_TIME (60 * configTICK_RATE_HZ)
#define BUTTON_INACTIVITY_TIME   (60 * configTICK_RATE_HZ)

ButtonState   buttonsAtDeviceBoot;                                      // We record button state at startup, incase of jumping to debug modes
OperatingMode currentOperatingMode = OperatingMode::InitialisationDone; // Current mode we are rendering
guiContext    context;                                                  // Context passed to functions to aid in state during render passes

OperatingMode handle_post_init_state();

void guiRenderLoop(void) {
  OLED::clearScreen(); // Clear ready for render pass
  // Read button state
  ButtonState buttons = getButtonState();
  // Enforce screen on if buttons pressed, movement, hot tip etc
  if (buttons != BUTTON_NONE) {
    OLED::setDisplayState(OLED::DisplayState::ON);
  } else {
    // Buttons are none; check if we can sleep display
    uint32_t tipTemp = TipThermoModel::getTipInC();
    if ((tipTemp < 50) && getSettingValue(SettingsOptions::Sensitivity)
        && (((xTaskGetTickCount() - lastMovementTime) > MOVEMENT_INACTIVITY_TIME) && ((xTaskGetTickCount() - lastButtonTime) > BUTTON_INACTIVITY_TIME))) {
      OLED::setDisplayState(OLED::DisplayState::OFF);
      setStatusLED(LED_OFF);
    } else {
      OLED::setDisplayState(OLED::DisplayState::ON);
      if (tipTemp > 55) {
        setStatusLED(LED_COOLING_STILL_HOT);
      } else {
        setStatusLED(LED_STANDBY);
      }
    }
  }

  // Dispatch button state to gui mode
  OperatingMode newMode = currentOperatingMode;
  switch (currentOperatingMode) {
  case OperatingMode::StartupWarnings:
    newMode = showWarnings(buttons, &context);
    break;
  case OperatingMode::UsbPDDebug:
#ifdef HAS_POWER_DEBUG_MENU
    showPDDebug(buttons, &context);
    break;
#else
    /*fallthrough*/
#endif
  case OperatingMode::StartupLogo:
    BootLogo::handleShowingLogo((uint8_t *)FLASH_LOGOADDR); // TODO needs refactor
    newMode = OperatingMode::StartupWarnings;
    break;
  default:
    /* Fallthrough */
  case OperatingMode::HomeScreen:
    newMode = drawHomeScreen(buttons, &context);
    break;
  case OperatingMode::Soldering:
    newMode = gui_solderingMode(buttons, &context);
    break;
  case OperatingMode::SolderingProfile:
    newMode = gui_solderingProfileMode(buttons, &context);
    break;
  case OperatingMode::Sleeping:
    newMode = gui_SolderingSleepingMode(buttons, &context);
    break;
  case OperatingMode::TemperatureAdjust:
    newMode = gui_solderingTempAdjust(buttons, &context);
    break;
  case OperatingMode::DebugMenuReadout:
    newMode = showDebugMenu(buttons, &context);
    break;
  case OperatingMode::CJCCalibration:
    newMode = performCJCC(buttons, &context);
    break;
  case OperatingMode::SettingsMenu:
    newMode = gui_SettingsMenu(buttons, &context);
    break;
  case OperatingMode::InitialisationDone:
    newMode = handle_post_init_state();
    break;
  case OperatingMode::Hibernating:
    break;
  case OperatingMode::ThermalRunaway:
    break;
  };
  // Update state holders
  if (newMode != currentOperatingMode) {
    context.viewEnterTime = xTaskGetTickCount();
    context.previousMode  = currentOperatingMode;
    memset(&context.scratch_state, 0, sizeof(context.scratch_state));
    currentOperatingMode = newMode;
    // If the transition marker is set, we need to make the next draw occur to the secondary buffer so we have something to transition to
    if (context.transitionMode != TransitionAnimation::None) {
      OLED::refresh();
      OLED::useSecondaryFramebuffer(true);
      return; // Exit early to avoid refresh with new framebuffer
    }
  } else if (context.transitionMode != TransitionAnimation::None) {
    // We haven't changed mode but transition is set, so we are at the other side of a transition
    // We now want to transition from old contents (main buffer) to new contents (secondary buffer)
    switch (context.transitionMode) {
    case TransitionAnimation::Down:
      break;
    case TransitionAnimation::Left:
      break;
    case TransitionAnimation::Right:
      break;
    case TransitionAnimation::None:
    default:
      break; // Do nothing on unknown
    }
    OLED::useSecondaryFramebuffer(false);
    context.transitionMode = TransitionAnimation::None; // Clear transition flag
  }
  // Render done, draw it out
  OLED::refresh();
}

OperatingMode handle_post_init_state() {
#ifdef HAS_POWER_DEBUG_MENU
#ifdef DEBUG_POWER_MENU_BUTTON_B
  if (buttonsAtDeviceBoot == BUTTON_B_LONG || buttonsAtDeviceBoot == BUTTON_B_SHORT) {
#else
  if (buttonsAtDeviceBoot == BUTTON_F_LONG || buttonsAtDeviceBoot == BUTTON_F_SHORT) {
#endif
    return OperatingMode::UsbPDDebug;
  }
#endif

  if (getSettingValue(SettingsOptions::CalibrateCJC) > 0) {
    return OperatingMode::CJCCalibration;
  }

  if (getSettingValue(SettingsOptions::AutoStartMode) == 1) {
    // jump directly to the autostart mode
    return OperatingMode::Sleeping;
  }
  if (getSettingValue(SettingsOptions::AutoStartMode) == 2) {
    return OperatingMode::Hibernating;
  }

  return OperatingMode::HomeScreen;
}

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
  memset(&context, 0, sizeof(context));

  OLED::setRotation(getSettingValue(SettingsOptions::OrientationMode) & 1);
  buttonsAtDeviceBoot = getButtonState();

  // // If the boot logo is enabled with timeout and the autostart mode is enabled (but not set to sleep w/o heat), start heating during boot logo
  // if (getSettingValue(SettingsOptions::LOGOTime) > 0 && getSettingValue(SettingsOptions::LOGOTime) < 5 && getSettingValue(SettingsOptions::AutoStartMode) > 0
  //     && getSettingValue(SettingsOptions::AutoStartMode) < 3) {
  //   uint16_t sleepTempDegC;
  //   if (getSettingValue(SettingsOptions::TemperatureInF)) {
  //     sleepTempDegC = TipThermoModel::convertFtoC(getSettingValue(SettingsOptions::SleepTemp));
  //   } else {
  //     sleepTempDegC = getSettingValue(SettingsOptions::SleepTemp);
  //   }
  //   // Only heat to sleep temperature (but no higher than 75°C for safety)
  //   currentTempTargetDegC = min(sleepTempDegC, 75);
  // }
  // TODO

  TickType_t startRender = xTaskGetTickCount();
  for (;;) {
    guiRenderLoop();
    resetWatchdog();
    vTaskDelayUntil(&startRender, TICKS_100MS / 2); // Try and maintain 20fps ish update rate, way to fast but if we can its nice
  }
}
