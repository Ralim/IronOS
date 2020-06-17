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

#include "hard_reset.h"
#include "fusbpd.h"
#include <pd.h>
#include "policy_engine.h"
#include "protocol_rx.h"
#include "protocol_tx.h"
#include "fusb302b.h"

osThreadId ResetHandler::TaskHandle;
uint32_t ResetHandler::TaskBuffer[ResetHandler::TaskStackSize];
osStaticThreadDef_t ResetHandler::TaskControlBlock;

/*
 * PRL_HR_Reset_Layer state
 */
ResetHandler::hardrst_state ResetHandler::hardrst_reset_layer() {
	/* First, wait for the signal to run a hard reset. */
	eventmask_t evt = waitForEvent(
	PDB_EVT_HARDRST_RESET | PDB_EVT_HARDRST_I_HARDRST);

	/* Reset the Protocol RX machine */
	ProtocolReceive::notify( PDB_EVT_PRLRX_RESET);
	taskYIELD();
	/* Reset the Protocol TX machine */
	ProtocolTransmit::notify(PDB_EVT_PRLTX_RESET);
	taskYIELD();
	/* Continue the process based on what event started the reset. */
	if (evt & PDB_EVT_HARDRST_RESET) {
		/* Policy Engine started the reset. */
		return PRLHRRequestHardReset;
	} else {
		/* PHY started the reset */
		return PRLHRIndicateHardReset;
	}
}

ResetHandler::hardrst_state ResetHandler::hardrst_indicate_hard_reset() {
	/* Tell the PE that we're doing a hard reset */
	PolicyEngine::notify( PDB_EVT_PE_RESET);

	return PRLHRWaitPE;
}

ResetHandler::hardrst_state ResetHandler::hardrst_request_hard_reset() {
	/* Tell the PHY to send a hard reset */
	fusb_send_hardrst();

	return PRLHRWaitPHY;
}

ResetHandler::hardrst_state ResetHandler::hardrst_wait_phy() {
	/* Wait for the PHY to tell us that it's done sending the hard reset */
	waitForEvent(PDB_EVT_HARDRST_I_HARDSENT, PD_T_HARD_RESET_COMPLETE);

	/* Move on no matter what made us stop waiting. */
	return PRLHRHardResetRequested;
}

ResetHandler::hardrst_state ResetHandler::hardrst_hard_reset_requested() {
	/* Tell the PE that the hard reset was sent */
	PolicyEngine::notify( PDB_EVT_PE_HARD_SENT);

	return PRLHRWaitPE;
}

ResetHandler::hardrst_state ResetHandler::hardrst_wait_pe() {
	/* Wait for the PE to tell us that it's done */
	waitForEvent(PDB_EVT_HARDRST_DONE);

	return PRLHRComplete;
}

ResetHandler::hardrst_state ResetHandler::hardrst_complete() {
	/* I'm not aware of anything we have to tell the FUSB302B, so just finish
	 * the reset routine. */
	return PRLHRResetLayer;
}

void ResetHandler::init() {
	osThreadStaticDef(Task, Thread, PDB_PRIO_PRL, 0, TaskStackSize, TaskBuffer,
			&TaskControlBlock);
	TaskHandle = osThreadCreate(osThread(Task), NULL);
}

void ResetHandler::notify(uint32_t notification) {
	xTaskNotify(TaskHandle, notification,
			eNotifyAction::eSetBits);
}

void ResetHandler::Thread(const void *arg) {
	(void) arg;
	ResetHandler::hardrst_state state = PRLHRResetLayer;

	while (true) {
		switch (state) {
		case PRLHRResetLayer:
			state = hardrst_reset_layer();
			break;
		case PRLHRIndicateHardReset:
			state = hardrst_indicate_hard_reset();
			break;
		case PRLHRRequestHardReset:
			state = hardrst_request_hard_reset();
			break;
		case PRLHRWaitPHY:
			state = hardrst_wait_phy();
			break;
		case PRLHRHardResetRequested:
			state = hardrst_hard_reset_requested();
			break;
		case PRLHRWaitPE:
			state = hardrst_wait_pe();
			break;
		case PRLHRComplete:
			state = hardrst_complete();
			break;
		default:
			/* This is an error.  It really shouldn't happen.  We might
			 * want to handle it anyway, though. */
			break;
		}
	}
}

uint32_t ResetHandler::waitForEvent(uint32_t mask, uint32_t ticksToWait) {
	uint32_t pulNotificationValue;
	xTaskNotifyWait(0x00, mask, &pulNotificationValue, ticksToWait);
	return pulNotificationValue;
}
