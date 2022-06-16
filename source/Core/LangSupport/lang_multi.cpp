#include "OLED.hpp"
#include "Translation.h"
#include "Translation_multi.h"
#include "brieflz.h"
#include "configuration.h"
#include "settingsGUI.hpp"

const TranslationIndexTable *Tr                 = nullptr;
const char                  *TranslationStrings = nullptr;

static uint8_t selectedLangIndex = 255;

static void initSelectedLanguageIndex() {
  if (selectedLangIndex == 255) {

    const uint16_t wantedLanguageID = getSettingValue(SettingsOptions::UILanguage);

    for (size_t i = 0; i < LanguageCount; i++) {
      if (LanguageMetas[i].uniqueID == wantedLanguageID) {
        selectedLangIndex = i;
        return;
      }
    }
    // No match, use the first language.
    selectedLangIndex = 0;
  }
}

static void writeSelectedLanguageToSettings() { setSettingValue(SettingsOptions::UILanguage, LanguageMetas[selectedLangIndex].uniqueID); }

void prepareTranslations() {
  initSelectedLanguageIndex();
  if (selectedLangIndex >= LanguageCount) {
    // This shouldn't happen.
    return;
  }
  const LanguageMeta &langMeta = LanguageMetas[selectedLangIndex];

  const TranslationData *translationData;
  uint16_t               buffer_remaining_size = translation_data_out_buffer_size;
  uint8_t               *buffer_next_ptr       = translation_data_out_buffer;
  if (langMeta.translation_is_compressed) {
    unsigned int outsize;
    outsize = blz_depack_srcsize(langMeta.translation_data, buffer_next_ptr, langMeta.translation_size);

    translationData = reinterpret_cast<const TranslationData *>(buffer_next_ptr);
    buffer_remaining_size -= outsize;
    buffer_next_ptr += outsize;
  } else {
    translationData = reinterpret_cast<const TranslationData *>(langMeta.translation_data);
  }
  Tr                 = &translationData->indices;
  TranslationStrings = translationData->strings;

  memset(DynamicFontSections, 0, FontSectionsCount * sizeof(DynamicFontSections[0]));
  for (int i = 0; i < FontSectionDataCount; i++) {
    const auto &fontSectionDataInfo = FontSectionDataInfos[i];
    auto       &fontSection         = DynamicFontSections[i];
    fontSection.symbol_start        = fontSectionDataInfo.symbol_start;
    fontSection.symbol_end          = fontSection.symbol_start + fontSectionDataInfo.symbol_count;
    const uint16_t font12_size      = fontSectionDataInfo.symbol_count * (12 * 16 / 8);
    uint16_t       dataSize;
    if (fontSectionDataInfo.data_is_compressed) {
      unsigned int outsize;
      outsize = blz_depack_srcsize(fontSectionDataInfo.data_ptr, buffer_next_ptr, fontSectionDataInfo.data_size);

      fontSection.font12_start_ptr = buffer_next_ptr;
      dataSize                     = outsize;
      buffer_remaining_size -= outsize;
      buffer_next_ptr += outsize;
    } else {
      fontSection.font12_start_ptr = fontSectionDataInfo.data_ptr;
      dataSize                     = fontSectionDataInfo.data_size;
    }
    if (dataSize > font12_size) {
      fontSection.font06_start_ptr = fontSection.font12_start_ptr + font12_size;
    }
  }
}

bool settings_setLanguageSwitch(void) {
  selectedLangIndex = (selectedLangIndex + 1) % LanguageCount;
  writeSelectedLanguageToSettings();
  prepareTranslations();
  return selectedLangIndex == (LanguageCount - 1);
}

bool settings_showLanguageSwitch(void) { return true; }
void settings_displayLanguageSwitch(void) { OLED::printWholeScreen(translatedString(Tr->SettingsShortNames[static_cast<uint8_t>(SettingsItemIndex::LanguageSwitch)])); }
