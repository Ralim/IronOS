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
#include "BSP.h"

osThreadId InterruptHandler::TaskHandle;
uint32_t InterruptHandler::TaskBuffer[InterruptHandler::TaskStackSize];
osStaticThreadDef_t InterruptHandler::TaskControlBlock;

void InterruptHandler::init() {
	osThreadStaticDef(Task, Thread, PDB_PRIO_PE, 0, TaskStackSize, TaskBuffer,
			&TaskControlBlock);
	TaskHandle = osThreadCreate(osThread(Task), NULL);
}

void InterruptHandler::Thread(const void *arg) {
	(void) arg;
	union fusb_status status;
	eventmask_t events;
	//TODO use IRQ task notification to unblock this thread to stop it spinning
	while (true) {
		/* If the INT_N line is low */
		if (pd_irq_read() == 0) {
			/* Read the FUSB302B status and interrupt registers */
			fusb_get_status(&status);
			//Check for rx alerts
			{
				/* If the I_GCRCSENT flag is set, tell the Protocol RX thread */
				if (status.interruptb & FUSB_INTERRUPTB_I_GCRCSENT) {
					ProtocolReceive::notify(PDB_EVT_PRLRX_I_GCRCSENT);
				}
			}
			/* If the I_TXSENT or I_RETRYFAIL flag is set, tell the Protocol TX
			 * thread */
			{
				events = 0;
				if (status.interrupta & FUSB_INTERRUPTA_I_RETRYFAIL) {
					events |= PDB_EVT_PRLTX_I_RETRYFAIL;
				}
				if (status.interrupta & FUSB_INTERRUPTA_I_TXSENT) {
					events |= PDB_EVT_PRLTX_I_TXSENT;
				}
				if (events) {
					ProtocolTransmit::notify(events);
				}
			}
			/* If the I_HARDRST or I_HARDSENT flag is set, tell the Hard Reset
			 * thread */
			{
				events = 0;
				if (status.interrupta & FUSB_INTERRUPTA_I_HARDRST) {
					events |= PDB_EVT_HARDRST_I_HARDRST;
				}
				if (status.interrupta & FUSB_INTERRUPTA_I_HARDSENT) {
					events |= PDB_EVT_HARDRST_I_HARDSENT;
				}
				if (events) {
					ResetHandler::notify(events);
				}
			}
			{
				/* If the I_OCP_TEMP and OVRTEMP flags are set, tell the Policy
				 * Engine thread */
				if (status.interrupta & FUSB_INTERRUPTA_I_OCP_TEMP
						&& status.status1 & FUSB_STATUS1_OVRTEMP) {
					PolicyEngine::notify(PDB_EVT_PE_I_OVRTEMP);
				}
			}

		} else {
			osDelay(1);
		}
		osDelay(1);
	}
}
