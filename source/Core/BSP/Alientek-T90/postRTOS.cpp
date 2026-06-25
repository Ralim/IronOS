/*
 * postRTOS.cpp
 *
 *  Post-scheduler init for the Alientek T90.
 */

#include "BSP.h"
#include "I2CBB2.hpp"
#include "configuration.h"

// Initialisation to be performed with the scheduler active.
void postRToSInit() {
#ifdef I2C_SOFT_BUS_2
  // I2CBB2 backs the accelerometer (QMA6100P) on the soft-I2C bus shared with the CH224Q.
  // Its init() creates a FreeRTOS static mutex, so it must run after the scheduler has started
  // (this function is called from startPOWTask). The CH224Q was already talked to pre-scheduler
  // via ch224q_init(); re-initing the open-drain pins here is harmless. Called once, no double-init.
  I2CBB2::init();
#endif
}
