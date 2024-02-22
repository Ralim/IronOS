/*
 * Si7210.h
 *
 *  Created on: 5 Oct. 2020
 *      Author: Ralim
 */

#ifndef CORE_DRIVERS_SI7210_H_
#define CORE_DRIVERS_SI7210_H_
#include "configuration.h"
#include <stdint.h>

#ifdef MAG_SLEEP_SUPPORT
class Si7210 {
public:
  // Return true if present
  static bool detect();

  static bool    init();
  static int16_t read();

private:
  static bool write_reg(const uint8_t reg, const uint8_t mask, const uint8_t val);
  static bool read_reg(const uint8_t reg, uint8_t *val);
  static bool start_periodic_measurement();
  static bool get_field_strength(int16_t *field);
  static bool set_high_range();
};
#endif // MAG_SLEEP_SUPPORT
#endif /* CORE_DRIVERS_SI7210_H_ */
