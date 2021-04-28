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
#include "fusb302b.h"
#include "BSP.h"
#include "I2C_Wrapper.hpp"
#include "Setup.h"
#include "fusb_user.h"
#include "int_n.h"
#include <pd.h>
void fusb_send_message(const union pd_msg *msg) {

  /* Token sequences for the FUSB302B */
  static uint8_t       sop_seq[5] = {FUSB_FIFO_TX_SOP1, FUSB_FIFO_TX_SOP1, FUSB_FIFO_TX_SOP1, FUSB_FIFO_TX_SOP2, FUSB_FIFO_TX_PACKSYM};
  static const uint8_t eop_seq[4] = {FUSB_FIFO_TX_JAM_CRC, FUSB_FIFO_TX_EOP, FUSB_FIFO_TX_TXOFF, FUSB_FIFO_TX_TXON};

  /* Take the I2C2 mutex now so there can't be a race condition on sop_seq */
  /* Get the length of the message: a two-octet header plus NUMOBJ four-octet
   * data objects */
  uint8_t msg_len = 2 + 4 * PD_NUMOBJ_GET(msg);

  /* Set the number of bytes to be transmitted in the packet */
  sop_seq[4] = FUSB_FIFO_TX_PACKSYM | msg_len;

  /* Write all three parts of the message to the TX FIFO */
  fusb_write_buf(FUSB_FIFOS, 5, sop_seq);
  fusb_write_buf(FUSB_FIFOS, msg_len, msg->bytes);
  fusb_write_buf(FUSB_FIFOS, 4, eop_seq);
}

uint8_t fusb_read_message(union pd_msg *msg) {

  static uint8_t garbage[4];
  uint8_t        numobj;

  // Read the header. If its not a SOP we dont actually want it at all
  // But on some revisions of the fusb if you dont both pick them up and read them out of the fifo, it gets stuck
  fusb_read_byte(FUSB_FIFOS);
  /* Read the message header into msg */
  fusb_read_buf(FUSB_FIFOS, 2, msg->bytes);
  /* Get the number of data objects */
  numobj = PD_NUMOBJ_GET(msg);
  /* If there is at least one data object, read the data objects */
  if (numobj > 0) {
    fusb_read_buf(FUSB_FIFOS, numobj * 4, msg->bytes + 2);
  }
  /* Throw the CRC32 in the garbage, since the PHY already checked it. */
  fusb_read_buf(FUSB_FIFOS, 4, garbage);

  return 0;
}

void fusb_send_hardrst() {

  /* Send a hard reset */
  fusb_write_byte(FUSB_CONTROL3, 0x07 | FUSB_CONTROL3_SEND_HARD_RESET);
}

bool fusb_setup() {
  /* Fully reset the FUSB302B */
  fusb_write_byte(FUSB_RESET, FUSB_RESET_SW_RES);
  vTaskDelay(TICKS_10MS);
  uint8_t tries = 0;
  while (!fusb_read_id()) {
    vTaskDelay(TICKS_10MS);
    tries++;
    if (tries > 5) {
      return false; // Welp :(
    }
  }

  /* Turn on all power */
  fusb_write_byte(FUSB_POWER, 0x0F);

  /* Set interrupt masks */
  // Setting to 0 so interrupts are allowed
  fusb_write_byte(FUSB_MASK1, 0x00);
  fusb_write_byte(FUSB_MASKA, 0x00);
  fusb_write_byte(FUSB_MASKB, 0x00);
  fusb_write_byte(FUSB_CONTROL0, 0b11 << 2);

  /* Enable automatic retransmission */
  fusb_write_byte(FUSB_CONTROL3, 0x07);
  // set defaults
  fusb_write_byte(FUSB_CONTROL2, 0x00);
  /* Flush the RX buffer */
  fusb_write_byte(FUSB_CONTROL1, FUSB_CONTROL1_RX_FLUSH);

  /* Measure CC1 */
  fusb_write_byte(FUSB_SWITCHES0, 0x07);
  vTaskDelay(TICKS_10MS);
  uint8_t cc1 = fusb_read_byte(FUSB_STATUS0) & FUSB_STATUS0_BC_LVL;

  /* Measure CC2 */
  fusb_write_byte(FUSB_SWITCHES0, 0x0B);
  vTaskDelay(TICKS_10MS);
  uint8_t cc2 = fusb_read_byte(FUSB_STATUS0) & FUSB_STATUS0_BC_LVL;

  /* Select the correct CC line for BMC signaling; also enable AUTO_CRC */
  if (cc1 > cc2) {
    fusb_write_byte(FUSB_SWITCHES1, 0x25);
    fusb_write_byte(FUSB_SWITCHES0, 0x07);
  } else {
    fusb_write_byte(FUSB_SWITCHES1, 0x26);
    fusb_write_byte(FUSB_SWITCHES0, 0x0B);
  }

  fusb_reset();
  setupFUSBIRQ();
  return true;
}

void fusb_get_status(union fusb_status *status) {

  /* Read the interrupt and status flags into status */
  fusb_read_buf(FUSB_STATUS0A, 7, status->bytes);
}

enum fusb_typec_current fusb_get_typec_current() {

  /* Read the BC_LVL into a variable */
  enum fusb_typec_current bc_lvl = (enum fusb_typec_current)(fusb_read_byte(FUSB_STATUS0) & FUSB_STATUS0_BC_LVL);

  return bc_lvl;
}

void fusb_reset() {

  /* Flush the TX buffer */
  fusb_write_byte(FUSB_CONTROL0, 0x44);
  /* Flush the RX buffer */
  fusb_write_byte(FUSB_CONTROL1, FUSB_CONTROL1_RX_FLUSH);
  /* Reset the PD logic */
  //	fusb_write_byte( FUSB_RESET, FUSB_RESET_PD_RESET);
}

bool fusb_read_id() {
  // Return true if read of the revision ID is sane
  uint8_t version = 0;
  fusb_read_buf(FUSB_DEVICE_ID, 1, &version);
  if (version == 0 || version == 0xFF)
    return false;
  return true;
}