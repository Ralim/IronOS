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

#include "int_n.h"
#include "Defines.h"
#include "fusb302b.h"
#include "fusb_user.h"
#include "fusbpd.h"
#include "policy_engine.h"
#include "task.h"
#include <pd.h>
#include <string.h>

volatile osThreadId InterruptHandler::TaskHandle = NULL;
uint32_t            InterruptHandler::TaskBuffer[InterruptHandler::TaskStackSize];
osStaticThreadDef_t InterruptHandler::TaskControlBlock;
union pd_msg        InterruptHandler::tempMessage;

void InterruptHandler::init() {
  TaskHandle = NULL;
  osThreadStaticDef(intTask, Thread, PDB_PRIO_PRL_INT_N, 0, TaskStackSize, TaskBuffer, &TaskControlBlock);
  TaskHandle = osThreadCreate(osThread(intTask), NULL);
}
volatile uint32_t msgCounter  = 0;
volatile uint32_t msgCounter1 = 0;
void              InterruptHandler::readPendingMessage() {
  memset(&tempMessage, 0, sizeof(tempMessage));
  while (fusb_rx_pending()) {
    msgCounter++;
    /* Read the message */
    if (fusb_read_message(&tempMessage) == 0) {
      /* If it's a Soft_Reset, go to the soft reset state */
      if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_SOFT_RESET && PD_NUMOBJ_GET(&tempMessage) == 0) {
        /* TX transitions to its reset state */
        PolicyEngine::notify(PolicyEngine::Notifications::PDB_EVT_PE_RESET);
      } else {
        /* Tell PolicyEngine to discard the message being transmitted */
        PolicyEngine::notify(PolicyEngine::Notifications::PDB_EVT_TX_DISCARD);

        /* Pass the message to the policy engine. */
        PolicyEngine::handleMessage(&tempMessage);
      }
    } else {
      msgCounter1++;
    }
  }
}

void InterruptHandler::Thread(const void *arg) {
  (void)arg;
  union fusb_status status;
  for (;;) {
    // If the irq is low continue, otherwise wait for irq or timeout
    if (!getFUS302IRQLow()) {
      xTaskNotifyWait(0x00, 0x0F, NULL, TICKS_SECOND * 30);
    }
    /* Read the FUSB302B status and interrupt registers */
    if (fusb_get_status(&status)) {

      /* If the I_GCRCSENT flag is set, tell the Protocol RX thread */
      // This means a message was received with a good CRC
      if (status.interruptb & FUSB_INTERRUPTB_I_GCRCSENT) {
        readPendingMessage();
      }

      /* If the I_TXSENT or I_RETRYFAIL flag is set, tell the Protocol TX
       * thread */
      if (status.interrupta & FUSB_INTERRUPTA_I_TXSENT) {
        PolicyEngine::notify(PolicyEngine::Notifications::PDB_EVT_TX_I_TXSENT);
      }
      if (status.interrupta & FUSB_INTERRUPTA_I_RETRYFAIL) {
        PolicyEngine::notify(PolicyEngine::Notifications::PDB_EVT_TX_I_RETRYFAIL);
      }

      /* If the I_OCP_TEMP and OVRTEMP flags are set, tell the Policy
       * Engine thread */
      if ((status.interrupta & FUSB_INTERRUPTA_I_OCP_TEMP) && (status.status1 & FUSB_STATUS1_OVRTEMP)) {
        PolicyEngine::notify(PolicyEngine::Notifications::PDB_EVT_PE_I_OVRTEMP);
      }
    }
  }
}
void InterruptHandler::irqCallback() {
  if (TaskHandle != NULL) {
    BaseType_t taskWoke = pdFALSE;
    xTaskNotifyFromISR(TaskHandle, 0x01, eNotifyAction::eSetBits, &taskWoke);
    portYIELD_FROM_ISR(taskWoke);
  }
}
