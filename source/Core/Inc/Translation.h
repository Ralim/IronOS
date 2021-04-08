/*
 * Translation.h
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef TRANSLATION_H_
#define TRANSLATION_H_
#include "stdint.h"
extern const uint8_t     USER_FONT_12[];
extern const uint8_t     USER_FONT_6x8[];
extern const bool        HasFahrenheit;
extern const char *const DisplayLanguageNames[];
extern const char        InternalLanguageCodes[][8];
extern const uint8_t     LanguageCount;

extern const char *const SettingsShortNames[];
extern const char *const SettingsDescriptions[];
extern const char *const SettingsMenuEntries[];

extern const char *const SettingsCalibrationDone[];
extern const char *const SettingsCalibrationWarning[];
extern const char *const SettingsResetWarning[];
extern const char *const UVLOWarningString[];
extern const char *const UndervoltageString[];
extern const char *const InputVoltageString[];
extern const char *const WarningTipTempString[];
extern const char *const BadTipString[];

extern const char *const SleepingSimpleString[];
extern const char *const SleepingAdvancedString[];
extern const char *const WarningSimpleString[];
extern const char *const WarningAdvancedString[];
extern const char *const SleepingTipAdvancedString[];
extern const char *const IdleTipString[];
extern const char *const IdleSetString[];
extern const char *const TipDisconnectedString[];
extern const char *const SolderingAdvancedPowerPrompt[];
extern const char *const OffString[];
extern const char *const YourGainMessage[];

extern const char *const ResetOKMessage[];
extern const char *const SettingsResetMessage[];
extern const char *const NoAccelerometerMessage[];
extern const char *const NoPowerDeliveryMessage[];
extern const char *const LockingKeysString[];
extern const char *const UnlockingKeysString[];
extern const char *const WarningKeysLockedString[];

extern const char *const SettingRightChar[];
extern const char *const SettingLeftChar[];
extern const char *const SettingAutoChar[];
extern const char *const SettingStartSolderingChar[];
extern const char *const SettingStartSleepChar[];
extern const char *const SettingStartSleepOffChar[];
extern const char *const SettingStartNoneChar[];
extern const char *const SettingSensitivityOff[];
extern const char *const SettingSensitivityLow[];
extern const char *const SettingSensitivityMedium[];
extern const char *const SettingSensitivityHigh[];
extern const char *const SettingLockDisableChar[];
extern const char *const SettingLockBoostChar[];
extern const char *const SettingLockFullChar[];
extern const char *const SettingNAChar[];

extern const char *const SettingOffChar[];
extern const char *const SettingFastChar[];
extern const char *const SettingMediumChar[];
extern const char *const SettingSlowChar[];
extern const char *const TipModelStrings[];
extern const char *const DebugMenu[];
extern const char *      SymbolPlus;
extern const char *      SymbolMinus;
extern const char *      SymbolSpace;
extern const char *      SymbolDot;
extern const char *      SymbolDegC;
extern const char *      SymbolDegF;
extern const char *      SymbolMinutes;
extern const char *      SymbolSeconds;
extern const char *      SymbolWatts;
extern const char *      SymbolVolts;
extern const char *      SymbolDC;
extern const char *      SymbolCellCount;
extern const char *      SymbolVersionNumber;

extern const char *const DebugMenu[];

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
  LanguageSwitch,
};

constexpr uint8_t settings_item_index(const SettingsItemIndex i) { return static_cast<uint8_t>(i); }
// Use a constexpr function for type-checking.
#define SETTINGS_DESC(i) (settings_item_index(i) + 1)

const char *translatedString(const char *const *strs);
bool        settings_displayLanguageSwitch(void);
bool        settings_setLanguageSwitch(void);

#endif /* TRANSLATION_H_ */
