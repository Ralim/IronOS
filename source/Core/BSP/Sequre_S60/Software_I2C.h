/*
 * Software_I2C.h
 *
 *  Created on: 25 Jul 2020
 *      Author: Ralim
 */

#ifndef BSP_MINIWARE_SOFTWARE_I2C_H_
#define BSP_MINIWARE_SOFTWARE_I2C_H_
#include "BSP.h"
#include "configuration.h"
#include "stm32f1xx_hal.h"
#ifdef I2C_SOFT_BUS_2

#define SOFT_SCL2_HIGH() HAL_GPIO_WritePin(SCL2_GPIO_Port, SCL2_Pin, GPIO_PIN_SET)
#define SOFT_SCL2_LOW()  HAL_GPIO_WritePin(SCL2_GPIO_Port, SCL2_Pin, GPIO_PIN_RESET)
#define SOFT_SDA2_HIGH() HAL_GPIO_WritePin(SDA2_GPIO_Port, SDA2_Pin, GPIO_PIN_SET)
#define SOFT_SDA2_LOW()  HAL_GPIO_WritePin(SDA2_GPIO_Port, SDA2_Pin, GPIO_PIN_RESET)
#define SOFT_SDA2_READ() (HAL_GPIO_ReadPin(SDA2_GPIO_Port, SDA2_Pin) == GPIO_PIN_SET ? 1 : 0)
#define SOFT_SCL2_READ() (HAL_GPIO_ReadPin(SCL2_GPIO_Port, SCL2_Pin) == GPIO_PIN_SET ? 1 : 0)
#define SOFT_I2C_DELAY()              \
  {                                   \
    for (int xx = 0; xx < 12; xx++) { \
      asm("nop");                     \
    }                                 \
  }

#endif
// 40 ~= 100kHz; 15 gives around 250kHz or so which is fast _and_ stable

#endif /* BSP_MINIWARE_SOFTWARE_I2C_H_ */
