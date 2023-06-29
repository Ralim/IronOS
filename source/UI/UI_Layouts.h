#pragma once
#include "UI_Elements.h"
typedef enum {
  SimplifiedHome,        // Basic home
  SimplifiedHomeWarning, // Home with temp warning
  DetailedHome,          // Detailed home view
  DetailedHomeWarning,   // Home with temp warning
  DebugView,             // Debugging metrics
  settingsCategory,      // Settings category with icon
  SettingsEntryBool,     // Tickbox setting
  SettingsEntry3Number,  // Settings adjust with 3 number digits
  SettingsEntry2Number,  // Settings adjust with 2 number digits
  SettingsEntry1Number,  // Settings adjust with 2 number digits
  SettingsEntry1Text,    // Setting with single text char for state
  ScrollingText,         // Scrolling large text (warnings, help text)
  SolderingMode,         // Basic soldering mode
  DetailedSolderingMode, // Detailed soldering mode
  NumberAdjust,          // Number adjust of number with <> either side
} screenLayout_t;

typedef struct {
  struct {
    ElementTypes_t    elementType;
    ElementSettings_t elementSettings;
  } elements[5];
} ScreenLayoutRecord_t;