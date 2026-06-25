/*
 * Software_I2C.h
 *
 *  Bit-bang I2C macros for the Alientek T90 (Nations N32L40x).
 *  CH224Q USB-PD sink lives on the soft bus PB6 (SCL) / PB7 (SDA), device 0x22.
 *  The pins are configured as open-drain GPIO in Setup.cpp MX_GPIO_Init: driving
 *  HIGH releases the line (external/pull-up takes over), driving LOW pulls it down.
 *  The start/stop/write-byte/read-byte/ack primitives live in Core/Drivers/I2CBB2
 *  and are built from these macros; Power.cpp drives them for the CH224Q.
 */

#ifndef BSP_ALIENTEK_T90_SOFTWARE_I2C_H_
#define BSP_ALIENTEK_T90_SOFTWARE_I2C_H_
#include "BSP.h"
#include "Pins.h"
#include "configuration.h"
#include "n32l40x.h"
// The CH224Q PD chip (CH224_SOFT_I2C) and the QMA6100P accelerometer (I2C_SOFT_BUS_2 via I2CBB2)
// share this single soft-I2C bus on PB6/PB7, so expose the macros for either consumer.
#if defined(CH224_SOFT_I2C) || defined(I2C_SOFT_BUS_2)

#define SOFT_SCL2_HIGH() GPIO_SetBits(SCL2_GPIO_Port, SCL2_Pin)
#define SOFT_SCL2_LOW()  GPIO_ResetBits(SCL2_GPIO_Port, SCL2_Pin)
#define SOFT_SDA2_HIGH() GPIO_SetBits(SDA2_GPIO_Port, SDA2_Pin)
#define SOFT_SDA2_LOW()  GPIO_ResetBits(SDA2_GPIO_Port, SDA2_Pin)
#define SOFT_SDA2_READ() (GPIO_ReadInputDataBit(SDA2_GPIO_Port, SDA2_Pin) ? 1 : 0)
#define SOFT_SCL2_READ() (GPIO_ReadInputDataBit(SCL2_GPIO_Port, SCL2_Pin) ? 1 : 0)
// clang-format off
#define SOFT_I2C_DELAY()              \
  {                                   \
    for (int xx = 0; xx < 12; xx++) { \
      asm("nop");                     \
    }                                 \
  }
// clang-format on

#endif
// 40 ~= 100kHz; 15 gives around 250kHz or so which is fast _and_ stable

#endif /* BSP_ALIENTEK_T90_SOFTWARE_I2C_H_ */
