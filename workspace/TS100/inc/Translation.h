/*
 * Translation.h
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef TRANSLATION_H_
#define TRANSLATION_H_

enum ShortNameType {
  SHORT_NAME_SINGLE_LINE = 1,
  SHORT_NAME_DOUBLE_LINE = 2,
};

/*
 * When SettingsShortNameType is SHORT_NAME_SINGLE_LINE
 * use SettingsShortNames as SettingsShortNames[16][1].. second column undefined
 */
extern const enum ShortNameType SettingsShortNameType;
extern const char* SettingsShortNames[16][2];

extern const char* SettingsLongNames[16];
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
//#define LANG_RU
//#define LANG_ES
//#define LANG_SE
//#define LANG_IT
//#define LANG_FR
//#define LANG_DE

#endif /* TRANSLATION_H_ */
