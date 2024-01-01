#include "Buttons.hpp"
#include "OperatingModeUtilities.h"
extern TickType_t lastMovementTime;
#ifndef NO_SLEEP_MODE
void printCountdownUntilSleep(int sleepThres) {
  /*
   * Print seconds or minutes (if > 99 seconds) until sleep
   * mode is triggered.
   */
  TickType_t lastEventTime = lastButtonTime < lastMovementTime ? lastMovementTime : lastButtonTime;
  TickType_t downCount     = sleepThres - xTaskGetTickCount() + lastEventTime;
  if (downCount > (99 * TICKS_SECOND)) {
    OLED::printNumber(downCount / 60000 + 1, 2, FontStyle::SMALL);
    OLED::print(SmallSymbolMinutes, FontStyle::SMALL);
  } else {
    OLED::printNumber(downCount / 1000 + 1, 2, FontStyle::SMALL);
    OLED::print(SmallSymbolSeconds, FontStyle::SMALL);
  }
}
#endif