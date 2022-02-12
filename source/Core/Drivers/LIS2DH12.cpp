/*
 * LIS2DH12.cpp
 *
 *  Created on: 27Feb.,2018
 *      Author: Ralim
 */

#include <array>

#include "LIS2DH12.hpp"
#include "cmsis_os.h"

static const FRToSI2C::I2C_REG i2c_registers[] = {{LIS_CTRL_REG1, 0x17, 0},       // 25Hz
                                                  {LIS_CTRL_REG2, 0b00001000, 0}, // Highpass filter off
                                                  {LIS_CTRL_REG3, 0b01100000, 0}, // Setup interrupt pins
                                                  {LIS_CTRL_REG4, 0b00001000, 0}, // Block update mode off, HR on
                                                  {LIS_CTRL_REG5, 0b00000010, 0}, //
                                                  {LIS_CTRL_REG6, 0b01100010, 0},
                                                  // Basically setup the unit to run, and enable 4D orientation detection
                                                  {LIS_INT2_CFG, 0b01111110, 0}, // setup for movement detection
                                                  {LIS_INT2_THS, 0x28, 0},       //
                                                  {LIS_INT2_DURATION, 64, 0},    //
                                                  {LIS_INT1_CFG, 0b01111110, 0}, //
                                                  {LIS_INT1_THS, 0x28, 0},       //
                                                  {LIS_INT1_DURATION, 64, 0}};

bool LIS2DH12::initalize() { return FRToSI2C::writeRegistersBulk(LIS2DH_I2C_ADDRESS, i2c_registers, sizeof(i2c_registers) / sizeof(i2c_registers[0])); }

void LIS2DH12::getAxisReadings(int16_t &x, int16_t &y, int16_t &z) {
  std::array<int16_t, 3> sensorData;

  FRToSI2C::Mem_Read(LIS2DH_I2C_ADDRESS, 0xA8, reinterpret_cast<uint8_t *>(sensorData.begin()), sensorData.size() * sizeof(int16_t));

  x = sensorData[0];
  y = sensorData[1];
  z = sensorData[2];
}

bool LIS2DH12::detect() {
  if (!FRToSI2C::probe(LIS2DH_I2C_ADDRESS)) {
    return false;
  }
  // Read chip id to ensure its not an address collision
  uint8_t id = 0;
  if (FRToSI2C::Mem_Read(LIS2DH_I2C_ADDRESS, LIS2DH_WHOAMI_REG, &id, 1)) {
    return id == LIS2DH_WHOAMI_ID;
  }
  return false; // cant read ID
}
