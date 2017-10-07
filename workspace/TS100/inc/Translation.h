/*
 * Translation.h
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef TRANSLATION_H_
#define TRANSLATION_H_

extern const char* SettingsLongNames[15];
extern const char* SettingsShortNames[15];
extern const char* SettingsCalibrationWarning;
extern const char* UVLOWarningString;
extern const char* SleepingSimpleString;
extern const char* SleepingAdvancedString;
extern const char* WarningSimpleString;
extern const char* WarningAdvancedString;

extern const char SettingTrueChar;
extern const char SettingFalseChar;
extern const char SettingRightChar;
extern const char SettingLeftChar;
extern const char SettingAutoChar;

#define LANG_EN
#define LANG

#ifndef LANG
#define LANG_EN
#define LANG
#endif

#ifndef LANG
#error NO LANGUAGE DEFINED
#endif


#endif /* TRANSLATION_H_ */
