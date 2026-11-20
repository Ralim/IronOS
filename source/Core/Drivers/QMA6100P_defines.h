/*
 * QMA6100P_defines.h
 *
 *  Register map for the QST QMA6100P 3-axis accelerometer.
 *  Used on the Alientek T90 (shares the soft-I2C bus with the CH224Q PD chip).
 *  Values taken from the QST QMA6100P datasheet / vendor Linux driver.
 */

#ifndef CORE_DRIVERS_QMA6100P_DEFINES_H_
#define CORE_DRIVERS_QMA6100P_DEFINES_H_

// 7-bit I2C address is 0x12; this codebase stores addresses pre-shifted (8-bit write byte).
#define QMA6100P_ADDRESS (0x12 << 1)

// Chip id register and its expected value.
#define QMA6100P_REG_CHIP_ID  0x00
#define QMA6100P_CHIP_ID      0x90

// Acceleration output registers; 6 bytes starting at XOUT_LSB (X/Y/Z, little endian, 14-bit).
#define QMA6100P_REG_XOUT_LSB 0x01
#define QMA6100P_REG_XOUT_MSB 0x02
#define QMA6100P_REG_YOUT_LSB 0x03
#define QMA6100P_REG_YOUT_MSB 0x04
#define QMA6100P_REG_ZOUT_LSB 0x05
#define QMA6100P_REG_ZOUT_MSB 0x06

// Configuration registers.
#define QMA6100P_REG_RANGE_FILTER 0x0F // full-scale range + filter
#define QMA6100P_REG_BAND_WIDTH   0x10 // ODR / bandwidth
#define QMA6100P_REG_PM           0x11 // power mode + master clock
#define QMA6100P_REG_SW_RESET     0x36 // soft reset
#define QMA6100P_REG_TST0_ANA     0x4A // analog tuning (vendor init)
#define QMA6100P_REG_AFE_ANA      0x56 // analog front end (vendor init)
#define QMA6100P_REG_TST1_ANA     0x5F // analog tuning (vendor init)

// Register values used during init (from the QST vendor init sequence).
#define QMA6100P_SW_RESET_START 0xB6 // write to start soft reset
#define QMA6100P_SW_RESET_STOP  0x00 // write to release soft reset
#define QMA6100P_PM_WAKE_MCLK   0x84 // wake mode, MCLK 51.2kHz
#define QMA6100P_TST0_ANA_VAL   0x20
#define QMA6100P_AFE_ANA_VAL    0x01
#define QMA6100P_TST1_ANA_SET   0x80
#define QMA6100P_TST1_ANA_CLR   0x00
#define QMA6100P_RANGE_8G       0x04 // +/-8g full scale
#define QMA6100P_BW_DEFAULT     0x00 // default bandwidth / ODR

#endif /* CORE_DRIVERS_QMA6100P_DEFINES_H_ */
