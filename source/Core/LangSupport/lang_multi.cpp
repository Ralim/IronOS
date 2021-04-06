#include "../../configuration.h"
#include "OLED.hpp"
#include "Translation.h"
#include "gui.hpp"

bool settings_setLanguageSwitch(void) {
  systemSettings.uiLanguage = (systemSettings.uiLanguage + 1) % LanguageCount;
  return systemSettings.uiLanguage == (LanguageCount - 1);
}

bool settings_displayLanguageSwitch(void) {
  const char *langCodeStr = LanguageCodes;
  uint16_t    len         = str_display_len(langCodeStr);
  printShortDescription(SettingsItemIndex::LanguageSwitch, 8 - len);
  OLED::print(langCodeStr, FontStyle::LARGE);
  return false;
}
