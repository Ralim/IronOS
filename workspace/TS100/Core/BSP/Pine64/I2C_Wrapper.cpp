/*
 * FRToSI2C.cpp
 *
 *  Created on: 14Apr.,2018
 *      Author: Ralim
 */
#include "BSP.h"
#include "Setup.h"
#include <I2C_Wrapper.hpp>
#define I2CUSESDMA
SemaphoreHandle_t FRToSI2C::I2CSemaphore;
StaticSemaphore_t FRToSI2C::xSemaphoreBuffer;
#define FLAG_TIMEOUT 1000

void FRToSI2C::CpltCallback()
{
	//TODO
}

/** Send START command
 *
 *  @param obj The I2C object
 */
int i2c_start()
{
	int timeout;

	/* clear I2C_FLAG_AERR Flag */
	i2c_flag_clear(I2C0, I2C_FLAG_AERR);

	/* wait until I2C_FLAG_I2CBSY flag is reset */
	timeout = FLAG_TIMEOUT;
	while ((i2c_flag_get(I2C0, I2C_FLAG_I2CBSY)) == SET)
	{
		if ((timeout--) == 0)
		{

			return (int)-1;
		}
	}

	/* ensure the i2c has been stopped */
	timeout = FLAG_TIMEOUT;
	while ((I2C_CTL0(I2C0) & I2C_CTL0_STOP) == I2C_CTL0_STOP)
	{
		if ((timeout--) == 0)
		{
			return (int)-1;
		}
	}

	/* generate a START condition */
	i2c_start_on_bus(I2C0);

	/* ensure the i2c has been started successfully */
	timeout = FLAG_TIMEOUT;
	while ((i2c_flag_get(I2C0, I2C_FLAG_SBSEND)) == RESET)
	{
		if ((timeout--) == 0)
		{
			return (int)-1;
		}
	}

	return (int)0;
}

/** Send STOP command
 *
 *  @param obj The I2C object
 */
int i2c_stop()
{

	/* generate a STOP condition */
	i2c_stop_on_bus(I2C0);

	/* wait for STOP bit reset */
	int timeout = FLAG_TIMEOUT;
	while ((I2C_CTL0(I2C0) & I2C_CTL0_STOP))
	{
		if ((timeout--) == 0)
		{
			return -1;
		}
	}

	return 0;
}

/** Read one byte
 *
 *  @param obj The I2C object
 *  @param last Acknoledge
 *  @return The read byte
 */
int i2c_byte_read(int last)
{
	int timeout;

	if (last)
	{
		/* disable acknowledge */
		i2c_ack_config(I2C0, I2C_ACK_DISABLE);
	}
	else
	{
		/* enable acknowledge */
		i2c_ack_config(I2C0, I2C_ACK_ENABLE);
	}

	/* wait until the byte is received */
	timeout = FLAG_TIMEOUT;
	while ((i2c_flag_get(I2C0, I2C_FLAG_RBNE)) == RESET)
	{
		if ((timeout--) == 0)
		{
			return -1;
		}
	}

	return (int)i2c_data_receive(I2C0);
}

/** Write one byte
 *
 *  @param obj The I2C object
 *  @param data Byte to be written
 *  @return 0 if NAK was received, 1 if ACK was received, 2 for timeout.
 */
int i2c_byte_write(int data)
{
	int timeout;
	i2c_data_transmit(I2C0, data);

	/* wait until the byte is transmitted */
	timeout = FLAG_TIMEOUT;
	while (((i2c_flag_get(I2C0, I2C_FLAG_TBE)) == RESET) || ((i2c_flag_get(I2C0, I2C_FLAG_BTC)) == RESET))
	{
		if ((timeout--) == 0)
		{
			return 2;
		}
	}

	return 1;
}

bool FRToSI2C::Mem_Read(uint16_t DevAddress, uint16_t MemAddress,
						uint8_t *pData, uint16_t Size)
{
	if (!lock())
		return false;

	uint32_t count = 0;
	int timeout = 0;

	/* wait until I2C_FLAG_I2CBSY flag is reset */
	timeout = FLAG_TIMEOUT;
	while ((i2c_flag_get(I2C0, I2C_FLAG_I2CBSY)) == SET)
	{
		if ((timeout--) == 0)
		{
			i2c_stop();
			unlock();
			return false;
		}
		else
		{
			if (timeout % 5 == 0)
			{
				i2c_stop();
			}
		}
	}
	/* generate a START condition */
	i2c_start_on_bus(I2C0);

	/* ensure the i2c has been started successfully */
	timeout = FLAG_TIMEOUT;
	while ((i2c_flag_get(I2C0, I2C_FLAG_SBSEND)) == RESET)
	{
		if ((timeout--) == 0)
		{
			i2c_stop();
			unlock();
			return false;
		}
	}

	/* send slave address */
	i2c_master_addressing(I2C0, DevAddress, I2C_TRANSMITTER);

	timeout = 0;
	/* wait until I2C_FLAG_ADDSEND flag is set */
	while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
	{
		timeout++;
		if (timeout > 100000)
		{
			i2c_stop();
			unlock();
			return false;
		}
	}
	bool no_ack = i2c_flag_get(I2C0, I2C_FLAG_AERR);
	no_ack |= i2c_flag_get(I2C0, I2C_FLAG_BERR);
	if (no_ack)
	{
		i2c_stop();
		unlock();
		return false;
	}
	/* clear ADDSEND */
	i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
	int status = i2c_byte_write(MemAddress);
	no_ack |= i2c_flag_get(I2C0, I2C_FLAG_BERR);
	no_ack |= i2c_flag_get(I2C0, I2C_FLAG_LOSTARB);
	if (status == 2 || no_ack)
	{
		i2c_stop();
		unlock();
		return false;
	}
	////////////////////////////	//Restart into read

	/* generate a START condition */
	i2c_start_on_bus(I2C0);

	/* ensure the i2c has been started successfully */
	timeout = FLAG_TIMEOUT;
	while ((i2c_flag_get(I2C0, I2C_FLAG_SBSEND)) == RESET)
	{
		if ((timeout--) == 0)
		{
			i2c_stop();
			unlock();
			return false;
		}
	}

	/* send slave address */
	i2c_master_addressing(I2C0, DevAddress, I2C_RECEIVER);

	timeout = 0;
	/* wait until I2C_FLAG_ADDSEND flag is set */
	while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
	{
		timeout++;
		if (timeout > 100000)
		{
			i2c_stop();
			unlock();
			return false;
		}
	}

	/* clear ADDSEND */
	i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
	no_ack = i2c_flag_get(I2C0, I2C_FLAG_AERR);
	if (no_ack)
	{
		i2c_stop();
		unlock();
		return false;
	}
	for (count = 0; count < Size; count++)
	{
		pData[count] = i2c_byte_read(count == (uint32_t)(Size - 1));
	}

	/* if not sequential write, then send stop */

	i2c_stop();
	unlock();
	return true;
}
void FRToSI2C::I2C_RegisterWrite(uint8_t address, uint8_t reg, uint8_t data)
{
	Mem_Write(address, reg, &data, 1);
}

uint8_t FRToSI2C::I2C_RegisterRead(uint8_t add, uint8_t reg)
{
	uint8_t temp = 0;
	Mem_Read(add, reg, &temp, 1);
	return temp;
}
void FRToSI2C::Mem_Write(uint16_t DevAddress, uint16_t MemAddress,
						 uint8_t *pData, uint16_t Size)
{
	if (!lock())
		return;

	uint32_t count = 0;
	int timeout = 0;

	/* wait until I2C_FLAG_I2CBSY flag is reset */
	timeout = FLAG_TIMEOUT;
	while ((i2c_flag_get(I2C0, I2C_FLAG_I2CBSY)) == SET)
	{
		if ((timeout--) == 0)
		{
			i2c_stop();
			unlock();
			return;
		}
		else
		{
			if (timeout % 5 == 0)
			{
				i2c_stop();
			}
		}
	}
	/* generate a START condition */
	i2c_start_on_bus(I2C0);

	/* ensure the i2c has been started successfully */
	timeout = FLAG_TIMEOUT;
	while ((i2c_flag_get(I2C0, I2C_FLAG_SBSEND)) == RESET)
	{
		if ((timeout--) == 0)
		{
			i2c_stop();
			unlock();
			return;
		}
	}

	/* send slave address */
	i2c_master_addressing(I2C0, DevAddress, I2C_TRANSMITTER);

	timeout = 0;
	/* wait until I2C_FLAG_ADDSEND flag is set */
	while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
	{
		timeout++;
		if (timeout > 100000)
		{
			i2c_stop();
			unlock();
			return;
		}
	}

	/* clear ADDSEND */
	i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
	int status = i2c_byte_write(MemAddress);
	for (count = 0; count < Size; count++)
	{
		status = i2c_byte_write(pData[count]);
		if (status != 1)
		{
			i2c_stop();
			unlock();
			return;
		}
	}

	/* if not sequential write, then send stop */

	i2c_stop();
	unlock();
}

void FRToSI2C::Transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
	if (!lock())
		return;
	uint32_t count = 0;
	int timeout = 0;

	/* wait until I2C_FLAG_I2CBSY flag is reset */
	timeout = FLAG_TIMEOUT;
	while ((i2c_flag_get(I2C0, I2C_FLAG_I2CBSY)) == SET)
	{
		if ((timeout--) == 0)
		{
			i2c_stop();
			unlock();
			return;
		}
		else
		{
			if (timeout % 5 == 0)
			{
				i2c_stop();
			}
		}
	}
	/* generate a START condition */
	i2c_start_on_bus(I2C0);

	/* ensure the i2c has been started successfully */
	timeout = FLAG_TIMEOUT;
	while ((i2c_flag_get(I2C0, I2C_FLAG_SBSEND)) == RESET)
	{
		if ((timeout--) == 0)
		{
			i2c_stop();
			unlock();
			return;
		}
	}

	/* send slave address */
	i2c_master_addressing(I2C0, DevAddress, I2C_TRANSMITTER);

	timeout = 0;
	/* wait until I2C_FLAG_ADDSEND flag is set */
	while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
	{
		timeout++;
		if (timeout > 100000)
		{
			i2c_stop();
			unlock();
			return;
		}
	}

	/* clear ADDSEND */
	i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

	for (count = 0; count < Size; count++)
	{
		int status = i2c_byte_write(pData[count]);
		if (status != 1)
		{
			i2c_stop();
			unlock();
			return;
		}
	}

	/* if not sequential write, then send stop */

	i2c_stop();
	unlock();
}

bool FRToSI2C::probe(uint16_t DevAddress)
{
	if (!lock())
		return false;
	i2c_start();
	/* send slave address to I2C bus */
	i2c_master_addressing(I2C0, DevAddress, I2C_TRANSMITTER);
	/* wait until ADDSEND bit is set */
	int timeout = FLAG_TIMEOUT;
	while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
	{
		if (i2c_flag_get(I2C0, I2C_FLAG_AERR) || i2c_flag_get(I2C0, I2C_FLAG_BERR))
		{
			i2c_stop();
			unlock();
			return false;
		}
		if (timeout-- == 0)
		{
			i2c_stop();
			unlock();
			return false;
		}
	}

	/* clear ADDSEND bit */
	i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
	/* wait until the transmit data buffer is empty */
	while (!i2c_flag_get(I2C0, I2C_FLAG_TBE))
		;
	bool no_ack = i2c_flag_get(I2C0, I2C_FLAG_AERR);
	/* send a stop condition to I2C bus */
	i2c_stop_on_bus(I2C0);
	/* wait until stop condition generate */
	while (I2C_CTL0(I2C0) & 0x0200)
		;
	unlock();
	return !no_ack;
}

void FRToSI2C::I2C_Unstick()
{
	unstick_I2C();
}

bool FRToSI2C::lock()
{
	if (I2CSemaphore == nullptr)
		return true;
	return xSemaphoreTake(I2CSemaphore, 1000) == pdTRUE;
}

void FRToSI2C::unlock()
{
	if (I2CSemaphore == nullptr)
		return;
	xSemaphoreGive(I2CSemaphore);
}
