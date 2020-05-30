/*
 * Translation.h
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef TRANSLATION_H_
#define TRANSLATION_H_
#include "unit.h"
#include "stdint.h"
enum ShortNameType {
	SHORT_NAME_SINGLE_LINE = 1, SHORT_NAME_DOUBLE_LINE = 2,
};
extern const uint8_t USER_FONT_12[];
extern const uint8_t USER_FONT_6x8[];
/*
 * When SettingsShortNameType is SHORT_NAME_SINGLE_LINE
 * use SettingsShortNames as SettingsShortNames[16][1].. second column undefined
 */
extern const enum ShortNameType SettingsShortNameType;
extern const char *SettingsShortNames[28][2];
extern const char *SettingsDescriptions[28];
extern const char *SettingsMenuEntries[4];

extern const char *SettingsCalibrationDone;
extern const char *SettingsCalibrationWarning;
extern const char *SettingsResetWarning;
extern const char *UVLOWarningString;
extern const char *UndervoltageString;
extern const char *InputVoltageString;
extern const char *WarningTipTempString;
extern const char *BadTipString;

extern const char *SleepingSimpleString;
extern const char *SleepingAdvancedString;
extern const char *WarningSimpleString;
extern const char *WarningAdvancedString;
extern const char *SleepingTipAdvancedString;
extern const char *IdleTipString;
extern const char *IdleSetString;
extern const char *TipDisconnectedString;
extern const char *SolderingAdvancedPowerPrompt;
extern const char *OffString;
extern const char *ResetOKMessage;
extern const char *YourGainMessage;
extern const char *SettingsResetMessage;

extern const char *SettingTrueChar;
extern const char *SettingFalseChar;
extern const char *SettingRightChar;
extern const char *SettingLeftChar;
extern const char *SettingAutoChar;
extern const char *SettingStartSolderingChar;
extern const char *SettingStartSleepChar;
extern const char *SettingStartSleepOffChar;
extern const char *SettingStartNoneChar;

extern const char *SettingFastChar;
extern const char *SettingSlowChar;
extern const char *TipModelStrings[];
extern const char *DebugMenu[];
extern const char *SymbolPlus;
extern const char *SymbolMinus;
extern const char *SymbolSpace;
extern const char *SymbolDot;
extern const char *SymbolDegC;
#ifdef ENABLED_FAHRENHEIT_SUPPORT
	extern const char *SymbolDegF;
#endif
extern const char *SymbolMinutes;
extern const char *SymbolSeconds;
extern const char *SymbolWatts;
extern const char *SymbolVolts;
extern const char *SymbolDC;
extern const char *SymbolCellCount;
extern const char *SymbolVersionNumber;

extern const char *DebugMenu[];
#endif /* TRANSLATION_H_ */
