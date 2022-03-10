/*
 * BMA223.hpp
 *
 *  Created on: 18 Sep. 2020
 *      Author: Ralim
 */

#ifndef CORE_DRIVERS_SC7A20_HPP_
#define CORE_DRIVERS_SC7A20_HPP_
#include "BSP.h"
#include "I2C_Wrapper.hpp"
#include "SC7A20_defines.h"

class SC7A20 {
public:
  static bool detect();
  static bool initalize();
  // 1 = rh, 2,=lh, 8=flat
  static Orientation getOrientation() {
    uint8_t val = ((FRToSI2C::I2C_RegisterRead(isInImitationMode ? SC7A20_ADDRESS2 : SC7A20_ADDRESS, SC7A20_INT2_SOURCE) >> 2) - 1);
    if (val == 1) {
#ifdef SC7_ORI_FLIP
      return Orientation::ORIENTATION_RIGHT_HAND;
#else
      return Orientation::ORIENTATION_LEFT_HAND;
#endif
    } else if (val == 4 || val == 0) {
#ifdef SC7_ORI_FLIP
      return Orientation::ORIENTATION_LEFT_HAND;
#else
      return Orientation::ORIENTATION_RIGHT_HAND;
#endif
    } else
      return Orientation::ORIENTATION_FLAT;
  }
  static void getAxisReadings(int16_t &x, int16_t &y, int16_t &z);

private:
  static bool isInImitationMode;
};

#endif /* CORE_DRIVERS_BMA223_HPP_ */
