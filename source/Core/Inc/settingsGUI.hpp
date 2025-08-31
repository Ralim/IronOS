/*
 * settingsGUI.h
 *
 *  Created on: 3Sep.,2017
 *      Author: Ben V. Brown
 */

#ifndef GUI_HPP_
#define GUI_HPP_
#include "BSP.h"
#include "Buttons.hpp"
#include "FreeRTOS.h"
#include "Settings.h"
#include "Translation.h"

#define PRESS_ACCEL_STEP         (TICKS_100MS / 3)
#define PRESS_ACCEL_INTERVAL_MIN TICKS_100MS
#define PRESS_ACCEL_INTERVAL_MAX (TICKS_100MS * 3)

// GUI holds the menu structure and all its methods for the menu itself

// Declarations for all the methods for the settings menu (at end of this file)

// Struct for holding the function pointers and descriptions
typedef struct {
  // The settings description index, please use the `SETTINGS_DESC` macro with
  // the `SettingsItemIndex` enum. Use 0 for no description.
  uint8_t description;
  void (*const incrementHandler)(void);
  void (*const draw)(void); // Must not be nullptr, as that marks end of menu
  bool (*const isVisible)(void);
  // If this is set, we will automatically use the settings increment handler instead, set >= num settings to disable
  SettingsOptions   autoSettingOption;
  SettingsItemIndex shortDescriptionIndex;
  uint8_t           shortDescriptionSize;
} menuitem;

void                   enterSettingsMenu();
extern const menuitem  rootSettingsMenu[];
extern const menuitem *subSettingsMenus[];

#endif /* GUI_HPP_ */
