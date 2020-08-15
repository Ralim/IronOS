/*!
    \file  I2C1_IE.c
    \brief I2C1 slave receiver interrupt program
    
    \version 2019-06-05, V1.0.0, firmware for GD32VF103

*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32vf103_i2c.h"

#include "I2C_IE.h"

uint32_t event2;

/*!
    \brief      handle I2C1 event interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void I2C1_EventIRQ_Handler(void) {
  if (i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_ADDSEND)) {
    /* clear the ADDSEND bit */
    i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_ADDSEND);
  } else if (i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_RBNE)) {
    /* if reception data register is not empty ,I2C1 will read a data from I2C_DATA */
    *i2c_rxbuffer++ = i2c_data_receive(I2C1);
  } else if (i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_STPDET)) {
    status = SUCCESS;
    /* clear the STPDET bit */
    i2c_enable(I2C1);
    /* disable I2C1 interrupt */
    i2c_interrupt_disable(I2C1, I2C_INT_ERR);
    i2c_interrupt_disable(I2C1, I2C_INT_BUF);
    i2c_interrupt_disable(I2C1, I2C_INT_EV);
  }
}

/*!
    \brief      handle I2C1 error interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void I2C1_ErrorIRQ_Handler(void) {
  /* no acknowledge received */
  if (i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_AERR)) {
    i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_AERR);
  }

  /* SMBus alert */
  if (i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_SMBALT)) {
    i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_SMBALT);
  }

  /* bus timeout in SMBus mode */
  if (i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_SMBTO)) {
    i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_SMBTO);
  }

  /* over-run or under-run when SCL stretch is disabled */
  if (i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_OUERR)) {
    i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_OUERR);
  }

  /* arbitration lost */
  if (i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_LOSTARB)) {
    i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_LOSTARB);
  }

  /* bus error */
  if (i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_BERR)) {
    i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_BERR);
  }

  /* CRC value doesn't match */
  if (i2c_interrupt_flag_get(I2C1, I2C_INT_FLAG_PECERR)) {
    i2c_interrupt_flag_clear(I2C1, I2C_INT_FLAG_PECERR);
  }

  /* disable the error interrupt */
  i2c_interrupt_disable(I2C1, I2C_INT_ERR);
  i2c_interrupt_disable(I2C1, I2C_INT_BUF);
  i2c_interrupt_disable(I2C1, I2C_INT_EV);
}
