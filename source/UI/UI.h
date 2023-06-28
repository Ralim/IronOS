#pragma once
#include <stdint.h>

typedef enum {
  SimplifiedHome,        // Basic home
  SimplifiedHomeWarning, // Home with temp warning
  DetailedHome,          // Detailed home view
  DetailedHomeWarning,   // Home with temp warning
  DebugMenu,             // Debugging metrics
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

typedef void (*render_prep_fn)();
typedef void (*tick_fn)();
typedef void (*button_handler_fn)();

typedef struct {
  // on_enter
  // on_exit
  tick_fn           tick;
  render_prep_fn    render_prepare;
  screenLayout_t    layout; // Render layout used
  button_handler_fn handle_button;
} Screen_t;

typedef union {
  int32_t i32;
  void   *ptr;
} screen_arg_t;

typedef struct {
  screen_arg_t args[4];
} ScreenContext_t;

//
void ui_render_screen(Screen_t *screen, ScreenContext_t *context);
