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
#include "I2CBB.hpp"
#include "LIS2DH12.hpp"
#include "MMA8652FC.hpp"
#include "OLED.hpp"
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
#if POW_PD
#include "USBPD.h"
#include "pd.h"
#endif
// File local variables
extern uint32_t   currentTempTargetDegC;
extern TickType_t lastMovementTime;
extern bool       heaterThermalRunaway;
extern osThreadId GUITaskHandle;
extern osThreadId MOVTaskHandle;
extern osThreadId PIDTaskHandle;
static bool       shouldBeSleeping(bool inAutoStart = false);
static bool       shouldShutdown();
void              showWarnings();
#define MOVEMENT_INACTIVITY_TIME (60 * configTICK_RATE_HZ)
#define BUTTON_INACTIVITY_TIME   (60 * configTICK_RATE_HZ)
static TickType_t lastHallEffectSleepStart = 0;
static uint16_t   min(uint16_t a, uint16_t b) {
  if (a > b)
    return b;
  else
    return a;
}

void warnUser(const char *warning, const int timeout) {
  OLED::clearScreen();
  OLED::printWholeScreen(warning);
  OLED::refresh();
  waitForButtonPressOrTimeout(timeout);
}

void printVoltage() {
  uint32_t volt = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);
  OLED::printNumber(volt / 10, 2, FontStyle::SMALL);
  OLED::print(SymbolDot, FontStyle::SMALL);
  OLED::printNumber(volt % 10, 1, FontStyle::SMALL);
}
void GUIDelay() {
  // Called in all UI looping tasks,
  // This limits the re-draw rate to the LCD and also lets the DMA run
  // As the gui task can very easily fill this bus with transactions, which will
  // prevent the movement detection from running
  vTaskDelay(5 * TICKS_10MS);
}
void gui_drawTipTemp(bool symbol, const FontStyle font) {
  // Draw tip temp handling unit conversion & tolerance near setpoint
  uint32_t Temp = 0;
  if (getSettingValue(SettingsOptions::TemperatureInF)) {
    Temp = TipThermoModel::getTipInF();
  } else {
    Temp = TipThermoModel::getTipInC();
  }

  OLED::printNumber(Temp, 3, font); // Draw the tip temp out
  if (symbol) {
    if (font == FontStyle::LARGE) {
      // Big font, can draw nice symbols
      if (getSettingValue(SettingsOptions::TemperatureInF))
        OLED::drawSymbol(0);
      else
        OLED::drawSymbol(1);
    } else {
      // Otherwise fall back to chars
      if (getSettingValue(SettingsOptions::TemperatureInF))
        OLED::print(SymbolDegF, FontStyle::SMALL);
      else
        OLED::print(SymbolDegC, FontStyle::SMALL);
    }
  }
}

#ifdef POW_DC
// returns true if undervoltage has occured
static bool checkVoltageForExit() {
  if (!getIsPoweredByDCIN()) {
    return false;
  }
  uint16_t v = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);

  // Dont check for first 2 seconds while the ADC stabilizes and the DMA fills
  // the buffer
  if (xTaskGetTickCount() > (TICKS_SECOND * 2)) {
    if ((v < lookupVoltageLevel())) {
      currentTempTargetDegC = 0;
      OLED::clearScreen();
      OLED::setCursor(0, 0);
      if (getSettingValue(SettingsOptions::DetailedSoldering)) {
        OLED::print(translatedString(Tr->UndervoltageString), FontStyle::SMALL);
        OLED::setCursor(0, 8);
        OLED::print(translatedString(Tr->InputVoltageString), FontStyle::SMALL);
        printVoltage();
        OLED::print(SymbolVolts, FontStyle::SMALL);
      } else {
        OLED::print(translatedString(Tr->UVLOWarningString), FontStyle::LARGE);
      }

      OLED::refresh();
      GUIDelay();
      waitForButtonPress();
      return true;
    }
  }
  return false;
}
#endif
static void gui_drawBatteryIcon() {
#if defined(POW_PD) || defined(POW_QC)
  if (!getIsPoweredByDCIN()) {
    // On TS80 we replace this symbol with the voltage we are operating on
    // If <9V then show single digit, if not show dual small ones vertically stacked
    uint8_t V = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);
    if (V % 10 >= 5)
      V = V / 10 + 1; // round up
    else
      V = V / 10;
    if (V >= 10) {
      int16_t xPos = OLED::getCursorX();
      OLED::printNumber(V / 10, 1, FontStyle::SMALL);
      OLED::setCursor(xPos, 8);
      OLED::printNumber(V % 10, 1, FontStyle::SMALL);
      OLED::setCursor(xPos + 12, 0); // need to reset this as if we drew a wide char
    } else {
      OLED::printNumber(V, 1, FontStyle::LARGE);
    }
    return;
  }
#endif
#ifdef POW_DC
  if (getSettingValue(SettingsOptions::MinDCVoltageCells)) {
    // User is on a lithium battery
    // we need to calculate which of the 10 levels they are on
    uint8_t  cellCount = getSettingValue(SettingsOptions::MinDCVoltageCells) + 2;
    uint32_t cellV     = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0) / cellCount;
    // Should give us approx cell voltage X10
    // Range is 42 -> Minimum voltage setting (systemSettings.minVoltageCells) = 9 steps therefore we will use battery 0-9
    if (cellV < getSettingValue(SettingsOptions::MinVoltageCells))
      cellV = getSettingValue(SettingsOptions::MinVoltageCells);
    cellV -= getSettingValue(SettingsOptions::MinVoltageCells); // Should leave us a number of 0-9
    if (cellV > 9)
      cellV = 9;
    OLED::drawBattery(cellV + 1);
  } else {
    OLED::drawSymbol(15); // Draw the DC Logo
  }
#endif
}
static void gui_solderingTempAdjust() {
  uint32_t lastChange                = xTaskGetTickCount();
  currentTempTargetDegC              = 0;
  uint32_t    autoRepeatTimer        = 0;
  uint8_t     autoRepeatAcceleration = 0;
  bool        waitForRelease         = false;
  ButtonState buttons                = getButtonState();
  if (buttons != BUTTON_NONE) {
    // Temp adjust entered by long-pressing F button.
    waitForRelease = true;
  }
  for (;;) {
    OLED::setCursor(0, 0);
    OLED::clearScreen();
    buttons = getButtonState();
    if (buttons) {
      if (waitForRelease) {
        buttons = BUTTON_NONE;
      }
      lastChange = xTaskGetTickCount();
    } else {
      waitForRelease = false;
    }
    int16_t delta = 0;
    switch (buttons) {
    case BUTTON_NONE:
      // stay
      autoRepeatAcceleration = 0;
      break;
    case BUTTON_BOTH:
      // exit
      return;
      break;
    case BUTTON_B_LONG:
      if (xTaskGetTickCount() - autoRepeatTimer + autoRepeatAcceleration > PRESS_ACCEL_INTERVAL_MAX) {
        delta           = -getSettingValue(SettingsOptions::TempChangeLongStep);
        autoRepeatTimer = xTaskGetTickCount();
        autoRepeatAcceleration += PRESS_ACCEL_STEP;
      }
      break;
    case BUTTON_B_SHORT:
      delta = -getSettingValue(SettingsOptions::TempChangeShortStep);
      break;
    case BUTTON_F_LONG:
      if (xTaskGetTickCount() - autoRepeatTimer + autoRepeatAcceleration > PRESS_ACCEL_INTERVAL_MAX) {
        delta           = getSettingValue(SettingsOptions::TempChangeLongStep);
        autoRepeatTimer = xTaskGetTickCount();
        autoRepeatAcceleration += PRESS_ACCEL_STEP;
      }
      break;
    case BUTTON_F_SHORT:
      delta = getSettingValue(SettingsOptions::TempChangeShortStep);
      break;
    default:
      break;
    }
    if ((PRESS_ACCEL_INTERVAL_MAX - autoRepeatAcceleration) < PRESS_ACCEL_INTERVAL_MIN) {
      autoRepeatAcceleration = PRESS_ACCEL_INTERVAL_MAX - PRESS_ACCEL_INTERVAL_MIN;
    }
    // If buttons are flipped; flip the delta
    if (getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled)) {
      delta = -delta;
    }
    if (delta != 0) {
      // constrain between the set temp limits, i.e. 10-450 C
      int16_t newTemp = getSettingValue(SettingsOptions::SolderingTemp);
      newTemp += delta;
      // Round to nearest increment of delta
      delta   = abs(delta);
      newTemp = (newTemp / delta) * delta;

      if (getSettingValue(SettingsOptions::TemperatureInF)) {
        if (newTemp > MAX_TEMP_F)
          newTemp = MAX_TEMP_F;
        if (newTemp < MIN_TEMP_F)
          newTemp = MIN_TEMP_F;
      } else {
        if (newTemp > MAX_TEMP_C)
          newTemp = MAX_TEMP_C;
        if (newTemp < MIN_TEMP_C)
          newTemp = MIN_TEMP_C;
      }
      setSettingValue(SettingsOptions::SolderingTemp, (uint16_t)newTemp);
    }
    if (xTaskGetTickCount() - lastChange > (TICKS_SECOND * 2))
      return; // exit if user just doesn't press anything for a bit

    if (OLED::getRotation()) {
      OLED::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? SymbolPlus : SymbolMinus, FontStyle::LARGE);
    } else {
      OLED::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? SymbolMinus : SymbolPlus, FontStyle::LARGE);
    }

    OLED::print(SymbolSpace, FontStyle::LARGE);
    OLED::printNumber(getSettingValue(SettingsOptions::SolderingTemp), 3, FontStyle::LARGE);
    if (getSettingValue(SettingsOptions::TemperatureInF))
      OLED::drawSymbol(0);
    else {
      OLED::drawSymbol(1);
    }
    OLED::print(SymbolSpace, FontStyle::LARGE);
    if (OLED::getRotation()) {
      OLED::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? SymbolMinus : SymbolPlus, FontStyle::LARGE);
    } else {
      OLED::print(getSettingValue(SettingsOptions::ReverseButtonTempChangeEnabled) ? SymbolPlus : SymbolMinus, FontStyle::LARGE);
    }
    OLED::refresh();
    GUIDelay();
  }
}
static bool shouldShutdown() {
  if (getSettingValue(SettingsOptions::ShutdownTime)) { // only allow shutdown exit if time > 0
    if (lastMovementTime) {
      if (((TickType_t)(xTaskGetTickCount() - lastMovementTime)) > (TickType_t)(getSettingValue(SettingsOptions::ShutdownTime) * TICKS_MIN)) {
        return true;
      }
    }
    if (lastHallEffectSleepStart) {
      if (((TickType_t)(xTaskGetTickCount() - lastHallEffectSleepStart)) > (TickType_t)(getSettingValue(SettingsOptions::ShutdownTime) * TICKS_MIN)) {
        return true;
      }
    }
  }
  if (getButtonState() == BUTTON_B_LONG) { // allow also if back button is pressed long
    return true;
  }
  return false;
}
static int gui_SolderingSleepingMode(bool stayOff, bool autoStarted) {
  // Drop to sleep temperature and display until movement or button press

  for (;;) {
    // user moved or pressed a button, go back to soldering
    // If in the first two seconds we disable this to let accelerometer warm up

#ifdef POW_DC
    if (checkVoltageForExit())
      return 1; // return non-zero on error
#endif
    if (getSettingValue(SettingsOptions::TemperatureInF)) {
      currentTempTargetDegC = stayOff ? 0 : TipThermoModel::convertFtoC(min(getSettingValue(SettingsOptions::SleepTemp), getSettingValue(SettingsOptions::SolderingTemp)));
    } else {
      currentTempTargetDegC = stayOff ? 0 : min(getSettingValue(SettingsOptions::SleepTemp), getSettingValue(SettingsOptions::SolderingTemp));
    }
    // draw the lcd
    uint16_t tipTemp;
    if (getSettingValue(SettingsOptions::TemperatureInF))
      tipTemp = TipThermoModel::getTipInF();
    else {
      tipTemp = TipThermoModel::getTipInC();
    }

    OLED::clearScreen();
    OLED::setCursor(0, 0);
    if (getSettingValue(SettingsOptions::DetailedSoldering)) {
      OLED::print(translatedString(Tr->SleepingAdvancedString), FontStyle::SMALL);
      OLED::setCursor(0, 8);
      OLED::print(translatedString(Tr->SleepingTipAdvancedString), FontStyle::SMALL);
      OLED::printNumber(tipTemp, 3, FontStyle::SMALL);
      if (getSettingValue(SettingsOptions::TemperatureInF))
        OLED::print(SymbolDegF, FontStyle::SMALL);
      else {
        OLED::print(SymbolDegC, FontStyle::SMALL);
      }

      OLED::print(SymbolSpace, FontStyle::SMALL);
      printVoltage();
      OLED::print(SymbolVolts, FontStyle::SMALL);
    } else {
      OLED::print(translatedString(Tr->SleepingSimpleString), FontStyle::LARGE);
      OLED::printNumber(tipTemp, 3, FontStyle::LARGE);
      if (getSettingValue(SettingsOptions::TemperatureInF))
        OLED::drawSymbol(0);
      else {
        OLED::drawSymbol(1);
      }
    }

    OLED::refresh();
    GUIDelay();
#ifdef ACCEL_EXITS_ON_MOVEMENT
    // If the accel works in reverse where movement will cause exiting the soldering mode
    if (getSettingValue(SettingsOptions::Sensitivity)) {
      if (lastMovementTime) {
        if (lastMovementTime > TICKS_SECOND * 10) {
          // If we have moved recently; in the last second
          // Then exit soldering mode

          if (((TickType_t)(xTaskGetTickCount() - lastMovementTime)) < (TickType_t)(TICKS_SECOND)) {
            currentTempTargetDegC = 0;
            return 1;
          }
        }
      }
    }
#else

    if (!shouldBeSleeping(autoStarted)) {
      return 0;
    }

#endif
    if (shouldShutdown()) {
      // shutdown
      currentTempTargetDegC = 0;
      return 1; // we want to exit soldering mode
    }
  }
  return 0;
}
#ifndef NO_SLEEP_MODE

static void display_countdown(int sleepThres) {
  /*
   * Print seconds or minutes (if > 99 seconds) until sleep
   * mode is triggered.
   */
  int        lastEventTime = lastButtonTime < lastMovementTime ? lastMovementTime : lastButtonTime;
  TickType_t downCount     = sleepThres - xTaskGetTickCount() + lastEventTime;
  if (downCount > (99 * TICKS_SECOND)) {
    OLED::printNumber(downCount / 60000 + 1, 2, FontStyle::SMALL);
    OLED::print(SymbolMinutes, FontStyle::SMALL);
  } else {
    OLED::printNumber(downCount / 1000 + 1, 2, FontStyle::SMALL);
    OLED::print(SymbolSeconds, FontStyle::SMALL);
  }
}
static uint32_t getSleepTimeout() {

  if (getSettingValue(SettingsOptions::Sensitivity) && getSettingValue(SettingsOptions::SleepTime)) {

    uint32_t sleepThres = 0;
    if (getSettingValue(SettingsOptions::SleepTime) < 6)
      sleepThres = getSettingValue(SettingsOptions::SleepTime) * 10 * 1000;
    else
      sleepThres = (getSettingValue(SettingsOptions::SleepTime) - 5) * 60 * 1000;
    return sleepThres;
  }
  return 0;
}
#endif
static bool shouldBeSleeping(bool inAutoStart) {
#ifndef NO_SLEEP_MODE
  // Return true if the iron should be in sleep mode
  if (getSettingValue(SettingsOptions::Sensitivity) && getSettingValue(SettingsOptions::SleepTime)) {
    if (inAutoStart) {
      // In auto start we are asleep until movement
      if (lastMovementTime == 0 && lastButtonTime == 0) {
        return true;
      }
    }
    if (lastMovementTime > 0 || lastButtonTime > 0) {
      if (((xTaskGetTickCount() - lastMovementTime) > getSleepTimeout()) && ((xTaskGetTickCount() - lastButtonTime) > getSleepTimeout())) {
        return true;
      }
    }
  }

#ifdef HALL_SENSOR
  // If the hall effect sensor is enabled in the build, check if its over
  // threshold, and if so then we force sleep
  if (getHallSensorFitted() && lookupHallEffectThreshold()) {
    int16_t hallEffectStrength = getRawHallEffect();
    if (hallEffectStrength < 0)
      hallEffectStrength = -hallEffectStrength;
    // Have absolute value of measure of magnetic field strength
    if (hallEffectStrength > lookupHallEffectThreshold()) {
      if (lastHallEffectSleepStart == 0) {
        lastHallEffectSleepStart = xTaskGetTickCount();
      }
      if ((xTaskGetTickCount() - lastHallEffectSleepStart) > TICKS_SECOND) {
        return true;
      }
    } else {
      lastHallEffectSleepStart = 0;
    }
  }
#endif
#endif
  return false;
}
static void gui_solderingMode(uint8_t jumpToSleep) {
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
        break;
      case BUTTON_BOTH:
        // exit
        return;
        break;
      case BUTTON_B_LONG:
        return; // exit on back long hold
        break;
      case BUTTON_F_LONG:
        // if boost mode is enabled turn it on
        if (getSettingValue(SettingsOptions::BoostTemp))
          boostModeOn = true;
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
        display_countdown(getSleepTimeout());
      }
#endif

      if (boostModeOn) {
        if (OLED::getRotation()) {
          OLED::setCursor(38, 8);
        } else {
          OLED::setCursor(55, 8);
        }
        OLED::print(SymbolPlus, FontStyle::SMALL);
      }

      if (OLED::getRotation()) {
        OLED::setCursor(0, 0);
      } else {
        OLED::setCursor(67, 0);
      }
      OLED::printNumber(x10WattHistory.average() / 10, 2, FontStyle::SMALL);
      OLED::print(SymbolDot, FontStyle::SMALL);
      OLED::printNumber(x10WattHistory.average() % 10, 1, FontStyle::SMALL);
      OLED::print(SymbolWatts, FontStyle::SMALL);

      if (OLED::getRotation()) {
        OLED::setCursor(0, 8);
      } else {
        OLED::setCursor(67, 8);
      }
      printVoltage();
      OLED::print(SymbolVolts, FontStyle::SMALL);
    } else {
      OLED::setCursor(0, 0);
      // We switch the layout direction depending on the orientation of the oled
      if (OLED::getRotation()) {
        // battery
        gui_drawBatteryIcon();
        OLED::print(SymbolSpace, FontStyle::LARGE); // Space out gap between battery <-> temp
        gui_drawTipTemp(true, FontStyle::LARGE);    // Draw current tip temp

        // We draw boost arrow if boosting, or else gap temp <-> heat
        // indicator
        if (boostModeOn)
          OLED::drawSymbol(2);
        else
          OLED::print(SymbolSpace, FontStyle::LARGE);

        // Draw heating/cooling symbols
        OLED::drawHeatSymbol(X10WattsToPWM(x10WattHistory.average()));
      } else {
        // Draw heating/cooling symbols
        OLED::drawHeatSymbol(X10WattsToPWM(x10WattHistory.average()));
        // We draw boost arrow if boosting, or else gap temp <-> heat
        // indicator
        if (boostModeOn)
          OLED::drawSymbol(2);
        else
          OLED::print(SymbolSpace, FontStyle::LARGE);
        gui_drawTipTemp(true, FontStyle::LARGE); // Draw current tip temp

        OLED::print(SymbolSpace, FontStyle::LARGE); // Space out gap between battery <-> temp

        gui_drawBatteryIcon();
      }
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

#ifdef POW_DC
    // Undervoltage test
    if (checkVoltageForExit()) {
      lastButtonTime = xTaskGetTickCount();
      return;
    }
#endif

    if (shouldBeSleeping()) {
      if (gui_SolderingSleepingMode(false, false)) {
        return; // If the function returns non-0 then exit
      }
    }
    // Update LED status
    int error = currentTempTargetDegC - TipThermoModel::getTipInC();
    if (error >= -10 && error <= 10) {
      // converged
      setStatusLED(LED_HOT);
    } else {
      setStatusLED(LED_HEATING);
    }
    // If we have tripped thermal runaway, turn off heater and show warning
    if (heaterThermalRunaway) {
      currentTempTargetDegC = 0; // heater control off
                                 // TODO WARNING

      warnUser(translatedString(Tr->WarningThermalRunaway), 10 * TICKS_SECOND);
      heaterThermalRunaway = false;
      return;
    }
    // slow down ui update rate
    GUIDelay();
  }
}

void showDebugMenu(void) {
  uint8_t     screen = 0;
  ButtonState b;
  for (;;) {
    OLED::clearScreen();                                // Ensure the buffer starts clean
    OLED::setCursor(0, 0);                              // Position the cursor at the 0,0 (top left)
    OLED::print(SymbolVersionNumber, FontStyle::SMALL); // Print version number
    OLED::setCursor(0, 8);                              // second line
    OLED::print(DebugMenu[screen], FontStyle::SMALL);
    switch (screen) {
    case 0: // Just prints date
      break;
    case 1:
      // High water mark for GUI
      OLED::printNumber(uxTaskGetStackHighWaterMark(GUITaskHandle), 5, FontStyle::SMALL);
      break;
    case 2:
      // High water mark for the Movement task
      OLED::printNumber(uxTaskGetStackHighWaterMark(MOVTaskHandle), 5, FontStyle::SMALL);
      break;
    case 3:
      // High water mark for the PID task
      OLED::printNumber(uxTaskGetStackHighWaterMark(PIDTaskHandle), 5, FontStyle::SMALL);
      break;
    case 4:
      // system up time stamp
      OLED::printNumber(xTaskGetTickCount() / TICKS_100MS, 5, FontStyle::SMALL);
      break;
    case 5:
      // Movement time stamp
      OLED::printNumber(lastMovementTime / TICKS_100MS, 5, FontStyle::SMALL);
      break;
    case 6:
      // Raw Tip
      { OLED::printNumber(TipThermoModel::convertTipRawADCTouV(getTipRawTemp(0), true), 6, FontStyle::SMALL); }
      break;
    case 7:
      // Temp in C
      OLED::printNumber(TipThermoModel::getTipInC(), 5, FontStyle::SMALL);
      break;
    case 8:
      // Handle Temp
      OLED::printNumber(getHandleTemperature(0), 6, FontStyle::SMALL);
      break;
    case 9:
      // Voltage input
      printVoltage();
      break;
    case 10:
      // Print ACC type
      OLED::print(AccelTypeNames[(int)DetectedAccelerometerVersion], FontStyle::SMALL);
      break;
    case 11:
      // Power negotiation status
      {
        int sourceNumber = 0;
        if (getIsPoweredByDCIN()) {
          sourceNumber = 0;
        } else {
          // We are not powered via DC, so want to display the appropriate state for PD or QC
          bool poweredbyPD        = false;
          bool pdHasVBUSConnected = false;
#if POW_PD
          if (USBPowerDelivery::fusbPresent()) {
            // We are PD capable
            if (USBPowerDelivery::negotiationComplete()) {
              // We are powered via PD
              poweredbyPD = true;
#ifdef VBUS_MOD_TEST
              pdHasVBUSConnected = USBPowerDelivery::isVBUSConnected();
#endif
            }
          }
#endif
          if (poweredbyPD) {

            if (pdHasVBUSConnected) {
              sourceNumber = 2;
            } else {
              sourceNumber = 3;
            }
          } else {
            sourceNumber = 1;
          }
        }
        OLED::print(PowerSourceNames[sourceNumber], FontStyle::SMALL);
      }
      break;
    case 12:
      // Max deg C limit
      OLED::printNumber(TipThermoModel::getTipMaxInC(), 3, FontStyle::SMALL);
      break;
#ifdef HALL_SENSOR
    case 13:
      // Print raw hall effect value if availabe, none if hall effect disabled.
      {
        int16_t hallEffectStrength = getRawHallEffect();
        if (hallEffectStrength < 0)
          hallEffectStrength = -hallEffectStrength;
        OLED::printNumber(hallEffectStrength, 6, FontStyle::SMALL);
      }
      break;
#endif
    default:
      break;
    }

    OLED::refresh();
    b = getButtonState();
    if (b == BUTTON_B_SHORT)
      return;
    else if (b == BUTTON_F_SHORT) {
      screen++;
#ifdef HALL_SENSOR
      screen = screen % 14;
#else
      screen = screen % 13;
#endif
    }
    GUIDelay();
  }
}

#if POW_PD
#ifdef HAS_POWER_DEBUG_MENU
static void showPDDebug(void) {
  // Print out the USB-PD state
  // Basically this is like the Debug menu, but instead we want to print out the PD status
  uint8_t     screen = 0;
  ButtonState b;
  for (;;) {
    OLED::clearScreen();                          // Ensure the buffer starts clean
    OLED::setCursor(0, 0);                        // Position the cursor at the 0,0 (top left)
    OLED::print(SymbolPDDebug, FontStyle::SMALL); // Print Title
    OLED::setCursor(0, 8);                        // second line
    if (screen == 0) {
      // Print the PD state machine
      OLED::print(SymbolState, FontStyle::SMALL);
      OLED::print(SymbolSpace, FontStyle::SMALL);
      OLED::printNumber(USBPowerDelivery::getStateNumber(), 2, FontStyle::SMALL, true);
      OLED::print(SymbolSpace, FontStyle::SMALL);
      // Also print vbus mod status
      if (USBPowerDelivery::fusbPresent()) {
        if (USBPowerDelivery::negotiationComplete() || (xTaskGetTickCount() > (TICKS_SECOND * 10))) {
          if (!USBPowerDelivery::isVBUSConnected()) {
            OLED::print(SymbolNoVBus, FontStyle::SMALL);
          } else {
            OLED::print(SymbolVBus, FontStyle::SMALL);
          }
        }
      }
    } else {
      // Print out the Proposed power options one by one
      auto    lastCaps = USBPowerDelivery::getLastSeenCapabilities();
      uint8_t numobj   = PD_NUMOBJ_GET(lastCaps);
      if ((screen - 1) < numobj) {
        int voltage_mv     = 0;
        int min_voltage    = 0;
        int current_a_x100 = 0;
        if ((lastCaps->obj[screen - 1] & PD_PDO_TYPE) == PD_PDO_TYPE_FIXED) {
          voltage_mv     = PD_PDV2MV(PD_PDO_SRC_FIXED_VOLTAGE_GET(lastCaps->obj[screen - 1])); // voltage in mV units
          current_a_x100 = PD_PDO_SRC_FIXED_CURRENT_GET(lastCaps->obj[screen - 1]);            // current in 10mA units
        } else {
          voltage_mv     = PD_PAV2MV(PD_APDO_PPS_MAX_VOLTAGE_GET(lastCaps->obj[screen - 1]));
          min_voltage    = PD_PAV2MV(PD_APDO_PPS_MIN_VOLTAGE_GET(lastCaps->obj[screen - 1]));
          current_a_x100 = PD_PAI2CA(PD_APDO_PPS_CURRENT_GET(lastCaps->obj[screen - 1])); // max current in 10mA units
        }
        // print out this entry of the proposal
        OLED::printNumber(screen, 1, FontStyle::SMALL, true); // print the entry number
        OLED::print(SymbolSpace, FontStyle::SMALL);
        if (min_voltage > 0) {
          OLED::printNumber(min_voltage / 1000, 2, FontStyle::SMALL, true); // print the voltage
          OLED::print(SymbolMinus, FontStyle::SMALL);
        }
        OLED::printNumber(voltage_mv / 1000, 2, FontStyle::SMALL, true); // print the voltage
        OLED::print(SymbolVolts, FontStyle::SMALL);
        OLED::print(SymbolSpace, FontStyle::SMALL);
        OLED::printNumber(current_a_x100 / 100, 2, FontStyle::SMALL, true); // print the current in 0.1A res
        OLED::print(SymbolDot, FontStyle::SMALL);
        OLED::printNumber(current_a_x100 % 100, 2, FontStyle::SMALL, true); // print the current in 0.1A res
        OLED::print(SymbolAmps, FontStyle::SMALL);

      } else {
        screen = 0;
      }
    }

    OLED::refresh();
    b = getButtonState();
    if (b == BUTTON_B_SHORT)
      return;
    else if (b == BUTTON_F_SHORT) {
      screen++;
    }
    GUIDelay();
  }
}
#endif
#endif
void showWarnings() {
  // Display alert if settings were reset
  if (settingsWereReset) {
    warnUser(translatedString(Tr->SettingsResetMessage), 10 * TICKS_SECOND);
  }
#ifndef NO_WARN_MISSING
  // We also want to alert if accel or pd is not detected / not responding
  // In this case though, we dont want to nag the user _too_ much
  // So only show first 2 times
  while (DetectedAccelerometerVersion == AccelType::Scanning) {
    osDelay(5);
  }
  // Display alert if accelerometer is not detected
  if (DetectedAccelerometerVersion == AccelType::None) {
    if (getSettingValue(SettingsOptions::AccelMissingWarningCounter) < 2) {
      nextSettingValue(SettingsOptions::AccelMissingWarningCounter);
      saveSettings();
      warnUser(translatedString(Tr->NoAccelerometerMessage), 10 * TICKS_SECOND);
    }
  }
#if POW_PD
  // We expect pd to be present
  if (!USBPowerDelivery::fusbPresent()) {
    if (getSettingValue(SettingsOptions::PDMissingWarningCounter) < 2) {
      nextSettingValue(SettingsOptions::PDMissingWarningCounter);
      saveSettings();
      warnUser(translatedString(Tr->NoPowerDeliveryMessage), 10 * TICKS_SECOND);
    }
  }
#endif
#endif
}

uint8_t buttonAF[sizeof(buttonA)];
uint8_t buttonBF[sizeof(buttonB)];
uint8_t disconnectedTipF[sizeof(disconnectedTip)];
/* StartGUITask function */
void startGUITask(void const *argument) {
  (void)argument;
  prepareTranslations();

  OLED::initialize(); // start up the LCD
  OLED::setBrightness(getSettingValue(SettingsOptions::OLEDBrightness));
  OLED::setInverseDisplay(getSettingValue(SettingsOptions::OLEDInversion));

  uint8_t tempWarningState       = 0;
  bool    buttonLockout          = false;
  bool    tempOnDisplay          = false;
  bool    tipDisconnectedDisplay = false;
  bool    showExitMenuTransition = false;
  {
    // Generate the flipped screen into ram for later use
    // flipped is generated by flipping each row
    for (int row = 0; row < 2; row++) {
      for (int x = 0; x < 42; x++) {
        buttonAF[(row * 42) + x]         = buttonA[(row * 42) + (41 - x)];
        buttonBF[(row * 42) + x]         = buttonB[(row * 42) + (41 - x)];
        disconnectedTipF[(row * 42) + x] = disconnectedTip[(row * 42) + (41 - x)];
      }
    }
  }
  getTipRawTemp(1); // reset filter
  OLED::setRotation(getSettingValue(SettingsOptions::OrientationMode) & 1);
  // If the front button is held down, on supported devices, show PD debugging metrics
#if POW_PD
#ifdef HAS_POWER_DEBUG_MENU
  if (getButtonA()) {
    showPDDebug();
  }
#endif
#endif
  BootLogo::handleShowingLogo((uint8_t *)FLASH_LOGOADDR);

  showWarnings();

  if (getSettingValue(SettingsOptions::AutoStartMode)) {
    // jump directly to the autostart mode
    gui_solderingMode(getSettingValue(SettingsOptions::AutoStartMode) - 1);
    buttonLockout = true;
  }

  for (;;) {
    ButtonState buttons = getButtonState();
    if (buttons != BUTTON_NONE) {
      OLED::setDisplayState(OLED::DisplayState::ON);
    }
    if (tempWarningState == 2)
      buttons = BUTTON_F_SHORT;
    if (buttons != BUTTON_NONE && buttonLockout)
      buttons = BUTTON_NONE;
    else
      buttonLockout = false;

    switch (buttons) {
    case BUTTON_NONE:
      // Do nothing
      break;
    case BUTTON_BOTH:
      // Not used yet
      // In multi-language this might be used to reset language on a long hold
      // or some such
      break;

    case BUTTON_B_LONG:
      // Show the version information
      showDebugMenu();
      break;
    case BUTTON_F_LONG:
      gui_solderingTempAdjust();
      saveSettings();
      break;
    case BUTTON_F_SHORT:
      if (!isTipDisconnected()) {
        gui_solderingMode(0); // enter soldering mode
        buttonLockout = true;
      }
      break;
    case BUTTON_B_SHORT:
      enterSettingsMenu(); // enter the settings menu
      {
        OLED::useSecondaryFramebuffer(true);
        showExitMenuTransition = true;
      }
      buttonLockout = true;
      break;
    default:
      break;
    }

    currentTempTargetDegC = 0; // ensure tip is off
    getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);
    uint32_t tipTemp = TipThermoModel::getTipInC();
    if (tipTemp > 55) {
      setStatusLED(LED_COOLING_STILL_HOT);
    } else {
      setStatusLED(LED_STANDBY);
    }
    // Preemptively turn the display on.  Turn it off if and only if
    // the tip temperature is below 50 degrees C *and* motion sleep
    // detection is enabled *and* there has been no activity (movement or
    // button presses) in a while.
    // This is zero cost really as state is only changed on display updates
    OLED::setDisplayState(OLED::DisplayState::ON);

    if ((tipTemp < 50) && getSettingValue(SettingsOptions::Sensitivity)
        && (((xTaskGetTickCount() - lastMovementTime) > MOVEMENT_INACTIVITY_TIME) && ((xTaskGetTickCount() - lastButtonTime) > BUTTON_INACTIVITY_TIME))) {
      OLED::setDisplayState(OLED::DisplayState::OFF);
      setStatusLED(LED_OFF);
    }
    // Clear the lcd buffer
    OLED::clearScreen();
    if (OLED::getRotation()) {
      OLED::setCursor(50, 0);
    } else {
      OLED::setCursor(-1, 0);
    }
    if (getSettingValue(SettingsOptions::DetailedIDLE)) {
      if (isTipDisconnected()) {
        if (OLED::getRotation()) {
          // in right handed mode we want to draw over the first part
          OLED::drawArea(54, 0, 42, 16, disconnectedTipF);
        } else {
          OLED::drawArea(0, 0, 42, 16, disconnectedTip);
        }
        if (OLED::getRotation()) {
          OLED::setCursor(-1, 0);
        } else {
          OLED::setCursor(42, 0);
        }
        uint32_t Vlt = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);
        OLED::printNumber(Vlt / 10, 2, FontStyle::LARGE);
        OLED::print(SymbolDot, FontStyle::LARGE);
        OLED::printNumber(Vlt % 10, 1, FontStyle::LARGE);
        if (OLED::getRotation()) {
          OLED::setCursor(48, 8);
        } else {
          OLED::setCursor(91, 8);
        }
        OLED::print(SymbolVolts, FontStyle::SMALL);
      } else {
        if (!(getSettingValue(SettingsOptions::CoolingTempBlink) && (tipTemp > 55) && (xTaskGetTickCount() % 1000 < 300)))
          // Blink temp if setting enable and temp < 55°
          // 1000 tick/sec
          // OFF 300ms ON 700ms
          gui_drawTipTemp(true, FontStyle::LARGE); // draw in the temp
        if (OLED::getRotation()) {
          OLED::setCursor(6, 0);
        } else {
          OLED::setCursor(73, 0); // top right
        }
        OLED::printNumber(getSettingValue(SettingsOptions::SolderingTemp), 3, FontStyle::SMALL); // draw set temp
        if (getSettingValue(SettingsOptions::TemperatureInF))
          OLED::print(SymbolDegF, FontStyle::SMALL);
        else
          OLED::print(SymbolDegC, FontStyle::SMALL);
        if (OLED::getRotation()) {
          OLED::setCursor(0, 8);
        } else {
          OLED::setCursor(67, 8); // bottom right
        }
        printVoltage(); // draw voltage then symbol (v)
        OLED::print(SymbolVolts, FontStyle::SMALL);
      }

    } else {
      if (OLED::getRotation()) {
        OLED::drawArea(54, 0, 42, 16, buttonAF);
        OLED::drawArea(12, 0, 42, 16, buttonBF);
        OLED::setCursor(0, 0);
        gui_drawBatteryIcon();
      } else {
        OLED::drawArea(0, 0, 42, 16, buttonA);  // Needs to be flipped so button ends up
        OLED::drawArea(42, 0, 42, 16, buttonB); // on right side of screen
        OLED::setCursor(84, 0);
        gui_drawBatteryIcon();
      }
      tipDisconnectedDisplay = false;
      if (tipTemp > 55)
        tempOnDisplay = true;
      else if (tipTemp < 45)
        tempOnDisplay = false;
      if (isTipDisconnected()) {
        tempOnDisplay          = false;
        tipDisconnectedDisplay = true;
      }
      if (tempOnDisplay || tipDisconnectedDisplay) {
        // draw temp over the start soldering button
        // Location changes on screen rotation
        if (OLED::getRotation()) {
          // in right handed mode we want to draw over the first part
          OLED::fillArea(55, 0, 41, 16, 0); // clear the area for the temp
          OLED::setCursor(56, 0);
        } else {
          OLED::fillArea(0, 0, 41, 16, 0); // clear the area
          OLED::setCursor(0, 0);
        }
        // If we have a tip connected draw the temp, if not we leave it blank
        if (!tipDisconnectedDisplay) {
          // draw in the temp
          if (!(getSettingValue(SettingsOptions::CoolingTempBlink) && (xTaskGetTickCount() % 260 < 160)))
            gui_drawTipTemp(false, FontStyle::LARGE); // draw in the temp
        } else {
          // Draw in missing tip symbol

          if (OLED::getRotation()) {
            // in right handed mode we want to draw over the first part
            OLED::drawArea(54, 0, 42, 16, disconnectedTipF);
          } else {
            OLED::drawArea(0, 0, 42, 16, disconnectedTip);
          }
        }
      }
    }

    if (showExitMenuTransition) {
      OLED::useSecondaryFramebuffer(false);
      OLED::transitionSecondaryFramebuffer(false);
      showExitMenuTransition = false;
    } else {
      OLED::refresh();
      GUIDelay();
    }
  }
}
