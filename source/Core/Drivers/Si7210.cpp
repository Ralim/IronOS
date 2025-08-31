/*
 * Si7210.cpp
 *
 *  Created on: 5 Oct. 2020
 *      Author: Ralim
 *
 *      This is based on the very nice sample code by Sean Farrelly (@FARLY7)
 *      Over here : https://github.com/FARLY7/si7210-driver
 *
 *      This class is licensed as MIT to match this code base
 */

#include "Si7210_defines.h"
#include "accelerometers_common.h"
#include <Si7210.h>
#ifdef MAG_SLEEP_SUPPORT
bool Si7210::detect() { return FRToSI2C::wakePart(SI7210_ADDRESS); }

bool Si7210::init() {
  // Turn on auto increment and sanity check ID
  // Load OTP cal

  uint8_t temp;
  if (ACCEL_I2C_CLASS::Mem_Read(SI7210_ADDRESS, SI7210_REG_ID, &temp, 1)) {
    // We don't really care what model it is etc, just probing to check its probably this iC
    if (temp != 0x00 && temp != 0xFF) {
      temp = 0x00;

      /* Set device and internal driver settings */
      if (!write_reg(SI7210_CTRL1, (uint8_t)~SW_LOW4FIELD_MASK, 0)) {
        return false;
      }

      /* Disable periodic auto-wakeup by device, and tamper detect. */
      if ((!write_reg(SI7210_CTRL3, (uint8_t)~SL_TIMEENA_MASK, 0))) {
        return false;
      }

      /* Disable tamper detection by setting sw_tamper to 63 */
      if (!write_reg(SI7210_CTRL3, SL_FAST_MASK | SL_TIMEENA_MASK, 63 << 2)) {
        return false;
      }

      if (!set_high_range()) {
        return false;
      }

      /* Stop the control loop by setting stop bit */
      if (!write_reg(SI7210_POWER_CTRL, MEAS_MASK | USESTORE_MASK, STOP_MASK)) { /* WARNING: Removed USE_STORE MASK */
        return false;
      }

      /* Use a burst size of 128/4096 samples in FIR and IIR modes */
      if (!write_reg(SI7210_CTRL4, 0, DF_BURSTSIZE_128 | DF_BW_4096)) {
        return false;
      }

      /* Select field strength measurement */
      if (!write_reg(SI7210_DSPSIGSEL, 0, DSP_SIGSEL_FIELD_MASK)) {
        return false;
      }

      return true; // start_periodic_measurement();
    }
  }
  return false;
}

int16_t Si7210::read() {
  // Read the two regs
  int16_t temp = 0;
  if (!get_field_strength(&temp)) {
    temp = 0;
  }
  return temp;
}

bool Si7210::write_reg(const uint8_t reg, const uint8_t mask, const uint8_t val) {
  uint8_t temp = 0;
  if (mask) {
    if (!read_reg(reg, &temp)) {
      return false;
    }
    temp &= mask;
  }
  temp |= val;
  return ACCEL_I2C_CLASS::Mem_Write(SI7210_ADDRESS, reg, &temp, 1);
}

bool Si7210::read_reg(const uint8_t reg, uint8_t *val) { return ACCEL_I2C_CLASS::Mem_Read(SI7210_ADDRESS, reg, val, 1); }

bool Si7210::start_periodic_measurement() {
  /* Enable periodic wakeup */
  if (!write_reg(SI7210_CTRL3, (uint8_t)~SL_TIMEENA_MASK, SL_TIMEENA_MASK)) {
    return false;
  }

  /* Start measurement */
  /* Change to ~STOP_MASK with STOP_MASK */
  return write_reg(SI7210_POWER_CTRL, MEAS_MASK | USESTORE_MASK, 0);
}

bool Si7210::get_field_strength(int16_t *field) {
  *field      = 0;
  uint8_t val = 0;
  ACCEL_I2C_CLASS::wakePart(SI7210_ADDRESS);

  if (!write_reg(SI7210_POWER_CTRL, MEAS_MASK | USESTORE_MASK, STOP_MASK)) {
    return false;
  }

  /* Read most-significant byte */
  if (!read_reg(SI7210_DSPSIGM, &val)) {
    return false;
  }
  *field = (val & DSP_SIGM_DATA_MASK) << 8;

  /* Read least-significant byte of data */
  if (!read_reg(SI7210_DSPSIGL, &val)) {
    return false;
  }

  *field += val;
  *field -= 16384U;
  // field is now a +- measurement
  // In units of 0.0125 mT
  // Aka 12.5uT
  // Clear flags
  read_reg(SI7210_CTRL1, &val);
  read_reg(SI7210_CTRL2, &val);
  // Start next one

  /* Use a burst size of 128/4096 samples in FIR and IIR modes */
  write_reg(SI7210_CTRL4, 0, DF_BURSTSIZE_128 | DF_BW_4096);

  /* Selet field strength measurement */
  write_reg(SI7210_DSPSIGSEL, 0, DSP_SIGSEL_FIELD_MASK);

  /* Start measurement */
  write_reg(SI7210_POWER_CTRL, MEAS_MASK | USESTORE_MASK, ONEBURST_MASK);

  return true;
}

bool Si7210::set_high_range() {
  // To set the unit into 200mT range, no magnet temperature calibration
  // We want to copy OTP 0x27->0x2C into a0->a5
  uint8_t base_addr = 0x27; // You can change this to pick the temp calibration
  bool    worked    = true;
  uint8_t val       = 0;

  /* Load A0 register */
  worked &= write_reg(SI7210_OTP_ADDR, 0, base_addr);
  worked &= write_reg(SI7210_OTP_CTRL, 0, OTP_READ_EN_MASK);
  worked &= read_reg(SI7210_OTP_DATA, &val);
  worked &= write_reg(SI7210_A0, 0, val);

  /* Load A1 register */
  worked &= write_reg(SI7210_OTP_ADDR, 0, base_addr + 1);
  worked &= write_reg(SI7210_OTP_CTRL, 0, OTP_READ_EN_MASK);
  worked &= read_reg(SI7210_OTP_DATA, &val);
  worked &= write_reg(SI7210_A1, 0, val);

  /* Load A2 register */
  worked &= write_reg(SI7210_OTP_ADDR, 0, base_addr + 2);
  worked &= write_reg(SI7210_OTP_CTRL, 0, OTP_READ_EN_MASK);
  worked &= read_reg(SI7210_OTP_DATA, &val);
  worked &= write_reg(SI7210_A2, 0, val);

  /* Load A3 register */
  worked &= write_reg(SI7210_OTP_ADDR, 0, base_addr + 3);
  worked &= write_reg(SI7210_OTP_CTRL, 0, OTP_READ_EN_MASK);
  worked &= read_reg(SI7210_OTP_DATA, &val);
  worked &= write_reg(SI7210_A3, 0, val);

  /* Load A4 register */
  worked &= write_reg(SI7210_OTP_ADDR, 0, base_addr + 4);
  worked &= write_reg(SI7210_OTP_CTRL, 0, OTP_READ_EN_MASK);
  worked &= read_reg(SI7210_OTP_DATA, &val);
  worked &= write_reg(SI7210_A4, 0, val);

  /* Load A5 register */
  worked &= write_reg(SI7210_OTP_ADDR, 0, base_addr + 5);
  worked &= write_reg(SI7210_OTP_CTRL, 0, OTP_READ_EN_MASK);
  worked &= read_reg(SI7210_OTP_DATA, &val);
  worked &= write_reg(SI7210_A5, 0, val);
  return worked;
}
#endif // MAG_SLEEP_SUPPORT