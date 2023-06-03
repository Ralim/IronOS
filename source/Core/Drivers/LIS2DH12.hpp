/*
 * LIS2DH12.hpp
 *
 *  Created on: 27Feb.,2018
 *      Author: Ralim
 */

#ifndef LIS2DH12_HPP_
#define LIS2DH12_HPP_
#include "BSP.h"

#include "LIS2DH12_defines.hpp"
#include "accelerometers_common.h"

class LIS2DH12 {
public:
  static bool detect();
  static bool isClone();
  static bool initalize();
  // 1 = rh, 2,=lh, 8=flat
  static Orientation getOrientation() {
#ifdef LIS_ORI_FLIP
    uint8_t val = (ACCEL_I2C_CLASS::I2C_RegisterRead(LIS2DH_I2C_ADDRESS, LIS_INT2_SRC) >> 2);
    if (val == 8)
      val = 3;
    else if (val == 1)
      val = 1;
    else if (val == 2)
      val = 0;
    else
      val = 3;
    return static_cast<Orientation>(val);
#else
    return static_cast<Orientation>((ACCEL_I2C_CLASS::I2C_RegisterRead(LIS2DH_I2C_ADDRESS, LIS_INT2_SRC) >> 2) - 1);
#endif
  }
  static void getAxisReadings(int16_t &x, int16_t &y, int16_t &z);

private:
};

#endif /* LIS2DH12_HPP_ */
