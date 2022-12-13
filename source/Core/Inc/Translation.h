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
extern const char *SymbolAmps;
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
extern const char *SymbolPDDebug;
extern const char *SymbolState;
extern const char *SymbolNoVBus;
extern const char *SymbolVBus;

extern const char *DebugMenu[];
extern const char *AccelTypeNames[];
extern const char *PowerSourceNames[];

enum class SettingsItemIndex : uint8_t {
  DCInCutoff,
  MinVolCell,
  QCMaxVoltage,
  PDNegTimeout,
  BoostTemperature,
  AutoStart,
  TempChangeShortStep,
  TempChangeLongStep,
  LockingMode,
  MotionSensitivity,
  SleepTemperature,
  SleepTimeout,
  ShutdownTimeout,
  HallEffSensitivity,
  TemperatureUnit,
  DisplayRotation,
  CooldownBlink,
  ScrollingSpeed,
  ReverseButtonTempChange,
  AnimSpeed,
  AnimLoop,
  Brightness,
  ColourInversion,
  LOGOTime,
  AdvancedIdle,
  AdvancedSoldering,
  PowerLimit,
  CalibrateCJC,
  VoltageCalibration,
  PowerPulsePower,
  PowerPulseWait,
  PowerPulseDuration,
  SettingsReset,
  LanguageSwitch,
  NUM_ITEMS,
};

struct TranslationIndexTable {
  uint16_t SettingsCalibrationWarning;
  uint16_t CJCCalibrating;
  uint16_t SettingsResetWarning;
  uint16_t UVLOWarningString;
  uint16_t UndervoltageString;
  uint16_t InputVoltageString;

  uint16_t SleepingSimpleString;
  uint16_t SleepingAdvancedString;
  uint16_t SleepingTipAdvancedString;
  uint16_t OffString;
  uint16_t DeviceFailedValidationWarning;

  uint16_t CJCCalibrationDone;
  uint16_t ResetOKMessage;
  uint16_t SettingsResetMessage;
  uint16_t NoAccelerometerMessage;
  uint16_t NoPowerDeliveryMessage;
  uint16_t LockingKeysString;
  uint16_t UnlockingKeysString;
  uint16_t WarningKeysLockedString;
  uint16_t WarningThermalRunaway;

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

  uint16_t SettingsDescriptions[static_cast<uint32_t>(SettingsItemIndex::NUM_ITEMS)];
  uint16_t SettingsShortNames[static_cast<uint32_t>(SettingsItemIndex::NUM_ITEMS)];
  uint16_t SettingsMenuEntries[5];
  uint16_t SettingsMenuEntriesDescriptions[5]; // unused
};

extern const TranslationIndexTable *Tr;

extern const char *TranslationStrings;

struct TranslationData {
  TranslationIndexTable indices;
  // Translation strings follows the translation index table.
  // C++ does not support flexible array member as in C, so we use a 1-element
  // array as a placeholder.
  char strings[1];
};

struct FontSection {
  /// Start index of font section, inclusive
  uint16_t symbol_start;
  /// End index of font section, exclusive
  uint16_t       symbol_end;
  const uint8_t *font12_start_ptr;
  const uint8_t *font06_start_ptr;
};

extern const FontSection *const FontSections;
extern const uint8_t            FontSectionsCount;

constexpr uint8_t settings_item_index(const SettingsItemIndex i) { return static_cast<uint8_t>(i); }
// Use a constexpr function for type-checking.
#define SETTINGS_DESC(i) (settings_item_index(i) + 1)

const char *translatedString(uint16_t index);

void prepareTranslations();
void settings_displayLanguageSwitch(void);
bool settings_showLanguageSwitch(void);
bool settings_setLanguageSwitch(void);

#endif /* TRANSLATION_H_ */
