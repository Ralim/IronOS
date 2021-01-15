/*
 * BMA223.hpp
 *
 *  Created on: 18 Sep. 2020
 *      Author: Ralim
 */

#ifndef CORE_DRIVERS_SC7A20_HPP_
#define CORE_DRIVERS_SC7A20_HPP_
#include "I2C_Wrapper.hpp"
#include "BSP.h"
#include "SC7A20_defines.h"

class SC7A20 {
public:
	static bool detect();
	static bool initalize();
	//1 = rh, 2,=lh, 8=flat
	static Orientation getOrientation() {
		uint8_t val = ((FRToSI2C::I2C_RegisterRead(SC7A20_ADDRESS, SC7A20_INT2_SOURCE) >> 2) - 1);
		if (val == 8)
			val = 3;
		else if (val == 1)
			val = 1;
		else if (val == 2)
			val = 0;
		else
			val = 3;
		return static_cast<Orientation>(val);
	}
	static void getAxisReadings(int16_t &x, int16_t &y, int16_t &z);

private:
};

#endif /* CORE_DRIVERS_BMA223_HPP_ */
