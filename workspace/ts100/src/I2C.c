/*
 * I2C.h hardware interface class
 * Based on the STM32 app note AN2824
 */
#include "I2C.h"

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

/*
 * Read Page of data using I2C1 peripheral
 */

void I2C_PageRead(u8* buf, u8 nbyte, u8 deviceaddr, u8 readaddr) {
	I2C_GenerateSTART(I2C1, ENABLE);
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_SB) == RESET)
		;
	I2C_Send7bitAddress(I2C1, deviceaddr << 1, I2C_Direction_Transmitter);
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == RESET)
		;
	I2C_GetFlagStatus(I2C1, I2C_FLAG_MSL);
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE) == RESET)
		;
	// Send an 8bit byte address
	I2C_SendData(I2C1, readaddr);
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) {
	}
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Current);
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) {
	}
	I2C_Send7bitAddress(I2C1, deviceaddr << 1, I2C_Direction_Receiver);
	while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR)) {
	}
	if (nbyte == 1) {
		// Clear Ack bit
		I2C_AcknowledgeConfig(I2C1, DISABLE);
		// EV6_1 -- must be atomic -- Clear ADDR, generate STOP
		__disable_irq();
		(void) I2C1->SR2;
		I2C_GenerateSTOP(I2C1, ENABLE);
		__enable_irq();
		// Receive data   EV7
		while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE)) {
		}
		*buf++ = I2C_ReceiveData(I2C1);
	} else if (nbyte == 2) {
		// Set POS flag
		I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Next);
		// EV6_1 -- must be atomic and in this order
		__disable_irq();
		(void) I2C1->SR2;                           // Clear ADDR flag
		I2C_AcknowledgeConfig(I2C1, DISABLE);       // Clear Ack bit
		__enable_irq();
		// EV7_3  -- Wait for BTF, program stop, read data twice
		while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF)) {
		}
		__disable_irq();
		I2C_GenerateSTOP(I2C1, ENABLE);
		*buf++ = I2C1->DR;
		__enable_irq();
		*buf++ = I2C1->DR;
	} else {
		(void) I2C1->SR2;                           // Clear ADDR flag
		while (nbyte-- != 3) {
			// EV7 -- cannot guarantee 1 transfer completion time, wait for BTF
			//        instead of RXNE
			while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF)) {
			}
			*buf++ = I2C_ReceiveData(I2C1);
		}

		while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF)) {
		}
		// EV7_2 -- Figure 1 has an error, doesn't read N-2 !
		I2C_AcknowledgeConfig(I2C1, DISABLE);           // clear ack bit
		__disable_irq();
		*buf++ = I2C_ReceiveData(I2C1);             // receive byte N-2
		I2C_GenerateSTOP(I2C1, ENABLE);                  // program stop
		__enable_irq();
		*buf++ = I2C_ReceiveData(I2C1);             // receive byte N-1
		// wait for byte N
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
		}
		*buf++ = I2C_ReceiveData(I2C1);
		nbyte = 0;
	}
	// Wait for stop
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF)) {
	}
	return;
}

