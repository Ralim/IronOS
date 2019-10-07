/*
 * MMA8652FC.h
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef MMA8652FC_HPP_
#define MMA8652FC_HPP_
#include "stm32f1xx_hal.h"
#include "MMA8652FC_defines.h"
#include "FRToSI2C.hpp"
#include "hardware.h"

class MMA8652FC {

public:


	static void initalize();		// Initalize the system
	static Orientation getOrientation();// Reads the I2C register and returns the orientation (true == left)
	static void getAxisReadings(int16_t& x, int16_t& y, int16_t& z);

private:
};

#endif /* MMA8652FC_HPP_ */
