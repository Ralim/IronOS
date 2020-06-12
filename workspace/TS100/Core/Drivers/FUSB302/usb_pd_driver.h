/*
 * usb_pd_driver.h
 *
 * Created: 11/11/2017 23:55:25
 *  Author: jason
 */ 


#ifndef USB_PD_DRIVER_H_
#define USB_PD_DRIVER_H_

#include "USBC_PD/usb_pd.h"
#include "cmsis_os.h"
#include <stdint.h>

//#define CONFIG_BBRAM
#define CONFIG_CHARGE_MANAGER
//#define CONFIG_USBC_BACKWARDS_COMPATIBLE_DFP
//#define CONFIG_USBC_VCONN_SWAP
#define CONFIG_USB_PD_ALT_MODE
//#define CONFIG_USB_PD_CHROMEOS
#define CONFIG_USB_PD_DUAL_ROLE
//#define CONFIG_USB_PD_GIVE_BACK
//#define CONFIG_USB_PD_SIMPLE_DFP
//#define CONFIG_USB_PD_TCPM_TCPCI
#define PD_PREFER_HIGH_VOLTAGE

/* Default pull-up value on the USB-C ports when they are used as source. */
#define CONFIG_USB_PD_PULLUP TYPEC_RP_USB

/* Override PD_ROLE_DEFAULT in usb_pd.h */
#define PD_ROLE_DEFAULT(port) (PD_ROLE_SINK)

/* Don't automatically change roles */
#undef CONFIG_USB_PD_INITIAL_DRP_STATE
#define CONFIG_USB_PD_INITIAL_DRP_STATE PD_DRP_FREEZE

/* Prefer higher voltage, and more importantly, lower current */
#define PD_PREFER_HIGH_VOLTAGE
//#define PD_PREFER_LOW_VOLTAGE

/* board specific type-C power constants */
/*
 * delay to turn on the power supply max is ~16ms.
 * delay to turn off the power supply max is about ~180ms.
 */
#define PD_POWER_SUPPLY_TURN_ON_DELAY  10000  /* us */
#define PD_POWER_SUPPLY_TURN_OFF_DELAY 20000 /* us */

/* Define typical operating power and max power */
#define PD_OPERATING_POWER_MW 15000
#define PD_MAX_POWER_MW       100000
#define PD_MAX_CURRENT_MA     5000
#define PD_MAX_VOLTAGE_MV     20000

#define PDO_FIXED_FLAGS (PDO_FIXED_DUAL_ROLE | PDO_FIXED_DATA_SWAP |\
PDO_FIXED_COMM_CAP)

/* USB configuration */
#define CONFIG_USB_PID 0x502f // Stolen, so should change for anything useful
#define CONFIG_USB_BCD_DEV 0x0001 /* v 0.01 */

/* Optional features */
#define CONFIG_USB_PD_IDENTITY_HW_VERS 1
#define CONFIG_USB_PD_IDENTITY_SW_VERS 1

#define usleep(us) (osDelay(1))
#define msleep(us) (osDelay(us))

typedef union {
	uint64_t val;
	struct {
		uint32_t lo;
		uint32_t hi;
		} le /* little endian words */;
	} timestamp_t;

uint32_t pd_task_set_event(uint32_t event, int wait_for_reply);
void pd_power_supply_reset(int port);

// Get the current timestamp from the system timer.
timestamp_t get_time(void);

/* Standard macros / definitions */
#ifndef MAX
#define MAX(a, b)					\
({						\
	__typeof__(a) temp_a = (a);		\
	__typeof__(b) temp_b = (b);		\
	\
	temp_a > temp_b ? temp_a : temp_b;	\
})
#endif
#ifndef MIN
#define MIN(a, b)					\
({						\
	__typeof__(a) temp_a = (a);		\
	__typeof__(b) temp_b = (b);		\
	\
	temp_a < temp_b ? temp_a : temp_b;	\
})
#endif

#endif /* USB_PD_DRIVER_H_ */
