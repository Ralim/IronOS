/*
 * fusbpd.h
 *
 *  Created on: 13 Jun 2020
 *      Author: Ralim
 */

#ifndef DRIVERS_FUSB302_FUSBPD_H_
#define DRIVERS_FUSB302_FUSBPD_H_
//Wrapper for all of the FUSB302 PD work
extern struct pdb_config pdb_config_data;
#include <stdint.h>

//returns 1 if the FUSB302 is on the I2C bus
uint8_t fusb302_detect();

void fusb302_start_processing();
#endif /* DRIVERS_FUSB302_FUSBPD_H_ */
