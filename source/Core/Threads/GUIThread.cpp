/*
 * GUIThread.cpp
 *
 *  Created on: 19 Aug 2019
 *      Author: ralim
 */
extern "C" {
#include "FreeRTOSConfig.h"
}
#include "../../configuration.h"
#include "Buttons.hpp"
#include "I2CBB.hpp"
#include "LIS2DH12.hpp"
#include "Settings.h"
#include "TipThermoModel.h"
#include "Translation.h"
#include "cmsis_os.h"
#include "main.hpp"
#include "stdlib.h"
#include "string.h"
#include "unit.h"
#include <MMA8652FC.hpp>
#include <gui.hpp>
#include <history.hpp>
#include <power.hpp>
#ifdef POW_PD
#include "policy_engine.h"
#endif
// File local variables
extern uint32_t   currentTempTargetDegC;
extern TickType_t lastMovementTime;
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
void warnUser(const char *warning, const int font, const int timeout) {
  OLED::setFont(font);
  OLED::clearScreen();
  OLED::setCursor(0, 0);
  OLED::print(warning);
  OLED::refresh();
  waitForButtonPressOrTimeout(timeout);
}

void printVoltage() {
  uint32_t volt = getInputVoltageX10(systemSettings.voltageDiv, 0);
  OLED::printNumber(volt / 10, 2);
  OLED::print(SymbolDot);
  OLED::printNumber(volt % 10, 1);
}
void GUIDelay() {
  // Called in all UI looping tasks,
  // This limits the re-draw rate to the LCD and also lets the DMA run
  // As the gui task can very easily fill this bus with transactions, which will
  // prevent the movement detection from running
  osDelay(50);
}
void gui_drawTipTemp(bool symbol) {
  // Draw tip temp handling unit conversion & tolerance near setpoint
  uint32_t Temp = 0;
#ifdef ENABLED_FAHRENHEIT_SUPPORT
  if (systemSettings.temperatureInF) {
    Temp = TipThermoModel::getTipInF();
  } else
#endif
  {
    Temp = TipThermoModel::getTipInC();
  }

  OLED::printNumber(Temp, 3); // Draw the tip temp out
  if (symbol) {
    if (OLED::getFont() == 0) {
      // Big font, can draw nice symbols
#ifdef ENABLED_FAHRENHEIT_SUPPORT
      if (systemSettings.temperatureInF)
        OLED::drawSymbol(0);
      else
#endif
        OLED::drawSymbol(1);
    } else {
      // Otherwise fall back to chars
#ifdef ENABLED_FAHRENHEIT_SUPPORT
      if (systemSettings.temperatureInF)
        OLED::print(SymbolDegF);
      else
#endif
        OLED::print(SymbolDegC);
    }
  }
}

#ifdef POW_DC
// returns true if undervoltage has occured
static bool checkVoltageForExit() {
  if (!getIsPoweredByDCIN()) {
    return false;
  }
  uint16_t v = getInputVoltageX10(systemSettings.voltageDiv, 0);

  // Dont check for first 2 seconds while the ADC stabilizes and the DMA fills
  // the buffer
  if (xTaskGetTickCount() > (TICKS_SECOND * 2)) {
    if ((v < lookupVoltageLevel())) {
      currentTempTargetDegC = 0;
      OLED::clearScreen();
      OLED::setCursor(0, 0);
      if (systemSettings.detailedSoldering) {
        OLED::setFont(1);
        OLED::print(UndervoltageString);
        OLED::setCursor(0, 8);
        OLED::print(InputVoltageString);
        printVoltage();
        OLED::print(SymbolVolts);
      } else {
        OLED::setFont(0);
        OLED::print(UVLOWarningString);
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
    uint8_t V = getInputVoltageX10(systemSettings.voltageDiv, 0);
    if (V % 10 >= 5)
      V = V / 10 + 1; // round up
    else
      V = V / 10;
    if (V >= 10) {
      int16_t xPos = OLED::getCursorX();
      OLED::setFont(1);
      OLED::printNumber(V / 10, 1);
      OLED::setCursor(xPos, 8);
      OLED::printNumber(V % 10, 1);
      OLED::setFont(0);
      OLED::setCursor(xPos + 12, 0); // need to reset this as if we drew a wide char
    } else {
      OLED::printNumber(V, 1);
    }
    return;
  }
#endif
#ifdef POW_DC
  if (systemSettings.minDCVoltageCells) {
    // User is on a lithium battery
    // we need to calculate which of the 10 levels they are on
    uint8_t  cellCount = systemSettings.minDCVoltageCells + 2;
    uint32_t cellV     = getInputVoltageX10(systemSettings.voltageDiv, 0) / cellCount;
    // Should give us approx cell voltage X10
    // Range is 42 -> 33 = 9 steps therefore we will use battery 0-9
    if (cellV < 33)
      cellV = 33;
    cellV -= 33; // Should leave us a number of 0-9
    if (cellV > 9)
      cellV = 9;
    OLED::drawBattery(cellV + 1);
  } else {
    OLED::drawSymbol(15); // Draw the DC Logo
  }
#endif
}
static void gui_solderingTempAdjust() {
  uint32_t lastChange             = xTaskGetTickCount();
  currentTempTargetDegC           = 0;
  uint32_t autoRepeatTimer        = 0;
  uint8_t  autoRepeatAcceleration = 0;
  for (;;) {
    OLED::setCursor(0, 0);
    OLED::clearScreen();
    OLED::setFont(0);
    ButtonState buttons = getButtonState();
    if (buttons)
      lastChange = xTaskGetTickCount();
    switch (buttons) {
    case BUTTON_NONE:
      // stay
      break;
    case BUTTON_BOTH:
      // exit
      return;
      break;
    case BUTTON_B_LONG:
      if (xTaskGetTickCount() - autoRepeatTimer + autoRepeatAcceleration > PRESS_ACCEL_INTERVAL_MAX) {
        if (systemSettings.ReverseButtonTempChangeEnabled) {
          systemSettings.SolderingTemp += systemSettings.TempChangeLongStep;
        } else
          systemSettings.SolderingTemp -= systemSettings.TempChangeLongStep;

        autoRepeatTimer = xTaskGetTickCount();
        autoRepeatAcceleration += PRESS_ACCEL_STEP;
      }
      break;
    case BUTTON_B_SHORT:
      if (systemSettings.ReverseButtonTempChangeEnabled) {
        systemSettings.SolderingTemp += systemSettings.TempChangeShortStep;
      } else
        systemSettings.SolderingTemp -= systemSettings.TempChangeShortStep;
      break;
    case BUTTON_F_LONG:
      if (xTaskGetTickCount() - autoRepeatTimer + autoRepeatAcceleration > PRESS_ACCEL_INTERVAL_MAX) {
        if (systemSettings.ReverseButtonTempChangeEnabled) {
          systemSettings.SolderingTemp -= systemSettings.TempChangeLongStep;
        } else
          systemSettings.SolderingTemp += systemSettings.TempChangeLongStep;
        autoRepeatTimer = xTaskGetTickCount();
        autoRepeatAcceleration += PRESS_ACCEL_STEP;
      }
      break;
    case BUTTON_F_SHORT:
      if (systemSettings.ReverseButtonTempChangeEnabled) {
        systemSettings.SolderingTemp -= systemSettings.TempChangeShortStep; // add 10
      } else
        systemSettings.SolderingTemp += systemSettings.TempChangeShortStep; // add 10
      break;
    default:
      break;
    }
    if ((PRESS_ACCEL_INTERVAL_MAX - autoRepeatAcceleration) < PRESS_ACCEL_INTERVAL_MIN) {
      autoRepeatAcceleration = PRESS_ACCEL_INTERVAL_MAX - PRESS_ACCEL_INTERVAL_MIN;
    }
    // constrain between 10-450 C
#ifdef ENABLED_FAHRENHEIT_SUPPORT
    if (systemSettings.temperatureInF) {
      if (systemSettings.SolderingTemp > 850)
        systemSettings.SolderingTemp = 850;
      if (systemSettings.SolderingTemp < 60)
        systemSettings.SolderingTemp = 60;
    } else
#endif
    {
      if (systemSettings.SolderingTemp > 450)
        systemSettings.SolderingTemp = 450;
      if (systemSettings.SolderingTemp < 10)
        systemSettings.SolderingTemp = 10;
    }

    if (xTaskGetTickCount() - lastChange > 2000)
      return; // exit if user just doesn't press anything for a bit

#ifdef OLED_FLIP
    if (!OLED::getRotation()) {
#else
    if (OLED::getRotation()) {
#endif
      OLED::print(systemSettings.ReverseButtonTempChangeEnabled ? SymbolPlus : SymbolMinus);
    } else {
      OLED::print(systemSettings.ReverseButtonTempChangeEnabled ? SymbolMinus : SymbolPlus);
    }

    OLED::print(SymbolSpace);
    OLED::printNumber(systemSettings.SolderingTemp, 3);
#ifdef ENABLED_FAHRENHEIT_SUPPORT
    if (systemSettings.temperatureInF)
      OLED::drawSymbol(0);
    else
#endif
    {
      OLED::drawSymbol(1);
    }
    OLED::print(SymbolSpace);
#ifdef OLED_FLIP
    if (!OLED::getRotation()) {
#else
    if (OLED::getRotation()) {
#endif
      OLED::print(systemSettings.ReverseButtonTempChangeEnabled ? SymbolMinus : SymbolPlus);
    } else {
      OLED::print(systemSettings.ReverseButtonTempChangeEnabled ? SymbolPlus : SymbolMinus);
    }
    OLED::refresh();
    GUIDelay();
  }
}
static bool shouldShutdown() {
  if (systemSettings.ShutdownTime) { // only allow shutdown exit if time > 0
    if (lastMovementTime) {
      if (((TickType_t)(xTaskGetTickCount() - lastMovementTime)) > (TickType_t)(systemSettings.ShutdownTime * TICKS_MIN)) {
        return true;
      }
    }
    if (lastHallEffectSleepStart) {
      if (((TickType_t)(xTaskGetTickCount() - lastHallEffectSleepStart)) > (TickType_t)(systemSettings.ShutdownTime * TICKS_MIN)) {
        return true;
      }
    }
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
#ifdef ENABLED_FAHRENHEIT_SUPPORT
    if (systemSettings.temperatureInF) {
      currentTempTargetDegC = stayOff ? 0 : TipThermoModel::convertFtoC(min(systemSettings.SleepTemp, systemSettings.SolderingTemp));
    } else
#endif
    {
      currentTempTargetDegC = stayOff ? 0 : min(systemSettings.SleepTemp, systemSettings.SolderingTemp);
    }
    // draw the lcd
    uint16_t tipTemp;
#ifdef ENABLED_FAHRENHEIT_SUPPORT
    if (systemSettings.temperatureInF)
      tipTemp = TipThermoModel::getTipInF();
    else
#endif
    {
      tipTemp = TipThermoModel::getTipInC();
    }

    OLED::clearScreen();
    OLED::setCursor(0, 0);
    if (systemSettings.detailedSoldering) {
      OLED::setFont(1);
      OLED::print(SleepingAdvancedString);
      OLED::setCursor(0, 8);
      OLED::print(SleepingTipAdvancedString);
      OLED::printNumber(tipTemp, 3);
#ifdef ENABLED_FAHRENHEIT_SUPPORT
      if (systemSettings.temperatureInF)
        OLED::print(SymbolDegF);
      else
#endif
      {
        OLED::print(SymbolDegC);
      }

      OLED::print(SymbolSpace);
      printVoltage();
      OLED::print(SymbolVolts);
    } else {
      OLED::setFont(0);
      OLED::print(SleepingSimpleString);
      OLED::printNumber(tipTemp, 3);
#ifdef ENABLED_FAHRENHEIT_SUPPORT
      if (systemSettings.temperatureInF)
        OLED::drawSymbol(0);
      else
#endif
      {
        OLED::drawSymbol(1);
      }
    }

    OLED::refresh();
    GUIDelay();
    if (!shouldBeSleeping(autoStarted)) {
      return 0;
    }
    if (shouldShutdown()) {
      // shutdown
      currentTempTargetDegC = 0;
      return 1; // we want to exit soldering mode
    }
  }
  return 0;
}

static void display_countdown(int sleepThres) {
  /*
   * Print seconds or minutes (if > 99 seconds) until sleep
   * mode is triggered.
   */
  int        lastEventTime = lastButtonTime < lastMovementTime ? lastMovementTime : lastButtonTime;
  TickType_t downCount     = sleepThres - xTaskGetTickCount() + lastEventTime;
  if (downCount > (99 * TICKS_SECOND)) {
    OLED::printNumber(downCount / 60000 + 1, 2);
    OLED::print(SymbolMinutes);
  } else {
    OLED::printNumber(downCount / 1000 + 1, 2);
    OLED::print(SymbolSeconds);
  }
}
static uint32_t getSleepTimeout() {
  if (systemSettings.sensitivity && systemSettings.SleepTime) {

    uint32_t sleepThres = 0;
    if (systemSettings.SleepTime < 6)
      sleepThres = systemSettings.SleepTime * 10 * 1000;
    else
      sleepThres = (systemSettings.SleepTime - 5) * 60 * 1000;
    return sleepThres;
  }
  return 0;
}
static bool shouldBeSleeping(bool inAutoStart) {
  // Return true if the iron should be in sleep mode
  if (systemSettings.sensitivity && systemSettings.SleepTime) {
    if (inAutoStart) {
      // In auto start we are asleep until movement
      if (lastMovementTime == 0 && lastButtonTime == 0) {
        return true;
      }
    }
    if (lastMovementTime > 0 || lastButtonTime > 0) {
      if ((xTaskGetTickCount() - lastMovementTime) > getSleepTimeout() && (xTaskGetTickCount() - lastButtonTime) > getSleepTimeout()) {
        return true;
      }
    }
  }

#ifdef HALL_SENSOR
  // If the hall effect sensor is enabled in the build, check if its over
  // threshold, and if so then we force sleep
  if (lookupHallEffectThreshold()) {
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
    if (buttonsLocked && (systemSettings.lockingMode != 0)) { // If buttons locked
      switch (buttons) {
      case BUTTON_NONE:
        boostModeOn = false;
        break;
      case BUTTON_BOTH_LONG:
        // Unlock buttons
        buttonsLocked = false;
        warnUser(UnlockingKeysString, 0, TICKS_SECOND);
        break;
      case BUTTON_F_LONG:
        // if boost mode is enabled turn it on
        if (systemSettings.BoostTemp && (systemSettings.lockingMode == 1)) {
          boostModeOn = true;
        }
        break;
        // fall through
      case BUTTON_BOTH:
      case BUTTON_B_LONG:
      case BUTTON_F_SHORT:
      case BUTTON_B_SHORT:
        // Do nothing and display a lock warming
        warnUser(WarningKeysLockedString, 0, TICKS_SECOND / 2);
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
        if (systemSettings.BoostTemp)
          boostModeOn = true;
        break;
      case BUTTON_F_SHORT:
      case BUTTON_B_SHORT: {
        uint16_t oldTemp = systemSettings.SolderingTemp;
        gui_solderingTempAdjust(); // goto adjust temp mode
        if (oldTemp != systemSettings.SolderingTemp) {
          saveSettings(); // only save on change
        }
      } break;
      case BUTTON_BOTH_LONG:
        if (systemSettings.lockingMode != 0) {
          // Lock buttons
          buttonsLocked = true;
          warnUser(LockingKeysString, 0, TICKS_SECOND);
        }
        break;
      default:
        break;
      }
    }
    // else we update the screen information
    OLED::setCursor(0, 0);
    OLED::clearScreen();
    OLED::setFont(0);
    // Draw in the screen details
    if (systemSettings.detailedSoldering) {
      OLED::setFont(1);
      OLED::print(SolderingAdvancedPowerPrompt); // Power:
      OLED::printNumber(x10WattHistory.average() / 10, 2);
      OLED::print(SymbolDot);
      OLED::printNumber(x10WattHistory.average() % 10, 1);
      OLED::print(SymbolWatts);

      if (systemSettings.sensitivity && systemSettings.SleepTime) {
        OLED::print(SymbolSpace);
        display_countdown(getSleepTimeout());
      }

      OLED::setCursor(0, 8);
      OLED::print(SleepingTipAdvancedString);
      gui_drawTipTemp(true);
      OLED::print(SymbolSpace);
      printVoltage();
      OLED::print(SymbolVolts);
    } else {
      // We switch the layout direction depending on the orientation of the oled
      if (OLED::getRotation()) {
        // battery
        gui_drawBatteryIcon();
        OLED::print(SymbolSpace); // Space out gap between battery <-> temp
        gui_drawTipTemp(true);    // Draw current tip temp

        // We draw boost arrow if boosting, or else gap temp <-> heat
        // indicator
        if (boostModeOn)
          OLED::drawSymbol(2);
        else
          OLED::print(SymbolSpace);

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
          OLED::print(SymbolSpace);
        gui_drawTipTemp(true); // Draw current tip temp

        OLED::print(SymbolSpace); // Space out gap between battery <-> temp

        gui_drawBatteryIcon();
      }
    }
    OLED::refresh();

    // Update the setpoints for the temperature
    if (boostModeOn) {
#ifdef ENABLED_FAHRENHEIT_SUPPORT
      if (systemSettings.temperatureInF)
        currentTempTargetDegC = TipThermoModel::convertFtoC(systemSettings.BoostTemp);
      else
#endif
      {
        currentTempTargetDegC = (systemSettings.BoostTemp);
      }
    } else {
#ifdef ENABLED_FAHRENHEIT_SUPPORT
      if (systemSettings.temperatureInF)
        currentTempTargetDegC = TipThermoModel::convertFtoC(systemSettings.SolderingTemp);
      else
#endif
      {
        currentTempTargetDegC = (systemSettings.SolderingTemp);
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
    // slow down ui update rate
    GUIDelay();
  }
}

void showDebugMenu(void) {
  uint8_t     screen = 0;
  ButtonState b;
  OLED::setFont(1); // small font
  for (;;) {
    OLED::clearScreen();              // Ensure the buffer starts clean
    OLED::setCursor(0, 0);            // Position the cursor at the 0,0 (top left)
    OLED::print(SymbolVersionNumber); // Print version number
    OLED::setCursor(0, 8);            // second line
    OLED::print(DebugMenu[screen]);
    switch (screen) {
    case 0: // Just prints date
      break;
    case 1:
      // High water mark for GUI
      OLED::printNumber(uxTaskGetStackHighWaterMark(GUITaskHandle), 5);
      break;
    case 2:
      // High water mark for the Movement task
      OLED::printNumber(uxTaskGetStackHighWaterMark(MOVTaskHandle), 5);
      break;
    case 3:
      // High water mark for the PID task
      OLED::printNumber(uxTaskGetStackHighWaterMark(PIDTaskHandle), 5);
      break;
    case 4:
      // system up time stamp
      OLED::printNumber(xTaskGetTickCount() / 100, 5);
      break;
    case 5:
      // Movement time stamp
      OLED::printNumber(lastMovementTime / 100, 5);
      break;
    case 6:
      // Raw Tip
      {
        uint32_t temp                    = systemSettings.CalibrationOffset;
        systemSettings.CalibrationOffset = 0;
        OLED::printNumber(TipThermoModel::convertTipRawADCTouV(getTipRawTemp(0)), 6);
        systemSettings.CalibrationOffset = temp;
      }
      break;
    case 7:
      // Temp in C
      OLED::printNumber(TipThermoModel::getTipInC(), 5);
      break;
    case 8:
      // Handle Temp
      OLED::printNumber(getHandleTemperature(), 3);
      break;
    case 9:
      // Voltage input
      printVoltage();
      break;
    case 10:
      // Print PCB ID number
      OLED::printNumber(DetectedAccelerometerVersion, 2);
      break;
    case 11:
      // Power negotiation status
      if (getIsPoweredByDCIN()) {
        OLED::printNumber(0, 1);
      } else {
        // We are not powered via DC, so want to display the appropriate state for PD or QC
        bool poweredbyPD = false;
#ifdef POW_PD
        if (usb_pd_detect()) {
          // We are PD capable
          if (PolicyEngine::pdHasNegotiated()) {
            // We are powered via PD
            poweredbyPD = true;
          }
        }
#endif
        if (poweredbyPD) {
          OLED::printNumber(2, 1);
        } else {

          OLED::printNumber(1, 1);
        }
      }
      break;
    case 12:
      // Max deg C limit
      OLED::printNumber(TipThermoModel::getTipMaxInC(), 3);
      break;
    default:
      break;
    }

    OLED::refresh();
    b = getButtonState();
    if (b == BUTTON_B_SHORT)
      return;
    else if (b == BUTTON_F_SHORT) {
      screen++;
      screen = screen % 13;
    }
    GUIDelay();
  }
}

void showWarnings() {
  // Display alert if settings were reset
  if (settingsWereReset) {
    warnUser(SettingsResetMessage, 1, 10 * TICKS_SECOND);
  }
#ifndef NO_WARN_MISSING
  // We also want to alert if accel or pd is not detected / not responding
  // In this case though, we dont want to nag the user _too_ much
  // So only show first 2 times
  while (DetectedAccelerometerVersion == ACCELEROMETERS_SCANNING) {
    osDelay(5);
  }
  // Display alert if accelerometer is not detected
  if (DetectedAccelerometerVersion == NO_DETECTED_ACCELEROMETER) {
    if (systemSettings.accelMissingWarningCounter < 2) {
      systemSettings.accelMissingWarningCounter++;
      saveSettings();
      warnUser(NoAccelerometerMessage, 1, 10 * TICKS_SECOND);
    }
  }
#ifdef POW_PD
  // We expect pd to be present
  if (!usb_pd_detect()) {
    if (systemSettings.pdMissingWarningCounter < 2) {
      systemSettings.pdMissingWarningCounter++;
      saveSettings();
      warnUser(NoPowerDeliveryMessage, 1, 10 * TICKS_SECOND);
    }
  }
#endif
#endif
}

uint8_t idleScreenBGF[sizeof(idleScreenBG)];
/* StartGUITask function */
void startGUITask(void const *argument __unused) {
  OLED::initialize(); // start up the LCD

  uint8_t tempWarningState       = 0;
  bool    buttonLockout          = false;
  bool    tempOnDisplay          = false;
  bool    tipDisconnectedDisplay = false;
  {
    // Generate the flipped screen into ram for later use
    // flipped is generated by flipping each row
    for (int row = 0; row < 2; row++) {
      for (int x = 0; x < 84; x++) {
        idleScreenBGF[(row * 84) + x] = idleScreenBG[(row * 84) + (83 - x)];
      }
    }
  }
  getTipRawTemp(1); // reset filter
  OLED::setRotation(systemSettings.OrientationMode & 1);
  uint32_t ticks = xTaskGetTickCount();
  ticks += 4000; // 4 seconds from now
  while (xTaskGetTickCount() < ticks) {
    if (showBootLogoIfavailable() == false)
      ticks = xTaskGetTickCount();
    ButtonState buttons = getButtonState();
    if (buttons)
      ticks = xTaskGetTickCount(); // make timeout now so we will exit
    GUIDelay();
  }

  showWarnings();

  if (systemSettings.autoStartMode) {
    // jump directly to the autostart mode
    gui_solderingMode(systemSettings.autoStartMode - 1);
    buttonLockout = true;
  }

  for (;;) {
    ButtonState buttons = getButtonState();
    if (buttons != BUTTON_NONE) {
      OLED::setDisplayState(OLED::DisplayState::ON);
      OLED::setFont(0);
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
      gui_solderingMode(0); // enter soldering mode
      buttonLockout = true;
      break;
    case BUTTON_B_SHORT:
      enterSettingsMenu(); // enter the settings menu
      buttonLockout = true;
      break;
    default:
      break;
    }

    currentTempTargetDegC = 0; // ensure tip is off
    getInputVoltageX10(systemSettings.voltageDiv, 0);
    uint32_t tipTemp = TipThermoModel::getTipInC();

    // Preemptively turn the display on.  Turn it off if and only if
    // the tip temperature is below 50 degrees C *and* motion sleep
    // detection is enabled *and* there has been no activity (movement or
    // button presses) in a while.
    // This is zero cost really as state is only changed on display updates
    OLED::setDisplayState(OLED::DisplayState::ON);

    if ((tipTemp < 50) && systemSettings.sensitivity && (((xTaskGetTickCount() - lastMovementTime) > MOVEMENT_INACTIVITY_TIME) && ((xTaskGetTickCount() - lastButtonTime) > BUTTON_INACTIVITY_TIME))) {
      OLED::setDisplayState(OLED::DisplayState::OFF);
    }
    uint16_t tipDisconnectedThres = TipThermoModel::getTipMaxInC() - 5;
    // Clear the lcd buffer
    OLED::clearScreen();
    OLED::setCursor(0, 0);
    if (systemSettings.detailedIDLE) {
      OLED::setFont(1);
      if (tipTemp > tipDisconnectedThres) {
        OLED::print(TipDisconnectedString);
      } else {
        OLED::print(IdleTipString);
        gui_drawTipTemp(false);
        OLED::print(IdleSetString);
        OLED::printNumber(systemSettings.SolderingTemp, 3);
      }
      OLED::setCursor(0, 8);

      OLED::print(InputVoltageString);
      printVoltage();

    } else {
      OLED::setFont(0);
#ifdef OLED_FLIP
      if (!OLED::getRotation()) {
#else
      if (OLED::getRotation()) {
#endif
        OLED::drawArea(12, 0, 84, 16, idleScreenBG);
        OLED::setCursor(0, 0);
        gui_drawBatteryIcon();
      } else {
        OLED::drawArea(0, 0, 84, 16, idleScreenBGF); // Needs to be flipped so button ends up
                                                     // on right side of screen
        OLED::setCursor(84, 0);
        gui_drawBatteryIcon();
      }
      tipDisconnectedDisplay = false;
      if (tipTemp > 55)
        tempOnDisplay = true;
      else if (tipTemp < 45)
        tempOnDisplay = false;
      if (tipTemp > tipDisconnectedThres) {
        tempOnDisplay          = false;
        tipDisconnectedDisplay = true;
      }
      if (tempOnDisplay || tipDisconnectedDisplay) {
        // draw temp over the start soldering button
        // Location changes on screen rotation
#ifdef OLED_FLIP
        if (!OLED::getRotation()) {
#else
        if (OLED::getRotation()) {
#endif
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
          if (!(systemSettings.coolingTempBlink && (xTaskGetTickCount() % 26 < 16)))
            gui_drawTipTemp(false); // draw in the temp
        } else {
          // Draw in missing tip symbol

#ifdef OLED_FLIP
          if (!OLED::getRotation()) {
#else
          if (OLED::getRotation()) {
#endif
            // in right handed mode we want to draw over the first part
            OLED::drawArea(55, 0, 41, 16, disconnectedTipIcon);

          } else {
            OLED::drawArea(0, 0, 41, 16, disconnectedTipIcon);
          }
        }
      }
    }

    OLED::refresh();
    GUIDelay();
  }
}
