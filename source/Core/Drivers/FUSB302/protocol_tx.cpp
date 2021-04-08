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

#include "protocol_tx.h"
#include "Defines.h"
#include "fusb302b.h"
#include "fusbpd.h"
#include "policy_engine.h"
#include <pd.h>

osThreadId          ProtocolTransmit::TaskHandle = NULL;
uint32_t            ProtocolTransmit::TaskBuffer[ProtocolTransmit::TaskStackSize];
osStaticThreadDef_t ProtocolTransmit::TaskControlBlock;
StaticQueue_t       ProtocolTransmit::xStaticQueue;
bool                ProtocolTransmit::messageSending = false;
uint8_t             ProtocolTransmit::ucQueueStorageArea[PDB_MSG_POOL_SIZE * sizeof(union pd_msg)];
QueueHandle_t       ProtocolTransmit::messagesWaiting = NULL;
uint8_t             ProtocolTransmit::_tx_messageidcounter;
union pd_msg        ProtocolTransmit::temp_msg;
EventGroupHandle_t  ProtocolTransmit::xEventGroupHandle = NULL;
StaticEventGroup_t  ProtocolTransmit::xCreatedEventGroup;
/*
 * PRL_Tx_PHY_Layer_Reset state
 */
ProtocolTransmit::protocol_tx_state ProtocolTransmit::protocol_tx_phy_reset() {
  /* Reset the PHY */
  fusb_reset();

  /* If a message was pending when we got here, tell the policy engine that
   * we failed to send it */
  if (messagePending()) {
    /* Tell the policy engine that we failed */
    PolicyEngine::notify(PolicyEngine::Notifications::PDB_EVT_PE_TX_ERR);
    /* Finish failing to send the message */
    while (messagePending()) {
      getMessage(); // Discard
    }
  }

  /* Wait for a message request */
  return PRLTxWaitMessage;
}

/*
 * PRL_Tx_Wait_for_Message_Request state
 */
ProtocolTransmit::protocol_tx_state ProtocolTransmit::protocol_tx_wait_message() {
  /* Wait for an event */
  ProtocolTransmit::Notifications evt = waitForEvent((uint32_t)Notifications::PDB_EVT_PRLTX_RESET | (uint32_t)Notifications::PDB_EVT_PRLTX_DISCARD | (uint32_t)Notifications::PDB_EVT_PRLTX_MSG_TX);

  if ((uint32_t)evt & (uint32_t)Notifications::PDB_EVT_PRLTX_RESET) {
    return PRLTxPHYReset;
  }

  /* If the policy engine is trying to send a message */
  if ((uint32_t)evt & (uint32_t)Notifications::PDB_EVT_PRLTX_MSG_TX) {
    /* Get the message */
    getMessage();

    /* If it's a Soft_Reset, reset the TX layer first */
    if (PD_MSGTYPE_GET(&temp_msg) == PD_MSGTYPE_SOFT_RESET && PD_NUMOBJ_GET(&(temp_msg)) == 0) {
      return PRLTxReset;
      /* Otherwise, just send the message */
    } else {
      return PRLTxConstructMessage;
    }
  }

  /* Silence the compiler warning */
  return PRLTxWaitMessage;
}

ProtocolTransmit::protocol_tx_state ProtocolTransmit::protocol_tx_reset() {
  /* Clear MessageIDCounter */
  _tx_messageidcounter = 0;

  return PRLTxConstructMessage;
}

/*
 * PRL_Tx_Construct_Message state
 */
ProtocolTransmit::protocol_tx_state ProtocolTransmit::protocol_tx_construct_message() {
  /* Set the correct MessageID in the message */
  temp_msg.hdr &= ~PD_HDR_MESSAGEID;
  temp_msg.hdr |= (_tx_messageidcounter % 8) << PD_HDR_MESSAGEID_SHIFT;

  /* PD 3.0 collision avoidance */
  if (PolicyEngine::isPD3_0()) {
    /* If we're starting an AMS, wait for permission to transmit */
    while (fusb_get_typec_current() != fusb_sink_tx_ok) {
      vTaskDelay(TICKS_10MS);
    }
  }
  messageSending = true;
  /* Send the message to the PHY */
  fusb_send_message(&temp_msg);

  return PRLTxWaitResponse;
}

/*
 * PRL_Tx_Wait_for_PHY_Response state
 */
ProtocolTransmit::protocol_tx_state ProtocolTransmit::protocol_tx_wait_response() {
  /* Wait for an event.  There is no need to run CRCReceiveTimer, since the
   * FUSB302B handles that as part of its retry mechanism. */
  ProtocolTransmit::Notifications evt = waitForEvent((uint32_t)Notifications::PDB_EVT_PRLTX_RESET | (uint32_t)Notifications::PDB_EVT_PRLTX_DISCARD | (uint32_t)Notifications::PDB_EVT_PRLTX_I_TXSENT
                                                     | (uint32_t)Notifications::PDB_EVT_PRLTX_I_RETRYFAIL);

  if ((uint32_t)evt & (uint32_t)Notifications::PDB_EVT_PRLTX_RESET) {
    return PRLTxPHYReset;
  }
  if ((uint32_t)evt & (uint32_t)Notifications::PDB_EVT_PRLTX_DISCARD) {
    return PRLTxDiscardMessage;
  }

  /* If the message was sent successfully */
  if ((uint32_t)evt & (uint32_t)Notifications::PDB_EVT_PRLTX_I_TXSENT) {
    return PRLTxMatchMessageID;
  }
  /* If the message failed to be sent */
  if ((uint32_t)evt & (uint32_t)Notifications::PDB_EVT_PRLTX_I_RETRYFAIL) {
    return PRLTxTransmissionError;
  }

  /* Silence the compiler warning */
  return PRLTxDiscardMessage;
}

/*
 * PRL_Tx_Match_MessageID state
 */
ProtocolTransmit::protocol_tx_state ProtocolTransmit::protocol_tx_match_messageid() {
  union pd_msg goodcrc;

  /* Read the GoodCRC */
  fusb_read_message(&goodcrc);

  /* Check that the message is correct */
  if (PD_MSGTYPE_GET(&goodcrc) == PD_MSGTYPE_GOODCRC && PD_NUMOBJ_GET(&goodcrc) == 0 && PD_MESSAGEID_GET(&goodcrc) == _tx_messageidcounter) {
    return PRLTxMessageSent;
  } else {
    return PRLTxTransmissionError;
  }
}

ProtocolTransmit::protocol_tx_state ProtocolTransmit::protocol_tx_transmission_error() {
  /* Increment MessageIDCounter */
  _tx_messageidcounter = (_tx_messageidcounter + 1) % 8;

  /* Tell the policy engine that we failed */
  PolicyEngine::notify(PolicyEngine::Notifications::PDB_EVT_PE_TX_ERR);

  return PRLTxWaitMessage;
}

ProtocolTransmit::protocol_tx_state ProtocolTransmit::protocol_tx_message_sent() {
  messageSending = false;
  /* Increment MessageIDCounter */
  _tx_messageidcounter = (_tx_messageidcounter + 1) % 8;

  /* Tell the policy engine that we succeeded */
  PolicyEngine::notify(PolicyEngine::Notifications::PDB_EVT_PE_TX_DONE);

  return PRLTxWaitMessage;
}

ProtocolTransmit::protocol_tx_state ProtocolTransmit::protocol_tx_discard_message() {
  /* If we were working on sending a message, increment MessageIDCounter */
  if (messageSending) {
    _tx_messageidcounter = (_tx_messageidcounter + 1) % 8;

    return PRLTxPHYReset;
  } else {
    return PRLTxWaitMessage;
  }
}
void ProtocolTransmit::thread(const void *args) {
  (void)args;
  ProtocolTransmit::protocol_tx_state state = PRLTxPHYReset;

  // Init the incoming message queue

  while (true) {
    switch (state) {
    case PRLTxPHYReset:
      state = protocol_tx_phy_reset();
      break;
    case PRLTxWaitMessage:
      state = protocol_tx_wait_message();
      break;
    case PRLTxReset:
      state = protocol_tx_reset();
      break;
    case PRLTxConstructMessage:
      state = protocol_tx_construct_message();
      break;
    case PRLTxWaitResponse:
      state = protocol_tx_wait_response();
      break;
    case PRLTxMatchMessageID:
      state = protocol_tx_match_messageid();
      break;
    case PRLTxTransmissionError:
      state = protocol_tx_transmission_error();
      break;
    case PRLTxMessageSent:
      state = protocol_tx_message_sent();
      break;
    case PRLTxDiscardMessage:
      state = protocol_tx_discard_message();
      break;
    default:
      state = PRLTxPHYReset;
      break;
    }
  }
}

void ProtocolTransmit::notify(ProtocolTransmit::Notifications notification) {
  if (xEventGroupHandle != NULL) {
    xEventGroupSetBits(xEventGroupHandle, (uint32_t)notification);
  }
}

void ProtocolTransmit::init() {
  messagesWaiting = xQueueCreateStatic(PDB_MSG_POOL_SIZE, sizeof(union pd_msg), ucQueueStorageArea, &xStaticQueue);

  osThreadStaticDef(pd_txTask, thread, PDB_PRIO_PRL, 0, TaskStackSize, TaskBuffer, &TaskControlBlock);
  TaskHandle        = osThreadCreate(osThread(pd_txTask), NULL);
  xEventGroupHandle = xEventGroupCreateStatic(&xCreatedEventGroup);
}

void ProtocolTransmit::pushMessage(union pd_msg *msg) {
  if (messagesWaiting) {
    if (xQueueSend(messagesWaiting, msg, 100) == pdTRUE) {
      notify(ProtocolTransmit::Notifications::PDB_EVT_PRLTX_MSG_TX);
    }
  }
}

bool ProtocolTransmit::messagePending() {
  if (messagesWaiting) {
    return uxQueueMessagesWaiting(messagesWaiting) > 0;
  }
  return false;
}

void ProtocolTransmit::getMessage() {
  // Loads the pending message into the buffer
  if (messagesWaiting) {
    xQueueReceive(messagesWaiting, &temp_msg, 1);
  }
}

ProtocolTransmit::Notifications ProtocolTransmit::waitForEvent(uint32_t mask, TickType_t ticksToWait) {
  if (xEventGroupHandle) {
    return (Notifications)xEventGroupWaitBits(xEventGroupHandle, mask, mask, pdFALSE, ticksToWait);
  }
  return (Notifications)0;
}
