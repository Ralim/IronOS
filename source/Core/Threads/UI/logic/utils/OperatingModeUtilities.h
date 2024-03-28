#ifndef OPERATING_MODE_UTILITIES_H_
#define OPERATING_MODE_UTILITIES_H_
#include "Buttons.hpp"
#include "OLED.hpp"
#include <stdbool.h>

void     GUIDelay();                               //
bool     checkForUnderVoltage(void);               //
uint32_t getSleepTimeout(void);                    //
bool     shouldBeSleeping();                       //
bool     shouldShutdown(void);                     //
void     printVoltage(void);                       //
bool     checkForUnderVoltage(void);               //
uint16_t min(uint16_t a, uint16_t b);              //
void     printCountdownUntilSleep(int sleepThres); //
#endif