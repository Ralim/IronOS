#include "Translation.h"

const char *translatedString(const char *const *strs) { return strs[0]; }

bool settings_setLanguageSwitch(void) { return false; }

bool settings_displayLanguageSwitch(void) {
  return true; // skip
}
