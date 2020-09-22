/*
 * BMA223.hpp
 *
 *  Created on: 18 Sep. 2020
 *      Author: Ralim
 */

#ifndef CORE_DRIVERS_BMA223_HPP_
#define CORE_DRIVERS_BMA223_HPP_
#include "I2C_Wrapper.hpp"
#include "LIS2DH12_defines.hpp"
#include "BSP.h"


class BMA223 {
public:
	static bool detect();
	static bool initalize();
	//1 = rh, 2,=lh, 8=flat
	static Orientation getOrientation() {
#ifdef ACCEL_ORI_FLIP
		uint8_t val = (FRToSI2C::I2C_RegisterRead(LIS2DH_I2C_ADDRESS,
		LIS_INT2_SRC) >> 2);
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
		return static_cast<Orientation>((FRToSI2C::I2C_RegisterRead(LIS2DH_I2C_ADDRESS,LIS_INT2_SRC) >> 2) - 1);
#endif
	}
	static void getAxisReadings(int16_t& x, int16_t& y, int16_t& z);

private:
};

#endif /* CORE_DRIVERS_BMA223_HPP_ */
