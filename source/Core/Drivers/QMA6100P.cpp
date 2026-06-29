/*
 * QMA6100P.cpp
 *
 *  Driver for the QST QMA6100P 3-axis accelerometer.
 *  Init sequence, chip-id and data format follow the QST QMA6100P datasheet /
 *  vendor Linux driver. On the Alientek T90 register access goes over the
 *  soft-I2C bus (ACCEL_I2C_CLASS == I2CBB2), shared with the CH224Q PD chip.
 */

#include "QMA6100P.hpp"
#include "QMA6100P_defines.h"
#include "accelerometers_common.h"
#include <stdlib.h>

bool QMA6100P::detect() {
  if (!ACCEL_I2C_CLASS::probe(QMA6100P_ADDRESS)) {
    return false;
  }
  // Confirm the chip id so we do not mistake an address collision for a QMA6100P.
  uint8_t id = 0;
  if (!ACCEL_I2C_CLASS::Mem_Read(QMA6100P_ADDRESS, QMA6100P_REG_CHIP_ID, &id, 1)) {
    return false;
  }
  return id == QMA6100P_CHIP_ID;
}

// Power-up / range / ODR init, in the order the QST vendor driver uses.
// Soft reset needs the start/stop writes separated by a short pause, so the two
// reset writes carry a pause_ms; the rest are plain register writes.
static const ACCEL_I2C_CLASS::I2C_REG i2c_registers[] = {
    {    QMA6100P_REG_SW_RESET, QMA6100P_SW_RESET_START, 20}, // start soft reset, wait 20ms
    {    QMA6100P_REG_SW_RESET,  QMA6100P_SW_RESET_STOP, 30}, // release reset, wait 30ms
    {          QMA6100P_REG_PM,   QMA6100P_PM_WAKE_MCLK,  0}, // wake mode, MCLK 51.2kHz
    {    QMA6100P_REG_TST0_ANA,   QMA6100P_TST0_ANA_VAL,  0}, // analog tuning (vendor)
    {     QMA6100P_REG_AFE_ANA,    QMA6100P_AFE_ANA_VAL,  0}, // analog front end (vendor)
    {    QMA6100P_REG_TST1_ANA,   QMA6100P_TST1_ANA_SET,  0}, // analog tuning set (vendor)
    {    QMA6100P_REG_TST1_ANA,   QMA6100P_TST1_ANA_CLR,  0}, // analog tuning clear (vendor)
    {QMA6100P_REG_RANGE_FILTER,       QMA6100P_RANGE_8G,  0}, // +/-8g full scale
    {  QMA6100P_REG_BAND_WIDTH,     QMA6100P_BW_DEFAULT,  0}, // default ODR / bandwidth
};

bool QMA6100P::initalize() { return ACCEL_I2C_CLASS::writeRegistersBulk(QMA6100P_ADDRESS, i2c_registers, sizeof(i2c_registers) / sizeof(i2c_registers[0])); }

void QMA6100P::getAxisReadings(int16_t &x, int16_t &y, int16_t &z) {
  // 6 output bytes, little-endian per axis, starting at XOUT_LSB.
  uint8_t xyz[6] = {0};
  if (ACCEL_I2C_CLASS::Mem_Read(QMA6100P_ADDRESS, QMA6100P_REG_XOUT_LSB, xyz, 6) == false) {
    x = y = z = 0;
    return;
  }
  // Each axis is 14-bit, left-justified: MSB holds DATA[13:6], LSB[7:2] holds DATA[5:0] (LSB[1:0]
  // reserved). So (MSB<<8)|LSB places the 14 bits in [15:2]; casting to int16_t then arithmetic
  // right-shifting by 2 right-justifies to [13:0] and sign-extends from bit 13.
  x = (int16_t)(((int16_t)((xyz[1] << 8) | xyz[0])) >> 2);
  y = (int16_t)(((int16_t)((xyz[3] << 8) | xyz[2])) >> 2);
  z = (int16_t)(((int16_t)((xyz[5] << 8) | xyz[4])) >> 2);
}

Orientation QMA6100P::getOrientation() {
  // The QMA6100P has a hardware orientation engine, but it is not configured here, so derive
  // orientation in software from the current gravity vector. This is best-effort: the exact
  // axis-to-hand mapping (and which axis points "up" when flat) depends on how the sensor is
  // mounted in the T90 and must be tuned on hardware. Assumptions to revisit on-device:
  //   - Z is the axis normal to the PCB; when it dominates the iron is flat.
  //   - X distinguishes left vs right hand when the iron is tilted on its side.
  int16_t x = 0, y = 0, z = 0;
  getAxisReadings(x, y, z);

  int32_t ax = abs((int32_t)x);
  int32_t ay = abs((int32_t)y);
  int32_t az = abs((int32_t)z);

  // If gravity is mostly along Z, the iron lies flat.
  if (az >= ax && az >= ay) {
    return Orientation::ORIENTATION_FLAT;
  }
  // Otherwise pick the hand from the sign of the dominant lateral axis. // TODO tune on hardware
  if (ax >= ay) {
    return (x < 0) ? Orientation::ORIENTATION_LEFT_HAND : Orientation::ORIENTATION_RIGHT_HAND;
  }
  return (y < 0) ? Orientation::ORIENTATION_LEFT_HAND : Orientation::ORIENTATION_RIGHT_HAND;
}
