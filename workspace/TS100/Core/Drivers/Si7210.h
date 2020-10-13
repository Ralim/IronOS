/*
 * Si7210.h
 *
 *  Created on: 5 Oct. 2020
 *      Author: Ralim
 */

#ifndef CORE_DRIVERS_SI7210_H_
#define CORE_DRIVERS_SI7210_H_
#include <stdint.h>
class Si7210 {
public:
	//Return true if present
	static bool detect();

	static bool init();
	static int16_t read();
private:
};

#endif /* CORE_DRIVERS_SI7210_H_ */
