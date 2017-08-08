/*
 * I2C.h hardware interface class
 * Based on the STM32 app note AN2824
 */
#include "I2C.h"

/* I2C STOP mask */
#define CR1_STOP_Set            ((uint16_t)0x0200)
#define CR1_STOP_Reset          ((uint16_t)0xFDFF)

/* I2C ACK mask */
#define CR1_ACK_Set             ((uint16_t)0x0400)
#define CR1_ACK_Reset           ((uint16_t)0xFBFF)

/* I2C POS mask */
#define CR1_POS_Set             ((uint16_t)0x0800)
#define CR1_POS_Reset           ((uint16_t)0xF7FF)

#define NULL ((void *)0)
/*
 * Configure the I2C port hardware
 */
void I2C_Configuration(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	/* PB6,7 SCL and SDA */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* I2C1 configuration ------------------------------------------------------*/
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 400000; //400k
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
}

/*
 * Writes a page of data over I2C using the I2C1 peripheral in the stm32
 *
 */
void I2C_PageWrite(u8* buf, u8 nbyte, u8 deviceaddr) {

	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) {
	}
	// Intiate Start Sequence
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
	}
	// Send Address
	I2C_Send7bitAddress(I2C1, deviceaddr << 1, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
	}
	// Write first byte EV8_1
	I2C_SendData(I2C1, *buf++);

	while (--nbyte) {
		// wait on BTF
		while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF)) {
		}
		I2C_SendData(I2C1, *buf++);
	}

	while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF)) {
	}
	I2C_GenerateSTOP(I2C1, ENABLE);
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF)) {
	}

}
//Based on code from http://iamjustinwang.blogspot.com.au/2016/03/stm32f103-i2c-master-driver.html
int I2C_Master_Read(uint8_t deviceAddr, uint8_t readAddr, uint8_t* pBuffer,
		uint16_t numByteToRead) {

	__IO uint32_t temp = 0;
	volatile int I2C_TimeOut = temp;

	// /* While the bus is busy * /
	I2C_TimeOut = 1000;
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) {
		if (I2C_TimeOut-- <= 0) {
			return 1;
		}
	}

	// * Send START condition * /
	I2C_GenerateSTART(I2C1, ENABLE);

	// / * Test on EV5 and clear it * /
	I2C_TimeOut = 1000;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
		if (I2C_TimeOut-- <= 0) {
			return 1;
		}
	}

	// / * Send  address for write  * /
	I2C_Send7bitAddress(I2C1, deviceAddr, I2C_Direction_Transmitter);

	// / * Test on EV6 and clear it * /
	I2C_TimeOut = 1000;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		if (I2C_TimeOut-- <= 0) {
			return 1;
		}
	}

	// / * Send the internal address to read from: Only one byte address  * /
	I2C_SendData(I2C1, readAddr);

	/// * Test on EV8 and clear it * /
	I2C_TimeOut = 1000;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
		if (I2C_TimeOut-- <= 0) {
			return 1;
		}
	}

	/// * Send STRAT condition a second time * /
	I2C_GenerateSTART(I2C1, ENABLE);

	/// * Test on EV5 and clear it * /
	I2C_TimeOut = 1000;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
		if (I2C_TimeOut-- <= 0) {
			return 1;
		}
	}

	// * Send  address for read * /
	I2C_Send7bitAddress(I2C1, deviceAddr, I2C_Direction_Receiver);

	if (numByteToRead == 1) {
		/* Wait until ADDR is set */
		I2C_TimeOut = 1000;
		while ((I2C1->SR1 & 0x0002) != 0x0002) {
			if (I2C_TimeOut-- <= 0) {
				return 1;
			}
		}
		/* Clear ACK bit */
		I2C1->CR1 &= CR1_ACK_Reset;
		/* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
		 software sequence must complete before the current byte end of transfer */
		__disable_irq();
		/* Clear ADDR flag */
		temp = I2C1->SR2;
		/* Program the STOP */
		I2C_GenerateSTOP(I2C1, ENABLE);
		/* Re-enable IRQs */
		__enable_irq();
		/* Wait until a data is received in DR register (RXNE = 1) EV7 */
		I2C_TimeOut = 1000;
		while ((I2C1->SR1 & 0x00040) != 0x000040) {
			if (I2C_TimeOut-- <= 0) {
				return 1;
			}
		}
		/* Read the data */
		*pBuffer = I2C1->DR;

	} else if (numByteToRead == 2) {

		/* Set POS bit */
		I2C1->CR1 |= CR1_POS_Set;
		/* Wait until ADDR is set: EV6 */
		I2C_TimeOut = 1000;
		while ((I2C1->SR1 & 0x0002) != 0x0002) {
			if (I2C_TimeOut-- <= 0) {
				return 1;
			}
		}
		/* EV6_1: The acknowledge disable should be done just after EV6,
		 that is after ADDR is cleared, so disable all active IRQs around ADDR clearing and
		 ACK clearing */
		__disable_irq();
		/* Clear ADDR by reading SR2 register  */
		temp = I2C1->SR2;
		/* Clear ACK */
		I2C1->CR1 &= CR1_ACK_Reset;
		/*Re-enable IRQs */
		__enable_irq();
		/* Wait until BTF is set */
		I2C_TimeOut = 1000;
		while ((I2C1->SR1 & 0x00004) != 0x000004) {
			if (I2C_TimeOut-- <= 0) {
				return 1;
			}
		}
		/* Disable IRQs around STOP programming and data reading */
		__disable_irq();
		/* Program the STOP */
		I2C_GenerateSTOP(I2C1, ENABLE);
		/* Read first data */
		*pBuffer = I2C1->DR;
		/* Re-enable IRQs */
		__enable_irq();
		/**/
		pBuffer++;
		/* Read second data */
		*pBuffer = I2C1->DR;
		/* Clear POS bit */
		I2C1->CR1 &= CR1_POS_Reset;
	}

	else { //numByteToRead > 2
		   // * Test on EV6 and clear it * /
		I2C_TimeOut = 1000;
		while (!I2C_CheckEvent(I2C1,
		I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
			if (I2C_TimeOut-- <= 0) {
				return 1;
			}
		}
		// * While there is data to be read * /
		while (numByteToRead) {
			/* Receive bytes from first byte until byte N-3 */
			if (numByteToRead != 3) {
				/* Poll on BTF to receive data because in polling mode we can not guarantee the
				 EV7 software sequence is managed before the current byte transfer completes */
				I2C_TimeOut = 1000;
				while ((I2C1->SR1 & 0x00004) != 0x000004) {
					if (I2C_TimeOut-- <= 0) {
						return 1;
					}
				}
				/* Read data */
				*pBuffer = I2C1->DR;
				pBuffer++;
				/* Decrement the read bytes counter */
				numByteToRead--;
			}

			/* it remains to read three data: data N-2, data N-1, Data N */
			if (numByteToRead == 3) {
				/* Wait until BTF is set: Data N-2 in DR and data N -1 in shift register */
				I2C_TimeOut = 1000;
				while ((I2C1->SR1 & 0x00004) != 0x000004) {
					if (I2C_TimeOut-- <= 0) {
						return 1;
					}
				}
				/* Clear ACK */
				I2C1->CR1 &= CR1_ACK_Reset;

				/* Disable IRQs around data reading and STOP programming */
				__disable_irq();
				/* Read Data N-2 */
				*pBuffer = I2C1->DR;
				/* Increment */
				pBuffer++;
				/* Program the STOP */
				I2C1->CR1 |= CR1_STOP_Set;
				/* Read DataN-1 */
				*pBuffer = I2C1->DR;
				/* Re-enable IRQs */
				__enable_irq();
				/* Increment */
				pBuffer++;
				/* Wait until RXNE is set (DR contains the last data) */
				I2C_TimeOut = 1000;
				while ((I2C1->SR1 & 0x00040) != 0x000040) {
					if (I2C_TimeOut-- <= 0) {
						return 1;
					}
				}
				/* Read DataN */
				*pBuffer = I2C1->DR;
				/* Reset the number of bytes to be read by master */
				numByteToRead = 0;
			}
		}
	}

	/* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
	I2C_TimeOut = 1000;
	while ((I2C1->CR1 & 0x200) == 0x200) {
		if (I2C_TimeOut-- <= 0) {
			return 1;
		}
	}

	// * Enable Acknowledgment to be ready for another reception * /
	I2C_AcknowledgeConfig(I2C1, ENABLE);

	return 0;

}

