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

#include "policy_engine.h"
#include <stdbool.h>

#include <pd.h>
#include "protocol_tx.h"
#include "hard_reset.h"
#include "fusb302b.h"
bool PolicyEngine::pdNegotiationComplete;
bool PolicyEngine::pdHasEnteredLowPower;
int PolicyEngine::current_voltage_mv;
int PolicyEngine::_requested_voltage;
bool PolicyEngine::_unconstrained_power;
union pd_msg PolicyEngine::currentMessage;
uint16_t PolicyEngine::hdr_template;
bool PolicyEngine::_explicit_contract;
bool PolicyEngine::_min_power;
int8_t PolicyEngine::_hard_reset_counter;
int8_t PolicyEngine::_old_tcc_match;
uint8_t PolicyEngine::_pps_index;
uint8_t PolicyEngine::_last_pps;
osThreadId PolicyEngine::TaskHandle;
uint32_t PolicyEngine::TaskBuffer[PolicyEngine::TaskStackSize];
osStaticThreadDef_t PolicyEngine::TaskControlBlock;
union pd_msg PolicyEngine::tempMessage;
union pd_msg PolicyEngine::_last_dpm_request;
PolicyEngine::policy_engine_state PolicyEngine::state = PESinkStartup;
StaticQueue_t PolicyEngine::xStaticQueue;
uint8_t PolicyEngine::ucQueueStorageArea[PDB_MSG_POOL_SIZE
		* sizeof(union pd_msg)];
QueueHandle_t PolicyEngine::messagesWaiting;
void PolicyEngine::init() {
	messagesWaiting = xQueueCreateStatic(PDB_MSG_POOL_SIZE,
			sizeof(union pd_msg), ucQueueStorageArea, &xStaticQueue);
	//Create static thread at PDB_PRIO_PE priority
	osThreadStaticDef(Task, pe_task, PDB_PRIO_PE, 0, TaskStackSize, TaskBuffer,
			&TaskControlBlock);
	TaskHandle = osThreadCreate(osThread(Task), NULL);
}

void PolicyEngine::notify(uint32_t notification) {
	xTaskNotify(TaskHandle, notification, eNotifyAction::eSetBits);
}

void PolicyEngine::pe_task(const void *arg) {
	(void) arg;
//Internal thread loop
	hdr_template = PD_DATAROLE_UFP | PD_POWERROLE_SINK;
	/* Initialize the old_tcc_match */
	_old_tcc_match = -1;
	/* Initialize the pps_index */
	_pps_index = 8;
	/* Initialize the last_pps */
	_last_pps = 8;

	for (;;) {
		//Loop based on state
		switch (state) {
		case PESinkStartup:
			state = pe_sink_startup();
			break;
		case PESinkDiscovery:
			state = pe_sink_discovery();
			break;
		case PESinkWaitCap:
			state = pe_sink_wait_cap();
			break;
		case PESinkEvalCap:
			state = pe_sink_eval_cap();
			break;
		case PESinkSelectCap:
			state = pe_sink_select_cap();
			break;
		case PESinkTransitionSink:
			state = pe_sink_transition_sink();
			break;
		case PESinkReady:
			state = pe_sink_ready();
			break;
		case PESinkGetSourceCap:
			state = pe_sink_get_source_cap();
			break;
		case PESinkGiveSinkCap:
			state = pe_sink_give_sink_cap();
			break;
		case PESinkHardReset:
			state = pe_sink_hard_reset();
			break;
		case PESinkTransitionDefault:
			state = pe_sink_transition_default();
			break;
		case PESinkSoftReset:
			state = pe_sink_soft_reset();
			break;
		case PESinkSendSoftReset:
			state = pe_sink_send_soft_reset();
			break;
		case PESinkSendNotSupported:
			state = pe_sink_send_not_supported();
			break;
		case PESinkChunkReceived:
			state = pe_sink_chunk_received();
			break;
		case PESinkSourceUnresponsive:
			state = pe_sink_source_unresponsive();
			break;
		case PESinkNotSupportedReceived:
			state = pe_sink_not_supported_received();
			break;
		default:
			state = PESinkStartup;
			break;
		}
	}
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_startup() {
	/* We don't have an explicit contract currently */
	_explicit_contract = false;

//If desired could send an alert that PD is starting

	/* No need to reset the protocol layer here.  There are two ways into this
	 * state: startup and exiting hard reset.  On startup, the protocol layer
	 * is reset by the startup procedure.  When exiting hard reset, the
	 * protocol layer is reset by the hard reset state machine.  Since it's
	 * already done somewhere else, there's no need to do it again here. */

	return PESinkDiscovery;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_discovery() {
	/* Wait for VBUS.  Since it's our only power source, we already know that
	 * we have it, so just move on. */

	return PESinkWaitCap;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_wait_cap() {
	/* Fetch a message from the protocol layer */
	eventmask_t evt = waitForEvent(
	PDB_EVT_PE_MSG_RX | PDB_EVT_PE_I_OVRTEMP | PDB_EVT_PE_RESET,
	PD_T_TYPEC_SINK_WAIT_CAP);
	/* If we timed out waiting for Source_Capabilities, send a hard reset */
	if (evt == 0) {
		return PESinkHardReset;
	}
	/* If we got reset signaling, transition to default */
	if (evt & PDB_EVT_PE_RESET) {
		return PESinkTransitionDefault;
	}
	/* If we're too hot, we shouldn't negotiate power yet */
	if (evt & PDB_EVT_PE_I_OVRTEMP) {
		return PESinkWaitCap;
	}

	/* If we got a message */
	if (evt & PDB_EVT_PE_MSG_RX) {
		/* Get the message */
		readMessage();
		/* If we got a Source_Capabilities message, read it. */
		if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_SOURCE_CAPABILITIES
				&& PD_NUMOBJ_GET(&tempMessage) > 0) {
			/* First, determine what PD revision we're using */
			if ((hdr_template & PD_HDR_SPECREV) == PD_SPECREV_1_0) {
				/* If the other end is using at least version 3.0, we'll
				 * use version 3.0. */
//				if ((tempMessage.hdr & PD_HDR_SPECREV) >= PD_SPECREV_3_0) {
//					hdr_template |= PD_SPECREV_3_0;
//					/* Otherwise, use 2.0.  Don't worry about the 1.0 case
//					 * because we don't have hardware for PD 1.0 signaling. */
//				} else {
				hdr_template |= PD_SPECREV_2_0;
//				}
			}
			return PESinkEvalCap;
			/* If the message was a Soft_Reset, do the soft reset procedure */
		} else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_SOFT_RESET
				&& PD_NUMOBJ_GET(&tempMessage) == 0) {

			return PESinkSoftReset;
			/* If we got an unexpected message, reset */
		} else {
			/* Free the received message */
			return PESinkHardReset;

		}
	}

	/* If we failed to get a message, send a hard reset */
	return PESinkHardReset;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_eval_cap() {
	/* If we have a Source_Capabilities message, remember the index of the
	 * first PPS APDO so we can check if the request is for a PPS APDO in
	 * PE_SNK_Select_Cap. */
	/* Start by assuming we won't find a PPS APDO (set the index greater
	 * than the maximum possible) */
	_pps_index = 8;
	/* Search for the first PPS APDO */
	for (int8_t i = 0; i < PD_NUMOBJ_GET(&tempMessage); i++) {
		if ((tempMessage.obj[i] & PD_PDO_TYPE) == PD_PDO_TYPE_AUGMENTED
				&& (tempMessage.obj[i] & PD_APDO_TYPE) == PD_APDO_TYPE_PPS) {
			_pps_index = i + 1;
			break;
		}
	}
	/* New capabilities also means we can't be making a request from the
	 * same PPS APDO */
	_last_pps = 8;
	/* Get a message object for the request if we don't have one already */

	/* Remember the last PDO we requested if it was a PPS APDO */
	if (PD_RDO_OBJPOS_GET(&_last_dpm_request) >= _pps_index) {
		_last_pps = PD_RDO_OBJPOS_GET(&_last_dpm_request);
		/* Otherwise, forget any PPS APDO we had requested */
	} else {
		_last_pps = 8;
	}

	/* Ask the DPM what to request */
	pdbs_dpm_evaluate_capability(&tempMessage, &_last_dpm_request);

	return PESinkSelectCap;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_select_cap() {
	waitForEvent(
	PDB_EVT_PE_TX_DONE | PDB_EVT_PE_TX_ERR | PDB_EVT_PE_RESET, 0);
	/* Transmit the request */
	ProtocolTransmit::pushMessage(&_last_dpm_request);
//Send indication that there is a message pending
	ProtocolTransmit::notify(
			ProtocolTransmit::Notifications::PDB_EVT_PRLTX_MSG_TX);
	eventmask_t evt = waitForEvent(
	PDB_EVT_PE_TX_DONE | PDB_EVT_PE_TX_ERR | PDB_EVT_PE_RESET);
	/* Don't free the request; we might need it again */
	/* If we got reset signaling, transition to default */
	if (evt & PDB_EVT_PE_RESET) {
		return PESinkTransitionDefault;
	}
	/* If the message transmission failed, send a hard reset */
	if ((evt & PDB_EVT_PE_TX_DONE) == 0) {
		return PESinkHardReset;
	}

	/* If we're using PD 3.0 */
	if ((hdr_template & PD_HDR_SPECREV) == PD_SPECREV_3_0) {
		/* If the request was for a PPS APDO, start SinkPPSPeriodicTimer */
		if (PD_RDO_OBJPOS_GET(&_last_dpm_request) >= _pps_index) {
			start_pps_timer();
			/* Otherwise, stop SinkPPSPeriodicTimer */
		} else {
			stop_pps_timer();
		}
	}
	/* This will use a virtual timer to send an event flag to this thread after
	 * PD_T_PPS_REQUEST */

	/* Wait for a response */
	evt = waitForEvent(PDB_EVT_PE_MSG_RX | PDB_EVT_PE_RESET,
	PD_T_SENDER_RESPONSE);
	/* If we got reset signaling, transition to default */
	if (evt & PDB_EVT_PE_RESET) {
		return PESinkTransitionDefault;
	}
	/* If we didn't get a response before the timeout, send a hard reset */
	if (evt == 0) {
		return PESinkHardReset;
	}

	/* Get the response message */
	if (messageWaiting()) {
		readMessage();
		/* If the source accepted our request, wait for the new power */
		if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_ACCEPT
				&& PD_NUMOBJ_GET(&tempMessage) == 0) {
			/* Transition to Sink Standby if necessary */
			if (PD_RDO_OBJPOS_GET(&_last_dpm_request) != _last_pps) {
				pdbs_dpm_transition_standby();
			}

			_min_power = false;
			return PESinkTransitionSink;
			/* If the message was a Soft_Reset, do the soft reset procedure */
		} else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_SOFT_RESET
				&& PD_NUMOBJ_GET(&tempMessage) == 0) {
			return PESinkSoftReset;
			/* If the message was Wait or Reject */
		} else if ((PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_REJECT
				|| PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_WAIT)
				&& PD_NUMOBJ_GET(&tempMessage) == 0) {
			/* If we don't have an explicit contract, wait for capabilities */
			if (!_explicit_contract) {
				return PESinkWaitCap;
				/* If we do have an explicit contract, go to the ready state */
			} else {
				/* If we got here from a Wait message, we Should run
				 * SinkRequestTimer in the Ready state. */
				_min_power = (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_WAIT);

				return PESinkReady;
			}
		} else {
			return PESinkSendSoftReset;
		}
	}
	return PESinkHardReset;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_transition_sink() {
	/* Wait for the PS_RDY message */
	eventmask_t evt = waitForEvent(PDB_EVT_PE_MSG_RX | PDB_EVT_PE_RESET,
	PD_T_PS_TRANSITION);
	/* If we got reset signaling, transition to default */
	if (evt & PDB_EVT_PE_RESET) {
		return PESinkTransitionDefault;
	}
	/* If no message was received, send a hard reset */
	if (evt == 0) {
		return PESinkHardReset;
	}

	/* If we received a message, read it */
	if (messageWaiting()) {
		readMessage();
		/* If we got a PS_RDY, handle it */
		if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_PS_RDY
				&& PD_NUMOBJ_GET(&tempMessage) == 0) {
			/* We just finished negotiating an explicit contract */
			_explicit_contract = true;

			/* Set the output appropriately */
			if (!_min_power) {
				pdbs_dpm_transition_requested();
			}

			return PESinkReady;
			/* If there was a protocol error, send a hard reset */
		} else {
			/* Turn off the power output before this hard reset to make sure we
			 * don't supply an incorrect voltage to the device we're powering.
			 */
			pdbs_dpm_transition_default();

			return PESinkHardReset;
		}
	}

	return PESinkHardReset;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_ready() {
	eventmask_t evt;

	/* Wait for an event */
	if (_min_power) {
		evt = waitForEvent(
				PDB_EVT_PE_MSG_RX | PDB_EVT_PE_RESET | PDB_EVT_PE_I_OVRTEMP
						| PDB_EVT_PE_GET_SOURCE_CAP | PDB_EVT_PE_NEW_POWER
						| PDB_EVT_PE_PPS_REQUEST,
				PD_T_SINK_REQUEST);
	} else {
		evt = waitForEvent(
				PDB_EVT_PE_MSG_RX | PDB_EVT_PE_RESET | PDB_EVT_PE_I_OVRTEMP
						| PDB_EVT_PE_GET_SOURCE_CAP | PDB_EVT_PE_NEW_POWER
						| PDB_EVT_PE_PPS_REQUEST);
	}

	/* If we got reset signaling, transition to default */
	if (evt & PDB_EVT_PE_RESET) {
		return PESinkTransitionDefault;
	}

	/* If we overheated, send a hard reset */
	if (evt & PDB_EVT_PE_I_OVRTEMP) {
		return PESinkHardReset;
	}

	/* If the DPM wants us to, send a Get_Source_Cap message */
	if (evt & PDB_EVT_PE_GET_SOURCE_CAP) {
		/* Tell the protocol layer we're starting an AMS */
		ProtocolTransmit::notify(
				ProtocolTransmit::Notifications::PDB_EVT_PRLTX_START_AMS);
		return PESinkGetSourceCap;
	}

	/* If the DPM wants new power, let it figure out what power it wants
	 * exactly.  This isn't exactly the transition from the spec (that would be
	 * SelectCap, not EvalCap), but this works better with the particular
	 * design of this firmware. */
	if (evt & PDB_EVT_PE_NEW_POWER) {
		/* Tell the protocol layer we're starting an AMS */
		ProtocolTransmit::notify(
				ProtocolTransmit::Notifications::PDB_EVT_PRLTX_START_AMS);
		return PESinkEvalCap;
	}

	/* If SinkPPSPeriodicTimer ran out, send a new request */
	if (evt & PDB_EVT_PE_PPS_REQUEST) {
		/* Tell the protocol layer we're starting an AMS */
		ProtocolTransmit::notify(
				ProtocolTransmit::Notifications::PDB_EVT_PRLTX_START_AMS);
		return PESinkSelectCap;
	}

	/* If no event was received, the timer ran out. */
	if (evt == 0) {
		/* Repeat our Request message */
		return PESinkSelectCap;
	}

	/* If we received a message */
	if (evt & PDB_EVT_PE_MSG_RX) {
		if (messageWaiting()) {
			readMessage();
			/* Ignore vendor-defined messages */
			if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_VENDOR_DEFINED
					&& PD_NUMOBJ_GET(&tempMessage) > 0) {

				return PESinkReady;
				/* Ignore Ping messages */
			} else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_PING
					&& PD_NUMOBJ_GET(&tempMessage) == 0) {

				return PESinkReady;
				/* DR_Swap messages are not supported */
			} else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_DR_SWAP
					&& PD_NUMOBJ_GET(&tempMessage) == 0) {

				return PESinkSendNotSupported;
				/* Get_Source_Cap messages are not supported */
			} else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_GET_SOURCE_CAP
					&& PD_NUMOBJ_GET(&tempMessage) == 0) {

				return PESinkSendNotSupported;
				/* PR_Swap messages are not supported */
			} else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_PR_SWAP
					&& PD_NUMOBJ_GET(&tempMessage) == 0) {

				return PESinkSendNotSupported;
				/* VCONN_Swap messages are not supported */
			} else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_VCONN_SWAP
					&& PD_NUMOBJ_GET(&tempMessage) == 0) {

				return PESinkSendNotSupported;
				/* Request messages are not supported */
			} else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_REQUEST
					&& PD_NUMOBJ_GET(&tempMessage) > 0) {

				return PESinkSendNotSupported;
				/* Sink_Capabilities messages are not supported */
			} else if (PD_MSGTYPE_GET(&tempMessage)
					== PD_MSGTYPE_SINK_CAPABILITIES
					&& PD_NUMOBJ_GET(&tempMessage) > 0) {

				return PESinkSendNotSupported;
				/* Handle GotoMin messages */
			} else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_GOTOMIN
					&& PD_NUMOBJ_GET(&tempMessage) == 0) {
				if (pdbs_dpm_giveback_enabled()) {
					/* Transition to the minimum current level */
					pdbs_dpm_transition_min();
					_min_power = true;
					return PESinkTransitionSink;
				} else {
					/* GiveBack is not supported */

					return PESinkSendNotSupported;
				}
				/* Evaluate new Source_Capabilities */
			} else if (PD_MSGTYPE_GET(&tempMessage)
					== PD_MSGTYPE_SOURCE_CAPABILITIES
					&& PD_NUMOBJ_GET(&tempMessage) > 0) {
				/* Don't free the message: we need to keep the
				 * Source_Capabilities message so we can evaluate it. */
				return PESinkEvalCap;
				/* Give sink capabilities when asked */
			} else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_GET_SINK_CAP
					&& PD_NUMOBJ_GET(&tempMessage) == 0) {

				return PESinkGiveSinkCap;
				/* If the message was a Soft_Reset, do the soft reset procedure */
			} else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_SOFT_RESET
					&& PD_NUMOBJ_GET(&tempMessage) == 0) {

				return PESinkSoftReset;
				/* PD 3.0 messges */
			} else if ((hdr_template & PD_HDR_SPECREV) == PD_SPECREV_3_0) {
				/* If the message is a multi-chunk extended message, let it
				 * time out. */
				if ((tempMessage.hdr & PD_HDR_EXT)
						&& (PD_DATA_SIZE_GET(&tempMessage)
								> PD_MAX_EXT_MSG_LEGACY_LEN)) {

					return PESinkChunkReceived;
					/* Tell the DPM a message we sent got a response of
					 * Not_Supported. */
				} else if (PD_MSGTYPE_GET(&tempMessage)
						== PD_MSGTYPE_NOT_SUPPORTED
						&& PD_NUMOBJ_GET(&tempMessage) == 0) {

					return PESinkNotSupportedReceived;
					/* If we got an unknown message, send a soft reset */
				} else {

					return PESinkSendSoftReset;
				}
				/* If we got an unknown message, send a soft reset ??? */
			} else {

				return PESinkSendSoftReset;
			}
		}
	}

	return PESinkReady;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_get_source_cap() {
	/* Get a message object */
	union pd_msg *get_source_cap = &tempMessage;
	/* Make a Get_Source_Cap message */
	get_source_cap->hdr = hdr_template | PD_MSGTYPE_GET_SOURCE_CAP
			| PD_NUMOBJ(0);
	/* Transmit the Get_Source_Cap */
	ProtocolTransmit::pushMessage(get_source_cap);
	ProtocolTransmit::notify(
			ProtocolTransmit::Notifications::PDB_EVT_PRLTX_MSG_TX);
	eventmask_t evt = waitForEvent(
	PDB_EVT_PE_TX_DONE | PDB_EVT_PE_TX_ERR | PDB_EVT_PE_RESET);
	/* Free the sent message */
	/* If we got reset signaling, transition to default */
	if (evt & PDB_EVT_PE_RESET) {
		return PESinkTransitionDefault;
	}
	/* If the message transmission failed, send a hard reset */
	if ((evt & PDB_EVT_PE_TX_DONE) == 0) {
		return PESinkHardReset;
	}

	return PESinkReady;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_give_sink_cap() {
	/* Get a message object */
	union pd_msg *snk_cap = &tempMessage;
	/* Get our capabilities from the DPM */
	pdbs_dpm_get_sink_capability(snk_cap);

	/* Transmit our capabilities */
	ProtocolTransmit::pushMessage(snk_cap);
	ProtocolTransmit::notify(
			ProtocolTransmit::Notifications::PDB_EVT_PRLTX_MSG_TX);
	eventmask_t evt = waitForEvent(
	PDB_EVT_PE_TX_DONE | PDB_EVT_PE_TX_ERR | PDB_EVT_PE_RESET);

	/* Free the Sink_Capabilities message */

	/* If we got reset signaling, transition to default */
	if (evt & PDB_EVT_PE_RESET) {
		return PESinkTransitionDefault;
	}
	/* If the message transmission failed, send a hard reset */
	if ((evt & PDB_EVT_PE_TX_DONE) == 0) {
		return PESinkHardReset;
	}

	return PESinkReady;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_hard_reset() {
	/* If we've already sent the maximum number of hard resets, assume the
	 * source is unresponsive. */
	if (_hard_reset_counter > PD_N_HARD_RESET_COUNT) {
		return PESinkSourceUnresponsive;
	}

	/* Generate a hard reset signal */
	ResetHandler::notify(PDB_EVT_HARDRST_RESET);
	waitForEvent(PDB_EVT_PE_HARD_SENT);

	/* Increment HardResetCounter */
	_hard_reset_counter++;

	return PESinkTransitionDefault;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_transition_default() {
	_explicit_contract = false;

	/* Tell the DPM to transition to default power */
	pdbs_dpm_transition_default();

	/* There is no local hardware to reset. */
	/* Since we never change our data role from UFP, there is no reason to set
	 * it here. */

	/* Tell the protocol layer we're done with the reset */
	ResetHandler::notify( PDB_EVT_HARDRST_DONE);

	return PESinkStartup;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_soft_reset() {
	/* No need to explicitly reset the protocol layer here.  It resets itself
	 * when a Soft_Reset message is received. */

	/* Get a message object */
	union pd_msg accept;
	/* Make an Accept message */
	accept.hdr = hdr_template | PD_MSGTYPE_ACCEPT | PD_NUMOBJ(0);
	/* Transmit the Accept */
	ProtocolTransmit::pushMessage(&accept);
	ProtocolTransmit::notify(
			ProtocolTransmit::Notifications::PDB_EVT_PRLTX_MSG_TX);
	eventmask_t evt = waitForEvent(
	PDB_EVT_PE_TX_DONE | PDB_EVT_PE_TX_ERR | PDB_EVT_PE_RESET);
	/* Free the sent message */

	/* If we got reset signaling, transition to default */
	if (evt & PDB_EVT_PE_RESET) {
		return PESinkTransitionDefault;
	}
	/* If the message transmission failed, send a hard reset */
	if ((evt & PDB_EVT_PE_TX_DONE) == 0) {
		return PESinkHardReset;
	}

	return PESinkWaitCap;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_send_soft_reset() {
	/* No need to explicitly reset the protocol layer here.  It resets itself
	 * just before a Soft_Reset message is transmitted. */

	/* Get a message object */
	union pd_msg *softrst = &tempMessage;
	/* Make a Soft_Reset message */
	softrst->hdr = hdr_template | PD_MSGTYPE_SOFT_RESET | PD_NUMOBJ(0);
	/* Transmit the soft reset */
	ProtocolTransmit::pushMessage(softrst);
	ProtocolTransmit::notify(
			ProtocolTransmit::Notifications::PDB_EVT_PRLTX_MSG_TX);
	eventmask_t evt = waitForEvent(
	PDB_EVT_PE_TX_DONE | PDB_EVT_PE_TX_ERR | PDB_EVT_PE_RESET);
	/* If we got reset signaling, transition to default */
	if (evt & PDB_EVT_PE_RESET) {
		return PESinkTransitionDefault;
	}
	/* If the message transmission failed, send a hard reset */
	if ((evt & PDB_EVT_PE_TX_DONE) == 0) {
		return PESinkHardReset;
	}

	/* Wait for a response */
	evt = waitForEvent(PDB_EVT_PE_MSG_RX | PDB_EVT_PE_RESET,
	PD_T_SENDER_RESPONSE);
	/* If we got reset signaling, transition to default */
	if (evt & PDB_EVT_PE_RESET) {
		return PESinkTransitionDefault;
	}
	/* If we didn't get a response before the timeout, send a hard reset */
	if (evt == 0) {
		return PESinkHardReset;
	}

	/* Get the response message */
	if (messageWaiting()) {
		readMessage();
		/* If the source accepted our soft reset, wait for capabilities. */
		if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_ACCEPT
				&& PD_NUMOBJ_GET(&tempMessage) == 0) {

			return PESinkWaitCap;
			/* If the message was a Soft_Reset, do the soft reset procedure */
		} else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_SOFT_RESET
				&& PD_NUMOBJ_GET(&tempMessage) == 0) {

			return PESinkSoftReset;
			/* Otherwise, send a hard reset */
		} else {

			return PESinkHardReset;
		}
	}
	return PESinkHardReset;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_send_not_supported() {
	/* Get a message object */
	union pd_msg *not_supported = &tempMessage;

	if ((hdr_template & PD_HDR_SPECREV) == PD_SPECREV_2_0) {
		/* Make a Reject message */
		not_supported->hdr = hdr_template | PD_MSGTYPE_REJECT | PD_NUMOBJ(0);
	} else if ((hdr_template & PD_HDR_SPECREV) == PD_SPECREV_3_0) {
		/* Make a Not_Supported message */
		not_supported->hdr = hdr_template | PD_MSGTYPE_NOT_SUPPORTED
				| PD_NUMOBJ(0);
	}

	/* Transmit the message */
	ProtocolTransmit::pushMessage(not_supported);
	ProtocolTransmit::notify(
			ProtocolTransmit::Notifications::PDB_EVT_PRLTX_MSG_TX);
	eventmask_t evt = waitForEvent(
	PDB_EVT_PE_TX_DONE | PDB_EVT_PE_TX_ERR | PDB_EVT_PE_RESET);

	/* If we got reset signaling, transition to default */
	if (evt & PDB_EVT_PE_RESET) {
		return PESinkTransitionDefault;
	}
	/* If the message transmission failed, send a soft reset */
	if ((evt & PDB_EVT_PE_TX_DONE) == 0) {
		return PESinkSendSoftReset;
	}

	return PESinkReady;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_chunk_received() {

	/* Wait for tChunkingNotSupported */
	eventmask_t evt = waitForEvent(PDB_EVT_PE_RESET,
	PD_T_CHUNKING_NOT_SUPPORTED);
	/* If we got reset signaling, transition to default */
	if (evt & PDB_EVT_PE_RESET) {
		return PESinkTransitionDefault;
	}

	return PESinkSendNotSupported;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_not_supported_received() {
	/* Inform the Device Policy Manager that we received a Not_Supported
	 * message. */

	return PESinkReady;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_source_unresponsive() {
	/* If the DPM can evaluate the Type-C Current advertisement */
	//TS80P doesnt
//	if (cfg->dpm.evaluate_typec_current != NULL) {
//		/* Make the DPM evaluate the Type-C Current advertisement */
//		int tcc_match = cfg->dpm.evaluate_typec_current(cfg,
//				fusb_get_typec_current(&cfg->fusb));
//
//		/* If the last two readings are the same, set the output */
//		if (_old_tcc_match == tcc_match) {
//			cfg->dpm.transition_typec(cfg);
//		}
//
//		/* Remember whether or not the last measurement succeeded */
//		_old_tcc_match = tcc_match;
//	}
	/* Wait tPDDebounce between measurements */
	osDelay(PD_T_PD_DEBOUNCE);

	return PESinkSourceUnresponsive;
}

void PolicyEngine::PPSTimerCallBack() {
	notify(PDB_EVT_PE_PPS_REQUEST);
}

bool PolicyEngine::pdHasNegotiated() {
	return pdNegotiationComplete;
}

bool PolicyEngine::heatingAllowed() {
	if (pdHasNegotiated())
		return !pdHasEnteredLowPower;
	//Not pd -- pass through
	return true;
}

uint32_t PolicyEngine::waitForEvent(uint32_t mask, uint32_t ticksToWait) {
	uint32_t pulNotificationValue;
	xTaskNotifyWait(0x00, mask, &pulNotificationValue, ticksToWait);
	return pulNotificationValue & mask;
}

bool PolicyEngine::isPD3_0() {
	return (hdr_template & PD_HDR_SPECREV) == PD_SPECREV_3_0;
}

void PolicyEngine::start_pps_timer() {
}

void PolicyEngine::stop_pps_timer() {
}
