/*
 * FUSB302.hpp
 *
 *  Created on: 11-06-2020
 *      Author: Ralim
 */

#ifndef FUSB302_H_
#define FUSB302_H_
#include "BSP.h"
#include "FUSB302_includes.h"
#include "I2C_Wrapper.hpp"
//While the ST4500 is nice, the FUSB302 is _cheap_ so its what is used in the TS80P for example

class FUSB302 {
public:
  //Returns true if the FUSB302 is detected on the I2C bus
  bool detect();

private:
  // Bring up out of reset and clear fifo's
  void fusb302_reset();
  void flush_rx_fifo();
  void flush_tx_fifo();
  void enable_auto_good_crc();
  int set_rp_value(int rp);
  int set_cc(int pull);
  struct fusb302_chip_state {
    int cc_polarity;
    int vconn_enabled;
    /* 1 = pulling up (DFP) 0 = pulling down (UFP) */
    int pulling_up;
    int rx_enable;
    uint8_t mdac_vnc;
    uint8_t mdac_rd;
  };
  fusb302_chip_state state;
};

#endif /* LIS2DH12_HPP_ */
