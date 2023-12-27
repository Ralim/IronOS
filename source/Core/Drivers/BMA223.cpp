/*
 * BMA223.cpp
 *
 *  Created on: 18 Sep. 2020
 *      Author: Ralim
 */

#include "accelerometers_common.h"
#include <BMA223.hpp>
#include <array>

bool BMA223::detect() {
  if (ACCEL_I2C_CLASS::probe(BMA223_ADDRESS)) {
    // Read chip id to ensure its not an address collision
    uint8_t id = 0;
    if (ACCEL_I2C_CLASS::Mem_Read(BMA223_ADDRESS, BMA223_BGW_CHIPID, &id, 1)) {
      return id == 0b11111000;
    }
  }

  return false;
}

static const ACCEL_I2C_CLASS::I2C_REG i2c_registers[] = {
  //
  //
    {    BMA223_PMU_RANGE, 0b00000011, 0}, // 2G range
    {       BMA223_PMU_BW, 0b00001101, 0}, // 250Hz filter
    {      BMA223_PMU_LPW, 0b00000000, 0}, // Full power
    {     BMA223_ACCD_HBW, 0b00000000, 0}, // filtered data out
    { BMA223_INT_OUT_CTRL, 0b00001010, 0}, // interrupt active low and OD to get it hi-z
    {BMA223_INT_RST_LATCH, 0b10000000, 0}, // interrupt active low and OD to get it hi-z
    {     BMA223_INT_EN_0, 0b01000000, 0}, // Enable orientation
    {        BMA223_INT_A, 0b00100111, 0}, // Setup orientation detection

  //
};
bool BMA223::initalize() {
  // Setup acceleration readings
  // 2G range
  // bandwidth = 250Hz
  // High pass filter on (Slow compensation)
  // Turn off IRQ output pins
  // Orientation recognition in symmetrical mode
  // Hysteresis is set to ~ 16 counts
  // Theta blocking is set to 0b10

  return ACCEL_I2C_CLASS::writeRegistersBulk(BMA223_ADDRESS, i2c_registers, sizeof(i2c_registers) / sizeof(i2c_registers[0]));
}

void BMA223::getAxisReadings(int16_t &x, int16_t &y, int16_t &z) {
  // The BMA is odd in that its output data width is only 8 bits
  // And yet there are MSB and LSB registers _sigh_.
  uint8_t sensorData[6] = {0, 0, 0, 0, 0, 0};

  if (ACCEL_I2C_CLASS::Mem_Read(BMA223_ADDRESS, BMA223_ACCD_X_LSB, sensorData, 6) == false) {
    x = y = z = 0;
    return;
  }
  // Shift 6 to make its range ~= the other accelerometers
  x = sensorData[1] << 6;
  y = sensorData[3] << 6;
  z = sensorData[5] << 6;
}
