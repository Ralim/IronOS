/*
 * BMA223.hpp
 *
 *  Created on: 18 Sep. 2020
 *      Author: Ralim
 */

#ifndef CORE_DRIVERS_BMA223_HPP_
#define CORE_DRIVERS_BMA223_HPP_
#include "BMA223_defines.h"
#include "BSP.h"
#include "I2C_Wrapper.hpp"
#include "accelerometers_common.h"


class BMA223 {
public:
  static bool detect();
  static bool initalize();
  // 1 = rh, 2,=lh, 8=flat
  static Orientation getOrientation() {
    uint8_t val = ACCEL_I2C_CLASS::I2C_RegisterRead(BMA223_ADDRESS, BMA223_INT_STATUS_3);
    val >>= 4; // we dont need high values
    val &= 0b11;
    if (val & 0b10) {
      return ORIENTATION_FLAT;
    } else {
      return static_cast<Orientation>(!val);
    }
    // 0 = rhs
    // 1 =lhs
    // 2 & 3 == ignore
  }
  static void getAxisReadings(int16_t &x, int16_t &y, int16_t &z);

private:
};

#endif /* CORE_DRIVERS_BMA223_HPP_ */
