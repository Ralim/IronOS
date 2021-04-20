#include "../../configuration.h"
#include "OLED.hpp"
#include "Translation.h"
#include "Translation_multi.h"
#include "gui.hpp"
#include "lzfx.h"

const TranslationIndexTable *Tr                 = nullptr;
const char *                 TranslationStrings = nullptr;

static uint8_t selectedLangIndex = 255;

static void initSelectedLanguageIndex() {
  if (selectedLangIndex == 255) {
    const char *lang = const_cast<char *>(systemSettings.uiLanguage);
    for (size_t i = 0; i < LanguageCount; i++) {
      if (strncmp(lang, LanguageMetas[i].code, sizeof(systemSettings.uiLanguage)) == 0) {
        selectedLangIndex = i;
        return;
      }
    }
    // No match, use the first language.
    selectedLangIndex = 0;
  }
}

static void writeSelectedLanguageToSettings() {
  char *lang = const_cast<char *>(systemSettings.uiLanguage);
  strncpy(lang, LanguageMetas[selectedLangIndex].code, sizeof(systemSettings.uiLanguage));
}

void prepareTranslations() {
  initSelectedLanguageIndex();
  if (selectedLangIndex >= LanguageCount) {
    // This shouldn't happen.
    return;
  }
  const LanguageMeta &langMeta = LanguageMetas[selectedLangIndex];

  const TranslationData *translationData;
  if (langMeta.translation_is_compressed) {
    unsigned int outsize;
    outsize = translation_data_out_buffer_size;
    lzfx_decompress(langMeta.translation_data, langMeta.translation_size, translation_data_out_buffer, &outsize);
    translationData = reinterpret_cast<const TranslationData *>(translation_data_out_buffer);
  } else {
    translationData = reinterpret_cast<const TranslationData *>(langMeta.translation_data);
  }
  Tr                 = &translationData->indices;
  TranslationStrings = translationData->strings;
}

bool settings_setLanguageSwitch(void) {
  selectedLangIndex = (selectedLangIndex + 1) % LanguageCount;
  writeSelectedLanguageToSettings();
  prepareTranslations();
  return selectedLangIndex == (LanguageCount - 1);
}

bool settings_displayLanguageSwitch(void) {
  OLED::printWholeScreen(translatedString(Tr->SettingsShortNames[static_cast<uint8_t>(SettingsItemIndex::LanguageSwitch)]));
  return false;
}
