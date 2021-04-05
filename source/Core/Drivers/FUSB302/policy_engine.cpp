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
#include "Defines.h"
#include "fusb302b.h"
#include "int_n.h"
#include "protocol_tx.h"
#include <pd.h>
#include <stdbool.h>
bool                              PolicyEngine::pdNegotiationComplete;
int                               PolicyEngine::current_voltage_mv;
int                               PolicyEngine::_requested_voltage;
bool                              PolicyEngine::_unconstrained_power;
union pd_msg                      PolicyEngine::currentMessage;
uint16_t                          PolicyEngine::hdr_template;
bool                              PolicyEngine::_explicit_contract;
int8_t                            PolicyEngine::_hard_reset_counter;
int8_t                            PolicyEngine::_old_tcc_match;
uint8_t                           PolicyEngine::_pps_index;
osThreadId                        PolicyEngine::TaskHandle = NULL;
uint32_t                          PolicyEngine::TaskBuffer[PolicyEngine::TaskStackSize];
osStaticThreadDef_t               PolicyEngine::TaskControlBlock;
union pd_msg                      PolicyEngine::tempMessage;
union pd_msg                      PolicyEngine::_last_dpm_request;
PolicyEngine::policy_engine_state PolicyEngine::state = PESinkStartup;
StaticQueue_t                     PolicyEngine::xStaticQueue;
uint8_t                           PolicyEngine::ucQueueStorageArea[PDB_MSG_POOL_SIZE * sizeof(union pd_msg)];
QueueHandle_t                     PolicyEngine::messagesWaiting   = NULL;
EventGroupHandle_t                PolicyEngine::xEventGroupHandle = NULL;
StaticEventGroup_t                PolicyEngine::xCreatedEventGroup;
bool                              PolicyEngine::PPSTimerEnabled  = false;
TickType_t                        PolicyEngine::PPSTimeLastEvent = 0;
void                              PolicyEngine::init() {
  messagesWaiting = xQueueCreateStatic(PDB_MSG_POOL_SIZE, sizeof(union pd_msg), ucQueueStorageArea, &xStaticQueue);
  // Create static thread at PDB_PRIO_PE priority
  osThreadStaticDef(PolEng, pe_task, PDB_PRIO_PE, 0, TaskStackSize, TaskBuffer, &TaskControlBlock);
  TaskHandle        = osThreadCreate(osThread(PolEng), NULL);
  xEventGroupHandle = xEventGroupCreateStatic(&xCreatedEventGroup);
}

void PolicyEngine::notify(PolicyEngine::Notifications notification) {
  EventBits_t val = (EventBits_t)notification;
  if (xEventGroupHandle != NULL) {
    xEventGroupSetBits(xEventGroupHandle, val);
  }
}

void PolicyEngine::pe_task(const void *arg) {
  (void)arg;
  // Internal thread loop
  hdr_template = PD_DATAROLE_UFP | PD_POWERROLE_SINK;
  /* Initialize the old_tcc_match */
  _old_tcc_match = -1;
  /* Initialize the pps_index */
  _pps_index = 0xFF;

  for (;;) {
    // Loop based on state
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
  PPSTimerEnabled    = false;
  // If desired could send an alert that PD is starting

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
  EventBits_t evt = waitForEvent((uint32_t)Notifications::PDB_EVT_PE_MSG_RX | (uint32_t)Notifications::PDB_EVT_PE_I_OVRTEMP | (uint32_t)Notifications::PDB_EVT_PE_RESET,
                                 // Wait for cap timeout
                                 PD_T_TYPEC_SINK_WAIT_CAP);

  /* If we timed out waiting for Source_Capabilities, send a hard reset */
  if (evt == 0) {
    return PESinkHardReset;
  }
  /* If we got reset signaling, transition to default */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_RESET) {
    return PESinkWaitCap;
  }
  /* If we're too hot, we shouldn't negotiate power yet */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_I_OVRTEMP) {
    return PESinkWaitCap;
  }

  /* If we got a message */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_MSG_RX) {
    /* Get the message */
    while (readMessage()) {
      /* If we got a Source_Capabilities message, read it. */
      if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_SOURCE_CAPABILITIES && PD_NUMOBJ_GET(&tempMessage) > 0) {
        /* First, determine what PD revision we're using */
        if ((hdr_template & PD_HDR_SPECREV) == PD_SPECREV_1_0) {
          /* If the other end is using at least version 3.0, we'll
           * use version 3.0. */
          if ((tempMessage.hdr & PD_HDR_SPECREV) >= PD_SPECREV_3_0) {
            hdr_template |= PD_SPECREV_3_0;
            /* Otherwise, use 2.0.  Don't worry about the 1.0 case
             * because we don't have hardware for PD 1.0 signaling. */
          } else {
            hdr_template |= PD_SPECREV_2_0;
          }
        }
        return PESinkEvalCap;
      }
    }
    return PESinkWaitCap; // wait for more messages?
  }

  /* If we failed to get a message, wait longer */
  return PESinkWaitCap;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_eval_cap() {
  /* If we have a Source_Capabilities message, remember the index of the
   * first PPS APDO so we can check if the request is for a PPS APDO in
   * PE_SNK_Select_Cap. */
  /* Start by assuming we won't find a PPS APDO (set the index greater
   * than the maximum possible) */
  _pps_index = 0xFF;
  /* New capabilities also means we can't be making a request from the
   * same PPS APDO */
  /* Search for the first PPS APDO */
  for (int i = 0; i < PD_NUMOBJ_GET(&tempMessage); i++) {
    if ((tempMessage.obj[i] & PD_PDO_TYPE) == PD_PDO_TYPE_AUGMENTED && (tempMessage.obj[i] & PD_APDO_TYPE) == PD_APDO_TYPE_PPS) {
      _pps_index = i + 1;
      break;
    }
  }

  /* Ask the DPM what to request */
  if (pdbs_dpm_evaluate_capability(&tempMessage, &_last_dpm_request)) {
    /* If we're using PD 3.0 */
    if ((hdr_template & PD_HDR_SPECREV) == PD_SPECREV_3_0) {
      /* If the request was for a PPS APDO, start time callbacks if not started */
      if (PD_RDO_OBJPOS_GET(&_last_dpm_request) >= _pps_index) {
        PPSTimerEnabled = true;
      } else {
        PPSTimerEnabled = false;
      }
    }
    return PESinkSelectCap;
  }

  return PESinkWaitCap;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_select_cap() {

  /* Transmit the request */
  waitForEvent((uint32_t)Notifications::PDB_EVT_PE_ALL, 0); // clear pending
  ProtocolTransmit::pushMessage(&_last_dpm_request);
  // Send indication that there is a message pending
  EventBits_t evt = waitForEvent((uint32_t)Notifications::PDB_EVT_PE_TX_DONE | (uint32_t)Notifications::PDB_EVT_PE_TX_ERR | (uint32_t)Notifications::PDB_EVT_PE_RESET);
  /* If we got reset signaling, transition to default */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_RESET || evt == 0) {
    return PESinkTransitionDefault;
  }
  /* If the message transmission failed, send a hard reset */
  if ((evt & (uint32_t)Notifications::PDB_EVT_PE_TX_ERR) == (uint32_t)Notifications::PDB_EVT_PE_TX_ERR) {
    return PESinkHardReset;
  }

  /* Wait for a response */
  evt = waitForEvent((uint32_t)Notifications::PDB_EVT_PE_MSG_RX | (uint32_t)Notifications::PDB_EVT_PE_RESET, PD_T_SENDER_RESPONSE);
  /* If we got reset signaling, transition to default */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_RESET) {
    return PESinkTransitionDefault;
  }
  /* If we didn't get a response before the timeout, send a hard reset */
  if (evt == 0) {
    return PESinkSoftReset;
  }

  /* Get the response message */
  if (messageWaiting()) {
    readMessage();
    /* If the source accepted our request, wait for the new power */
    if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_ACCEPT && PD_NUMOBJ_GET(&tempMessage) == 0) {
      return PESinkTransitionSink;
      /* If the message was a Soft_Reset, do the soft reset procedure */
    } else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_SOFT_RESET && PD_NUMOBJ_GET(&tempMessage) == 0) {
      return PESinkSoftReset;
      /* If the message was Wait or Reject */
    } else if ((PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_REJECT || PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_WAIT) && PD_NUMOBJ_GET(&tempMessage) == 0) {
      /* If we don't have an explicit contract, wait for capabilities */
      if (!_explicit_contract) {
        return PESinkWaitCap;
        /* If we do have an explicit contract, go to the ready state */
      } else {
        return PESinkReady;
      }
    } else {
      return PESinkSoftReset;
    }
  }
  return PESinkHardReset;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_transition_sink() {
  /* Wait for the PS_RDY message */
  EventBits_t evt = waitForEvent((uint32_t)Notifications::PDB_EVT_PE_MSG_RX | (uint32_t)Notifications::PDB_EVT_PE_RESET, PD_T_PS_TRANSITION);
  /* If we got reset signaling, transition to default */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_RESET) {
    return PESinkTransitionDefault;
  }

  /* If we received a message, read it */
  while (messageWaiting()) {
    readMessage();
    /* If we got a PS_RDY, handle it */
    if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_PS_RDY && PD_NUMOBJ_GET(&tempMessage) == 0) {
      /* We just finished negotiating an explicit contract */
      _explicit_contract = true;

      /* Negotiation finished */
      pdbs_dpm_transition_requested();

      return PESinkReady;
      /* If there was a protocol error, send a hard reset */
    }
  }
  return PESinkSoftReset;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_ready() {
  EventBits_t evt = waitForEvent((uint32_t)Notifications::PDB_EVT_PE_ALL);
  /* If SinkPPSPeriodicTimer ran out, send a new request */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_PPS_REQUEST) {
    return PESinkSelectCap;
  }
  /* If we got reset signaling, transition to default */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_RESET) {
    return PESinkTransitionDefault;
  }

  /* If we overheated, send a hard reset */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_I_OVRTEMP) {
    return PESinkHardReset;
  }
  /* If the DPM wants us to, send a Get_Source_Cap message */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_GET_SOURCE_CAP) {
    return PESinkGetSourceCap;
  }
  /* If the DPM wants new power, let it figure out what power it wants
   * exactly.  This isn't exactly the transition from the spec (that would be
   * SelectCap, not EvalCap), but this works better with the particular
   * design of this firmware. */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_NEW_POWER) {
    /* Tell the protocol layer we're starting an AMS */
    return PESinkEvalCap;
  }

  /* If we received a message */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_MSG_RX) {
    if (messageWaiting()) {
      readMessage();
      /* Ignore vendor-defined messages */
      if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_VENDOR_DEFINED && PD_NUMOBJ_GET(&tempMessage) > 0) {
        return PESinkReady;
        /* Ignore Ping messages */
      } else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_PING && PD_NUMOBJ_GET(&tempMessage) == 0) {
        return PESinkReady;
        /* DR_Swap messages are not supported */
      } else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_DR_SWAP && PD_NUMOBJ_GET(&tempMessage) == 0) {
        return PESinkSendNotSupported;
        /* Get_Source_Cap messages are not supported */
      } else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_GET_SOURCE_CAP && PD_NUMOBJ_GET(&tempMessage) == 0) {
        return PESinkSendNotSupported;
        /* PR_Swap messages are not supported */
      } else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_PR_SWAP && PD_NUMOBJ_GET(&tempMessage) == 0) {
        return PESinkSendNotSupported;
        /* VCONN_Swap messages are not supported */
      } else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_VCONN_SWAP && PD_NUMOBJ_GET(&tempMessage) == 0) {
        return PESinkSendNotSupported;
        /* Request messages are not supported */
      } else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_REQUEST && PD_NUMOBJ_GET(&tempMessage) > 0) {
        return PESinkSendNotSupported;
        /* Sink_Capabilities messages are not supported */
      } else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_SINK_CAPABILITIES && PD_NUMOBJ_GET(&tempMessage) > 0) {
        return PESinkSendNotSupported;
        /* Handle GotoMin messages */
      } else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_GOTOMIN && PD_NUMOBJ_GET(&tempMessage) == 0) {
        return PESinkSendNotSupported;
        /* Evaluate new Source_Capabilities */
      } else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_SOURCE_CAPABILITIES && PD_NUMOBJ_GET(&tempMessage) > 0) {
        return PESinkEvalCap;
        /* Give sink capabilities when asked */
      } else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_GET_SINK_CAP && PD_NUMOBJ_GET(&tempMessage) == 0) {
        return PESinkGiveSinkCap;
        /* If the message was a Soft_Reset, do the soft reset procedure */
      } else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_SOFT_RESET && PD_NUMOBJ_GET(&tempMessage) == 0) {
        return PESinkSoftReset;
        /* PD 3.0 messges */
      } else if ((hdr_template & PD_HDR_SPECREV) == PD_SPECREV_3_0) {
        /* If the message is a multi-chunk extended message, let it
         * time out. */
        if ((tempMessage.hdr & PD_HDR_EXT) && (PD_DATA_SIZE_GET(&tempMessage) > PD_MAX_EXT_MSG_LEGACY_LEN)) {

          return PESinkChunkReceived;
          /* Tell the DPM a message we sent got a response of
           * Not_Supported. */
        } else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_NOT_SUPPORTED && PD_NUMOBJ_GET(&tempMessage) == 0) {

          return PESinkNotSupportedReceived;
          /* If we got an unknown message, send a soft reset */
        } else {

          return PESinkSendSoftReset;
        }
      }
    }
  }

  return PESinkReady;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_get_source_cap() {
  /* Get a message object */
  union pd_msg *get_source_cap = &tempMessage;
  /* Make a Get_Source_Cap message */
  get_source_cap->hdr = hdr_template | PD_MSGTYPE_GET_SOURCE_CAP | PD_NUMOBJ(0);
  /* Transmit the Get_Source_Cap */
  ProtocolTransmit::pushMessage(get_source_cap);
  EventBits_t evt = waitForEvent((uint32_t)Notifications::PDB_EVT_PE_TX_DONE | (uint32_t)Notifications::PDB_EVT_PE_TX_ERR | (uint32_t)Notifications::PDB_EVT_PE_RESET);
  /* Free the sent message */
  /* If we got reset signaling, transition to default */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_RESET) {
    return PESinkTransitionDefault;
  }
  /* If the message transmission failed, send a hard reset */
  if ((evt & (uint32_t)Notifications::PDB_EVT_PE_TX_DONE) == 0) {
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
  EventBits_t evt = waitForEvent((uint32_t)Notifications::PDB_EVT_PE_TX_DONE | (uint32_t)Notifications::PDB_EVT_PE_TX_ERR | (uint32_t)Notifications::PDB_EVT_PE_RESET);

  /* Free the Sink_Capabilities message */

  /* If we got reset signaling, transition to default */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_RESET) {
    return PESinkTransitionDefault;
  }
  /* If the message transmission failed, send a hard reset */
  if ((evt & (uint32_t)Notifications::PDB_EVT_PE_TX_DONE) == 0) {
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
  // So, we could send a hardreset here; however that will cause a power cycle on the PSU end.. Which will then reset this MCU
  // So therefore we went get anywhere :)
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
  EventBits_t evt = waitForEvent((uint32_t)Notifications::PDB_EVT_PE_TX_DONE | (uint32_t)Notifications::PDB_EVT_PE_TX_ERR | (uint32_t)Notifications::PDB_EVT_PE_RESET);
  /* Free the sent message */

  /* If we got reset signaling, transition to default */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_RESET) {
    return PESinkTransitionDefault;
  }
  /* If the message transmission failed, send a hard reset */
  if ((evt & (uint32_t)Notifications::PDB_EVT_PE_TX_DONE) == 0) {
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
  EventBits_t evt = waitForEvent((uint32_t)Notifications::PDB_EVT_PE_TX_DONE | (uint32_t)Notifications::PDB_EVT_PE_TX_ERR | (uint32_t)Notifications::PDB_EVT_PE_RESET);
  /* If we got reset signaling, transition to default */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_RESET) {
    return PESinkTransitionDefault;
  }
  /* If the message transmission failed, send a hard reset */
  if ((evt & (uint32_t)Notifications::PDB_EVT_PE_TX_DONE) == 0) {
    return PESinkHardReset;
  }

  /* Wait for a response */
  evt = waitForEvent((uint32_t)Notifications::PDB_EVT_PE_MSG_RX | (uint32_t)Notifications::PDB_EVT_PE_RESET, PD_T_SENDER_RESPONSE);
  /* If we got reset signaling, transition to default */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_RESET) {
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
    if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_ACCEPT && PD_NUMOBJ_GET(&tempMessage) == 0) {

      return PESinkWaitCap;
      /* If the message was a Soft_Reset, do the soft reset procedure */
    } else if (PD_MSGTYPE_GET(&tempMessage) == PD_MSGTYPE_SOFT_RESET && PD_NUMOBJ_GET(&tempMessage) == 0) {

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

  if ((hdr_template & PD_HDR_SPECREV) == PD_SPECREV_2_0) {
    /* Make a Reject message */
    tempMessage.hdr = hdr_template | PD_MSGTYPE_REJECT | PD_NUMOBJ(0);
  } else if ((hdr_template & PD_HDR_SPECREV) == PD_SPECREV_3_0) {
    /* Make a Not_Supported message */
    tempMessage.hdr = hdr_template | PD_MSGTYPE_NOT_SUPPORTED | PD_NUMOBJ(0);
  }

  /* Transmit the message */
  ProtocolTransmit::pushMessage(&tempMessage);
  EventBits_t evt = waitForEvent((uint32_t)Notifications::PDB_EVT_PE_TX_DONE | (uint32_t)Notifications::PDB_EVT_PE_TX_ERR | (uint32_t)Notifications::PDB_EVT_PE_RESET);

  /* If we got reset signaling, transition to default */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_RESET) {
    return PESinkTransitionDefault;
  }
  /* If the message transmission failed, send a soft reset */
  if ((evt & (uint32_t)Notifications::PDB_EVT_PE_TX_DONE) == 0) {
    return PESinkSendSoftReset;
  }

  return PESinkReady;
}

PolicyEngine::policy_engine_state PolicyEngine::pe_sink_chunk_received() {

  /* Wait for tChunkingNotSupported */
  EventBits_t evt = waitForEvent((uint32_t)Notifications::PDB_EVT_PE_RESET, PD_T_CHUNKING_NOT_SUPPORTED);
  /* If we got reset signaling, transition to default */
  if (evt & (uint32_t)Notifications::PDB_EVT_PE_RESET) {
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
  // Sit and chill, as PD is not working
  osDelay(PD_T_PD_DEBOUNCE);

  return PESinkSourceUnresponsive;
}

EventBits_t PolicyEngine::waitForEvent(uint32_t mask, TickType_t ticksToWait) { return xEventGroupWaitBits(xEventGroupHandle, mask, mask, pdFALSE, ticksToWait); }

bool PolicyEngine::isPD3_0() { return (hdr_template & PD_HDR_SPECREV) == PD_SPECREV_3_0; }

void PolicyEngine::handleMessage(union pd_msg *msg) {
  xQueueSend(messagesWaiting, msg, 100);
  notify(PolicyEngine::Notifications::PDB_EVT_PE_MSG_RX);
}

void PolicyEngine::PPSTimerCallback() {
  if (PPSTimerEnabled && state == policy_engine_state::PESinkReady) {
    // I believe even once per second is totally fine, but leaning on faster since everything seems cool with faster
    // Have seen everything from 10ms to 1 second :D
    if ((xTaskGetTickCount() - PPSTimeLastEvent) > (TICKS_SECOND)) {
      // Send a new PPS message
      PolicyEngine::notify(Notifications::PDB_EVT_PE_PPS_REQUEST);
      PPSTimeLastEvent = xTaskGetTickCount();
    }
  }
}
