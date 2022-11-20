

#include <FreeRTOS.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <task.h>

#include "types.h"

#include "ble_peripheral.h"
#include "bluetooth.h"
#include "conn.h"
#include "gatt.h"
#include "hal_clock.h"
#include "hci_core.h"
#include "log.h"
#include "uuid.h"
#include "configuration.h"
#include "BSP.h"
#include "TipThermoModel.h"

#include "ble_characteristics.h"
#include "ble_handlers.h"
#include "power.hpp"

 int ble_char_read_status_callback(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  if (attr == NULL || attr->uuid == NULL) {
    return 0;
  }
  uint16_t uuid_value = ((struct bt_uuid_16 *)attr->uuid)->val;
  uint32_t temp=0;
  switch (uuid_value) {
  case 1: // Live temp
  {
    temp = TipThermoModel::getTipInC();
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
  } break;
  case 2://Setpoint temp
    temp = getSettingValue(SettingsOptions::SolderingTemp);
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
    break;
  case 3: // DC Input
    temp = getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0);
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
    break;
  case 4: // Handle temp
    temp = getHandleTemperature(0);
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
    break;
  case 5: // power level
    // return current PWM level
    temp = X10WattsToPWM(x10WattHistory.average());
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
 
    break;
  case 6: // power src
 // Todo return enum for current power source
    break;
  }
  MSG("Unhandled attr read %d | %d\n", (uint32_t)attr->uuid, uuid_value);
  return 0;
}

/*************************************************************************
NAME
    ble_tp_recv_wr(receive data from client)
*/

int ble_tp_recv_wr(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, u16_t len, u16_t offset, u8_t flags) {
  BT_WARN("recv data len=%d, offset=%d, flag=%d\n", len, offset, flags);
  BT_WARN("recv data:%s\n", bt_hex(buf, len));

  if (flags & BT_GATT_WRITE_FLAG_PREPARE) {
    // Don't use prepare write data, execute write will upload data again.
    BT_WARN("recv prepare write request\n");
    return 0;
  }

  if (flags & BT_GATT_WRITE_FLAG_CMD) {
    // Use write command data.
    BT_WARN("recv write command\n");
  } else {
    // Use write request / execute write data.
    BT_WARN("recv write request / exce write\n");
  }

  return len;
}