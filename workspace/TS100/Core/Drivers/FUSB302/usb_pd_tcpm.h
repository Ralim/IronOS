/* Copyright 2015 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* USB Power delivery port management */

#ifndef __CROS_EC_USB_PD_TCPM_H
#define __CROS_EC_USB_PD_TCPM_H

/* List of common error codes that can be returned */
enum ec_error_list {
	/* Success - no error */
	EC_SUCCESS = 0,
	/* Unknown error */
	EC_ERROR_UNKNOWN = 1,
	/* Function not implemented yet */
	EC_ERROR_UNIMPLEMENTED = 2,
	/* Overflow error; too much input provided. */
	EC_ERROR_OVERFLOW = 3,
	/* Timeout */
	EC_ERROR_TIMEOUT = 4,
	/* Invalid argument */
	EC_ERROR_INVAL = 5,
	/* Already in use, or not ready yet */
	EC_ERROR_BUSY = 6,
	/* Access denied */
	EC_ERROR_ACCESS_DENIED = 7,
	/* Failed because component does not have power */
	EC_ERROR_NOT_POWERED = 8,
	/* Failed because component is not calibrated */
	EC_ERROR_NOT_CALIBRATED = 9,
	/* Failed because CRC error */
	EC_ERROR_CRC = 10,
	/* Invalid console command param (PARAMn means parameter n is bad) */
	EC_ERROR_PARAM1 = 11,
	EC_ERROR_PARAM2 = 12,
	EC_ERROR_PARAM3 = 13,
	EC_ERROR_PARAM4 = 14,
	EC_ERROR_PARAM5 = 15,
	EC_ERROR_PARAM6 = 16,
	EC_ERROR_PARAM7 = 17,
	EC_ERROR_PARAM8 = 18,
	EC_ERROR_PARAM9 = 19,
	/* Wrong number of params */
	EC_ERROR_PARAM_COUNT = 20,
	/* Interrupt event not handled */
	EC_ERROR_NOT_HANDLED = 21,
	/* Data has not changed */
	EC_ERROR_UNCHANGED = 22,
	/* Memory allocation */
	EC_ERROR_MEMORY_ALLOCATION = 23,

	/* Verified boot errors */
	EC_ERROR_VBOOT_SIGNATURE = 0x1000, /* 4096 */
	EC_ERROR_VBOOT_SIG_MAGIC = 0x1001,
	EC_ERROR_VBOOT_SIG_SIZE = 0x1002,
	EC_ERROR_VBOOT_SIG_ALGORITHM = 0x1003,
	EC_ERROR_VBOOT_HASH_ALGORITHM = 0x1004,
	EC_ERROR_VBOOT_SIG_OFFSET = 0x1005,
	EC_ERROR_VBOOT_DATA_SIZE = 0x1006,

	/* Verified boot key errors */
	EC_ERROR_VBOOT_KEY = 0x1100,
	EC_ERROR_VBOOT_KEY_MAGIC = 0x1101,
	EC_ERROR_VBOOT_KEY_SIZE = 0x1102,

	/* Verified boot data errors */
	EC_ERROR_VBOOT_DATA = 0x1200,
	EC_ERROR_VBOOT_DATA_VERIFY = 0x1201,

	/* Module-internal error codes may use this range.   */
	EC_ERROR_INTERNAL_FIRST = 0x10000,
	EC_ERROR_INTERNAL_LAST =  0x1FFFF
};

/* Flags for i2c_xfer() */
#define I2C_XFER_START (1 << 0)  /* Start smbus session from idle state */
#define I2C_XFER_STOP (1 << 1)  /* Terminate smbus session with stop bit */
#define I2C_XFER_SINGLE (I2C_XFER_START | I2C_XFER_STOP)  /* One transaction */

/* Default retry count for transmitting */
#define PD_RETRY_COUNT 3

/* Time to wait for TCPC to complete transmit */
#define PD_T_TCPC_TX_TIMEOUT  (100*MSEC)

enum tcpc_cc_voltage_status {
	TYPEC_CC_VOLT_OPEN = 0,
	TYPEC_CC_VOLT_RA = 1,
	TYPEC_CC_VOLT_RD = 2,
	TYPEC_CC_VOLT_SNK_DEF = 5,
	TYPEC_CC_VOLT_SNK_1_5 = 6,
	TYPEC_CC_VOLT_SNK_3_0 = 7,
};

enum tcpc_cc_pull {
	TYPEC_CC_RA = 0,
	TYPEC_CC_RP = 1,
	TYPEC_CC_RD = 2,
	TYPEC_CC_OPEN = 3,
};

enum tcpc_rp_value {
	TYPEC_RP_USB = 0,
	TYPEC_RP_1A5 = 1,
	TYPEC_RP_3A0 = 2,
	TYPEC_RP_RESERVED = 3,
};

enum tcpm_transmit_type {
	TCPC_TX_SOP = 0,
	TCPC_TX_SOP_PRIME = 1,
	TCPC_TX_SOP_PRIME_PRIME = 2,
	TCPC_TX_SOP_DEBUG_PRIME = 3,
	TCPC_TX_SOP_DEBUG_PRIME_PRIME = 4,
	TCPC_TX_HARD_RESET = 5,
	TCPC_TX_CABLE_RESET = 6,
	TCPC_TX_BIST_MODE_2 = 7
};

enum tcpc_transmit_complete {
	TCPC_TX_COMPLETE_SUCCESS =   0,
	TCPC_TX_COMPLETE_DISCARDED = 1,
	TCPC_TX_COMPLETE_FAILED =    2,
};

struct tcpm_drv {
	/**
	 * Initialize TCPM driver and wait for TCPC readiness.
	 *
	 * @param port Type-C port number
	 *
	 * @return EC_SUCCESS or error
	 */
	int (*init)(int port);

	/**
	 * Release the TCPM hardware and disconnect the driver.
	 * Only .init() can be called after .release().
	 *
	 * @param port Type-C port number
	 *
	 * @return EC_SUCCESS or error
	 */
	int (*release)(int port);

	/**
	 * Read the CC line status.
	 *
	 * @param port Type-C port number
	 * @param cc1 pointer to CC status for CC1
	 * @param cc2 pointer to CC status for CC2
	 *
	 * @return EC_SUCCESS or error
	 */
	int (*get_cc)(int port, int *cc1, int *cc2);

	/**
	 * Read VBUS
	 *
	 * @param port Type-C port number
	 *
	 * @return 0 => VBUS not detected, 1 => VBUS detected
	 */
	int (*get_vbus_level)(int port);

	/**
	 * Set the value of the CC pull-up used when we are a source.
	 *
	 * @param port Type-C port number
	 * @param rp One of enum tcpc_rp_value
	 *
	 * @return EC_SUCCESS or error
	 */
	int (*select_rp_value)(int port, int rp);

	/**
	 * Set the CC pull resistor. This sets our role as either source or sink.
	 *
	 * @param port Type-C port number
	 * @param pull One of enum tcpc_cc_pull
	 *
	 * @return EC_SUCCESS or error
	 */
	int (*set_cc)(int port, int pull);

	/**
	 * Set polarity
	 *
	 * @param port Type-C port number
	 * @param polarity 0=> transmit on CC1, 1=> transmit on CC2
	 *
	 * @return EC_SUCCESS or error
	 */
	int (*set_polarity)(int port, int polarity);

	/**
	 * Set Vconn.
	 *
	 * @param port Type-C port number
	 * @param polarity Polarity of the CC line to read
	 *
	 * @return EC_SUCCESS or error
	 */
	int (*set_vconn)(int port, int enable);

	/**
	 * Set PD message header to use for goodCRC
	 *
	 * @param port Type-C port number
	 * @param power_role Power role to use in header
	 * @param data_role Data role to use in header
	 *
	 * @return EC_SUCCESS or error
	 */
	int (*set_msg_header)(int port, int power_role, int data_role);

	/**
	 * Set RX enable flag
	 *
	 * @param port Type-C port number
	 * @enable true for enable, false for disable
	 *
	 * @return EC_SUCCESS or error
	 */
	int (*set_rx_enable)(int port, int enable);

	/**
	 * Read last received PD message.
	 *
	 * @param port Type-C port number
	 * @param payload Pointer to location to copy payload of message
	 * @param header of message
	 *
	 * @return EC_SUCCESS or error
	 */
	int (*get_message)(int port, uint32_t *payload, int *head);

	/**
	 * Transmit PD message
	 *
	 * @param port Type-C port number
	 * @param type Transmit type
	 * @param header Packet header
	 * @param cnt Number of bytes in payload
	 * @param data Payload
	 *
	 * @return EC_SUCCESS or error
	 */
	int (*transmit)(int port, enum tcpm_transmit_type type, uint16_t header,
					const uint32_t *data);

	/**
	 * TCPC is asserting alert
	 *
	 * @param port Type-C port number
	 */
	void (*tcpc_alert)(int port);

	/**
	 * Discharge PD VBUS on src/sink disconnect & power role swap
	 *
	 * @param port Type-C port number
	 * @param enable Discharge enable or disable
	 */
	void (*tcpc_discharge_vbus)(int port, int enable);

#ifdef CONFIG_USB_PD_DUAL_ROLE_AUTO_TOGGLE
	/**
	 * Enable TCPC auto DRP toggling.
	 *
	 * @param port Type-C port number
	 * @param enable 1: Enable 0: Disable
	 *
	 * @return EC_SUCCESS or error
	 */
	int (*drp_toggle)(int port, int enable);
#endif

	/**
	 * Get firmware version.
	 *
	 * @param port Type-C port number
	 * @param renew Force renewal
	 * @param info Pointer to pointer to PD chip info
	 *
	 * @return EC_SUCCESS or error
	 */
	int (*get_chip_info)(int port, int renew,
			struct ec_response_pd_chip_info **info);
};

enum tcpc_alert_polarity {
	TCPC_ALERT_ACTIVE_LOW,
	TCPC_ALERT_ACTIVE_HIGH,
};

struct tcpc_config_t {
	int i2c_host_port;
	int i2c_slave_addr;
	const struct tcpm_drv *drv;
	enum tcpc_alert_polarity pol;
};

/**
 * Returns the PD_STATUS_TCPC_ALERT_* mask corresponding to the TCPC ports
 * that are currently asserting ALERT.
 *
 * @return     PD_STATUS_TCPC_ALERT_* mask.
 */
uint16_t tcpc_get_alert_status(void);

/**
 * Optional, set the TCPC power mode.
 *
 * @param port Type-C port number
 * @param mode 0: off/sleep, 1: on/awake
 */
void board_set_tcpc_power_mode(int port, int mode) __attribute__((weak));

/**
 * Initialize TCPC.
 *
 * @param port Type-C port number
 */
void tcpc_init(int port);

/**
 * TCPC is asserting alert
 *
 * @param port Type-C port number
 */
void tcpc_alert_clear(int port);

/**
 * Run TCPC task once. This checks for incoming messages, processes
 * any outgoing messages, and reads CC lines.
 *
 * @param port Type-C port number
 * @param evt Event type that woke up this task
 */
int tcpc_run(int port, int evt);

/**
 * Initialize board specific TCPC functions post TCPC initialization.
 *
 * @param port Type-C port number
 *
 * @return EC_SUCCESS or error
 */
int board_tcpc_post_init(int port) __attribute__((weak));

#endif /* __CROS_EC_USB_PD_TCPM_H */
