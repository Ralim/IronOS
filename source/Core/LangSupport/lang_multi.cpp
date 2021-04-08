#include "../../configuration.h"
#include "OLED.hpp"
#include "Translation.h"
#include "gui.hpp"

static uint8_t selectedLangIndex = 255;

static void initSelectedLanguageIndex() {
  if (selectedLangIndex == 255) {
    const char *lang = const_cast<char *>(systemSettings.uiLanguage);
    for (size_t i = 0; i < LanguageCount; i++) {
      if (strncmp(lang, InternalLanguageCodes[i], sizeof(systemSettings.uiLanguage)) == 0) {
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
  strncpy(lang, InternalLanguageCodes[selectedLangIndex], sizeof(systemSettings.uiLanguage));
}

const char *translatedString(const char *const *strs) {
  initSelectedLanguageIndex();
  return strs[selectedLangIndex];
}

bool settings_setLanguageSwitch(void) {
  selectedLangIndex = (selectedLangIndex + 1) % LanguageCount;
  writeSelectedLanguageToSettings();
  return selectedLangIndex == (LanguageCount - 1);
}

bool settings_displayLanguageSwitch(void) {
  const char *langCodeStr = translatedString(DisplayLanguageNames);
  uint16_t    len         = str_display_len(langCodeStr);
  printShortDescription(SettingsItemIndex::LanguageSwitch, 8 - len);
  OLED::print(langCodeStr, FontStyle::LARGE);
  return false;
}
