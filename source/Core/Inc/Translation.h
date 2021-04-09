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

extern const char TranslationStrings[];

extern const uint16_t SettingsShortNames[];
extern const uint16_t SettingsDescriptions[];
extern const uint16_t SettingsMenuEntries[];

extern const uint16_t SettingsCalibrationWarning;
extern const uint16_t SettingsResetWarning;
extern const uint16_t UVLOWarningString;
extern const uint16_t UndervoltageString;
extern const uint16_t InputVoltageString;

extern const uint16_t SleepingSimpleString;
extern const uint16_t SleepingAdvancedString;
extern const uint16_t SleepingTipAdvancedString;
extern const uint16_t IdleTipString;
extern const uint16_t IdleSetString;
extern const uint16_t TipDisconnectedString;
extern const uint16_t SolderingAdvancedPowerPrompt;
extern const uint16_t OffString;

extern const uint16_t ResetOKMessage;
extern const uint16_t SettingsResetMessage;
extern const uint16_t NoAccelerometerMessage;
extern const uint16_t NoPowerDeliveryMessage;
extern const uint16_t LockingKeysString;
extern const uint16_t UnlockingKeysString;
extern const uint16_t WarningKeysLockedString;

extern const uint16_t SettingRightChar;
extern const uint16_t SettingLeftChar;
extern const uint16_t SettingAutoChar;
extern const uint16_t SettingStartSolderingChar;
extern const uint16_t SettingStartSleepChar;
extern const uint16_t SettingStartSleepOffChar;
extern const uint16_t SettingStartNoneChar;
extern const uint16_t SettingSensitivityOff;
extern const uint16_t SettingSensitivityLow;
extern const uint16_t SettingSensitivityMedium;
extern const uint16_t SettingSensitivityHigh;
extern const uint16_t SettingLockDisableChar;
extern const uint16_t SettingLockBoostChar;
extern const uint16_t SettingLockFullChar;
extern const uint16_t SettingNAChar;

extern const uint16_t SettingOffChar;
extern const uint16_t SettingFastChar;
extern const uint16_t SettingMediumChar;
extern const uint16_t SettingSlowChar;

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
#define SETTINGS_DESC(i) (settings_item_index(i) + 1)

const char *translatedString(uint16_t index);

#endif /* TRANSLATION_H_ */
