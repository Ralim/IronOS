/*!
    \file    gd32vf103_spi.c
    \brief   SPI driver

    \version 2019-06-05, V1.0.0, firmware for GD32VF103
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

#include "gd32vf103_spi.h"
#include "gd32vf103_rcu.h"

/* SPI/I2S parameter initialization mask */
#define SPI_INIT_MASK ((uint32_t)0x00003040U) /*!< SPI parameter initialization mask */
#define I2S_INIT_MASK ((uint32_t)0x0000F047U) /*!< I2S parameter initialization mask */

/* I2S clock source selection, multiplication and division mask */
#define I2S1_CLOCK_SEL     ((uint32_t)0x00020000U) /* I2S1 clock source selection */
#define I2S2_CLOCK_SEL     ((uint32_t)0x00040000U) /* I2S2 clock source selection */
#define I2S_CLOCK_MUL_MASK ((uint32_t)0x0000F000U) /* I2S clock multiplication mask */
#define I2S_CLOCK_DIV_MASK ((uint32_t)0x000000F0U) /* I2S clock division mask */

/* default value and offset */
#define SPI_I2SPSC_DEFAULT_VALUE ((uint32_t)0x00000002U) /* default value of SPI_I2SPSC register */
#define RCU_CFG1_PREDV1_OFFSET   4U                      /* PREDV1 offset in RCU_CFG1 */
#define RCU_CFG1_PLL2MF_OFFSET   12U                     /* PLL2MF offset in RCU_CFG1 */

/*!
    \brief      reset SPI and I2S
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void spi_i2s_deinit(uint32_t spi_periph) {
  switch (spi_periph) {
  case SPI0:
    /* reset SPI0 */
    rcu_periph_reset_enable(RCU_SPI0RST);
    rcu_periph_reset_disable(RCU_SPI0RST);
    break;
  case SPI1:
    /* reset SPI1 and I2S1 */
    rcu_periph_reset_enable(RCU_SPI1RST);
    rcu_periph_reset_disable(RCU_SPI1RST);
    break;
  case SPI2:
    /* reset SPI2 and I2S2 */
    rcu_periph_reset_enable(RCU_SPI2RST);
    rcu_periph_reset_disable(RCU_SPI2RST);
    break;
  default:
    break;
  }
}

/*!
    \brief      initialize the parameters of SPI struct with the default values
    \param[in]  spi_struct: SPI parameter stuct
    \param[out] none
    \retval     none
*/
void spi_struct_para_init(spi_parameter_struct *spi_struct) {
  /* set the SPI struct with the default values */
  spi_struct->device_mode          = SPI_SLAVE;
  spi_struct->trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
  spi_struct->frame_size           = SPI_FRAMESIZE_8BIT;
  spi_struct->nss                  = SPI_NSS_HARD;
  spi_struct->clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
  spi_struct->prescale             = SPI_PSC_2;
}

/*!
    \brief      initialize SPI parameter
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[in]  spi_struct: SPI parameter initialization stuct members of the structure
                            and the member values are shown as below:
                  device_mode: SPI_MASTER, SPI_SLAVE
                  trans_mode: SPI_TRANSMODE_FULLDUPLEX, SPI_TRANSMODE_RECEIVEONLY,
                              SPI_TRANSMODE_BDRECEIVE, SPI_TRANSMODE_BDTRANSMIT
                  frame_size: SPI_FRAMESIZE_16BIT, SPI_FRAMESIZE_8BIT
                  nss: SPI_NSS_SOFT, SPI_NSS_HARD
                  endian: SPI_ENDIAN_MSB, SPI_ENDIAN_LSB
                  clock_polarity_phase: SPI_CK_PL_LOW_PH_1EDGE, SPI_CK_PL_HIGH_PH_1EDGE
                                        SPI_CK_PL_LOW_PH_2EDGE, SPI_CK_PL_HIGH_PH_2EDGE
                  prescale: SPI_PSC_n (n=2,4,8,16,32,64,128,256)
    \param[out] none
    \retval     none
*/
void spi_init(uint32_t spi_periph, spi_parameter_struct *spi_struct) {
  uint32_t reg = 0U;
  reg          = SPI_CTL0(spi_periph);
  reg &= SPI_INIT_MASK;

  /* select SPI as master or slave */
  reg |= spi_struct->device_mode;
  /* select SPI transfer mode */
  reg |= spi_struct->trans_mode;
  /* select SPI frame size */
  reg |= spi_struct->frame_size;
  /* select SPI NSS use hardware or software */
  reg |= spi_struct->nss;
  /* select SPI LSB or MSB */
  reg |= spi_struct->endian;
  /* select SPI polarity and phase */
  reg |= spi_struct->clock_polarity_phase;
  /* select SPI prescale to adjust transmit speed */
  reg |= spi_struct->prescale;

  /* write to SPI_CTL0 register */
  SPI_CTL0(spi_periph) = (uint32_t)reg;

  SPI_I2SCTL(spi_periph) &= (uint32_t)(~SPI_I2SCTL_I2SSEL);
}

/*!
    \brief      enable SPI
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void spi_enable(uint32_t spi_periph) { SPI_CTL0(spi_periph) |= (uint32_t)SPI_CTL0_SPIEN; }

/*!
    \brief      disable SPI
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void spi_disable(uint32_t spi_periph) { SPI_CTL0(spi_periph) &= (uint32_t)(~SPI_CTL0_SPIEN); }

/*!
    \brief      initialize I2S parameter
    \param[in]  spi_periph: SPIx(x=1,2)
    \param[in]  mode: I2S operation mode
                only one parameter can be selected which is shown as below:
      \arg        I2S_MODE_SLAVETX: I2S slave transmit mode
      \arg        I2S_MODE_SLAVERX: I2S slave receive mode
      \arg        I2S_MODE_MASTERTX: I2S master transmit mode
      \arg        I2S_MODE_MASTERRX: I2S master receive mode
    \param[in]  standard: I2S standard
                only one parameter can be selected which is shown as below:
      \arg        I2S_STD_PHILLIPS: I2S phillips standard
      \arg        I2S_STD_MSB: I2S MSB standard
      \arg        I2S_STD_LSB: I2S LSB standard
      \arg        I2S_STD_PCMSHORT: I2S PCM short standard
      \arg        I2S_STD_PCMLONG: I2S PCM long standard
    \param[in]  ckpl: I2S idle state clock polarity
                only one parameter can be selected which is shown as below:
      \arg        I2S_CKPL_LOW: I2S clock polarity low level
      \arg        I2S_CKPL_HIGH: I2S clock polarity high level
    \param[out] none
    \retval     none
*/
void i2s_init(uint32_t spi_periph, uint32_t mode, uint32_t standard, uint32_t ckpl) {
  uint32_t reg = 0U;
  reg          = SPI_I2SCTL(spi_periph);
  reg &= I2S_INIT_MASK;

  /* enable I2S mode */
  reg |= (uint32_t)SPI_I2SCTL_I2SSEL;
  /* select I2S mode */
  reg |= (uint32_t)mode;
  /* select I2S standard */
  reg |= (uint32_t)standard;
  /* select I2S polarity */
  reg |= (uint32_t)ckpl;

  /* write to SPI_I2SCTL register */
  SPI_I2SCTL(spi_periph) = (uint32_t)reg;
}

/*!
    \brief      configure I2S prescaler
    \param[in]  spi_periph: SPIx(x=1,2)
    \param[in]  audiosample: I2S audio sample rate
                only one parameter can be selected which is shown as below:
      \arg        I2S_AUDIOSAMPLE_8K: audio sample rate is 8KHz
      \arg        I2S_AUDIOSAMPLE_11K: audio sample rate is 11KHz
      \arg        I2S_AUDIOSAMPLE_16K: audio sample rate is 16KHz
      \arg        I2S_AUDIOSAMPLE_22K: audio sample rate is 22KHz
      \arg        I2S_AUDIOSAMPLE_32K: audio sample rate is 32KHz
      \arg        I2S_AUDIOSAMPLE_44K: audio sample rate is 44KHz
      \arg        I2S_AUDIOSAMPLE_48K: audio sample rate is 48KHz
      \arg        I2S_AUDIOSAMPLE_96K: audio sample rate is 96KHz
      \arg        I2S_AUDIOSAMPLE_192K: audio sample rate is 192KHz
    \param[in]  frameformat: I2S data length and channel length
                only one parameter can be selected which is shown as below:
      \arg        I2S_FRAMEFORMAT_DT16B_CH16B: I2S data length is 16 bit and channel length is 16 bit
      \arg        I2S_FRAMEFORMAT_DT16B_CH32B: I2S data length is 16 bit and channel length is 32 bit
      \arg        I2S_FRAMEFORMAT_DT24B_CH32B: I2S data length is 24 bit and channel length is 32 bit
      \arg        I2S_FRAMEFORMAT_DT32B_CH32B: I2S data length is 32 bit and channel length is 32 bit
    \param[in]  mckout: I2S master clock output
                only one parameter can be selected which is shown as below:
      \arg        I2S_MCKOUT_ENABLE: I2S master clock output enable
      \arg        I2S_MCKOUT_DISABLE: I2S master clock output disable
    \param[out] none
    \retval     none
*/
void i2s_psc_config(uint32_t spi_periph, uint32_t audiosample, uint32_t frameformat, uint32_t mckout) {
  uint32_t i2sdiv = 2U, i2sof = 0U;
  uint32_t clks     = 0U;
  uint32_t i2sclock = 0U;

  /* deinit SPI_I2SPSC register */
  SPI_I2SPSC(spi_periph) = SPI_I2SPSC_DEFAULT_VALUE;

  /* get the I2S clock source */
  if (SPI1 == ((uint32_t)spi_periph)) {
    /* I2S1 clock source selection */
    clks = I2S1_CLOCK_SEL;
  } else {
    /* I2S2 clock source selection */
    clks = I2S2_CLOCK_SEL;
  }

  if (0U != (RCU_CFG1 & clks)) {
    /* get RCU PLL2 clock multiplication factor */
    clks = (uint32_t)((RCU_CFG1 & I2S_CLOCK_MUL_MASK) >> RCU_CFG1_PLL2MF_OFFSET);

    if ((clks > 5U) && (clks < 15U)) {
      /* multiplier is between 8 and 16 */
      clks += 2U;
    } else {
      if (15U == clks) {
        /* multiplier is 20 */
        clks = 20U;
      }
    }

    /* get the PREDV1 value */
    i2sclock = (uint32_t)(((RCU_CFG1 & I2S_CLOCK_DIV_MASK) >> RCU_CFG1_PREDV1_OFFSET) + 1U);
    /* calculate I2S clock based on PLL2 and PREDV1 */
    i2sclock = (uint32_t)((HXTAL_VALUE / i2sclock) * clks * 2U);
  } else {
    /* get system clock */
    i2sclock = rcu_clock_freq_get(CK_SYS);
  }

  /* config the prescaler depending on the mclk output state, the frame format and audio sample rate */
  if (I2S_MCKOUT_ENABLE == mckout) {
    clks = (uint32_t)(((i2sclock / 256U) * 10U) / audiosample);
  } else {
    if (I2S_FRAMEFORMAT_DT16B_CH16B == frameformat) {
      clks = (uint32_t)(((i2sclock / 32U) * 10U) / audiosample);
    } else {
      clks = (uint32_t)(((i2sclock / 64U) * 10U) / audiosample);
    }
  }

  /* remove the floating point */
  clks   = (clks + 5U) / 10U;
  i2sof  = (clks & 0x00000001U);
  i2sdiv = ((clks - i2sof) / 2U);
  i2sof  = (i2sof << 8U);

  /* set the default values */
  if ((i2sdiv < 2U) || (i2sdiv > 255U)) {
    i2sdiv = 2U;
    i2sof  = 0U;
  }
  /* configure SPI_I2SPSC */
  SPI_I2SPSC(spi_periph) = (uint32_t)(i2sdiv | i2sof | mckout);

  /* clear SPI_I2SCTL_DTLEN and SPI_I2SCTL_CHLEN bits */
  SPI_I2SCTL(spi_periph) &= (uint32_t)(~(SPI_I2SCTL_DTLEN | SPI_I2SCTL_CHLEN));
  /* configure data frame format */
  SPI_I2SCTL(spi_periph) |= (uint32_t)frameformat;
}

/*!
    \brief      enable I2S
    \param[in]  spi_periph: SPIx(x=1,2)
    \param[out] none
    \retval     none
*/
void i2s_enable(uint32_t spi_periph) { SPI_I2SCTL(spi_periph) |= (uint32_t)SPI_I2SCTL_I2SEN; }

/*!
    \brief      disable I2S
    \param[in]  spi_periph: SPIx(x=1,2)
    \param[out] none
    \retval     none
*/
void i2s_disable(uint32_t spi_periph) { SPI_I2SCTL(spi_periph) &= (uint32_t)(~SPI_I2SCTL_I2SEN); }

/*!
    \brief      enable SPI NSS output
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void spi_nss_output_enable(uint32_t spi_periph) { SPI_CTL1(spi_periph) |= (uint32_t)SPI_CTL1_NSSDRV; }

/*!
    \brief      disable SPI NSS output
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void spi_nss_output_disable(uint32_t spi_periph) { SPI_CTL1(spi_periph) &= (uint32_t)(~SPI_CTL1_NSSDRV); }

/*!
    \brief      SPI NSS pin high level in software mode
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void spi_nss_internal_high(uint32_t spi_periph) { SPI_CTL0(spi_periph) |= (uint32_t)SPI_CTL0_SWNSS; }

/*!
    \brief      SPI NSS pin low level in software mode
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void spi_nss_internal_low(uint32_t spi_periph) { SPI_CTL0(spi_periph) &= (uint32_t)(~SPI_CTL0_SWNSS); }

/*!
    \brief      enable SPI DMA send or receive
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[in]  dma: SPI DMA mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_DMA_TRANSMIT: SPI transmit data using DMA
      \arg        SPI_DMA_RECEIVE: SPI receive data using DMA
    \param[out] none
    \retval     none
*/
void spi_dma_enable(uint32_t spi_periph, uint8_t dma) {
  if (SPI_DMA_TRANSMIT == dma) {
    SPI_CTL1(spi_periph) |= (uint32_t)SPI_CTL1_DMATEN;
  } else {
    SPI_CTL1(spi_periph) |= (uint32_t)SPI_CTL1_DMAREN;
  }
}

/*!
    \brief      disable SPI DMA send or receive
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[in]  dma: SPI DMA mode
                only one parameter can be selected which is shown as below:
      \arg        SPI_DMA_TRANSMIT: SPI transmit data using DMA
      \arg        SPI_DMA_RECEIVE: SPI receive data using DMA
    \param[out] none
    \retval     none
*/
void spi_dma_disable(uint32_t spi_periph, uint8_t dma) {
  if (SPI_DMA_TRANSMIT == dma) {
    SPI_CTL1(spi_periph) &= (uint32_t)(~SPI_CTL1_DMATEN);
  } else {
    SPI_CTL1(spi_periph) &= (uint32_t)(~SPI_CTL1_DMAREN);
  }
}

/*!
    \brief      configure SPI/I2S data frame format
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[in]  frame_format: SPI frame size
                only one parameter can be selected which is shown as below:
      \arg        SPI_FRAMESIZE_16BIT: SPI frame size is 16 bits
      \arg        SPI_FRAMESIZE_8BIT: SPI frame size is 8 bits
    \param[out] none
    \retval     none
*/
void spi_i2s_data_frame_format_config(uint32_t spi_periph, uint16_t frame_format) {
  /* clear SPI_CTL0_FF16 bit */
  SPI_CTL0(spi_periph) &= (uint32_t)(~SPI_CTL0_FF16);
  /* configure SPI_CTL0_FF16 bit */
  SPI_CTL0(spi_periph) |= (uint32_t)frame_format;
}

/*!
    \brief      SPI transmit data
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[in]  data: 16-bit data
    \param[out] none
    \retval     none
*/
void spi_i2s_data_transmit(uint32_t spi_periph, uint16_t data) { SPI_DATA(spi_periph) = (uint32_t)data; }

/*!
    \brief      SPI receive data
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[out] none
    \retval     16-bit data
*/
uint16_t spi_i2s_data_receive(uint32_t spi_periph) { return ((uint16_t)SPI_DATA(spi_periph)); }

/*!
    \brief      configure SPI bidirectional transfer direction
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[in]  transfer_direction: SPI transfer direction
                only one parameter can be selected which is shown as below:
      \arg        SPI_BIDIRECTIONAL_TRANSMIT: SPI work in transmit-only mode
      \arg        SPI_BIDIRECTIONAL_RECEIVE: SPI work in receive-only mode
    \param[out] none
    \retval     none
*/
void spi_bidirectional_transfer_config(uint32_t spi_periph, uint32_t transfer_direction) {
  if (SPI_BIDIRECTIONAL_TRANSMIT == transfer_direction) {
    /* set the transmit-only mode */
    SPI_CTL0(spi_periph) |= (uint32_t)SPI_BIDIRECTIONAL_TRANSMIT;
  } else {
    /* set the receive-only mode */
    SPI_CTL0(spi_periph) &= SPI_BIDIRECTIONAL_RECEIVE;
  }
}

/*!
    \brief      set SPI CRC polynomial
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[in]  crc_poly: CRC polynomial value
    \param[out] none
    \retval     none
*/
void spi_crc_polynomial_set(uint32_t spi_periph, uint16_t crc_poly) {
  /* enable SPI CRC */
  SPI_CTL0(spi_periph) |= (uint32_t)SPI_CTL0_CRCEN;

  /* set SPI CRC polynomial */
  SPI_CRCPOLY(spi_periph) = (uint32_t)crc_poly;
}

/*!
    \brief      get SPI CRC polynomial
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[out] none
    \retval     16-bit CRC polynomial
*/
uint16_t spi_crc_polynomial_get(uint32_t spi_periph) { return ((uint16_t)SPI_CRCPOLY(spi_periph)); }

/*!
    \brief      turn on CRC function
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void spi_crc_on(uint32_t spi_periph) { SPI_CTL0(spi_periph) |= (uint32_t)SPI_CTL0_CRCEN; }

/*!
    \brief      turn off CRC function
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void spi_crc_off(uint32_t spi_periph) { SPI_CTL0(spi_periph) &= (uint32_t)(~SPI_CTL0_CRCEN); }

/*!
    \brief      SPI next data is CRC value
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void spi_crc_next(uint32_t spi_periph) { SPI_CTL0(spi_periph) |= (uint32_t)SPI_CTL0_CRCNT; }

/*!
    \brief      get SPI CRC send value or receive value
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[in]  crc: SPI crc value
                only one parameter can be selected which is shown as below:
      \arg        SPI_CRC_TX: get transmit crc value
      \arg        SPI_CRC_RX: get receive crc value
    \param[out] none
    \retval     16-bit CRC value
*/
uint16_t spi_crc_get(uint32_t spi_periph, uint8_t crc) {
  if (SPI_CRC_TX == crc) {
    return ((uint16_t)(SPI_TCRC(spi_periph)));
  } else {
    return ((uint16_t)(SPI_RCRC(spi_periph)));
  }
}

/*!
    \brief      enable SPI TI mode
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void spi_ti_mode_enable(uint32_t spi_periph) { SPI_CTL1(spi_periph) |= (uint32_t)SPI_CTL1_TMOD; }

/*!
    \brief      disable SPI TI mode
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void spi_ti_mode_disable(uint32_t spi_periph) { SPI_CTL1(spi_periph) &= (uint32_t)(~SPI_CTL1_TMOD); }

/*!
    \brief      enable SPI NSS pulse mode
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void spi_nssp_mode_enable(uint32_t spi_periph) { SPI_CTL1(spi_periph) |= (uint32_t)SPI_CTL1_NSSP; }

/*!
    \brief      disable SPI NSS pulse mode
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void spi_nssp_mode_disable(uint32_t spi_periph) { SPI_CTL1(spi_periph) &= (uint32_t)(~SPI_CTL1_NSSP); }

/*!
    \brief      enable SPI and I2S interrupt
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[in]  interrupt: SPI/I2S interrupt
                only one parameter can be selected which is shown as below:
      \arg        SPI_I2S_INT_TBE: transmit buffer empty interrupt
      \arg        SPI_I2S_INT_RBNE: receive buffer not empty interrupt
      \arg        SPI_I2S_INT_ERR: CRC error,configuration error,reception overrun error,
                                   transmission underrun error and format error interrupt
    \param[out] none
    \retval     none
*/
void spi_i2s_interrupt_enable(uint32_t spi_periph, uint8_t interrupt) {
  switch (interrupt) {
  /* SPI/I2S transmit buffer empty interrupt */
  case SPI_I2S_INT_TBE:
    SPI_CTL1(spi_periph) |= (uint32_t)SPI_CTL1_TBEIE;
    break;
  /* SPI/I2S receive buffer not empty interrupt */
  case SPI_I2S_INT_RBNE:
    SPI_CTL1(spi_periph) |= (uint32_t)SPI_CTL1_RBNEIE;
    break;
  /* SPI/I2S error */
  case SPI_I2S_INT_ERR:
    SPI_CTL1(spi_periph) |= (uint32_t)SPI_CTL1_ERRIE;
    break;
  default:
    break;
  }
}

/*!
    \brief      disable SPI and I2S interrupt
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[in]  interrupt: SPI/I2S interrupt
                only one parameter can be selected which is shown as below:
      \arg        SPI_I2S_INT_TBE: transmit buffer empty interrupt
      \arg        SPI_I2S_INT_RBNE: receive buffer not empty interrupt
      \arg        SPI_I2S_INT_ERR: CRC error,configuration error,reception overrun error,
                                   transmission underrun error and format error interrupt
    \param[out] none
    \retval     none
*/
void spi_i2s_interrupt_disable(uint32_t spi_periph, uint8_t interrupt) {
  switch (interrupt) {
  /* SPI/I2S transmit buffer empty interrupt */
  case SPI_I2S_INT_TBE:
    SPI_CTL1(spi_periph) &= (uint32_t)(~SPI_CTL1_TBEIE);
    break;
  /* SPI/I2S receive buffer not empty interrupt */
  case SPI_I2S_INT_RBNE:
    SPI_CTL1(spi_periph) &= (uint32_t)(~SPI_CTL1_RBNEIE);
    break;
  /* SPI/I2S error */
  case SPI_I2S_INT_ERR:
    SPI_CTL1(spi_periph) &= (uint32_t)(~SPI_CTL1_ERRIE);
    break;
  default:
    break;
  }
}

/*!
    \brief      get SPI and I2S interrupt flag status
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[in]  interrupt: SPI/I2S interrupt flag status
                only one parameter can be selected which is shown as below:
      \arg        SPI_I2S_INT_FLAG_TBE: transmit buffer empty interrupt flag
      \arg        SPI_I2S_INT_FLAG_RBNE: receive buffer not empty interrupt flag
      \arg        SPI_I2S_INT_FLAG_RXORERR: overrun interrupt flag
      \arg        SPI_INT_FLAG_CONFERR: config error interrupt flag
      \arg        SPI_INT_FLAG_CRCERR: CRC error interrupt flag
      \arg        I2S_INT_FLAG_TXURERR: underrun error interrupt flag
      \arg        SPI_I2S_INT_FLAG_FERR: format error interrupt flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus spi_i2s_interrupt_flag_get(uint32_t spi_periph, uint8_t interrupt) {
  uint32_t reg1 = SPI_STAT(spi_periph);
  uint32_t reg2 = SPI_CTL1(spi_periph);

  switch (interrupt) {
  /* SPI/I2S transmit buffer empty interrupt */
  case SPI_I2S_INT_FLAG_TBE:
    reg1 = reg1 & SPI_STAT_TBE;
    reg2 = reg2 & SPI_CTL1_TBEIE;
    break;
  /* SPI/I2S receive buffer not empty interrupt */
  case SPI_I2S_INT_FLAG_RBNE:
    reg1 = reg1 & SPI_STAT_RBNE;
    reg2 = reg2 & SPI_CTL1_RBNEIE;
    break;
  /* SPI/I2S overrun interrupt */
  case SPI_I2S_INT_FLAG_RXORERR:
    reg1 = reg1 & SPI_STAT_RXORERR;
    reg2 = reg2 & SPI_CTL1_ERRIE;
    break;
  /* SPI config error interrupt */
  case SPI_INT_FLAG_CONFERR:
    reg1 = reg1 & SPI_STAT_CONFERR;
    reg2 = reg2 & SPI_CTL1_ERRIE;
    break;
  /* SPI CRC error interrupt */
  case SPI_INT_FLAG_CRCERR:
    reg1 = reg1 & SPI_STAT_CRCERR;
    reg2 = reg2 & SPI_CTL1_ERRIE;
    break;
  /* I2S underrun error interrupt */
  case I2S_INT_FLAG_TXURERR:
    reg1 = reg1 & SPI_STAT_TXURERR;
    reg2 = reg2 & SPI_CTL1_ERRIE;
    break;
  /* SPI/I2S format error interrupt */
  case SPI_I2S_INT_FLAG_FERR:
    reg1 = reg1 & SPI_STAT_FERR;
    reg2 = reg2 & SPI_CTL1_ERRIE;
    break;
  default:
    break;
  }
  /* get SPI/I2S interrupt flag status */
  if ((0U != reg1) && (0U != reg2)) {
    return SET;
  } else {
    return RESET;
  }
}

/*!
    \brief      get SPI and I2S flag status
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[in]  flag: SPI/I2S flag status
                one or more parameters can be selected which are shown as below:
      \arg        SPI_FLAG_TBE: transmit buffer empty flag
      \arg        SPI_FLAG_RBNE: receive buffer not empty flag
      \arg        SPI_FLAG_TRANS: transmit on-going flag
      \arg        SPI_FLAG_RXORERR: receive overrun error flag
      \arg        SPI_FLAG_CONFERR: mode config error flag
      \arg        SPI_FLAG_CRCERR: CRC error flag
      \arg        SPI_FLAG_FERR: format error interrupt flag
      \arg        I2S_FLAG_TBE: transmit buffer empty flag
      \arg        I2S_FLAG_RBNE: receive buffer not empty flag
      \arg        I2S_FLAG_TRANS: transmit on-going flag
      \arg        I2S_FLAG_RXORERR: overrun error flag
      \arg        I2S_FLAG_TXURERR: underrun error flag
      \arg        I2S_FLAG_CH: channel side flag
      \arg        I2S_FLAG_FERR: format error interrupt flag
    \param[out] none
    \retval     FlagStatus: SET or RESET
*/
FlagStatus spi_i2s_flag_get(uint32_t spi_periph, uint32_t flag) {
  if (RESET != (SPI_STAT(spi_periph) & flag)) {
    return SET;
  } else {
    return RESET;
  }
}

/*!
    \brief      clear SPI CRC error flag status
    \param[in]  spi_periph: SPIx(x=0,1,2)
    \param[out] none
    \retval     none
*/
void spi_crc_error_clear(uint32_t spi_periph) { SPI_STAT(spi_periph) &= (uint32_t)(~SPI_FLAG_CRCERR); }
