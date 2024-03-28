
#include "OperatingModeUtilities.h"

void GUIDelay() {
  // Called in all UI looping tasks,
  // This limits the re-draw rate to the LCD and also lets the DMA run
  // As the gui task can very easily fill this bus with transactions, which will
  // prevent the movement detection from running
  vTaskDelay(5 * TICKS_10MS);
}