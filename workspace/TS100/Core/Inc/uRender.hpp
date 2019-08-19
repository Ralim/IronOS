/*
 * uRender.h
 *
 *  Created on: 30Aug.,2017
 *      Author: Ben V. Brown
 */

#ifndef URENDER_HPP_
#define URENDER_HPP_
#include "stm32f1xx_hal.h"
class uRender {
public:
void render(uint8_t* bufferOut);

private:
	uint8_t x_start,x_end,width,height;
	uint8_t localBuffer[96*16/2];//local storage buffer

};

#endif /* URENDER_HPP_ */
