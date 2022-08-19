/*!
    \file    gd32vf103_usart.c
    \brief   USART driver

    \version 2019-06-05, V1.0.0, firmware for GD32VF103
    \version 2019-09-18, V1.0.1, firmware for GD32VF103
    \version 2020-08-04, V1.1.0, firmware for GD32VF103
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

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

#include "gd32vf103_usart.h"
#include "gd32vf103_rcu.h"

/*!
    \brief      reset USART/UART
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[out] none
    \retval     none
*/
void usart_deinit(uint32_t usart_periph) {
  switch (usart_periph) {
  case USART0:
    /* reset USART0 */
    rcu_periph_reset_enable(RCU_USART0RST);
    rcu_periph_reset_disable(RCU_USART0RST);
    break;
  case USART1:
    /* reset USART1 */
    rcu_periph_reset_enable(RCU_USART1RST);
    rcu_periph_reset_disable(RCU_USART1RST);
    break;
  case USART2:
    /* reset USART2 */
    rcu_periph_reset_enable(RCU_USART2RST);
    rcu_periph_reset_disable(RCU_USART2RST);
    break;
  case UART3:
    /* reset UART3 */
    rcu_periph_reset_enable(RCU_UART3RST);
    rcu_periph_reset_disable(RCU_UART3RST);
    break;
  case UART4:
    /* reset UART4 */
    rcu_periph_reset_enable(RCU_UART4RST);
    rcu_periph_reset_disable(RCU_UART4RST);
    break;
  default:
    break;
  }
}

/*!
    \brief      configure USART baud rate value
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in]  baudval: baud rate value
    \param[out] none
    \retval     none
*/
void usart_baudrate_set(uint32_t usart_periph, uint32_t baudval) {
  uint32_t uclk = 0U, intdiv = 0U, fradiv = 0U, udiv = 0U;
  switch (usart_periph) {
    /* get clock frequency */
  case USART0:
    /* get USART0 clock */
    uclk = rcu_clock_freq_get(CK_APB2);
    break;
  case USART1:
    /* get USART1 clock */
    uclk = rcu_clock_freq_get(CK_APB1);
    break;
  case USART2:
    /* get USART2 clock */
    uclk = rcu_clock_freq_get(CK_APB1);
    break;
  case UART3:
    /* get UART3 clock */
    uclk = rcu_clock_freq_get(CK_APB1);
    break;
  case UART4:
    /* get UART4 clock */
    uclk = rcu_clock_freq_get(CK_APB1);
    break;
  default:
    break;
  }
  /* oversampling by 16, configure the value of USART_BAUD */
  udiv                     = (uclk + baudval / 2U) / baudval;
  intdiv                   = udiv & (0x0000fff0U);
  fradiv                   = udiv & (0x0000000fU);
  USART_BAUD(usart_periph) = ((USART_BAUD_FRADIV | USART_BAUD_INTDIV) & (intdiv | fradiv));
}

/*!
    \brief     configure USART parity
    \param[in] usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in] paritycfg: configure USART parity
               only one parameter can be selected which is shown as below:
      \arg       USART_PM_NONE: no parity
      \arg       USART_PM_ODD:  odd parity
      \arg       USART_PM_EVEN: even parity
    \param[out] none
    \retval     none
*/
void usart_parity_config(uint32_t usart_periph, uint32_t paritycfg) {
  /* clear USART_CTL0 PM,PCEN bits */
  USART_CTL0(usart_periph) &= ~(USART_CTL0_PM | USART_CTL0_PCEN);
  /* configure USART parity mode */
  USART_CTL0(usart_periph) |= paritycfg;
}

/*!
    \brief     configure USART word length
    \param[in] usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in] wlen: USART word length configure
               only one parameter can be selected which is shown as below:
      \arg       USART_WL_8BIT: 8 bits
      \arg       USART_WL_9BIT: 9 bits
    \param[out] none
    \retval     none
*/
void usart_word_length_set(uint32_t usart_periph, uint32_t wlen) {
  /* clear USART_CTL0 WL bit */
  USART_CTL0(usart_periph) &= ~USART_CTL0_WL;
  /* configure USART word length */
  USART_CTL0(usart_periph) |= wlen;
}

/*!
    \brief     configure USART stop bit length
    \param[in] usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in] stblen: USART stop bit configure
               only one parameter can be selected which is shown as below:
      \arg       USART_STB_1BIT:   1 bit
      \arg       USART_STB_0_5BIT: 0.5 bit, not available for UARTx(x=3,4)
      \arg       USART_STB_2BIT:   2 bits
      \arg       USART_STB_1_5BIT: 1.5 bits, not available for UARTx(x=3,4)
    \param[out] none
    \retval     none
*/
void usart_stop_bit_set(uint32_t usart_periph, uint32_t stblen) {
  /* clear USART_CTL1 STB bits */
  USART_CTL1(usart_periph) &= ~USART_CTL1_STB;
  /* configure USART stop bits */
  USART_CTL1(usart_periph) |= stblen;
}

/*!
    \brief      enable USART
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[out] none
    \retval     none
*/
void usart_enable(uint32_t usart_periph) { USART_CTL0(usart_periph) |= USART_CTL0_UEN; }

/*!
    \brief     disable USART
    \param[in] usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[out] none
    \retval     none
*/
void usart_disable(uint32_t usart_periph) { USART_CTL0(usart_periph) &= ~(USART_CTL0_UEN); }

/*!
    \brief      configure USART transmitter
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in]  txconfig: enable or disable USART transmitter
                only one parameter can be selected which is shown as below:
      \arg        USART_TRANSMIT_ENABLE: enable USART transmission
      \arg        USART_TRANSMIT_DISABLE: disable USART transmission
    \param[out] none
    \retval     none
*/
void usart_transmit_config(uint32_t usart_periph, uint32_t txconfig) {
  uint32_t ctl = 0U;

  ctl = USART_CTL0(usart_periph);
  ctl &= ~USART_CTL0_TEN;
  ctl |= txconfig;
  /* configure transfer mode */
  USART_CTL0(usart_periph) = ctl;
}

/*!
    \brief      configure USART receiver
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in]  rxconfig: enable or disable USART receiver
                only one parameter can be selected which is shown as below:
      \arg        USART_RECEIVE_ENABLE: enable USART reception
      \arg        USART_RECEIVE_DISABLE: disable USART reception
    \param[out] none
    \retval     none
*/
void usart_receive_config(uint32_t usart_periph, uint32_t rxconfig) {
  uint32_t ctl = 0U;

  ctl = USART_CTL0(usart_periph);
  ctl &= ~USART_CTL0_REN;
  ctl |= rxconfig;
  /* configure receiver mode */
  USART_CTL0(usart_periph) = ctl;
}

/*!
    \brief      USART transmit data function
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in]  data: data of transmission
    \param[out] none
    \retval     none
*/
void usart_data_transmit(uint32_t usart_periph, uint32_t data) { USART_DATA(usart_periph) = USART_DATA_DATA & data; }

/*!
    \brief      USART receive data function
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[out] none
    \retval     data of received
*/
uint16_t usart_data_receive(uint32_t usart_periph) { return (uint16_t)(GET_BITS(USART_DATA(usart_periph), 0U, 8U)); }

/*!
    \brief      configure the address of the USART in wake up by address match mode
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in]  addr: address of USART/UART
    \param[out] none
    \retval     none
*/
void usart_address_config(uint32_t usart_periph, uint8_t addr) {
  USART_CTL1(usart_periph) &= ~(USART_CTL1_ADDR);
  USART_CTL1(usart_periph) |= (USART_CTL1_ADDR & addr);
}

/*!
    \brief      receiver in mute mode
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[out] none
    \retval     none
*/
void usart_mute_mode_enable(uint32_t usart_periph) { USART_CTL0(usart_periph) |= USART_CTL0_RWU; }

/*!
    \brief      receiver in active mode
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[out] none
    \retval     none
*/
void usart_mute_mode_disable(uint32_t usart_periph) { USART_CTL0(usart_periph) &= ~(USART_CTL0_RWU); }

/*!
    \brief      configure wakeup method in mute mode
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in]  wmethod: two methods be used to enter or exit the mute mode
                only one parameter can be selected which is shown as below:
      \arg        USART_WM_IDLE: idle line
      \arg        USART_WM_ADDR: address mask
    \param[out] none
    \retval     none
*/
void usart_mute_mode_wakeup_config(uint32_t usart_periph, uint32_t wmethod) {
  USART_CTL0(usart_periph) &= ~(USART_CTL0_WM);
  USART_CTL0(usart_periph) |= wmethod;
}

/*!
    \brief      enable LIN mode
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[out] none
    \retval     none
*/
void usart_lin_mode_enable(uint32_t usart_periph) { USART_CTL1(usart_periph) |= USART_CTL1_LMEN; }

/*!
    \brief      disable LIN mode
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[out] none
    \retval     none
*/
void usart_lin_mode_disable(uint32_t usart_periph) { USART_CTL1(usart_periph) &= ~(USART_CTL1_LMEN); }

/*!
    \brief      configure lin break frame length
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in]  lblen: lin break frame length
                only one parameter can be selected which is shown as below:
      \arg        USART_LBLEN_10B: 10 bits
      \arg        USART_LBLEN_11B: 11 bits
    \param[out] none
    \retval     none
*/
void usart_lin_break_detection_length_config(uint32_t usart_periph, uint32_t lblen) {
  USART_CTL1(usart_periph) &= ~(USART_CTL1_LBLEN);
  USART_CTL1(usart_periph) |= (USART_CTL1_LBLEN & lblen);
}

/*!
    \brief      send break frame
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[out] none
    \retval     none
*/
void usart_send_break(uint32_t usart_periph) { USART_CTL0(usart_periph) |= USART_CTL0_SBKCMD; }

/*!
    \brief      enable half duplex mode
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[out] none
    \retval     none
*/
void usart_halfduplex_enable(uint32_t usart_periph) { USART_CTL2(usart_periph) |= USART_CTL2_HDEN; }

/*!
    \brief      disable half duplex mode
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[out] none
    \retval     none
*/
void usart_halfduplex_disable(uint32_t usart_periph) { USART_CTL2(usart_periph) &= ~(USART_CTL2_HDEN); }

/*!
    \brief      enable CK pin in synchronous mode
    \param[in]  usart_periph: USARTx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void usart_synchronous_clock_enable(uint32_t usart_periph) { USART_CTL1(usart_periph) |= USART_CTL1_CKEN; }

/*!
    \brief      disable CK pin in synchronous mode
    \param[in]  usart_periph: USARTx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void usart_synchronous_clock_disable(uint32_t usart_periph) { USART_CTL1(usart_periph) &= ~(USART_CTL1_CKEN); }

/*!
    \brief      configure USART synchronous mode parameters
    \param[in]  usart_periph: USARTx(x=0,1,2)
    \param[in]  clen: CK length
                only one parameter can be selected which is shown as below:
      \arg        USART_CLEN_NONE: there are 7 CK pulses for an 8 bit frame and 8 CK pulses for a 9 bit frame
      \arg        USART_CLEN_EN:   there are 8 CK pulses for an 8 bit frame and 9 CK pulses for a 9 bit frame
    \param[in]  cph: clock phase
                only one parameter can be selected which is shown as below:
      \arg        USART_CPH_1CK: first clock transition is the first data capture edge
      \arg        USART_CPH_2CK: second clock transition is the first data capture edge
    \param[in]  cpl: clock polarity
                only one parameter can be selected which is shown as below:
      \arg        USART_CPL_LOW:  steady low value on CK pin
      \arg        USART_CPL_HIGH: steady high value on CK pin
    \param[out] none
    \retval     none
*/
void usart_synchronous_clock_config(uint32_t usart_periph, uint32_t clen, uint32_t cph, uint32_t cpl) {
  uint32_t ctl = 0U;

  /* read USART_CTL1 register */
  ctl = USART_CTL1(usart_periph);
  ctl &= ~(USART_CTL1_CLEN | USART_CTL1_CPH | USART_CTL1_CPL);
  /* set CK length, CK phase, CK polarity */
  ctl |= (USART_CTL1_CLEN & clen) | (USART_CTL1_CPH & cph) | (USART_CTL1_CPL & cpl);

  USART_CTL1(usart_periph) = ctl;
}

/*!
    \brief      configure guard time value in smartcard mode
    \param[in]  usart_periph: USARTx(x=0,1,2)
    \param[in]  gaut: guard time value
    \param[out] none
    \retval     none
*/
void usart_guard_time_config(uint32_t usart_periph, uint32_t gaut) {
  USART_GP(usart_periph) &= ~(USART_GP_GUAT);
  USART_GP(usart_periph) |= (USART_GP_GUAT & ((gaut) << 8));
}

/*!
    \brief      enable smartcard mode
    \param[in]  usart_periph: USARTx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void usart_smartcard_mode_enable(uint32_t usart_periph) { USART_CTL2(usart_periph) |= USART_CTL2_SCEN; }

/*!
    \brief      disable smartcard mode
    \param[in]  usart_periph: USARTx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void usart_smartcard_mode_disable(uint32_t usart_periph) { USART_CTL2(usart_periph) &= ~(USART_CTL2_SCEN); }

/*!
    \brief      enable NACK in smartcard mode
    \param[in]  usart_periph: USARTx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void usart_smartcard_mode_nack_enable(uint32_t usart_periph) { USART_CTL2(usart_periph) |= USART_CTL2_NKEN; }

/*!
    \brief      disable NACK in smartcard mode
    \param[in]  usart_periph: USARTx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void usart_smartcard_mode_nack_disable(uint32_t usart_periph) { USART_CTL2(usart_periph) &= ~(USART_CTL2_NKEN); }

/*!
    \brief      enable IrDA mode
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[out] none
    \retval     none
*/
void usart_irda_mode_enable(uint32_t usart_periph) { USART_CTL2(usart_periph) |= USART_CTL2_IREN; }

/*!
    \brief      disable IrDA mode
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[out] none
    \retval     none
*/
void usart_irda_mode_disable(uint32_t usart_periph) { USART_CTL2(usart_periph) &= ~(USART_CTL2_IREN); }

/*!
    \brief      configure the peripheral clock prescaler in USART IrDA low-power mode
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in]  psc: 0x00-0xFF
    \param[out] none
    \retval     none
*/
void usart_prescaler_config(uint32_t usart_periph, uint8_t psc) {
  USART_GP(usart_periph) &= ~(USART_GP_PSC);
  USART_GP(usart_periph) |= psc;
}

/*!
    \brief      configure IrDA low-power
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in]  irlp: IrDA low-power or normal
                only one parameter can be selected which is shown as below:
      \arg        USART_IRLP_LOW: low-power
      \arg        USART_IRLP_NORMAL: normal
    \param[out] none
    \retval     none
*/
void usart_irda_lowpower_config(uint32_t usart_periph, uint32_t irlp) {
  USART_CTL2(usart_periph) &= ~(USART_CTL2_IRLP);
  USART_CTL2(usart_periph) |= (USART_CTL2_IRLP & irlp);
}

/*!
    \brief      configure hardware flow control RTS
    \param[in]  usart_periph: USARTx(x=0,1,2)
    \param[in]  rtsconfig: enable or disable RTS
                only one parameter can be selected which is shown as below:
      \arg        USART_RTS_ENABLE:  enable RTS
      \arg        USART_RTS_DISABLE: disable RTS
    \param[out] none
    \retval     none
*/
void usart_hardware_flow_rts_config(uint32_t usart_periph, uint32_t rtsconfig) {
  uint32_t ctl = 0U;

  ctl = USART_CTL2(usart_periph);
  ctl &= ~USART_CTL2_RTSEN;
  ctl |= rtsconfig;
  /* configure RTS */
  USART_CTL2(usart_periph) = ctl;
}

/*!
    \brief      configure hardware flow control CTS
    \param[in]  usart_periph: USARTx(x=0,1,2)
    \param[in]  ctsconfig: enable or disable CTS
                only one parameter can be selected which is shown as below:
      \arg        USART_CTS_ENABLE:  enable CTS
      \arg        USART_CTS_DISABLE: disable CTS
    \param[out] none
    \retval     none
*/
void usart_hardware_flow_cts_config(uint32_t usart_periph, uint32_t ctsconfig) {
  uint32_t ctl = 0U;

  ctl = USART_CTL2(usart_periph);
  ctl &= ~USART_CTL2_CTSEN;
  ctl |= ctsconfig;
  /* configure CTS */
  USART_CTL2(usart_periph) = ctl;
}

/*!
    \brief      configure USART DMA reception
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3)
    \param[in]  dmacmd: enable or disable DMA for reception
                only one parameter can be selected which is shown as below:
      \arg        USART_DENR_ENABLE:  DMA enable for reception
      \arg        USART_DENR_DISABLE: DMA disable for reception
    \param[out] none
    \retval     none
*/
void usart_dma_receive_config(uint32_t usart_periph, uint32_t dmacmd) {
  uint32_t ctl = 0U;

  ctl = USART_CTL2(usart_periph);
  ctl &= ~USART_CTL2_DENR;
  ctl |= dmacmd;
  /* configure DMA reception */
  USART_CTL2(usart_periph) = ctl;
}

/*!
    \brief      configure USART DMA transmission
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3)
    \param[in]  dmacmd: enable or disable DMA for transmission
                only one parameter can be selected which is shown as below:
      \arg        USART_DENT_ENABLE:  DMA enable for transmission
      \arg        USART_DENT_DISABLE: DMA disable for transmission
    \param[out] none
    \retval     none
*/
void usart_dma_transmit_config(uint32_t usart_periph, uint32_t dmacmd) {
  uint32_t ctl = 0U;

  ctl = USART_CTL2(usart_periph);
  ctl &= ~USART_CTL2_DENT;
  ctl |= dmacmd;
  /* configure DMA transmission */
  USART_CTL2(usart_periph) = ctl;
}

/*!
    \brief      get flag in STAT register
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in]  flag: USART flags, refer to usart_flag_enum
                only one parameter can be selected which is shown as below:
      \arg        USART_FLAG_CTS: CTS change flag
      \arg        USART_FLAG_LBD: LIN break detected flag
      \arg        USART_FLAG_TBE: transmit data buffer empty
      \arg        USART_FLAG_TC: transmission complete
      \arg        USART_FLAG_RBNE: read data buffer not empty
      \arg        USART_FLAG_IDLE: IDLE frame detected flag
      \arg        USART_FLAG_ORERR: overrun error
      \arg        USART_FLAG_NERR: noise error flag
      \arg        USART_FLAG_FERR: frame error flag
      \arg        USART_FLAG_PERR: parity error flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus usart_flag_get(uint32_t usart_periph, usart_flag_enum flag) {
  if (RESET != (USART_REG_VAL(usart_periph, flag) & BIT(USART_BIT_POS(flag)))) {
    return SET;
  } else {
    return RESET;
  }
}

/*!
    \brief      clear flag in STAT register
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in]  flag: USART flags, refer to usart_flag_enum
                only one parameter can be selected which is shown as below:
      \arg        USART_FLAG_CTS: CTS change flag
      \arg        USART_FLAG_LBD: LIN break detected flag
      \arg        USART_FLAG_TC: transmission complete
      \arg        USART_FLAG_RBNE: read data buffer not empty
    \param[out] none
    \retval     none
*/
void usart_flag_clear(uint32_t usart_periph, usart_flag_enum flag) { USART_REG_VAL(usart_periph, flag) &= ~BIT(USART_BIT_POS(flag)); }

/*!
    \brief      enable USART interrupt
     \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in]  interrupt
                only one parameter can be selected which is shown as below:
      \arg        USART_INT_PERR: parity error interrupt
      \arg        USART_INT_TBE: transmitter buffer empty interrupt
      \arg        USART_INT_TC: transmission complete interrupt
      \arg        USART_INT_RBNE: read data buffer not empty interrupt and overrun error interrupt
      \arg        USART_INT_IDLE: IDLE line detected interrupt
      \arg        USART_INT_LBD: LIN break detected interrupt
      \arg        USART_INT_ERR: error interrupt
      \arg        USART_INT_CTS: CTS interrupt
    \param[out] none
    \retval     none
*/
void usart_interrupt_enable(uint32_t usart_periph, uint32_t interrupt) { USART_REG_VAL(usart_periph, interrupt) |= BIT(USART_BIT_POS(interrupt)); }

/*!
    \brief      disable USART interrupt
     \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in]  interrupt
                only one parameter can be selected which is shown as below:
      \arg        USART_INT_PERR: parity error interrupt
      \arg        USART_INT_TBE: transmitter buffer empty interrupt
      \arg        USART_INT_TC: transmission complete interrupt
      \arg        USART_INT_RBNE: read data buffer not empty interrupt and overrun error interrupt
      \arg        USART_INT_IDLE: IDLE line detected interrupt
      \arg        USART_INT_LBD: LIN break detected interrupt
      \arg        USART_INT_ERR: error interrupt
      \arg        USART_INT_CTS: CTS interrupt
    \param[out] none
    \retval     none
*/
void usart_interrupt_disable(uint32_t usart_periph, uint32_t interrupt) { USART_REG_VAL(usart_periph, interrupt) &= ~BIT(USART_BIT_POS(interrupt)); }

/*!
    \brief      get USART interrupt and flag status
     \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in]  int_flag
                only one parameter can be selected which is shown as below:
      \arg        USART_INT_FLAG_PERR: parity error interrupt and flag
      \arg        USART_INT_FLAG_TBE: transmitter buffer empty interrupt and flag
      \arg        USART_INT_FLAG_TC: transmission complete interrupt and flag
      \arg        USART_INT_FLAG_RBNE: read data buffer not empty interrupt and flag
      \arg        USART_INT_FLAG_RBNE_ORERR: read data buffer not empty interrupt and overrun error flag
      \arg        USART_INT_FLAG_IDLE: IDLE line detected interrupt and flag
      \arg        USART_INT_FLAG_LBD: LIN break detected interrupt and flag
      \arg        USART_INT_FLAG_CTS: CTS interrupt and flag
      \arg        USART_INT_FLAG_ERR_ORERR: error interrupt and overrun error
      \arg        USART_INT_FLAG_ERR_NERR: error interrupt and noise error flag
      \arg        USART_INT_FLAG_ERR_FERR: error interrupt and frame error flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus usart_interrupt_flag_get(uint32_t usart_periph, uint32_t int_flag) {
  uint32_t intenable = 0U, flagstatus = 0U;
  /* get the interrupt enable bit status */
  intenable = (USART_REG_VAL(usart_periph, int_flag) & BIT(USART_BIT_POS(int_flag)));
  /* get the corresponding flag bit status */
  flagstatus = (USART_REG_VAL2(usart_periph, int_flag) & BIT(USART_BIT_POS2(int_flag)));

  if (flagstatus && intenable) {
    return SET;
  } else {
    return RESET;
  }
}

/*!
    \brief      clear USART interrupt flag in STAT register
    \param[in]  usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
    \param[in]  int_flag: USART interrupt flag
                only one parameter can be selected which is shown as below:
      \arg        USART_INT_FLAG_CTS: CTS change flag
      \arg        USART_INT_FLAG_LBD: LIN break detected flag
      \arg        USART_INT_FLAG_TC: transmission complete
      \arg        USART_INT_FLAG_RBNE: read data buffer not empty
    \param[out] none
    \retval     none
*/
void usart_interrupt_flag_clear(uint32_t usart_periph, uint32_t int_flag) { USART_REG_VAL2(usart_periph, int_flag) &= ~BIT(USART_BIT_POS2(int_flag)); }
