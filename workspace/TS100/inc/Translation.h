/*
 * Translation.h
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef TRANSLATION_H_
#define TRANSLATION_H_

enum ShortNameType {
	SHORT_NAME_SINGLE_LINE = 1, SHORT_NAME_DOUBLE_LINE = 2,
};

/*
 * When SettingsShortNameType is SHORT_NAME_SINGLE_LINE
 * use SettingsShortNames as SettingsShortNames[16][1].. second column undefined
 */
extern const enum ShortNameType SettingsShortNameType;
extern const char* SettingsShortNames[][2];
extern const char* SettingsDescriptions[];
extern const char* SettingsMenuEntries[4];
extern const char* SettingsMenuEntriesDescriptions[4];

extern const char* SettingsCalibrationWarning;
extern const char* SettingsResetWarning;
extern const char* UVLOWarningString;
extern const char* UndervoltageString;
extern const char* InputVoltageString;
extern const char* WarningTipTempString;
extern const char* BadTipString;

extern const char* SleepingSimpleString;
extern const char* SleepingAdvancedString;
extern const char* WarningSimpleString;
extern const char* WarningAdvancedString;
extern const char* SleepingTipAdvancedString;
extern const char* IdleTipString;
extern const char* IdleSetString;
extern const char* TipDisconnectedString;
extern const char* SolderingAdvancedPowerPrompt;
extern const char* OffString;

extern const char SettingTrueChar;
extern const char SettingFalseChar;
extern const char SettingRightChar;
extern const char SettingLeftChar;
extern const char SettingAutoChar;

extern const char SettingFastChar;
extern const char SettingSlowChar;



#endif /* TRANSLATION_H_ */
