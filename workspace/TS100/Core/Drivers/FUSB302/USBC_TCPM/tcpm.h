/* Copyright 2015 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* USB Power delivery port management - common header for TCPM drivers */

#ifndef __CROS_EC_USB_PD_TCPM_TCPM_H
#define __CROS_EC_USB_PD_TCPM_TCPM_H

#include "tcpm_driver.h"
#include "usb_pd_tcpm.h"

#if defined(CONFIG_USB_PD_DUAL_ROLE_AUTO_TOGGLE) && \
	!defined(CONFIG_USB_PD_DUAL_ROLE)
#error "DRP auto toggle requires board to have DRP support"
#error "Please upgrade your board configuration"
#endif

#ifndef CONFIG_USB_PD_TCPC
extern const struct tcpc_config_t tcpc_config[];

/* I2C wrapper functions - get I2C port / slave addr from config struct. */
int tcpc_write(int port, int reg, int val);
int tcpc_write16(int port, int reg, int val);
int tcpc_read(int port, int reg, int *val);
int tcpc_read16(int port, int reg, int *val);
int tcpc_xfer(int port,
		const uint8_t *out, int out_size,
		uint8_t *in, int in_size,
		int flags);

/* TCPM driver wrapper function */
static inline int tcpm_init(int port)
{
	int rv;

	rv = tcpc_config[port].drv->init(port);
	if (rv)
		return rv;

	/* Board specific post TCPC init */
	if (board_tcpc_post_init)
		rv = board_tcpc_post_init(port);

	return rv;
}

static inline int tcpm_release(int port)
{
	return tcpc_config[port].drv->release(port);
}

static inline int tcpm_get_cc(int port, int *cc1, int *cc2)
{
	return tcpc_config[port].drv->get_cc(port, cc1, cc2);
}

static inline int tcpm_get_vbus_level(int port)
{
	return tcpc_config[port].drv->get_vbus_level(port);
}

static inline int tcpm_select_rp_value(int port, int rp)
{
	return tcpc_config[port].drv->select_rp_value(port, rp);
}

static inline int tcpm_set_cc(int port, int pull)
{
	return tcpc_config[port].drv->set_cc(port, pull);
}

static inline int tcpm_set_polarity(int port, int polarity)
{
	return tcpc_config[port].drv->set_polarity(port, polarity);
}

static inline int tcpm_set_vconn(int port, int enable)
{
	return tcpc_config[port].drv->set_vconn(port, enable);
}

static inline int tcpm_set_msg_header(int port, int power_role, int data_role)
{
	return tcpc_config[port].drv->set_msg_header(port, power_role,
						     data_role);
}

static inline int tcpm_set_rx_enable(int port, int enable)
{
	return tcpc_config[port].drv->set_rx_enable(port, enable);
}

static inline int tcpm_get_message(int port, uint32_t *payload, int *head)
{
	return tcpc_config[port].drv->get_message(port, payload, head);
}

static inline int tcpm_transmit(int port, enum tcpm_transmit_type type,
		  uint16_t header, const uint32_t *data)
{
	return tcpc_config[port].drv->transmit(port, type, header, data);
}

static inline void tcpc_alert(int port)
{
	tcpc_config[port].drv->tcpc_alert(port);
}

static inline void tcpc_discharge_vbus(int port, int enable)
{
	tcpc_config[port].drv->tcpc_discharge_vbus(port, enable);
}

#ifdef CONFIG_USB_PD_DUAL_ROLE_AUTO_TOGGLE
static inline int tcpm_auto_toggle_supported(int port)
{
	return !!tcpc_config[port].drv->drp_toggle;
}

static inline int tcpm_set_drp_toggle(int port, int enable)
{
	return tcpc_config[port].drv->drp_toggle(port, enable);
}
#endif

#ifdef CONFIG_CMD_I2C_STRESS_TEST_TCPC
static inline int tcpc_i2c_read(const int port, const int addr,
				const int reg, int *data)
{
	return tcpc_read(port, reg, data);
}

static inline int tcpc_i2c_write(const int port, const int addr,
				 const int reg, int data)
{
	return tcpc_write(port, reg, data);
}
#endif

static inline int tcpm_get_chip_info(int port, int renew,
				     struct ec_response_pd_chip_info **info)
{
	if (tcpc_config[port].drv->get_chip_info)
		return tcpc_config[port].drv->get_chip_info(port, renew, info);
	return EC_ERROR_UNIMPLEMENTED;
}

#else

/**
 * Initialize TCPM driver and wait for TCPC readiness.
 *
 * @param port Type-C port number
 *
 * @return EC_SUCCESS or error
 */
int tcpm_init(int port);

/**
 * Read the CC line status.
 *
 * @param port Type-C port number
 * @param cc1 pointer to CC status for CC1
 * @param cc2 pointer to CC status for CC2
 *
 * @return EC_SUCCESS or error
 */
int tcpm_get_cc(int port, int *cc1, int *cc2);

/**
 * Read VBUS
 *
 * @param port Type-C port number
 *
 * @return 0 => VBUS not detected, 1 => VBUS detected
 */
int tcpm_get_vbus_level(int port);

/**
 * Set the value of the CC pull-up used when we are a source.
 *
 * @param port Type-C port number
 * @param rp One of enum tcpc_rp_value
 *
 * @return EC_SUCCESS or error
 */
int tcpm_select_rp_value(int port, int rp);

/**
 * Set the CC pull resistor. This sets our role as either source or sink.
 *
 * @param port Type-C port number
 * @param pull One of enum tcpc_cc_pull
 *
 * @return EC_SUCCESS or error
 */
int tcpm_set_cc(int port, int pull);

/**
 * Set polarity
 *
 * @param port Type-C port number
 * @param polarity 0=> transmit on CC1, 1=> transmit on CC2
 *
 * @return EC_SUCCESS or error
 */
int tcpm_set_polarity(int port, int polarity);

/**
 * Set Vconn.
 *
 * @param port Type-C port number
 * @param polarity Polarity of the CC line to read
 *
 * @return EC_SUCCESS or error
 */
int tcpm_set_vconn(int port, int enable);

/**
 * Set PD message header to use for goodCRC
 *
 * @param port Type-C port number
 * @param power_role Power role to use in header
 * @param data_role Data role to use in header
 *
 * @return EC_SUCCESS or error
 */
int tcpm_set_msg_header(int port, int power_role, int data_role);

/**
 * Set RX enable flag
 *
 * @param port Type-C port number
 * @enable true for enable, false for disable
 *
 * @return EC_SUCCESS or error
 */
int tcpm_set_rx_enable(int port, int enable);

/**
 * Read last received PD message.
 *
 * @param port Type-C port number
 * @param payload Pointer to location to copy payload of message
 * @param header of message
 *
 * @return EC_SUCCESS or error
 */
int tcpm_get_message(int port, uint32_t *payload, int *head);

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
int tcpm_transmit(int port, enum tcpm_transmit_type type, uint16_t header,
		  const uint32_t *data);

/**
 * TCPC is asserting alert
 *
 * @param port Type-C port number
 */
void tcpc_alert(int port);

#endif

#endif
