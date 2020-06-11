/*
 * tcpm_driver.c
 *
 * Created: 11/11/2017 18:42:26
 *  Author: jason
 */

#include "tcpm_driver.h"
#include "I2C_Wrapper.hpp"
extern const struct tcpc_config_t tcpc_config[CONFIG_USB_PD_PORT_COUNT];
#define STATUS_OK 0
/* I2C wrapper functions - get I2C port / slave addr from config struct. */
int tcpc_write(int port, int reg, int val) {

	FRToSI2C::Mem_Write(tcpc_config[port].i2c_slave_addr, reg, (uint8_t*) &val,
			1);
	return STATUS_OK;
}

int tcpc_write16(int port, int reg, int val) {
	uint8_t data[2];
	data[0] = (0xFF) & val;
	data[1] = (0xFF) & (val >> 8);
	FRToSI2C::Mem_Write(tcpc_config[port].i2c_slave_addr, reg, (uint8_t*) data,
			2);
	return STATUS_OK;
}

int tcpc_read(int port, int reg, int *val) {
	uint8_t data[1];

	FRToSI2C::Mem_Read(tcpc_config[port].i2c_slave_addr, reg, (uint8_t*) data,
			1);

	*val = data[0];

	return STATUS_OK;
}

int tcpc_read16(int port, int reg, int *val) {
	uint8_t data[2];
	FRToSI2C::Mem_Write(tcpc_config[port].i2c_slave_addr, reg, (uint8_t*) data,
			2);

	*val = data[0];
	*val |= (data[1] << 8);

	return STATUS_OK;
}

int tcpc_xfer(int port, const uint8_t *out, int out_size, uint8_t *in,
		int in_size, int flags) {
	// Write out the I2C port to the given slave address
	// Write out the out byte array to the device (sending a stop if the flag is set)
	// Then issue a read from the device

	if (flags & I2C_XFER_STOP) {
		//Issuing a stop between the requests
		//Send as a Tx followed by a Rx
		FRToSI2C::Transmit(tcpc_config[port].i2c_slave_addr, (uint8_t*)out, out_size);
		FRToSI2C::Receive(tcpc_config[port].i2c_slave_addr, in, in_size);
	} else {
		//issue as a continious transmit & recieve
		FRToSI2C::TransmitReceive(tcpc_config[port].i2c_slave_addr,  (uint8_t*)out,
				out_size, in, in_size);
	}

	return STATUS_OK;
}
