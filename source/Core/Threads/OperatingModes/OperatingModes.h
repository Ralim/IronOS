#ifndef OPERATING_MODES_H_
#define OPERATING_MODES_H_

extern "C" {
#include "FreeRTOSConfig.h"
}
#include "Buttons.hpp"
#include "OLED.hpp"
#include "OperatingModeUtilities.h"
#include "Settings.h"
#include "TipThermoModel.h"
#include "Translation.h"
#include "Types.h"
#include "cmsis_os.h"
#include "configuration.h"
#include "history.hpp"
#include "main.hpp"
#include "power.hpp"
#include "settingsGUI.hpp"
#include "stdlib.h"
#include "string.h"
#ifdef POW_PD
#include "USBPD.h"
#include "pd.h"
#endif

// Exposed modes
enum OperatingMode {
  idle      = 0,
  soldering = 1,
  boost     = 2,
  sleeping  = 3,
  settings  = 4,
  debug     = 5
};

// Main functions
void performCJCC(void);                                            // Used to calibrate the Cold Junction offset
void gui_solderingTempAdjust(void);                                // For adjusting the setpoint temperature of the iron
int  gui_SolderingSleepingMode(bool stayOff, bool autoStarted);    // Sleep mode
void gui_solderingMode(uint8_t jumpToSleep);                       // Main mode for hot pointy tool
void gui_solderingProfileMode();                                   // Profile mode for hot likely-not-so-pointy tool
void showDebugMenu(void);                                          // Debugging values
void showPDDebug(void);                                            // Debugging menu that shows PD adaptor info
void showWarnings(void);                                           // Shows user warnings if required
void drawHomeScreen(bool buttonLockout) __attribute__((noreturn)); // IDLE / Home screen
void renderHomeScreenAssets(void);                                 // Called to act as start delay and used to render out flipped images for home screen graphics

// Common helpers
int8_t            getPowerSourceNumber(void); // Returns number ID of power source
TemperatureType_t getTipTemp(void);           // Returns temperature of the tip in *C/*F (based on user settings)
#endif
