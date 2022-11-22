#ifndef OPERATING_MODES_H_
#define OPERATING_MODES_H_

extern "C" {
#include "FreeRTOSConfig.h"
}
#include "BootLogo.h"
#include "Buttons.hpp"
#include "I2CBB.hpp"
#include "LIS2DH12.hpp"
#include "MMA8652FC.hpp"
#include "OLED.hpp"
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

void performCJCC(void);                                         // Used to calibrate the Cold Junction offset
void gui_solderingTempAdjust(void);                             // For adjusting the setpoint temperature of the iron
int  gui_SolderingSleepingMode(bool stayOff, bool autoStarted); // Sleep mode
void gui_solderingMode(uint8_t jumpToSleep);
#endif