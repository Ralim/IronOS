/*
 * Strings.h
 *
 *  Created on: 4Aug.,2017
 *      Author: Ben V. Brown
 */

/*
 * This file is used to allow for different language builds of the firmware
 * This file houses all the strings for the text used on the iron.
 */
#ifndef STRINGS_H_
#define STRINGS_H_

extern const char* SettingsLongNames[14];
extern const char* SettingsShortNames[14];
extern const char* TempCalStatus[3]; //All fixed 8 chars
extern const char* UVLOWarningString; //Fixed width 8 chars
extern const char* CoolingPromptString; //Fixed width 5 chars
extern const char SettingTrueChar;
extern const char SettingFalseChar;
extern const char SettingSleepChar;
extern const char SettingFastChar;
extern const char SettingMediumChar;
extern const char SettingSlowChar;
extern const char SettingRightChar;
extern const char SettingLeftChar;
extern const char SettingAutoChar;
extern const char SettingTempCChar;
extern const char SettingTempFChar;
#endif /* STRINGS_H_ */
