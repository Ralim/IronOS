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

//These are unused for now
void I2C0_EV_IRQHandler(void) {

}

void I2C0_ER_IRQHandler(void) {

}
