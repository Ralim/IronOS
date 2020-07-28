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

#ifndef PDB_POLICY_ENGINE_H
#define PDB_POLICY_ENGINE_H

#include <pd.h>

/*
 * Events for the Policy Engine thread, used internally + sent by user code
 *
 */

#define PDB_EVT_PE_RESET EVENT_MASK(0)
#define PDB_EVT_PE_MSG_RX EVENT_MASK(1)
#define PDB_EVT_PE_TX_DONE EVENT_MASK(2)
#define PDB_EVT_PE_TX_ERR EVENT_MASK(3)
#define PDB_EVT_PE_HARD_SENT EVENT_MASK(4)
#define PDB_EVT_PE_I_OVRTEMP EVENT_MASK(5)
#define PDB_EVT_PE_PPS_REQUEST EVENT_MASK(6)
#define PDB_EVT_PE_MSG_RX_PEND EVENT_MASK(7) /* Never SEND THIS DIRECTLY*/

class PolicyEngine {
public:
	//Sets up internal state and registers the thread
	static void init();
	//Push an incoming message to the Policy Engine
	static void handleMessage(union pd_msg *msg);
	//Send a notification
	static void notify(uint32_t notification);
	//Returns true if headers indicate PD3.0 compliant
	static bool isPD3_0();

	static void PPSTimerCallBack();
	//Has pd negotiation completed
	static bool pdHasNegotiated();
private:
	static bool pdNegotiationComplete;
	static int current_voltage_mv; //The current voltage PD is expecting
	static int _requested_voltage; //The voltage the unit wanted to requests
	static bool _unconstrained_power; // If the source is unconstrained
	//Current message being handled
	static union pd_msg currentMessage;
	/* PD message header template */
	static uint16_t hdr_template;
	/* Whether or not we have an explicit contract */
	static bool _explicit_contract;
	/* The number of hard resets we've sent */
	static int8_t _hard_reset_counter;
	/* The result of the last Type-C Current match comparison */
	static int8_t _old_tcc_match;
	/* The index of the first PPS APDO */
	static uint8_t _pps_index;
	/* The index of the just-requested PPS APDO */
	static uint8_t _last_pps;
	static void pe_task(const void *arg);
	enum policy_engine_state {
		PESinkStartup,
		PESinkDiscovery,
		PESinkWaitCap,
		PESinkEvalCap,
		PESinkSelectCap,
		PESinkTransitionSink,
		PESinkReady,
		PESinkGetSourceCap,
		PESinkGiveSinkCap,
		PESinkHardReset,
		PESinkTransitionDefault,
		PESinkSoftReset,
		PESinkSendSoftReset,
		PESinkSendNotSupported,
		PESinkChunkReceived,
		PESinkNotSupportedReceived,
		PESinkSourceUnresponsive
	};
	static enum policy_engine_state pe_sink_startup();
	static enum policy_engine_state pe_sink_discovery();
	static enum policy_engine_state pe_sink_wait_cap();
	static enum policy_engine_state pe_sink_eval_cap();
	static enum policy_engine_state pe_sink_select_cap();
	static enum policy_engine_state pe_sink_transition_sink();
	static enum policy_engine_state pe_sink_ready();
	static enum policy_engine_state pe_sink_get_source_cap();
	static enum policy_engine_state pe_sink_give_sink_cap();
	static enum policy_engine_state pe_sink_hard_reset();
	static enum policy_engine_state pe_sink_transition_default();
	static enum policy_engine_state pe_sink_soft_reset();
	static enum policy_engine_state pe_sink_send_soft_reset();
	static enum policy_engine_state pe_sink_send_not_supported();
	static enum policy_engine_state pe_sink_chunk_received();
	static enum policy_engine_state pe_sink_not_supported_received();
	static enum policy_engine_state pe_sink_source_unresponsive();
	static EventGroupHandle_t xEventGroupHandle;
	static StaticEventGroup_t xCreatedEventGroup;
	static uint32_t waitForEvent(uint32_t mask, uint32_t ticksToWait =
	portMAX_DELAY);
	//Task resources
	static osThreadId TaskHandle;
	static const size_t TaskStackSize = 2048 / 4;
	static uint32_t TaskBuffer[TaskStackSize];
	static osStaticThreadDef_t TaskControlBlock;
	static union pd_msg tempMessage;
	static union pd_msg _last_dpm_request;
	static policy_engine_state state;
	//queue of up to PDB_MSG_POOL_SIZE messages to send
	static StaticQueue_t xStaticQueue;
	/* The array to use as the queue's storage area.  This must be at least
	 uxQueueLength * uxItemSize bytes. */
	static uint8_t ucQueueStorageArea[PDB_MSG_POOL_SIZE * sizeof(union pd_msg)];
	static QueueHandle_t messagesWaiting;
	static bool messageWaiting();
//Read a pending message into the temp message
	static bool readMessage();
	static void start_pps_timer();
	static void stop_pps_timer();

	// These callbacks are called to implement the logic for the iron to select the desired voltage

	/*
	 * Create a Request message based on the given Source_Capabilities message.  If
	 * capabilities is NULL, the last non-null Source_Capabilities message passes
	 * is used.  If none has been provided, the behavior is undefined.
	 *
	 * Returns true if sufficient power is available, false otherwise.
	 */
	static bool pdbs_dpm_evaluate_capability(const union pd_msg *capabilities,
			union pd_msg *request);

	/*
	 * Create a Sink_Capabilities message for our current capabilities.
	 */
	static void pdbs_dpm_get_sink_capability(union pd_msg *cap);

	/*
	 * Return whether or not GiveBack support is enabled.
	 */
	static bool pdbs_dpm_giveback_enabled();

	/*
	 * Evaluate whether or not the currently offered Type-C Current can fulfill our
	 * power needs.
	 *
	 * Returns true if sufficient power is available, false otherwise.
	 */
	static bool pdbs_dpm_evaluate_typec_current(enum fusb_typec_current tcc);

	/*
	 * Indicate that power negotiations are starting.
	 */
	static void pdbs_dpm_pd_start();

	/*
	 * Transition the sink to default power.
	 */
	static void pdbs_dpm_transition_default();

	/*
	 * Transition to the requested minimum current.
	 */
	static void pdbs_dpm_transition_min();

	/*
	 * Transition to Sink Standby if necessary.
	 */
	static void pdbs_dpm_transition_standby();

	/*
	 * Transition to the requested power level
	 */
	static void pdbs_dpm_transition_requested();

	/*
	 * Transition to the Type-C Current power level
	 */
	static void pdbs_dpm_transition_typec();
};

#endif /* PDB_POLICY_ENGINE_H */
