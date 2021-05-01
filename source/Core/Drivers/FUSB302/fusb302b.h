/*
 * PD Buddy Firmware Library - USB Power Delivery for everyone
 * Copyright 2017-2018 Clayton G. Hobbs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PDB_FUSB302B_H
#define PDB_FUSB302B_H

#include <stdint.h>

#include "pd.h"
#include <pdb_msg.h>

/* I2C addresses of the FUSB302B chips */
#define FUSB302B_ADDR   (0x22 << 1)
#define FUSB302B01_ADDR (0x23 << 1)
#define FUSB302B10_ADDR (0x24 << 1)
#define FUSB302B11_ADDR (0x25 << 1)

/* Device ID register */
#define FUSB_DEVICE_ID                   0x01
#define FUSB_DEVICE_ID_VERSION_ID_SHIFT  4
#define FUSB_DEVICE_ID_VERSION_ID        (0xF << FUSB_DEVICE_ID_VERSION_ID_SHIFT)
#define FUSB_DEVICE_ID_PRODUCT_ID_SHIFT  2
#define FUSB_DEVICE_ID_PRODUCT_ID        (0x3 << FUSB_DEVICE_ID_PRODUCT_ID_SHIFT)
#define FUSB_DEVICE_ID_REVISION_ID_SHIFT 0
#define FUSB_DEVICE_ID_REVISION_ID       (0x3 << FUSB_DEVICE_ID_REVISION_ID_SHIFT)

/* Switches0 register */
#define FUSB_SWITCHES0           0x02
#define FUSB_SWITCHES0_PU_EN2    (1 << 7)
#define FUSB_SWITCHES0_PU_EN1    (1 << 6)
#define FUSB_SWITCHES0_VCONN_CC2 (1 << 5)
#define FUSB_SWITCHES0_VCONN_CC1 (1 << 4)
#define FUSB_SWITCHES0_MEAS_CC2  (1 << 3)
#define FUSB_SWITCHES0_MEAS_CC1  (1 << 2)
#define FUSB_SWITCHES0_PDWN_2    (1 << 1)
#define FUSB_SWITCHES0_PDWN_1    1

/* Switches1 register */
#define FUSB_SWITCHES1               0x03
#define FUSB_SWITCHES1_POWERROLE     (1 << 7)
#define FUSB_SWITCHES1_SPECREV_SHIFT 5
#define FUSB_SWITCHES1_SPECREV       (0x3 << FUSB_SWITCHES1_SPECREV_SHIFT)
#define FUSB_SWITCHES1_DATAROLE      (1 << 4)
#define FUSB_SWITCHES1_AUTO_CRC      (1 << 2)
#define FUSB_SWITCHES1_TXCC2         (1 << 1)
#define FUSB_SWITCHES1_TXCC1         1

/* Measure register */
#define FUSB_MEASURE            0x04
#define FUSB_MEASURE_MEAS_VBUS  (1 << 6)
#define FUSB_MEASURE_MDAC_SHIFT 0
#define FUSB_MEASURE_MDAC       (0x3F << FUSB_MEASURE_MDAC_SHIFT)

/* Slice register */
#define FUSB_SLICE                0x05
#define FUSB_SLICE_SDAC_HYS_SHIFT 6
#define FUSB_SLICE_SDAC_HYS       (0x3 << FUSB_SLICE_SDAC_HYS_SHIFT)
#define FUSB_SLICE_SDAC_SHIFT     0
#define FUSB_SLICE_SDAC           (0x3F << FUSB_SLICE_SDAC_SHIFT)

/* Control0 register */
#define FUSB_CONTROL0                0x06
#define FUSB_CONTROL0_TX_FLUSH       (1 << 6)
#define FUSB_CONTROL0_INT_MASK       (1 << 5)
#define FUSB_CONTROL0_HOST_CUR_SHIFT 2
#define FUSB_CONTROL0_HOST_CUR       (0x3 << FUSB_CONTROL0_HOST_CUR_SHIFT)
#define FUSB_CONTROL0_AUTO_PRE       (1 << 1)
#define FUSB_CONTROL0_TX_START       1

/* Control1 register */
#define FUSB_CONTROL1            0x07
#define FUSB_CONTROL1_ENSOP2DB   (1 << 6)
#define FUSB_CONTROL1_ENSOP1DB   (1 << 5)
#define FUSB_CONTROL1_BIST_MODE2 (1 << 4)
#define FUSB_CONTROL1_RX_FLUSH   (1 << 2)
#define FUSB_CONTROL1_ENSOP2     (1 << 1)
#define FUSB_CONTROL1_ENSOP1     1

/* Control2 register */
#define FUSB_CONTROL2                    0x08
#define FUSB_CONTROL2_TOG_SAVE_PWR_SHIFT 6
#define FUSB_CONTROL2_TOG_SAVE_PWR       (0x3 << FUSB_CONTROL2_TOG_SAVE_PWR)
#define FUSB_CONTROL2_TOG_RD_ONLY        (1 << 5)
#define FUSB_CONTROL2_WAKE_EN            (1 << 3)
#define FUSB_CONTROL2_MODE_SHIFT         1
#define FUSB_CONTROL2_MODE               (0x3 << FUSB_CONTROL2_MODE_SHIFT)
#define FUSB_CONTROL2_TOGGLE             1

/* Control3 register */
#define FUSB_CONTROL3                 0x09
#define FUSB_CONTROL3_SEND_HARD_RESET (1 << 6)
#define FUSB_CONTROL3_BIST_TMODE      (1 << 5)
#define FUSB_CONTROL3_AUTO_HARDRESET  (1 << 4)
#define FUSB_CONTROL3_AUTO_SOFTRESET  (1 << 3)
#define FUSB_CONTROL3_N_RETRIES_SHIFT 1
#define FUSB_CONTROL3_N_RETRIES       (0x3 << FUSB_CONTROL3_N_RETRIES_SHIFT)
#define FUSB_CONTROL3_AUTO_RETRY      1

/* Mask1 register */
#define FUSB_MASK1             0x0A
#define FUSB_MASK1_M_VBUSOK    (1 << 7)
#define FUSB_MASK1_M_ACTIVITY  (1 << 6)
#define FUSB_MASK1_M_COMP_CHNG (1 << 5)
#define FUSB_MASK1_M_CRC_CHK   (1 << 4)
#define FUSB_MASK1_M_ALERT     (1 << 3)
#define FUSB_MASK1_M_WAKE      (1 << 2)
#define FUSB_MASK1_M_COLLISION (1 << 1)
#define FUSB_MASK1_M_BC_LVL    (1 << 0)

/* Power register */
#define FUSB_POWER      0x0B
#define FUSB_POWER_PWR3 (1 << 3)
#define FUSB_POWER_PWR2 (1 << 2)
#define FUSB_POWER_PWR1 (1 << 1)
#define FUSB_POWER_PWR0 1

/* Reset register */
#define FUSB_RESET          0x0C
#define FUSB_RESET_PD_RESET (1 << 1)
#define FUSB_RESET_SW_RES   1

/* OCPreg register */
#define FUSB_OCPREG               0x0D
#define FUSB_OCPREG_OCP_RANGE     (1 << 3)
#define FUSB_OCPREG_OCP_CUR_SHIFT 0
#define FUSB_OCPREG_OCP_CUR       (0x7 << FUSB_OCPREG_OCP_CUR_SHIFT)

/* Maska register */
#define FUSB_MASKA             0x0E
#define FUSB_MASKA_M_OCP_TEMP  (1 << 7)
#define FUSB_MASKA_M_TOGDONE   (1 << 6)
#define FUSB_MASKA_M_SOFTFAIL  (1 << 5)
#define FUSB_MASKA_M_RETRYFAIL (1 << 4)
#define FUSB_MASKA_M_HARDSENT  (1 << 3)
#define FUSB_MASKA_M_TXSENT    (1 << 2)
#define FUSB_MASKA_M_SOFTRST   (1 << 1)
#define FUSB_MASKA_M_HARDRST   1

/* Maskb register */
#define FUSB_MASKB            0x0F
#define FUSB_MASKB_M_GCRCSENT 1

/* Control4 register */
#define FUSB_CONTROL4              0x10
#define FUSB_CONTROL4_TOG_EXIT_AUD 1

/* Status0a register */
#define FUSB_STATUS0A           0x3C
#define FUSB_STATUS0A_SOFTFAIL  (1 << 5)
#define FUSB_STATUS0A_RETRYFAIL (1 << 4)
#define FUSB_STATUS0A_POWER3    (1 << 3)
#define FUSB_STATUS0A_POWER2    (1 << 2)
#define FUSB_STATUS0A_SOFTRST   (1 << 1)
#define FUSB_STATUS0A_HARDRST   1

/* Status1a register */
#define FUSB_STATUS1A             0x3D
#define FUSB_STATUS1A_TOGSS_SHIFT 3
#define FUSB_STATUS1A_TOGSS       (0x7 << FUSB_STATUS1A_TOGSS_SHIFT)
#define FUSB_STATUS1A_RXSOP2DB    (1 << 2)
#define FUSB_STATUS1A_RXSOP1DB    (1 << 1)
#define FUSB_STATUS1A_RXSOP       1

/* Interrupta register */
#define FUSB_INTERRUPTA             0x3E
#define FUSB_INTERRUPTA_I_OCP_TEMP  (1 << 7)
#define FUSB_INTERRUPTA_I_TOGDONE   (1 << 6)
#define FUSB_INTERRUPTA_I_SOFTFAIL  (1 << 5)
#define FUSB_INTERRUPTA_I_RETRYFAIL (1 << 4)
#define FUSB_INTERRUPTA_I_HARDSENT  (1 << 3)
#define FUSB_INTERRUPTA_I_TXSENT    (1 << 2)
#define FUSB_INTERRUPTA_I_SOFTRST   (1 << 1)
#define FUSB_INTERRUPTA_I_HARDRST   1

/* Interruptb register */
#define FUSB_INTERRUPTB            0x3F
#define FUSB_INTERRUPTB_I_GCRCSENT 1

/* Status0 register */
#define FUSB_STATUS0              0x40
#define FUSB_STATUS0_VBUSOK       (1 << 7)
#define FUSB_STATUS0_ACTIVITY     (1 << 6)
#define FUSB_STATUS0_COMP         (1 << 5)
#define FUSB_STATUS0_CRC_CHK      (1 << 4)
#define FUSB_STATUS0_ALERT        (1 << 3)
#define FUSB_STATUS0_WAKE         (1 << 2)
#define FUSB_STATUS0_BC_LVL_SHIFT 0
#define FUSB_STATUS0_BC_LVL       (0x3 << FUSB_STATUS0_BC_LVL_SHIFT)

/* Status1 register */
#define FUSB_STATUS1          0x41
#define FUSB_STATUS1_RXSOP2   (1 << 7)
#define FUSB_STATUS1_RXSOP1   (1 << 6)
#define FUSB_STATUS1_RX_EMPTY (1 << 5)
#define FUSB_STATUS1_RX_FULL  (1 << 4)
#define FUSB_STATUS1_TX_EMPTY (1 << 3)
#define FUSB_STATUS1_TX_FULL  (1 << 2)
#define FUSB_STATUS1_OVRTEMP  (1 << 1)
#define FUSB_STATUS1_OCP      1

/* Interrupt register */
#define FUSB_INTERRUPT             0x42
#define FUSB_INTERRUPT_I_VBUSOK    (1 << 7)
#define FUSB_INTERRUPT_I_ACTIVITY  (1 << 6)
#define FUSB_INTERRUPT_I_COMP_CHNG (1 << 5)
#define FUSB_INTERRUPT_I_CRC_CHK   (1 << 4)
#define FUSB_INTERRUPT_I_ALERT     (1 << 3)
#define FUSB_INTERRUPT_I_WAKE      (1 << 2)
#define FUSB_INTERRUPT_I_COLLISION (1 << 1)
#define FUSB_INTERRUPT_I_BC_LVL    1

/* FIFOs register */
#define FUSB_FIFOS 0x43

#define FUSB_FIFO_TX_TXON    0xA1
#define FUSB_FIFO_TX_SOP1    0x12
#define FUSB_FIFO_TX_SOP2    0x13
#define FUSB_FIFO_TX_SOP3    0x1B
#define FUSB_FIFO_TX_RESET1  0x15
#define FUSB_FIFO_TX_RESET2  0x16
#define FUSB_FIFO_TX_PACKSYM 0x80
#define FUSB_FIFO_TX_JAM_CRC 0xFF
#define FUSB_FIFO_TX_EOP     0x14
#define FUSB_FIFO_TX_TXOFF   0xFE

#define FUSB_FIFO_RX_TOKEN_BITS 0xE0
#define FUSB_FIFO_RX_SOP        0xE0
#define FUSB_FIFO_RX_SOP1       0xC0
#define FUSB_FIFO_RX_SOP2       0xA0
#define FUSB_FIFO_RX_SOP1DB     0x80
#define FUSB_FIFO_RX_SOP2DB     0x60

/*
 * FUSB status union
 *
 * Provides a nicer structure than just an array of uint8_t for working with
 * the FUSB302B status and interrupt flags.
 */
union fusb_status {
  uint8_t bytes[7];
  struct {
    uint8_t status0a;
    uint8_t status1a;
    uint8_t interrupta;
    uint8_t interruptb;
    uint8_t status0;
    uint8_t status1;
    uint8_t interrupt;
  };
};

/* FUSB functions */

/*
 * Send a USB Power Delivery message to the FUSB302B
 */
void fusb_send_message(const union pd_msg *msg);
bool fusb_rx_pending();
/*
 * Read a USB Power Delivery message from the FUSB302B
 */
uint8_t fusb_read_message(union pd_msg *msg);

/*
 * Tell the FUSB302B to send a hard reset signal
 */
void fusb_send_hardrst();

/*
 * Read the FUSB302B status and interrupt flags into *status
 */
bool fusb_get_status(union fusb_status *status);

/*
 * Read the FUSB302B BC_LVL as an enum fusb_typec_current
 */
enum fusb_typec_current fusb_get_typec_current();

/*
 * Initialization routine for the FUSB302B
 */
bool fusb_setup();

/*
 * Reset the FUSB302B
 */
void fusb_reset();

bool fusb_read_id();

#endif /* PDB_FUSB302B_H */
