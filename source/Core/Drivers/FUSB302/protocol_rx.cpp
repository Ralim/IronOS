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

#include "protocol_rx.h"

#include "fusb302b.h"
#include "policy_engine.h"
#include "protocol_tx.h"
#include "string.h"
#include <pd.h>
#include <stdlib.h>
osThreadId          ProtocolReceive::TaskHandle        = NULL;
EventGroupHandle_t  ProtocolReceive::xEventGroupHandle = NULL;
StaticEventGroup_t  ProtocolReceive::xCreatedEventGroup;
uint32_t            ProtocolReceive::TaskBuffer[ProtocolReceive::TaskStackSize];
osStaticThreadDef_t ProtocolReceive::TaskControlBlock;
union pd_msg        ProtocolReceive::tempMessage;
uint8_t             ProtocolReceive::_rx_messageid;
uint8_t             ProtocolReceive::_tx_messageidcounter;
/*
 * PRL_Rx_Wait_for_PHY_Message state
 */
ProtocolReceive::protocol_rx_state ProtocolReceive::protocol_rx_wait_phy() {
  /* Wait for an event */
  _rx_messageid   = 0;
  eventmask_t evt = waitForEvent(PDB_EVT_PRLRX_RESET | PDB_EVT_PRLRX_I_GCRCSENT | PDB_EVT_PRLRX_I_RXPEND);

  /* If we got a reset event, reset */
  if (evt & PDB_EVT_PRLRX_RESET) {
    waitForEvent(PDB_EVT_PRLRX_RESET, 0);
    return PRLRxWaitPHY;
  }
  /* If we got an I_GCRCSENT event, read the message and decide what to do */
  if (evt & PDB_EVT_PRLRX_I_GCRCSENT) {
    /* Get a buffer to read the message into.  Guaranteed to not fail
     * because we have a big enough pool and are careful. */
    union pd_msg *_rx_message = &tempMessage;
    memset(&tempMessage, 0, sizeof(tempMessage));
    /* Read the message */
    fusb_read_message(_rx_message);
    /* If it's a Soft_Reset, go to the soft reset state */
    if (PD_MSGTYPE_GET(_rx_message) == PD_MSGTYPE_SOFT_RESET && PD_NUMOBJ_GET(_rx_message) == 0) {
      return PRLRxReset;
    } else {
      /* Otherwise, check the message ID */
      return PRLRxCheckMessageID;
    }
  } else if (evt & PDB_EVT_PRLRX_I_RXPEND) {
    // There is an RX message pending that is not a Good CRC
    union pd_msg *_rx_message = &tempMessage;
    /* Read the message */
    fusb_read_message(_rx_message);
    return PRLRxWaitPHY;
  }

  return PRLRxWaitPHY;
}

/*
 * PRL_Rx_Layer_Reset_for_Receive state
 */
ProtocolReceive::protocol_rx_state ProtocolReceive::protocol_rx_reset() {
  /* Reset MessageIDCounter */
  _tx_messageidcounter = 0;

  /* Clear stored MessageID */
  _rx_messageid = -1;

  /* TX transitions to its reset state */
  ProtocolTransmit::notify(ProtocolTransmit::Notifications::PDB_EVT_PRLTX_RESET);
  taskYIELD();

  /* If we got a RESET signal, reset the machine */
  if (waitForEvent(PDB_EVT_PRLRX_RESET, 0) != 0) {
    return PRLRxWaitPHY;
  }

  /* Go to the Check_MessageID state */
  return PRLRxCheckMessageID;
}
volatile uint32_t rxCounter = 0;
/*
 * PRL_Rx_Check_MessageID state
 */
ProtocolReceive::protocol_rx_state ProtocolReceive::protocol_rx_check_messageid() {
  /* If we got a RESET signal, reset the machine */
  //	if (waitForEvent(PDB_EVT_PRLRX_RESET, 0) == PDB_EVT_PRLRX_RESET) {
  //		return PRLRxWaitPHY;
  //	}
  /* If the message has the stored ID, we've seen this message before.  Free
   * it and don't pass it to the policy engine. */

  /* Otherwise, there's either no stored ID or this message has an ID we
   * haven't just seen.  Transition to the Store_MessageID state. */
  //	if (PD_MESSAGEID_GET(&tempMessage) == _rx_messageid) {
  //		return PRLRxWaitPHY;
  //	} else
  {
    rxCounter++;
    return PRLRxStoreMessageID;
  }
}

/*
 * PRL_Rx_Store_MessageID state
 */
ProtocolReceive::protocol_rx_state ProtocolReceive::protocol_rx_store_messageid() {
  /* Tell ProtocolTX to discard the message being transmitted */

  ProtocolTransmit::notify(ProtocolTransmit::Notifications::PDB_EVT_PRLTX_DISCARD);

  /* Update the stored MessageID */
  _rx_messageid = PD_MESSAGEID_GET(&tempMessage);

  /* Pass the message to the policy engine. */

  PolicyEngine::handleMessage(&tempMessage);
  PolicyEngine::notify(PDB_EVT_PE_MSG_RX);
  taskYIELD();
  /* Don't check if we got a RESET because we'd do nothing different. */

  return PRLRxWaitPHY;
}

void ProtocolReceive::init() {
  osThreadStaticDef(protRX, thread, PDB_PRIO_PRL, 0, TaskStackSize, TaskBuffer, &TaskControlBlock);
  xEventGroupHandle = xEventGroupCreateStatic(&xCreatedEventGroup);
  TaskHandle        = osThreadCreate(osThread(protRX), NULL);
}

void ProtocolReceive::thread(const void *args) {
  (void)args;
  ProtocolReceive::protocol_rx_state state = PRLRxWaitPHY;

  while (true) {
    switch (state) {
    case PRLRxWaitPHY:
      state = protocol_rx_wait_phy();
      break;
    case PRLRxReset:
      state = protocol_rx_reset();
      break;
    case PRLRxCheckMessageID:
      state = protocol_rx_check_messageid();
      break;
    case PRLRxStoreMessageID:
      state = protocol_rx_store_messageid();
      break;
    default:
      /* This is an error.  It really shouldn't happen.  We might
       * want to handle it anyway, though. */
      state = PRLRxWaitPHY;
      break;
    }
  }
}

void ProtocolReceive::notify(uint32_t notification) {
  if (xEventGroupHandle != NULL) {
    xEventGroupSetBits(xEventGroupHandle, notification);
  }
}

uint32_t ProtocolReceive::waitForEvent(uint32_t mask, TickType_t ticksToWait) {
  if (xEventGroupHandle != NULL) {
    return xEventGroupWaitBits(xEventGroupHandle, mask, mask, pdFALSE, ticksToWait);
  }
  return 0;
}
