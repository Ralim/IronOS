/*
 * Translation.h
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef TRANSLATION_H_
#define TRANSLATION_H_
#include "stdint.h"
extern const uint8_t USER_FONT_12[];
extern const uint8_t USER_FONT_6x8[];
extern const bool    HasFahrenheit;

extern const char *SettingsShortNames[];
extern const char *SettingsDescriptions[];
extern const char *SettingsMenuEntries[];

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
extern const char *NoAccelerometerMessage;
extern const char *NoPowerDeliveryMessage;
extern const char *LockingKeysString;
extern const char *UnlockingKeysString;
extern const char *WarningKeysLockedString;

extern const char *SettingRightChar;
extern const char *SettingLeftChar;
extern const char *SettingAutoChar;
extern const char *SettingStartSolderingChar;
extern const char *SettingStartSleepChar;
extern const char *SettingStartSleepOffChar;
extern const char *SettingStartNoneChar;
extern const char *SettingSensitivityOff;
extern const char *SettingSensitivityLow;
extern const char *SettingSensitivityMedium;
extern const char *SettingSensitivityHigh;
extern const char *SettingLockDisableChar;
extern const char *SettingLockBoostChar;
extern const char *SettingLockFullChar;
extern const char *SettingNAChar;

extern const char *SettingOffChar;
extern const char *SettingFastChar;
extern const char *SettingMediumChar;
extern const char *SettingSlowChar;
extern const char *TipModelStrings[];
extern const char *DebugMenu[];
extern const char *SymbolPlus;
extern const char *SymbolMinus;
extern const char *SymbolSpace;
extern const char *SymbolDot;
extern const char *SymbolDegC;
extern const char *SymbolDegF;
extern const char *SymbolMinutes;
extern const char *SymbolSeconds;
extern const char *SymbolWatts;
extern const char *SymbolVolts;
extern const char *SymbolDC;
extern const char *SymbolCellCount;
extern const char *SymbolVersionNumber;

extern const char *DebugMenu[];

enum class SettingsItemIndex : uint8_t {
  DCInCutoff,
  SleepTemperature,
  SleepTimeout,
  ShutdownTimeout,
  MotionSensitivity,
  TemperatureUnit,
  AdvancedIdle,
  DisplayRotation,
  BoostTemperature,
  AutoStart,
  CooldownBlink,
  TemperatureCalibration,
  SettingsReset,
  VoltageCalibration,
  AdvancedSoldering,
  ScrollingSpeed,
  QCMaxVoltage,
  PowerLimit,
  ReverseButtonTempChange,
  TempChangeShortStep,
  TempChangeLongStep,
  PowerPulsePower,
  HallEffSensitivity,
  LockingMode,
  MinVolCell,
  AnimLoop,
  AnimSpeed,
  PowerPulseWait,
  PowerPulseDuration,
};

constexpr uint8_t settings_item_index(const SettingsItemIndex i) { return static_cast<uint8_t>(i); }
// Use a constexpr function for type-checking.
#define SETTINGS_DESC(i) (SettingsDescriptions[settings_item_index(i)])

#endif /* TRANSLATION_H_ */
