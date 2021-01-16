/*
 * LIS2DH12_defines.hpp
 *
 *  Created on: 27Feb.,2018
 *      Author: Ralim
 */

#ifndef LIS2DH12_DEFINES_HPP_
#define LIS2DH12_DEFINES_HPP_


#define LIS2DH_I2C_ADDRESS (25<<1)

#define LIS_CTRL_REG1 0x20|0x80
#define LIS_CTRL_REG2 0x21|0x80
#define LIS_CTRL_REG3 0x22|0x80
#define LIS_CTRL_REG4 0x23|0x80
#define LIS_CTRL_REG5 0x24|0x80
#define LIS_CTRL_REG6 0x25|0x80
#define LIS_INT1_CFG  0xB0|0x80
#define LIS_INT2_CFG  0xB4|0x80
#define LIS_INT1_DURATION 0x33|0x80
#define LIS_INT1_THS 0x32|0x80
#define LIS_INT1_SRC 0x31|0x80
#define LIS_INT2_DURATION 0x37|0x80
#define LIS_INT2_THS 0x36|0x80
#define LIS_INT2_SRC 0x35|0x80
#endif /* LIS2DH12_DEFINES_HPP_ */
