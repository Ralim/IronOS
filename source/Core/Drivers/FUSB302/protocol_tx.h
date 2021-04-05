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

#ifndef PDB_PROTOCOL_TX_H
#define PDB_PROTOCOL_TX_H

#include "policy_engine.h"

#include <pd.h>
#include <stdint.h>

/* Events for the Protocol TX thread */

class ProtocolTransmit {
public:
  static void init();
  // Push a message to the queue to be sent out the pd comms bus
  static void pushMessage(union pd_msg *msg);

  enum class Notifications {

    PDB_EVT_PRLTX_RESET       = EVENT_MASK(0), //
    PDB_EVT_PRLTX_I_TXSENT    = EVENT_MASK(1), //
    PDB_EVT_PRLTX_I_RETRYFAIL = EVENT_MASK(2), //
    PDB_EVT_PRLTX_DISCARD     = EVENT_MASK(3), //
    PDB_EVT_PRLTX_MSG_TX      = EVENT_MASK(4), //
  };
  static void notify(Notifications notification);

private:
  static void                thread(const void *args);
  static EventGroupHandle_t  xEventGroupHandle;
  static StaticEventGroup_t  xCreatedEventGroup;
  static osThreadId          TaskHandle;
  static const size_t        TaskStackSize = 1024 / 4;
  static uint32_t            TaskBuffer[TaskStackSize];
  static osStaticThreadDef_t TaskControlBlock;
  static bool                messageSending;
  /*
   * Protocol TX machine states
   *
   * Because the PHY can automatically send retries, the Check_RetryCounter state
   * has been removed, transitions relating to it are modified appropriately, and
   * we don't even keep a RetryCounter.
   */
  enum protocol_tx_state { PRLTxPHYReset, PRLTxWaitMessage, PRLTxReset, PRLTxConstructMessage, PRLTxWaitResponse, PRLTxMatchMessageID, PRLTxTransmissionError, PRLTxMessageSent, PRLTxDiscardMessage };
  // Internal states
  static protocol_tx_state protocol_tx_discard_message();
  static protocol_tx_state protocol_tx_message_sent();
  static protocol_tx_state protocol_tx_transmission_error();
  static protocol_tx_state protocol_tx_match_messageid();
  static protocol_tx_state protocol_tx_wait_response();
  static protocol_tx_state protocol_tx_construct_message();
  static protocol_tx_state protocol_tx_reset();
  static protocol_tx_state protocol_tx_wait_message();
  static protocol_tx_state protocol_tx_phy_reset();
  // queue of up to PDB_MSG_POOL_SIZE messages to send
  static StaticQueue_t xStaticQueue;
  /* The array to use as the queue's storage area.  This must be at least
   uxQueueLength * uxItemSize bytes. */
  static uint8_t       ucQueueStorageArea[PDB_MSG_POOL_SIZE * sizeof(union pd_msg)];
  static QueueHandle_t messagesWaiting;
  static uint8_t       _tx_messageidcounter;
  static bool          messagePending();
  // Reads a message off the queue into the temp message
  static void          getMessage();
  static union pd_msg  temp_msg;
  static Notifications waitForEvent(uint32_t mask, TickType_t ticksToWait = portMAX_DELAY);
};

#endif /* PDB_PROTOCOL_TX_H */
