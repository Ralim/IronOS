/*
 * GUIThread.cpp
 *
 *  Created on: 19 Aug 2019
 *      Author: ralim
 */
extern "C" {
#include "FreeRTOSConfig.h"
}
#include <MMA8652FC.hpp>
#include <gui.hpp>
#include <history.hpp>
#include "main.hpp"
#include <power.hpp>
#include "../../configuration.h"
#include "Buttons.hpp"
#include "LIS2DH12.hpp"
#include "Settings.h"
#include "TipThermoModel.h"
#include "Translation.h"
#include "cmsis_os.h"
#include "stdlib.h"
#include "string.h"
#include "unit.h"

// File local variables
extern uint32_t currentTempTargetDegC;
extern uint8_t accelInit;
extern uint32_t lastMovementTime;
extern int16_t idealQCVoltage;
extern osThreadId GUITaskHandle;
extern osThreadId MOVTaskHandle;
extern osThreadId PIDTaskHandle;

#define MOVEMENT_INACTIVITY_TIME (60 * configTICK_RATE_HZ)
#define BUTTON_INACTIVITY_TIME (60 * configTICK_RATE_HZ)

static uint16_t min(uint16_t a, uint16_t b) {
    if (a > b)
        return b;
    else
        return a;
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
    uint16_t Temp = 0;
#ifdef ENABLED_FAHRENHEIT_SUPPORT
    if (systemSettings.temperatureInF)
        Temp = TipThermoModel::getTipInF();
    else
#endif
        Temp = TipThermoModel::getTipInC();

    OLED::printNumber(Temp, 3);  // Draw the tip temp out finally
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

#ifdef MODEL_TS100
// returns true if undervoltage has occured
static bool checkVoltageForExit() {
    uint16_t v = getInputVoltageX10(systemSettings.voltageDiv, 0);

    // Dont check for first 1.5 seconds while the ADC stabilizes and the DMA fills the buffer
    if (xTaskGetTickCount() > 150) {
        if ((v < lookupVoltageLevel(systemSettings.cutoutSetting))) {
            GUIDelay();
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
            currentTempTargetDegC = 0;
            waitForButtonPress();
            return true;
        }
    }
    return false;
}
#endif
static void gui_drawBatteryIcon() {
#ifdef MODEL_TS100
    if (systemSettings.cutoutSetting) {
        // User is on a lithium battery
        // we need to calculate which of the 10 levels they are on
        uint8_t cellCount = systemSettings.cutoutSetting + 2;
        uint32_t cellV = getInputVoltageX10(systemSettings.voltageDiv, 0) / cellCount;
        // Should give us approx cell voltage X10
        // Range is 42 -> 33 = 9 steps therefore we will use battery 1-10
        if (cellV < 33) cellV = 33;
        cellV -= 33;  // Should leave us a number of 0-9
        if (cellV > 9) cellV = 9;
        OLED::drawBattery(cellV + 1);
    } else
        OLED::drawSymbol(15);  // Draw the DC Logo
#else
    // On TS80 we replace this symbol with the voltage we are operating on
    // If <9V then show single digit, if not show duals
    uint8_t V = getInputVoltageX10(systemSettings.voltageDiv, 0);
    if (V % 10 >= 5)
        V = V / 10 + 1;  // round up
    else
        V = V / 10;
    if (V >= 10) {
        int16_t xPos = OLED::getCursorX();
        OLED::setFont(1);
        OLED::printNumber(1, 1);
        OLED::setCursor(xPos, 8);
        OLED::printNumber(V % 10, 1);
        OLED::setFont(0);
        OLED::setCursor(xPos + 12, 0);  // need to reset this as if we drew a wide char
    } else {
        OLED::printNumber(V, 1);
    }
#endif
}
static void gui_solderingTempAdjust() {
    uint32_t lastChange = xTaskGetTickCount();
    currentTempTargetDegC = 0;
    uint32_t autoRepeatTimer = 0;
    uint8_t autoRepeatAcceleration = 0;
    for (;;) {
        OLED::setCursor(0, 0);
        OLED::clearScreen();
        OLED::setFont(0);
        ButtonState buttons = getButtonState();
        if (buttons) lastChange = xTaskGetTickCount();
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
                    systemSettings.SolderingTemp -= systemSettings.TempChangeShortStep;  // add 10
                } else
                    systemSettings.SolderingTemp += systemSettings.TempChangeShortStep;  // add 10
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
            if (systemSettings.SolderingTemp > 850) systemSettings.SolderingTemp = 850;
            if (systemSettings.SolderingTemp < 60) systemSettings.SolderingTemp = 60;
        } else
#endif
        {
            if (systemSettings.SolderingTemp > 450) systemSettings.SolderingTemp = 450;
            if (systemSettings.SolderingTemp < 10) systemSettings.SolderingTemp = 10;
        }

        if (xTaskGetTickCount() - lastChange > 200) return;  // exit if user just doesn't press anything for a bit

#ifdef MODEL_TS80
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
#ifdef MODEL_TS80
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

static int gui_SolderingSleepingMode(bool stayOff) {
    // Drop to sleep temperature and display until movement or button press

    for (;;) {
        ButtonState buttons = getButtonState();
        if (buttons) return 0;
        if ((xTaskGetTickCount() > 100) && ((accelInit && (xTaskGetTickCount() - lastMovementTime < 100)) || (xTaskGetTickCount() - lastButtonTime < 100))) return 0;  // user moved or pressed a button, go back to soldering
#ifdef MODEL_TS100
        if (checkVoltageForExit()) return 1;  // return non-zero on error
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
        if (systemSettings.ShutdownTime)  // only allow shutdown exit if time > 0
            if (lastMovementTime)
                if (((uint32_t)(xTaskGetTickCount() - lastMovementTime)) > (uint32_t)(systemSettings.ShutdownTime * 60 * 100)) {
                    // shutdown
                    currentTempTargetDegC = 0;
                    return 1;  // we want to exit soldering mode
                }
        OLED::refresh();
        GUIDelay();
    }
    return 0;
}

static void display_countdown(int sleepThres) {
    /*
     * Print seconds or minutes (if > 99 seconds) until sleep
     * mode is triggered.
     */
    int lastEventTime = lastButtonTime < lastMovementTime ? lastMovementTime : lastButtonTime;
    int downCount = sleepThres - xTaskGetTickCount() + lastEventTime;
    if (downCount > 9900) {
        OLED::printNumber(downCount / 6000 + 1, 2);
        OLED::print(SymbolMinutes);
    } else {
        OLED::printNumber(downCount / 100 + 1, 2);
        OLED::print(SymbolSeconds);
    }
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
     */
    bool boostModeOn = false;

    uint32_t sleepThres = 0;
    if (systemSettings.SleepTime < 6)
        sleepThres = systemSettings.SleepTime * 10 * 100;
    else
        sleepThres = (systemSettings.SleepTime - 5) * 60 * 100;
    if (jumpToSleep) {
        if (gui_SolderingSleepingMode(jumpToSleep == 2)) {
            lastButtonTime = xTaskGetTickCount();
            return;  // If the function returns non-0 then exit
        }
    }
    for (;;) {
        ButtonState buttons = getButtonState();
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
                return;  // exit on back long hold
                break;
            case BUTTON_F_LONG:
                // if boost mode is enabled turn it on
                if (systemSettings.boostModeEnabled) boostModeOn = true;
                break;
            case BUTTON_F_SHORT:
            case BUTTON_B_SHORT: {
                uint16_t oldTemp = systemSettings.SolderingTemp;
                gui_solderingTempAdjust();  // goto adjust temp mode
                if (oldTemp != systemSettings.SolderingTemp) {
                    saveSettings();  // only save on change
                }
            } break;
            default:
                break;
        }
        // else we update the screen information
        OLED::setCursor(0, 0);
        OLED::clearScreen();
        OLED::setFont(0);
        // Draw in the screen details
        if (systemSettings.detailedSoldering) {
            OLED::setFont(1);
            OLED::print(SolderingAdvancedPowerPrompt);  // Power:
            OLED::printNumber(x10WattHistory.average() / 10, 2);
            OLED::print(SymbolDot);
            OLED::printNumber(x10WattHistory.average() % 10, 1);
            OLED::print(SymbolWatts);

            if (systemSettings.sensitivity && systemSettings.SleepTime) {
                OLED::print(SymbolSpace);
                display_countdown(sleepThres);
            }

            OLED::setCursor(0, 8);
            OLED::print(SleepingTipAdvancedString);
            // OLED::printNumber(
            //		TipThermoModel::convertTipRawADCTouV(getTipRawTemp(0)), 5); // Draw the tip temp out finally

            gui_drawTipTemp(true);
            OLED::print(SymbolSpace);
            printVoltage();
            OLED::print(SymbolVolts);
        } else {
            // We switch the layout direction depending on the orientation of the
            // OLED::
            if (OLED::getRotation()) {
                // battery
                gui_drawBatteryIcon();
                OLED::print(SymbolSpace);  // Space out gap between battery <-> temp
                gui_drawTipTemp(true);     // Draw current tip temp

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
                gui_drawTipTemp(true);  // Draw current tip temp

                OLED::print(SymbolSpace);  // Space out gap between battery <-> temp

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

#ifdef MODEL_TS100
        // Undervoltage test
        if (checkVoltageForExit()) {
            lastButtonTime = xTaskGetTickCount();
            return;
        }
#else
        // on the TS80 we only want to check for over voltage to prevent tip damage
        /*if (getInputVoltageX10(systemSettings.voltageDiv, 1) > 150) {
         lastButtonTime = xTaskGetTickCount();
         currentlyActiveTemperatureTarget = 0;
         return;  // Over voltage
         }*/
#endif

        if (systemSettings.sensitivity && systemSettings.SleepTime)
            if (xTaskGetTickCount() - lastMovementTime > sleepThres && xTaskGetTickCount() - lastButtonTime > sleepThres) {
                if (gui_SolderingSleepingMode(false)) {
                    return;  // If the function returns non-0 then exit
                }
            }
        // slow down ui update rate
        GUIDelay();
    }
}

void showDebugMenu(void) {
    uint8_t screen = 0;
    ButtonState b;
    for (;;) {
        OLED::clearScreen();               // Ensure the buffer starts clean
        OLED::setCursor(0, 0);             // Position the cursor at the 0,0 (top left)
        OLED::setFont(1);                  // small font
        OLED::print(SymbolVersionNumber);  // Print version number
        OLED::setCursor(0, 8);             // second line
        OLED::print(DebugMenu[screen]);
        switch (screen) {
            case 0:  // Just prints date
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
                    uint32_t temp = systemSettings.CalibrationOffset;
                    systemSettings.CalibrationOffset = 0;
                    OLED::printNumber(TipThermoModel::convertTipRawADCTouV(getTipRawTemp(1)), 6);
                    systemSettings.CalibrationOffset = temp;
                }
                break;
            case 7:
                // Temp in C
                OLED::printNumber(TipThermoModel::getTipInC(1), 5);
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
                OLED::printNumber(PCBVersion, 1);
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
            screen = screen % 11;
        }
        GUIDelay();
    }
}

/* StartGUITask function */
void startGUITask(void const *argument __unused) {

    uint8_t tempWarningState = 0;
    bool buttonLockout = false;
    bool tempOnDisplay = false;
    getTipRawTemp(1);  // reset filter
    OLED::setRotation(systemSettings.OrientationMode & 1);
    uint32_t ticks = xTaskGetTickCount();
    ticks += 400;  // 4 seconds from now
    while (xTaskGetTickCount() < ticks) {
        if (showBootLogoIfavailable() == false) ticks = xTaskGetTickCount();
        ButtonState buttons = getButtonState();
        if (buttons) ticks = xTaskGetTickCount();  // make timeout now so we will exit
        GUIDelay();
    }

    if (settingsWereReset) {
        // Display alert settings were reset
        OLED::clearScreen();
        OLED::setFont(1);
        OLED::setCursor(0, 0);
        OLED::print(SettingsResetMessage);
        OLED::refresh();
        waitForButtonPressOrTimeout(1000);
    }

    if (systemSettings.autoStartMode) {
        // jump directly to the autostart mode
        if (systemSettings.autoStartMode == 1) {
            gui_solderingMode(0);
            buttonLockout = true;
        } else if (systemSettings.autoStartMode == 2) {
            gui_solderingMode(1);
            buttonLockout = true;
        } else if (systemSettings.autoStartMode == 3) {
            gui_solderingMode(2);
            buttonLockout = true;
        }
    }

    for (;;) {
        ButtonState buttons = getButtonState();
        if (buttons != BUTTON_NONE) {
            OLED::setDisplayState(OLED::DisplayState::ON);
            OLED::setFont(0);
        }
        if (tempWarningState == 2) buttons = BUTTON_F_SHORT;
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
                gui_solderingMode(0);  // enter soldering mode
                buttonLockout = true;
                break;
            case BUTTON_B_SHORT:
                enterSettingsMenu();  // enter the settings menu
                buttonLockout = true;
                break;
            default:
                break;
        }

        currentTempTargetDegC = 0;  // ensure tip is off
        getInputVoltageX10(systemSettings.voltageDiv, 0);
        uint16_t tipTemp = TipThermoModel::getTipInC();

        // Preemptively turn the display on.  Turn it off if and only if
        // the tip temperature is below 50 degrees C *and* motion sleep
        // detection is enabled *and* there has been no activity (movement or
        // button presses) in a while.
        OLED::setDisplayState(OLED::DisplayState::ON);

        if ((tipTemp < 50) && systemSettings.sensitivity && (((xTaskGetTickCount() - lastMovementTime) > MOVEMENT_INACTIVITY_TIME) && ((xTaskGetTickCount() - lastButtonTime) > BUTTON_INACTIVITY_TIME))) {
            OLED::setDisplayState(OLED::DisplayState::OFF);
        }

        // Clear the lcd buffer
        OLED::clearScreen();
        OLED::setCursor(0, 0);
        if (systemSettings.detailedIDLE) {
            OLED::setFont(1);
            if (tipTemp > 470) {
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
#ifdef MODEL_TS80
            if (!OLED::getRotation()) {
#else
            if (OLED::getRotation()) {
#endif
                OLED::drawArea(12, 0, 84, 16, idleScreenBG);
                OLED::setCursor(0, 0);
                gui_drawBatteryIcon();
            } else {
                OLED::drawArea(0, 0, 84, 16, idleScreenBGF);  // Needs to be flipped so button ends up
                                                              // on right side of screen
                OLED::setCursor(84, 0);
                gui_drawBatteryIcon();
            }
            if (tipTemp > 55)
                tempOnDisplay = true;
            else if (tipTemp < 45)
                tempOnDisplay = false;
            if (tempOnDisplay) {
                // draw temp over the start soldering button
                // Location changes on screen rotation
#ifdef MODEL_TS80
                if (!OLED::getRotation()) {
#else
                if (OLED::getRotation()) {
#endif
                    // in right handed mode we want to draw over the first part
                    OLED::fillArea(55, 0, 41, 16, 0);  // clear the area for the temp
                    OLED::setCursor(56, 0);

                } else {
                    OLED::fillArea(0, 0, 41, 16, 0);  // clear the area
                    OLED::setCursor(0, 0);
                }
                // draw in the temp
                if (!(systemSettings.coolingTempBlink && (xTaskGetTickCount() % 25 < 16))) gui_drawTipTemp(false);  // draw in the temp
            }
        }
        OLED::refresh();
        GUIDelay();
    }
}
