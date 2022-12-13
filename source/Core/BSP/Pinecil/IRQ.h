/*
 * Irqs.h
 *
 *  Created on: 30 May 2020
 *      Author: Ralim
 */

#ifndef BSP_PINE64_IRQ_H_
#define BSP_PINE64_IRQ_H_

#include "BSP.h"
#include "I2C_Wrapper.hpp"
#include "Setup.h"
#include "gd32vf103.h"
#include "main.hpp"

#ifdef __cplusplus
extern "C" {
#endif
void ADC0_1_IRQHandler(void);
void TIMER1_IRQHandler(void);
void EXTI5_9_IRQHandler(void);
/* handle I2C0 event interrupt request */
void I2C0_EV_IRQHandler(void);
/* handle I2C0 error interrupt request */
void I2C0_ER_IRQHandler(void);
typedef enum {
  I2C_SEND_ADDRESS_FIRST = 0,    // Sending slave address
  I2C_CLEAR_ADDRESS_FLAG_FIRST,  // Clear address send
  I2C_TRANSMIT_WRITE_READ_ADD,   // Transmit the memory address to read/write from
  I2C_SEND_ADDRESS_SECOND,       // Send address again for read
  I2C_CLEAR_ADDRESS_FLAG_SECOND, // Clear address again
  I2C_TRANSMIT_DATA,             // Transmit recieve data
  I2C_STOP,                      // Send stop
  I2C_ABORTED,                   //
  I2C_DONE,                      // I2C transfer is complete
  I2C_START,
  I2C_END,
  I2C_OK,
  I2C_SEND_ADDRESS,
  I2C_CLEAR_ADDRESS_FLAG,
} i2c_process_enum;
extern volatile uint8_t  i2c_slave_address;
extern volatile uint8_t  i2c_read_process;
extern volatile uint8_t  i2c_write_process;
extern volatile uint8_t  i2c_error_code;
extern volatile uint8_t *i2c_write;
extern volatile uint8_t *i2c_read;
extern volatile uint16_t i2c_nbytes;
extern volatile uint16_t i2c_write_dress;
extern volatile uint16_t i2c_read_dress;
extern volatile uint8_t  i2c_process_flag;
#ifdef __cplusplus
}
#endif
#endif /* BSP_PINE64_IRQ_H_ */
