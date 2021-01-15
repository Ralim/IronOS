/*
 * BMA223.hpp
 *
 *  Created on: 18 Sep. 2020
 *      Author: Ralim
 */

#ifndef CORE_DRIVERS_BMA223_HPP_
#define CORE_DRIVERS_BMA223_HPP_
#include "I2C_Wrapper.hpp"
#include "BSP.h"
#include "SC7A20_defines.h"

class SC7A20 {
public:
	static bool detect();
	static bool initalize();
	//1 = rh, 2,=lh, 8=flat
	static Orientation getOrientation() {
		return static_cast<Orientation>((FRToSI2C::I2C_RegisterRead(SC7A20_ADDRESS,SC7A20_INT2_SOURCE) >> 2) - 1);
	}
	static void getAxisReadings(int16_t& x, int16_t& y, int16_t& z);

private:
};

#endif /* CORE_DRIVERS_BMA223_HPP_ */
