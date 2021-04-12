/*
 * Translation.h
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef TRANSLATION_H_
#define TRANSLATION_H_
#include "stdint.h"

extern const bool HasFahrenheit;

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
  NUM_ITEMS,
};

struct TranslationIndexTable {
  uint16_t SettingsCalibrationWarning;
  uint16_t SettingsResetWarning;
  uint16_t UVLOWarningString;
  uint16_t UndervoltageString;
  uint16_t InputVoltageString;

  uint16_t SleepingSimpleString;
  uint16_t SleepingAdvancedString;
  uint16_t SleepingTipAdvancedString;
  uint16_t IdleTipString;
  uint16_t IdleSetString;
  uint16_t TipDisconnectedString;
  uint16_t SolderingAdvancedPowerPrompt;
  uint16_t OffString;

  uint16_t ResetOKMessage;
  uint16_t SettingsResetMessage;
  uint16_t NoAccelerometerMessage;
  uint16_t NoPowerDeliveryMessage;
  uint16_t LockingKeysString;
  uint16_t UnlockingKeysString;
  uint16_t WarningKeysLockedString;

  uint16_t SettingRightChar;
  uint16_t SettingLeftChar;
  uint16_t SettingAutoChar;
  uint16_t SettingFastChar;
  uint16_t SettingSlowChar;
  uint16_t SettingMediumChar;
  uint16_t SettingOffChar;
  uint16_t SettingStartSolderingChar;
  uint16_t SettingStartSleepChar;
  uint16_t SettingStartSleepOffChar;
  uint16_t SettingStartNoneChar;
  uint16_t SettingSensitivityOff;
  uint16_t SettingSensitivityLow;
  uint16_t SettingSensitivityMedium;
  uint16_t SettingSensitivityHigh;
  uint16_t SettingLockDisableChar;
  uint16_t SettingLockBoostChar;
  uint16_t SettingLockFullChar;
  uint16_t SettingNAChar;

  uint16_t SettingsDescriptions[static_cast<uint32_t>(SettingsItemIndex::NUM_ITEMS)];
  uint16_t SettingsShortNames[static_cast<uint32_t>(SettingsItemIndex::NUM_ITEMS)];
  uint16_t SettingsMenuEntries[5];
  uint16_t SettingsMenuEntriesDescriptions[5]; // unused
};

extern const TranslationIndexTable *const Tr;
extern const char *const                  TranslationStrings;

extern const uint8_t *const Font_12x16;
extern const uint8_t *const Font_6x8;

constexpr uint8_t settings_item_index(const SettingsItemIndex i) { return static_cast<uint8_t>(i); }
// Use a constexpr function for type-checking.
#define SETTINGS_DESC(i) (settings_item_index(i) + 1)

const char *translatedString(uint16_t index);

void prepareTranslations();

#endif /* TRANSLATION_H_ */
