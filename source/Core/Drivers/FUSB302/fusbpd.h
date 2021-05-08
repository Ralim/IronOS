/*
 * fusbpd.h
 *
 *  Created on: 13 Jun 2020
 *      Author: Ralim
 */

#ifndef DRIVERS_FUSB302_FUSBPD_H_
#define DRIVERS_FUSB302_FUSBPD_H_
// Wrapper for all of the FUSB302 PD work
extern struct pdb_config pdb_config_data;
#include <stdint.h>

void fusb302_start_processing();
#endif /* DRIVERS_FUSB302_FUSBPD_H_ */
