/*
 * Translation.c
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */
#include "Translation.h"

#ifdef LANG_EN
const char* SettingsLongNames[16] = {
/*These are all the help text for all the settings.*/
/*No requirements on spacing or length*/
"Power source. Sets cutoff voltage. <DC 10V> <S 3.3V per cell>",    //Power Source
		"Sleep Temperature <C>",    //Sleep Temp
		"Sleep Timeout <Minutes>",    //Sleep Timeout
		"Shutdown Timeout <Minutes>",    //Shutdown Time
		"Motion Sensitivity <0.Off 1.least sensitive 9.most sensitive>",    //Motion Sensitivity
		"Temperature Unit <C=Celsius F=Fahrenheit>",    //Temp Unit
		"Display detailed information in a smaller font on the idle screen.",    //Detailed Information
		"Display Orientation <A. Automatic L. Left Handed R. Right Handed>",    //Orientation
		"Enable front key enters boost mode 450C mode when soldering",    //Boost enable
		"Temperature when in \"boost\" mode",    //Boost Temp
		"Automatically starts the iron into soldering on power up. T=Soldering, S= Sleep mode,F=Off",    //Auto start
		"Blink the temperature on the cooling screen while the tip is still hot.",    //Cooling Blink
		"Calibrate tip offset.",    //Calibrate Tip
		"Reset all settings",    //Reset Settings
		"VIN Calibration. Buttons adjust, long press to exit",    //VIN Cal
		"Display detailed information while soldering",//ADV SLD
		};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";    // Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";    // <=17 chars
const char* WarningSimpleString = "HOT!";    //Must be <= 4 chars
const char* WarningAdvancedString = "WARNING! TIP HOT!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
#endif

const char* SettingsShortNames[16] = { /**/
"PWRSC ",    // Power Source (DC or batt)
		"STMP ", 	// Sleep Temperature
		"STME ", 	// Sleep Timeout
		"SHTME ",    // Shutdown Temperature
		"MSENSE ",    // Motion sensitivity level
		"TMPUNT ",			//Temp in F and C
		"ADVIDL ",    // Advanced idle display mode enable
		"DSPROT ",    // Display rotation mode
		"BOOST  ",    // Boost enabled
		"BTMP ", 	// Boost temperature
		"ASTART ",    // Automatic Start mode
		"CLBLNK ",    // Cooldown blink
		"TMP CAL?",    // Temperature calibration enter menu
		"RESET? ",    // Settings reset command
		"CAL VIN?",
		"ADVSLD ", //advanced soldering screens
		};
