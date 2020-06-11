/*
 * tcpm_driver.h
 *
 * Created: 11/11/2017 18:42:39
 *  Author: jason
 */ 


#ifndef TCPM_DRIVER_H_
#define TCPM_DRIVER_H_

#include <stdint.h>

// USB-C Stuff
#include "USBC_TCPM/tcpm.h"
#include "FUSB302.h"
#define CONFIG_USB_PD_PORT_COUNT 1
extern struct i2c_master_module i2c_master_instance;



#endif /* TCPM_DRIVER_H_ */
