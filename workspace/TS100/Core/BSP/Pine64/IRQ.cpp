/*
 * IRQ.c
 *
 *  Created on: 30 May 2020
 *      Author: Ralim
 */

#include "IRQ.h"
#include "Pins.h"
#include "int_n.h"
volatile uint8_t i2c_read_process = 0;
volatile uint8_t i2c_write_process = 0;
volatile uint8_t i2c_slave_address = 0;
volatile uint8_t i2c_error_code = 0;
volatile uint8_t* i2c_write;
volatile uint8_t* i2c_read;
volatile uint16_t i2c_nbytes;
volatile uint16_t i2c_write_dress;
volatile uint16_t i2c_read_dress;
volatile uint8_t i2c_process_flag = 0;
void ADC0_1_IRQHandler(void) {

	adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOIC);
	//unblock the PID controller thread
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if (pidTaskNotification) {
		vTaskNotifyGiveFromISR(pidTaskNotification, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

volatile uint16_t PWMSafetyTimer = 0;
volatile uint8_t pendingPWM = 0;
void TIMER1_IRQHandler(void) {

	if (timer_interrupt_flag_get(TIMER1, TIMER_INT_UP) == SET) {
		timer_interrupt_flag_clear(TIMER1, TIMER_INT_UP);
		//rollover turn on output if required
		if (PWMSafetyTimer && pendingPWM) {
			timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_0, 80);
		}
		if (PWMSafetyTimer) {
			PWMSafetyTimer--;
		}
	}
	if (timer_interrupt_flag_get(TIMER1, TIMER_INT_CH1) == SET) {
		timer_interrupt_flag_clear(TIMER1, TIMER_INT_CH1);
		//This is triggered on pwm setpoint trigger; we want to copy the pending PWM value into the output control reg

		timer_channel_output_pulse_value_config(TIMER2, TIMER_CH_0, 0);
		if (pendingPWM) {
			timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_1, pendingPWM);
		}
	}
}

void setTipPWM(uint8_t pulse) {
	PWMSafetyTimer = 10;
// This is decremented in the handler for PWM so that the tip pwm is
// disabled if the PID task is not scheduled often enough.

	pendingPWM = pulse;
}

void EXTI5_9_IRQHandler(void) {
#ifdef POW_PD
	if (RESET != exti_interrupt_flag_get(EXTI_5)) {
		exti_interrupt_flag_clear(EXTI_5);

		if (RESET == gpio_input_bit_get(FUSB302_IRQ_GPIO_Port, FUSB302_IRQ_Pin)) {
			InterruptHandler::irqCallback();
		}
	}
#endif
}

/*!
 \brief      handle I2C0 event interrupt request
 \param[in]  none
 \param[out] none
 \retval     none
 */
void I2C0_EV_IRQHandler(void) {
	if (RESET == i2c_process_flag) {
		switch (i2c_write_process) {
		case I2C_SEND_ADDRESS_FIRST:
			if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_SBSEND)) {
				/* send slave address */
				i2c_master_addressing(I2C0, i2c_slave_address, I2C_TRANSMITTER);
				i2c_write_process = I2C_CLEAR_ADDRESS_FLAG_FIRST;
			}
			break;
		case I2C_CLEAR_ADDRESS_FLAG_FIRST:
			if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_ADDSEND)) {
				/*clear ADDSEND bit */
				i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_ADDSEND);
				i2c_write_process = I2C_TRANSMIT_WRITE_READ_ADD;
			}
			break;
		case I2C_TRANSMIT_WRITE_READ_ADD:
			if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_TBE)) {
				i2c_data_transmit(I2C0, i2c_write_dress);
				/* wait until BTC bit is set */
				while (!i2c_flag_get(I2C0, I2C_FLAG_BTC))
					;
				i2c_write_process = I2C_TRANSMIT_DATA;
			}
			break;
		case I2C_TRANSMIT_DATA:
			if (i2c_nbytes) {
				if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_TBE)) {
					/* the master sends a data byte */
					i2c_data_transmit(I2C0, *i2c_write++);
					i2c_nbytes--;
					if (0 == i2c_nbytes) {
						i2c_write_process = I2C_STOP;
					}
				}
			} else {
				i2c_write_process = I2C_STOP;
			}
			break;
		case I2C_STOP:
			/* the master sends a stop condition to I2C bus */
			i2c_stop_on_bus(I2C0);
			/* disable the I2C0 interrupt */
			i2c_interrupt_disable(I2C0, I2C_INT_ERR);
			i2c_interrupt_disable(I2C0, I2C_INT_BUF);
			i2c_interrupt_disable(I2C0, I2C_INT_EV);
			i2c_write_process = I2C_SEND_ADDRESS_FIRST;
			break;
		default:
			break;
		}
	} else if (SET == i2c_process_flag) {
		switch (i2c_read_process) {
		case I2C_SEND_ADDRESS_FIRST:
			if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_SBSEND)) {
				/* send slave address */
				i2c_master_addressing(I2C0, i2c_slave_address, I2C_TRANSMITTER);
				i2c_read_process = I2C_CLEAR_ADDRESS_FLAG_FIRST;
			}
			break;
		case I2C_CLEAR_ADDRESS_FLAG_FIRST:
			if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_ADDSEND)) {
				/*clear ADDSEND bit */
				i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_ADDSEND);
				i2c_read_process = I2C_TRANSMIT_WRITE_READ_ADD;
			}
			break;
		case I2C_TRANSMIT_WRITE_READ_ADD:
			if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_TBE)) {
				i2c_data_transmit(I2C0, i2c_read_dress);
				/* wait until BTC bit is set */
				while (!i2c_flag_get(I2C0, I2C_FLAG_BTC))
					;
				/* send a start condition to I2C bus */
				i2c_start_on_bus(I2C0);
				i2c_read_process = I2C_SEND_ADDRESS_SECOND;
			}
			break;
		case I2C_SEND_ADDRESS_SECOND:
			if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_SBSEND)) {
				i2c_master_addressing(I2C0, i2c_slave_address, I2C_RECEIVER);
				if ((1 == i2c_nbytes) || (2 == i2c_nbytes)) {
					i2c_ackpos_config(I2C0, i2c_nbytes == 1 ? I2C_ACKPOS_CURRENT : I2C_ACKPOS_NEXT);
					/* clear the ACKEN before the ADDSEND is cleared */
					i2c_ack_config(I2C0, I2C_ACK_DISABLE);
				} else {
					i2c_ack_config(I2C0, I2C_ACK_ENABLE);

				}
				i2c_read_process = I2C_CLEAR_ADDRESS_FLAG_SECOND;
			}
			break;
		case I2C_CLEAR_ADDRESS_FLAG_SECOND:
			if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_ADDSEND)) {

				/*clear ADDSEND bit */
				i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_ADDSEND);
				i2c_read_process = I2C_TRANSMIT_DATA;

			}
			break;
		case I2C_TRANSMIT_DATA:
			if (i2c_nbytes > 0) {
				if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_RBNE)) {
					/* read a byte from the EEPROM */
					if (i2c_nbytes == 2) {
						/* wait until BTC bit is set */
						i2c_ackpos_config(I2C0, I2C_ACKPOS_CURRENT);
						/* clear the ACKEN before the ADDSEND is cleared */
						i2c_ack_config(I2C0, I2C_ACK_DISABLE);
					}
					*i2c_read = i2c_data_receive(I2C0);
					i2c_read++;
					i2c_nbytes--;
					if (i2c_nbytes == 0) {
						/* the master sends a stop condition to I2C bus */
						i2c_stop_on_bus(I2C0);
						/* disable the I2C0 interrupt */
						i2c_interrupt_disable(I2C0, I2C_INT_ERR);
						i2c_interrupt_disable(I2C0, I2C_INT_BUF);
						i2c_interrupt_disable(I2C0, I2C_INT_EV);
						i2c_process_flag = RESET;
						i2c_read_process = I2C_DONE;
					}
				}
			} else {
				i2c_read_process = I2C_STOP;
				/* the master sends a stop condition to I2C bus */
				i2c_stop_on_bus(I2C0);
				/* disable the I2C0 interrupt */
				i2c_interrupt_disable(I2C0, I2C_INT_ERR);
				i2c_interrupt_disable(I2C0, I2C_INT_BUF);
				i2c_interrupt_disable(I2C0, I2C_INT_EV);
				i2c_process_flag = RESET;
				i2c_read_process = I2C_DONE;
			}
			break;
		case I2C_STOP:
			/* the master sends a stop condition to I2C bus */
			i2c_stop_on_bus(I2C0);
			/* disable the I2C0 interrupt */
			i2c_interrupt_disable(I2C0, I2C_INT_ERR);
			i2c_interrupt_disable(I2C0, I2C_INT_BUF);
			i2c_interrupt_disable(I2C0, I2C_INT_EV);
			i2c_process_flag = RESET;
			i2c_read_process = I2C_DONE;
			break;
		default:
			break;
		}
	}
}

/*!
 \brief      handle I2C0 error interrupt request
 \param[in]  none
 \param[out] none
 \retval     none
 */
void I2C0_ER_IRQHandler(void) {
	/* no acknowledge received */
	if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_AERR)) {
		i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_AERR);
		i2c_error_code = 1; //NAK
	}

	/* SMBus alert */
	if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_SMBALT)) {
		i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_SMBALT);
		i2c_error_code = 2; //SMB Alert
	}

	/* bus timeout in SMBus mode */
	if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_SMBTO)) {
		i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_SMBTO);
		i2c_error_code = 3; //SMB Timeout
	}

	/* over-run or under-run when SCL stretch is disabled */
	if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_OUERR)) {
		i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_OUERR);
		i2c_error_code = 4; //OverRun
	}

	/* arbitration lost */
	if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_LOSTARB)) {
		i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_LOSTARB);
		i2c_error_code = 5; //Lost ARB -- multi master -- shouldnt happen
	}

	/* bus error */
	if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_BERR)) {
		i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_BERR);
		i2c_error_code = 6; //Bus Error
	}

	/* CRC value doesn't match */
	if (i2c_interrupt_flag_get(I2C0, I2C_INT_FLAG_PECERR)) {
		i2c_interrupt_flag_clear(I2C0, I2C_INT_FLAG_PECERR);
		i2c_error_code = 7; //CRC Fail -- Shouldnt Happen
	}

	i2c_stop_on_bus(I2C0);

}
