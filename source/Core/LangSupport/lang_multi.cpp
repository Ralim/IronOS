#include "../../configuration.h"
#include "OLED.hpp"
#include "Translation.h"
#include "gui.hpp"

const char *translatedString(const char *const *strs) {
  if (systemSettings.uiLanguage >= LanguageCount) {
    systemSettings.uiLanguage = 0;
  }
  return strs[systemSettings.uiLanguage];
}

bool settings_setLanguageSwitch(void) {
  systemSettings.uiLanguage = (systemSettings.uiLanguage + 1) % LanguageCount;
  return systemSettings.uiLanguage == (LanguageCount - 1);
}

bool settings_displayLanguageSwitch(void) {
  const char *langCodeStr = translatedString(LanguageCodes);
  uint16_t    len         = str_display_len(langCodeStr);
  printShortDescription(SettingsItemIndex::LanguageSwitch, 8 - len);
  OLED::print(langCodeStr, FontStyle::LARGE);
  return false;
}
