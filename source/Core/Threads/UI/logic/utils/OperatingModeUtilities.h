#ifndef OPERATING_MODE_UTILITIES_H_
#define OPERATING_MODE_UTILITIES_H_
#include "Buttons.hpp"
#include "OLED.hpp"
#include <stdbool.h>

void     GUIDelay();                                               //
bool     checkForUnderVoltage(void);                               //
uint32_t getSleepTimeout(void);                                    //
bool     shouldBeSleeping();                                       //
bool     shouldShutdown(void);                                     //
void     gui_drawTipTemp(bool symbol, const FontStyle font);       //
void     printVoltage(void);                                       //
bool     warnUser(const char *warning, const ButtonState buttons); //
void     gui_drawBatteryIcon(void);                                //
bool     checkForUnderVoltage(void);                               //
uint16_t min(uint16_t a, uint16_t b);                              //
void     printCountdownUntilSleep(int sleepThres);                 //
#endif