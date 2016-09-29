/*
 * I2C.h
 * I2C wrapper for the stm32 hardware I2C port
 */
#ifndef __I2C_H
#define __I2C_H

#include "stm32f10x_i2c.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

void I2C_Configuration(void);
void I2C_PageWrite(u8* pbuf, u8 numbyte, u8 deviceaddr);
void I2C_PageRead(u8* pbuf, u8 numbyte, u8 deviceaddr, u8 readaddr);
#endif
/******************************** END OF FILE *********************************/
