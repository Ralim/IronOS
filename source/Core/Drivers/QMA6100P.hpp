/*
 * QMA6100P.hpp
 *
 *  Driver for the QST QMA6100P 3-axis accelerometer.
 *  Structurally mirrors SC7A20: a static class accessed through ACCEL_I2C_CLASS.
 *  On the Alientek T90 this runs on the soft-I2C bus (I2CBB2) shared with the CH224Q.
 */

#ifndef CORE_DRIVERS_QMA6100P_HPP_
#define CORE_DRIVERS_QMA6100P_HPP_
#include "BSP.h"
#include "QMA6100P_defines.h"
#include "accelerometers_common.h"

class QMA6100P {
public:
  // Probe the bus and verify the chip id.
  static bool detect();
  // Power up, soft reset and configure range/ODR (note the deliberate "initalize" spelling
  // used across this codebase's accelerometer drivers).
  static bool initalize();
  // Best-effort orientation from the most recent axis read. Exact axis-to-hand mapping
  // must be tuned on the T90 hardware; see getOrientation() body for the assumptions.
  static Orientation getOrientation();
  // Read the 3 axes; each is a signed 14-bit value sign-extended into int16_t.
  static void getAxisReadings(int16_t &x, int16_t &y, int16_t &z);
};

#endif /* CORE_DRIVERS_QMA6100P_HPP_ */
