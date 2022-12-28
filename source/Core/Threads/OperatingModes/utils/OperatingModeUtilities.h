#ifndef OPERATING_MODE_UTILITIES_H_
#define OPERATING_MODE_UTILITIES_H_
#include "OLED.hpp"
#include <stdbool.h>

void     GUIDelay();                                         //
bool     checkForUnderVoltage(void);                         //
uint32_t getSleepTimeout(void);                              //
bool     shouldBeSleeping(bool inAutoStart);                 //
bool     shouldShutdown(void);                               //
void     gui_drawTipTemp(bool symbol, const FontStyle font); //
void     printVoltage(void);                                 //
void     warnUser(const char *warning, const int timeout);   //
void     gui_drawBatteryIcon(void);                          //
bool     checkForUnderVoltage(void);                         //
uint16_t min(uint16_t a, uint16_t b);                        //
void     printCountdownUntilSleep(int sleepThres);           //
#endif