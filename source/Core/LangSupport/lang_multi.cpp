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

  // Font 12 can be compressed; if it is then we want to decompress it to ram
  if (FontSectionInfo.font12_compressed_source != NULL) {
    blz_depack(FontSectionInfo.font12_compressed_source, (uint8_t *)FontSectionInfo.font12_start_ptr, FontSectionInfo.font12_decompressed_size);
  }

  // Font 06 can be compressed; if it is then we want to decompress it to ram
  if (FontSectionInfo.font06_compressed_source != NULL) {
    blz_depack(FontSectionInfo.font06_compressed_source, (uint8_t *)FontSectionInfo.font06_start_ptr, FontSectionInfo.font06_decompressed_size);
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
