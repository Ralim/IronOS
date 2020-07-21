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
#include "fusbpd.h"
#include <pd.h>
#include "fusb302b.h"
#include "protocol_rx.h"
#include "protocol_tx.h"
#include "hard_reset.h"
#include "policy_engine.h"
#include "protocol_rx.h"
#include "protocol_tx.h"
#include "task.h"
#include "BSP.h"

osThreadId InterruptHandler::TaskHandle;
uint32_t InterruptHandler::TaskBuffer[InterruptHandler::TaskStackSize];
osStaticThreadDef_t InterruptHandler::TaskControlBlock;

void InterruptHandler::init() {
	osThreadStaticDef(Task, Thread, PDB_PRIO_PRL_INT_N, 0, TaskStackSize,
			TaskBuffer, &TaskControlBlock);
	TaskHandle = osThreadCreate(osThread(Task), NULL);
}

void InterruptHandler::Thread(const void *arg) {
	(void) arg;
	union fusb_status status;
	volatile uint32_t events;
	while (true) {
		/* If the INT_N line is low */
		xTaskNotifyWait(0x00, 0x0F, NULL, 100);
		/* Read the FUSB302B status and interrupt registers */
		fusb_get_status(&status);
		/* If the I_GCRCSENT flag is set, tell the Protocol RX thread */
		//This means a message was recieved with a good CRC
		if (status.interruptb & FUSB_INTERRUPTB_I_GCRCSENT) {
			ProtocolReceive::notify(PDB_EVT_PRLRX_I_GCRCSENT);
		}
		if ((status.status1 & FUSB_STATUS1_RX_EMPTY) == 0) {
			ProtocolReceive::notify(PDB_EVT_PRLRX_I_GCRCSENT);
		}

		/* If the I_TXSENT or I_RETRYFAIL flag is set, tell the Protocol TX
		 * thread */
		events = 0;

		if (status.interrupta & FUSB_INTERRUPTA_I_TXSENT) {
			ProtocolTransmit::notify(
					ProtocolTransmit::Notifications::PDB_EVT_PRLTX_I_TXSENT);
		}
		if (status.interrupta & FUSB_INTERRUPTA_I_RETRYFAIL) {
			ProtocolTransmit::notify(
					ProtocolTransmit::Notifications::PDB_EVT_PRLTX_I_RETRYFAIL);
		}

		/* If the I_HARDRST or I_HARDSENT flag is set, tell the Hard Reset
		 * thread */

		events = 0;
		if (status.interrupta & FUSB_INTERRUPTA_I_HARDRST) {
//			events |= PDB_EVT_HARDRST_I_HARDRST;
		} else if (status.interrupta & FUSB_INTERRUPTA_I_HARDSENT) {
			events |= PDB_EVT_HARDRST_I_HARDSENT;
		}
		if (events) {
			ResetHandler::notify(events);
		}
		/* If the I_OCP_TEMP and OVRTEMP flags are set, tell the Policy
		 * Engine thread */
		if (status.interrupta & FUSB_INTERRUPTA_I_OCP_TEMP
				&& status.status1 & FUSB_STATUS1_OVRTEMP) {
			PolicyEngine::notify(PDB_EVT_PE_I_OVRTEMP);
		}
	}
}
volatile uint8_t irqs = 0;
void InterruptHandler::irqCallback() {
	irqs++;
	BaseType_t taskWoke = pdFALSE;
	xTaskNotifyFromISR(TaskHandle, 0x0F, eNotifyAction::eSetBits, &taskWoke);
	portYIELD_FROM_ISR(taskWoke);
}
