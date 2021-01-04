/*
 * MSA301_defines.h
 *
 *  Created on: 3 Jan 2021
 *      Author: Ralim
 */

#ifndef DRIVERS_MSA301_DEFINES_H_
#define DRIVERS_MSA301_DEFINES_H_
//Definitions from Adafruit <3

#define MSA301_REG_PARTID 0x01    ///< Register that contains the part ID
#define MSA301_REG_OUT_X_L 0x02   ///< Register address for X axis lower byte
#define MSA301_REG_OUT_X_H 0x03   ///< Register address for X axis higher byte
#define MSA301_REG_OUT_Y_L 0x04   ///< Register address for Y axis lower byte
#define MSA301_REG_OUT_Y_H 0x05   ///< Register address for Y axis higher byte
#define MSA301_REG_OUT_Z_L 0x06   ///< Register address for Z axis lower byte
#define MSA301_REG_OUT_Z_H 0x07   ///< Register address for Z axis higher byte
#define MSA301_REG_MOTIONINT 0x09 ///< Register address for motion interrupt
#define MSA301_REG_DATAINT 0x0A   ///< Register address for data interrupt
#define MSA301_REG_CLICKSTATUS 0x0B ///< Register address for click/doubleclick status
#define MSA301_REG_RESRANGE 0x0F  ///< Register address for resolution range
#define MSA301_REG_ODR 0x10       ///< Register address for data rate setting
#define MSA301_REG_POWERMODE 0x11 ///< Register address for power mode setting
#define MSA301_REG_INTSET0 0x16   ///< Register address for interrupt setting #0
#define MSA301_REG_INTSET1 0x17   ///< Register address for interrupt setting #1
#define MSA301_REG_INTMAP0 0x19   ///< Register address for interrupt map #0
#define MSA301_REG_INTMAP1 0x1A   ///< Register address for interrupt map #1
#define MSA301_REG_TAPDUR 0x2A    ///< Register address for tap duration
#define MSA301_REG_TAPTH 0x2B     ///< Register address for tap threshold
#define MSA301_REG_ORIENT_HY 0x2C ///< Register address for orientation Hysteresis
#define MSA301_REG_ORIENT_STATUS 0x0C ///< Register address for orientation hysteresis

#endif /* DRIVERS_MSA301_DEFINES_H_ */
