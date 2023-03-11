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
#include "cmsis_os.h"
#include "configuration.h"
#include "history.hpp"
#include "main.hpp"
#include "power.hpp"
#include "settingsGUI.hpp"
#include "stdlib.h"
#include "string.h"
#if POW_PD
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

void performCJCC(void);                                            // Used to calibrate the Cold Junction offset
void gui_solderingTempAdjust(void);                                // For adjusting the setpoint temperature of the iron
int  gui_SolderingSleepingMode(bool stayOff, bool autoStarted);    // Sleep mode
void gui_solderingMode(uint8_t jumpToSleep);                       // Main mode for hot pointy tool
void showDebugMenu(void);                                          // Debugging values
void showPDDebug(void);                                            // Debugging menu that hows PD adaptor info
void showWarnings(void);                                           // Shows user warnings if required
void drawHomeScreen(bool buttonLockout) __attribute__((noreturn)); // IDLE / Home screen
void renderHomeScreenAssets(void);                                 // Called to act as start delay and used to render out flipped images for home screen graphics
//
#endif