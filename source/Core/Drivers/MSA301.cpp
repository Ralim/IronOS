/*
 * MSA301.cpp
 *
 *  Created on: 3 Jan 2021
 *      Author: Ralim
 */

#include "MSA301_defines.h"
#include "accelerometers_common.h"
#include <MSA301.h>

#define MSA301_I2C_ADDRESS 0x26 << 1
bool MSA301::detect() { return ACCEL_I2C_CLASS::probe(MSA301_I2C_ADDRESS); }

static const ACCEL_I2C_CLASS::I2C_REG i2c_registers[] = {
  //
  //
    {      MSA301_REG_ODR, 0b00001000, 1}, // X/Y/Z enabled @ 250Hz
    {MSA301_REG_POWERMODE,  0b0001001, 1}, // Normal mode
    { MSA301_REG_RESRANGE, 0b00000001, 0}, // 14bit resolution @ 4G range
    {MSA301_REG_ORIENT_HY, 0b01000000, 0}, // 4*62.5mg hyst, no blocking, symmetrical
    {  MSA301_REG_INTSET0,     1 << 6, 0}, // Turn on orientation detection (by enabling its interrupt)
};

bool MSA301::initalize() { return ACCEL_I2C_CLASS::writeRegistersBulk(MSA301_I2C_ADDRESS, i2c_registers, sizeof(i2c_registers) / sizeof(i2c_registers[0])); }

Orientation MSA301::getOrientation() {
  uint8_t temp = 0;
  ACCEL_I2C_CLASS::Mem_Read(MSA301_I2C_ADDRESS, MSA301_REG_ORIENT_STATUS, &temp, 1);
  switch (temp) {
  case 112:
    return Orientation::ORIENTATION_LEFT_HAND;
  case 96:
    return Orientation::ORIENTATION_RIGHT_HAND;
  default:
    return Orientation::ORIENTATION_FLAT;
  }
}

void MSA301::getAxisReadings(int16_t &x, int16_t &y, int16_t &z) {
  uint8_t temp[6];
  // Bulk read all 6 regs
  ACCEL_I2C_CLASS::Mem_Read(MSA301_I2C_ADDRESS, MSA301_REG_OUT_X_L, temp, 6);
  x = int16_t(((int16_t)temp[1]) << 8 | temp[0]) >> 2;
  y = int16_t(((int16_t)temp[3]) << 8 | temp[2]) >> 2;
  z = int16_t(((int16_t)temp[5]) << 8 | temp[4]) >> 2;
}
