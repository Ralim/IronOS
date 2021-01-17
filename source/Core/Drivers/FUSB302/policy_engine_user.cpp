/*
 * policy_engine_user.cpp
 *
 *  Created on: 14 Jun 2020
 *      Author: Ralim
 */
#include "BSP_PD.h"
#include "pd.h"
#include "policy_engine.h"
/* The current draw when the output is disabled */
#define DPM_MIN_CURRENT PD_MA2PDI(50)
/*
 * Find the index of the first PDO from capabilities in the voltage range,
 * using the desired order.
 *
 * If there is no such PDO, returns -1 instead.
 */
static int8_t dpm_get_range_fixed_pdo_index(const union pd_msg *caps) {
  /* Get the number of PDOs */
  uint8_t numobj = PD_NUMOBJ_GET(caps);

  /* Get ready to iterate over the PDOs */
  int8_t i;
  int8_t step;
  i                   = numobj - 1;
  step                = -1;
  uint16_t current    = 100; // in centiamps
  uint16_t voltagemin = 8000;
  uint16_t voltagemax = 10000;
  /* Look at the PDOs to see if one falls in our voltage range. */
  while (0 <= i && i < numobj) {
    /* If we have a fixed PDO, its V is within our range, and its I is at
     * least our desired I */
    uint16_t v = PD_PDO_SRC_FIXED_VOLTAGE_GET(caps->obj[i]);
    if ((caps->obj[i] & PD_PDO_TYPE) == PD_PDO_TYPE_FIXED) {
      if (PD_PDO_SRC_FIXED_CURRENT_GET(caps->obj[i]) >= current) {
        if (v >= PD_MV2PDV(voltagemin) && v <= PD_MV2PDV(voltagemax)) {
          return i;
        }
      }
    }
    i += step;
  }
  return -1;
}
bool PolicyEngine::pdbs_dpm_evaluate_capability(const union pd_msg *capabilities, union pd_msg *request) {

  /* Get the number of PDOs */
  uint8_t numobj = PD_NUMOBJ_GET(capabilities);

  /* Get whether or not the power supply is constrained */
  _unconstrained_power = capabilities->obj[0] & PD_PDO_SRC_FIXED_UNCONSTRAINED;

  /* Make sure we have configuration */
  /* Look at the PDOs to see if one matches our desires */
  // Look against USB_PD_Desired_Levels to select in order of preference
  for (uint8_t desiredLevel = 0; desiredLevel < USB_PD_Desired_Levels_Len; desiredLevel++) {
    for (uint8_t i = 0; i < numobj; i++) {
      /* If we have a fixed PDO, its V equals our desired V, and its I is
       * at least our desired I */
      if ((capabilities->obj[i] & PD_PDO_TYPE) == PD_PDO_TYPE_FIXED) {
        // This is a fixed PDO entry
        int      voltage           = PD_PDV2MV(PD_PDO_SRC_FIXED_VOLTAGE_GET(capabilities->obj[i]));
        int      current           = PD_PDO_SRC_FIXED_CURRENT_GET(capabilities->obj[i]);
        uint16_t desiredVoltage    = USB_PD_Desired_Levels[(desiredLevel * 2) + 0];
        uint16_t desiredminCurrent = USB_PD_Desired_Levels[(desiredLevel * 2) + 1];
        // As pd stores current in 10mA increments, divide by 10
        desiredminCurrent /= 10;
        if (voltage == desiredVoltage) {
          if (current >= desiredminCurrent) {
            /* We got what we wanted, so build a request for that */
            request->hdr = hdr_template | PD_MSGTYPE_REQUEST | PD_NUMOBJ(1);

            /* GiveBack disabled */
            request->obj[0] = PD_RDO_FV_MAX_CURRENT_SET(current) | PD_RDO_FV_CURRENT_SET(current) | PD_RDO_NO_USB_SUSPEND | PD_RDO_OBJPOS_SET(i + 1);
            // We support usb comms (ish)
            request->obj[0] |= PD_RDO_USB_COMMS;

            /* Update requested voltage */
            _requested_voltage = voltage;

            return true;
          }
        }
      }
    }
  }

  /* Nothing matched (or no configuration), so get 5 V at low current */
  request->hdr    = hdr_template | PD_MSGTYPE_REQUEST | PD_NUMOBJ(1);
  request->obj[0] = PD_RDO_FV_MAX_CURRENT_SET(DPM_MIN_CURRENT) | PD_RDO_FV_CURRENT_SET(DPM_MIN_CURRENT) | PD_RDO_NO_USB_SUSPEND | PD_RDO_OBJPOS_SET(1);
  /* If the output is enabled and we got here, it must be a capability
   * mismatch. */
  if (pdNegotiationComplete) {
    request->obj[0] |= PD_RDO_CAP_MISMATCH;
  }
  request->obj[0] |= PD_RDO_USB_COMMS;

  /* Update requested voltage */
  _requested_voltage = 5000;

  return false;
}

void PolicyEngine::pdbs_dpm_get_sink_capability(union pd_msg *cap) {
  /* Keep track of how many PDOs we've added */
  int numobj = 0;

  /* If we have no configuration or want something other than 5 V, add a PDO
   * for vSafe5V */
  /* Minimum current, 5 V, and higher capability. */
  cap->obj[numobj++] = PD_PDO_TYPE_FIXED | PD_PDO_SNK_FIXED_VOLTAGE_SET(PD_MV2PDV(5000)) | PD_PDO_SNK_FIXED_CURRENT_SET(DPM_MIN_CURRENT);

  /* Get the current we want */
  uint16_t current = USB_PD_Desired_Levels[1] / 10; // In centi-amps
  uint16_t voltage = USB_PD_Desired_Levels[0];      // in mv
  /* Add a PDO for the desired power. */
  cap->obj[numobj++] = PD_PDO_TYPE_FIXED | PD_PDO_SNK_FIXED_VOLTAGE_SET(PD_MV2PDV(voltage)) | PD_PDO_SNK_FIXED_CURRENT_SET(current);

  /* Get the PDO from the voltage range */
  int8_t i = dpm_get_range_fixed_pdo_index(cap);

  /* If it's vSafe5V, set our vSafe5V's current to what we want */
  if (i == 0) {
    cap->obj[0] &= ~PD_PDO_SNK_FIXED_CURRENT;
    cap->obj[0] |= PD_PDO_SNK_FIXED_CURRENT_SET(current);
  } else {
    /* If we want more than 5 V, set the Higher Capability flag */
    if (PD_MV2PDV(voltage) != PD_MV2PDV(5000)) {
      cap->obj[0] |= PD_PDO_SNK_FIXED_HIGHER_CAP;
    }

    /* If the range PDO is a different voltage than the preferred
     * voltage, add it to the array. */
    if (i > 0 && PD_PDO_SRC_FIXED_VOLTAGE_GET(cap->obj[i]) != PD_MV2PDV(voltage)) {
      cap->obj[numobj++] = PD_PDO_TYPE_FIXED | PD_PDO_SNK_FIXED_VOLTAGE_SET(PD_PDO_SRC_FIXED_VOLTAGE_GET(cap->obj[i])) | PD_PDO_SNK_FIXED_CURRENT_SET(PD_PDO_SRC_FIXED_CURRENT_GET(cap->obj[i]));
    }

    /* If we have three PDOs at this point, make sure the last two are
     * sorted by voltage. */
    if (numobj == 3 && (cap->obj[1] & PD_PDO_SNK_FIXED_VOLTAGE) > (cap->obj[2] & PD_PDO_SNK_FIXED_VOLTAGE)) {
      cap->obj[1] ^= cap->obj[2];
      cap->obj[2] ^= cap->obj[1];
      cap->obj[1] ^= cap->obj[2];
    }
  }

  /* Set the unconstrained power flag. */
  if (_unconstrained_power) {
    cap->obj[0] |= PD_PDO_SNK_FIXED_UNCONSTRAINED;
  }
  /* Set the USB communications capable flag. */
  cap->obj[0] |= PD_PDO_SNK_FIXED_USB_COMMS;

  /* Set the Sink_Capabilities message header */
  cap->hdr = hdr_template | PD_MSGTYPE_SINK_CAPABILITIES | PD_NUMOBJ(numobj);
}

bool PolicyEngine::pdbs_dpm_evaluate_typec_current(enum fusb_typec_current tcc) {
  (void)tcc;
  // This is for evaluating 5V static current advertised by resistors
  /* We don't control the voltage anymore; it will always be 5 V. */
  current_voltage_mv = _requested_voltage = 5000;
  // For the soldering iron we accept this as a fallback, but it sucks
  pdNegotiationComplete = false;
  return true;
}

void PolicyEngine::pdbs_dpm_transition_default() {
  /* Cast the dpm_data to the right type */

  /* Pretend we requested 5 V */
  current_voltage_mv = 5000;
  /* Turn the output off */
  pdNegotiationComplete = false;
}

void PolicyEngine::pdbs_dpm_transition_requested() {
  /* Cast the dpm_data to the right type */
  pdNegotiationComplete = true;
}

void PolicyEngine::handleMessage(union pd_msg *msg) { xQueueSend(messagesWaiting, msg, 100); }

bool PolicyEngine::messageWaiting() { return uxQueueMessagesWaiting(messagesWaiting) > 0; }

bool PolicyEngine::readMessage() { return xQueueReceive(messagesWaiting, &tempMessage, 0) == pdTRUE; }

void PolicyEngine::pdbs_dpm_transition_typec() {
  // This means PD failed, so we either have a dump 5V only type C or a QC charger
  // For now; treat this as failed neg
  pdNegotiationComplete = false;
}
