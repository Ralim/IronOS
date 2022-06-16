/*
 * SC7A20_defines.h
 *
 *  Created on: 18 Sep. 2020
 *      Author: Ralim
 */

#ifndef CORE_DRIVERS_SC7A20_DEFINES_H_
#define CORE_DRIVERS_SC7A20_DEFINES_H_

#define SC7A20_ADDRESS 0x18 << 1
// Sometimes the SC7A20 turns up programmed to impersonate the LIS2DH12
#define SC7A20_ADDRESS2 (25 << 1)

#define SC7A20_WHO_AM_I_VALUE (0b00010001)
#define SC7A20_WHO_AMI_I      0x0F
#define SC7A20_CTRL_REG1      0x20
#define SC7A20_CTRL_REG2      0x21
#define SC7A20_CTRL_REG3      0x22
#define SC7A20_CTRL_REG4      0x23
#define SC7A20_CTRL_REG5      0x24
#define SC7A20_CTRL_REG6      0x25
#define SC7A20_REFERENCE      0x26
#define SC7A20_STATUS_REG     0x27
#define SC7A20_OUT_X_L        0x28
#define SC7A20_OUT_X_L_ALT    0xA8
#define SC7A20_OUT_X_H        0x29
#define SC7A20_OUT_Y_L        0x2A
#define SC7A20_OUT_Y_H        0x2B
#define SC7A20_OUT_Z_L        0x2C
#define SC7A20_OUT_Z_H        0x2D
#define SC7A20_FIFO_CTRL      0x2E
#define SC7A20_FIFO_SRC       0x2F
#define SC7A20_INT1_CFG       0x30
#define SC7A20_INT1_SOURCE    0x31
#define SC7A20_INT1_THS       0x32
#define SC7A20_INT1_DURATION  0x33
#define SC7A20_INT2_CFG       0x34
#define SC7A20_INT2_SOURCE    0x35
#define SC7A20_INT2_THS       0x36
#define SC7A20_INT2_DURATION  0x37
#define SC7A20_CLICK_CFG      0x38
#define SC7A20_CLICK_SRC      0x39
#define SC7A20_CLICK_THS      0x3A
#define SC7A20_TIME_LIMIT     0x3B
#define SC7A20_TIME_LATENCY   0x3C
#define SC7A20_TIME_WINDOW    0x3D
#define SC7A20_ACT_THS        0x3E
#define SC7A20_ACT_DURATION   0x3F

#endif /* CORE_DRIVERS_BMA223_DEFINES_H_ */
