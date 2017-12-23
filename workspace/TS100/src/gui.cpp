/*
 * gui.cpp
 *
 *  Created on: 3Sep.,2017
 *      Author: Ben V. Brown
 */

#include "gui.h"
#include "cmsis_os.h"
#include "hardware.h"
#include "string.h"

static void settings_setInputVRange(void);
static void settings_displayInputVRange(void);
static void settings_setSleepTemp(void);
static void settings_displaySleepTemp(void);
static void settings_setSleepTime(void);
static void settings_displaySleepTime(void);
static void settings_setShutdownTime(void);
static void settings_displayShutdownTime(void);
static void settings_setSensitivity(void);
static void settings_displaySensitivity(void);
static void settings_setTempF(void);
static void settings_displayTempF(void);
static void settings_setAdvancedSolderingScreens(void);
static void settings_displayAdvancedSolderingScreens(void);
static void settings_setAdvancedIDLEScreens(void);
static void settings_displayAdvancedIDLEScreens(void);

static void settings_setDisplayRotation(void);
static void settings_displayDisplayRotation(void);
static void settings_setBoostModeEnabled(void);
static void settings_displayBoostModeEnabled(void);
static void settings_setBoostTemp(void);
static void settings_displayBoostTemp(void);
static void settings_setAutomaticStartMode(void);
static void settings_displayAutomaticStartMode(void);
static void settings_setCoolingBlinkEnabled(void);
static void settings_displayCoolingBlinkEnabled(void);
static void settings_setResetSettings(void);
static void settings_displayResetSettings(void);
static void settings_setCalibrate(void);
static void settings_displayCalibrate(void);
static void settings_setCalibrateVIN(void);
static void settings_displayCalibrateVIN(void);

const menuitem settingsMenu[] = {
    /*Struct used for all settings options in the settings menu*/
    {(const char*)SettingsLongNames[0],
     {settings_setInputVRange},
     {settings_displayInputVRange}}, /*Voltage input*/
    {(const char*)SettingsLongNames[1],
     {settings_setSleepTemp},
     {settings_displaySleepTemp}}, /*Sleep Temp*/
    {(const char*)SettingsLongNames[2],
     {settings_setSleepTime},
     {settings_displaySleepTime}}, /*Sleep Time*/
    {(const char*)SettingsLongNames[3],
     {settings_setShutdownTime},
     {settings_displayShutdownTime}}, /*Shutdown Time*/
    {(const char*)SettingsLongNames[4],
     {settings_setSensitivity},
     {settings_displaySensitivity}}, /* Motion Sensitivity*/
    {(const char*)SettingsLongNames[5],
     {settings_setTempF},
     {settings_displayTempF}}, /* Motion Sensitivity*/
    {(const char*)SettingsLongNames[6],
     {settings_setAdvancedIDLEScreens},
     {settings_displayAdvancedIDLEScreens}}, /* Advanced screens*/
    {(const char*)SettingsLongNames[15],
     {settings_setAdvancedSolderingScreens},
     {settings_displayAdvancedSolderingScreens}}, /* Advanced screens*/
    {(const char*)SettingsLongNames[7],
     {settings_setDisplayRotation},
     {settings_displayDisplayRotation}}, /*Display Rotation*/
    {(const char*)SettingsLongNames[8],
     {settings_setBoostModeEnabled},
     {settings_displayBoostModeEnabled}}, /*Enable Boost*/
    {(const char*)SettingsLongNames[9],
     {settings_setBoostTemp},
     {settings_displayBoostTemp}}, /*Boost Temp*/
    {(const char*)SettingsLongNames[10],
     {settings_setAutomaticStartMode},
     {settings_displayAutomaticStartMode}}, /*Auto start*/
    {(const char*)SettingsLongNames[11],
     {settings_setCoolingBlinkEnabled},
     {settings_displayCoolingBlinkEnabled}}, /*Cooling blink warning*/
    {(const char*)SettingsLongNames[12],
     {settings_setCalibrate},
     {settings_displayCalibrate}}, /*Calibrate tip*/
    {(const char*)SettingsLongNames[13],
     {settings_setResetSettings},
     {settings_displayResetSettings}}, /*Resets settings*/
    {(const char*)SettingsLongNames[14],
     {settings_setCalibrateVIN},
     {settings_displayCalibrateVIN}}, /*Voltage input cal*/
    {NULL, {NULL}, {NULL}}            // end of menu marker. DO NOT REMOVE
};

static void printShortDescriptionSingleLine(uint32_t shortDescIndex) {
  lcd.setFont(0);
  lcd.setCharCursor(0, 0);
  lcd.print(SettingsShortNames[shortDescIndex][0]);
}

static void printShortDescriptionDoubleLine(uint32_t shortDescIndex) {
  lcd.setFont(1);
  lcd.setCharCursor(0, 0);
  lcd.print(SettingsShortNames[shortDescIndex][0]);
  lcd.setCharCursor(0, 1);
  lcd.print(SettingsShortNames[shortDescIndex][1]);
}

/**
 * Prints two small lines of short description
 * and prepares cursor in big font after it.
 * @param shortDescIndex Index to of short description.
 * @param cursorCharPosition Custom cursor char position to set after printing
 * description.
 */
static void printShortDescription(uint32_t shortDescIndex,
                                  uint16_t cursorCharPosition) {
  // print short description (default single line, explicit double line)
  if (SettingsShortNameType == SHORT_NAME_DOUBLE_LINE) {
    printShortDescriptionDoubleLine(shortDescIndex);
  } else {
    printShortDescriptionSingleLine(shortDescIndex);
  }

  // prepare cursor for value
  lcd.setFont(0);
  lcd.setCharCursor(cursorCharPosition, 0);
}

static int userConfirmation(const char* message) {
  uint8_t maxOffset = strlen(message) + 7;
  uint32_t messageStart = HAL_GetTick();

  lcd.setFont(0);
  lcd.setCursor(0, 0);

  for (;;) {
    int16_t messageOffset = (((HAL_GetTick() - messageStart) / 150) % maxOffset);

    lcd.clearScreen();
    lcd.setCursor(12 * (7 - messageOffset), 0);
    lcd.print(message);

    ButtonState buttons = getButtonState();
    switch (buttons) {
      case BUTTON_F_SHORT:
        //User confirmed
        return 1;

      case BUTTON_BOTH:
      case BUTTON_B_SHORT:
      case BUTTON_F_LONG:
      case BUTTON_B_LONG:
        return 0;

      case BUTTON_NONE:
        break;
    }

    lcd.refresh();
    osDelay(50);
  }
}


static void settings_setInputVRange(void) {
  systemSettings.cutoutSetting = (systemSettings.cutoutSetting + 1) % 5;
}

static void settings_displayInputVRange(void) {
  printShortDescription(0, 6);

  if (systemSettings.cutoutSetting) {
    lcd.drawChar('0' + 2 + systemSettings.cutoutSetting);
    lcd.drawChar('S');
  } else {
    lcd.print("DC");
  }
}


static void settings_setSleepTemp(void) {
  systemSettings.SleepTemp += 10;
  if (systemSettings.SleepTemp > 300) systemSettings.SleepTemp = 50;
}

static void settings_displaySleepTemp(void) {
  printShortDescription(1, 5);
  lcd.printNumber(systemSettings.SleepTemp, 3);
}


static void settings_setSleepTime(void) {
  systemSettings.SleepTime++;  // Go up 1 minute at a time
  if (systemSettings.SleepTime >= 16) {
    systemSettings.SleepTime = 1;  // can't set time over 10 mins
  }
  // Remember that ^ is the time of no movement
}

static void settings_displaySleepTime(void) {
  printShortDescription(2, 5);

  if (systemSettings.SleepTime < 6) {
    lcd.printNumber(systemSettings.SleepTime * 10, 2);
    lcd.drawChar('S');
  } else {
    lcd.printNumber(systemSettings.SleepTime - 5, 2);
    lcd.drawChar('M');
  }
}


static void settings_setShutdownTime(void) {
  systemSettings.ShutdownTime++;
  if (systemSettings.ShutdownTime > 60) {
    systemSettings.ShutdownTime = 0;  // wrap to off
  }
}

static void settings_displayShutdownTime(void) {
  printShortDescription(3, 6);
  lcd.printNumber(systemSettings.ShutdownTime, 2);
}


static void settings_setTempF(void) {
  systemSettings.temperatureInF = !systemSettings.temperatureInF;
}

static void settings_displayTempF(void) {
  printShortDescription(5, 7);

  lcd.drawChar((systemSettings.temperatureInF) ? 'F' : 'C');
}


static void settings_setSensitivity(void) {
  systemSettings.sensitivity++;
  systemSettings.sensitivity = systemSettings.sensitivity % 10;
}

static void settings_displaySensitivity(void) {
  printShortDescription(4, 7);
  lcd.printNumber(systemSettings.sensitivity, 1);
}


static void settings_setAdvancedSolderingScreens(void) {
  systemSettings.detailedSoldering = !systemSettings.detailedSoldering;
}

static void settings_displayAdvancedSolderingScreens(void) {
  printShortDescription(15, 7);

  lcd.drawSymbol((systemSettings.detailedSoldering) ? 17 : 18);
}


static void settings_setAdvancedIDLEScreens(void) {
  systemSettings.detailedIDLE = !systemSettings.detailedIDLE;
}

static void settings_displayAdvancedIDLEScreens(void) {
  printShortDescription(6, 7);

  lcd.drawSymbol((systemSettings.detailedIDLE) ? 17 : 18);
}


static void settings_setDisplayRotation(void) {
  systemSettings.OrientationMode++;
  systemSettings.OrientationMode = systemSettings.OrientationMode % 3;
}

static void settings_displayDisplayRotation(void) {
  printShortDescription(7, 7);

  switch (systemSettings.OrientationMode) {
    case 0:
      lcd.drawChar(SettingRightChar);
      break;
    case 1:
      lcd.drawChar(SettingLeftChar);
      break;
    case 2:
      lcd.drawChar(SettingAutoChar);
      break;
    default:
      lcd.drawChar(SettingRightChar);
      break;
  }
}


static void settings_setBoostModeEnabled(void) {
  systemSettings.boostModeEnabled = !systemSettings.boostModeEnabled;
}

static void settings_displayBoostModeEnabled(void) {
  printShortDescription(8, 7);

  lcd.drawSymbol((systemSettings.boostModeEnabled) ? 17 : 18);
}


static void settings_setBoostTemp(void) {
  systemSettings.BoostTemp += 10;  // Go up 10 at a time
  if (systemSettings.temperatureInF) {
    if (systemSettings.BoostTemp > 850) {
      systemSettings.BoostTemp = 480;  // loop back at 250
    }
  } else {
    if (systemSettings.BoostTemp > 450) {
      systemSettings.BoostTemp = 250;  // loop back at 250
    }
  }
}

static void settings_displayBoostTemp(void) {
  printShortDescription(9, 5);
  lcd.printNumber(systemSettings.BoostTemp, 3);
}


static void settings_setAutomaticStartMode(void) {
  systemSettings.autoStartMode++;
  systemSettings.autoStartMode %= 2;
}

static void settings_displayAutomaticStartMode(void) {
  printShortDescription(10, 7);

  lcd.drawSymbol((systemSettings.autoStartMode) ? 17 : 18);
}


static void settings_setCoolingBlinkEnabled(void) {
  systemSettings.coolingTempBlink = !systemSettings.coolingTempBlink;
}

static void settings_displayCoolingBlinkEnabled(void) {
  printShortDescription(11, 7);

  lcd.drawSymbol((systemSettings.coolingTempBlink) ? 17 : 18);
}


static void settings_setResetSettings(void) {
  if(userConfirmation(SettingsResetWarning)) {
    resetSettings();

    lcd.setFont(0);
    lcd.setCursor(0, 0);
    lcd.print("RESET OK");
    lcd.refresh();

    waitForButtonPressOrTimeout(2000);
  }
}

static void settings_displayResetSettings(void) {
  printShortDescription(13, 7);
}


static void settings_setCalibrate(void) {
  if(userConfirmation(SettingsCalibrationWarning)) {
    //User confirmed
    //So we now perform the actual calculation
    lcd.clearScreen();
    lcd.setCursor(0, 0);
    lcd.print(".....");
    lcd.refresh();

    setCalibrationOffset(0);            //turn off the current offset
    for (uint8_t i = 0; i < 20; i++) {
      getTipRawTemp(1);                 //cycle through the filter a fair bit to ensure we're stable.
      osDelay(20);
    }
    osDelay(100);

    uint16_t rawTempC = tipMeasurementToC(getTipRawTemp(0));
    //We now measure the current reported tip temperature
    uint16_t handleTempC = getHandleTemperature() / 10;
    //We now have an error between these that we want to store as the offset
    rawTempC = rawTempC - handleTempC;
    systemSettings.CalibrationOffset = rawTempC;
    setCalibrationOffset(rawTempC);       //store the error
    osDelay(100);
  }
}

static void settings_displayCalibrate(void) {
  printShortDescription(12, 5);
}


static void settings_setCalibrateVIN(void) {
  // Jump to the voltage calibration subscreen
  lcd.setFont(0);
  lcd.clearScreen();
  lcd.setCursor(0, 0);

  for (;;) {
    lcd.setCursor(0, 0);
    lcd.printNumber(getInputVoltageX10(systemSettings.voltageDiv) / 10, 2);
    lcd.print(".");
    lcd.printNumber(getInputVoltageX10(systemSettings.voltageDiv) % 10, 1);
    lcd.print("V");

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
        return;

      case BUTTON_NONE:
      default:
        break;
    }

    lcd.refresh();
    osDelay(50);

    // Cap to sensible values
    if (systemSettings.voltageDiv < 90) {
      systemSettings.voltageDiv = 90;
    } else if (systemSettings.voltageDiv > 130) {
      systemSettings.voltageDiv = 130;
    }
  }
}

static void settings_displayCalibrateVIN(void) {
  printShortDescription(14, 5);
}
