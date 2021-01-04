/*
 * MSA301.h
 *
 *  Created on: 3 Jan 2021
 *      Author: Ralim
 */

#ifndef DRIVERS_MSA301_H_
#define DRIVERS_MSA301_H_
#include "I2C_Wrapper.hpp"
#include "BSP.h"

class MSA301 {
public:
	//Returns true if this accelerometer is detected
	static bool detect();
	//Init any internal state
	static bool initalize();
	// Reads the I2C register and returns the orientation
	static Orientation getOrientation();
	//Return the x/y/z axis readings as signed int16's
	static void getAxisReadings(int16_t &x, int16_t &y, int16_t &z);

private:
};

#endif /* DRIVERS_MSA301_H_ */
