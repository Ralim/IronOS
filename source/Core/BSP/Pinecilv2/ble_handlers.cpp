

#include <FreeRTOS.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <task.h>

#include "types.h"

#include "BSP.h"
#include "TipThermoModel.h"
#include "ble_peripheral.h"
#include "bluetooth.h"
#include "configuration.h"
#include "conn.h"
#include "gatt.h"
#include "hal_clock.h"
#include "hci_core.h"
#include "log.h"
#include "uuid.h"

#include "USBPD.h"
#include "ble_characteristics.h"
#include "ble_handlers.h"
#include "pd.h"
#include "power.hpp"
#if POW_PD
#include "USBPD.h"
#include "pd.h"
#endif

extern TickType_t lastMovementTime;

int ble_char_read_status_callback(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  if (attr == NULL || attr->uuid == NULL) {
    return 0;
  }
  uint16_t uuid_value = ((struct bt_uuid_16 *)attr->uuid)->val;
  uint32_t temp       = 0;
  switch (uuid_value) {
  case 1: // Live temp
  {
    temp = TipThermoModel::getTipInC();
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
  } break;
  case 2: // Setpoint temp
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
    if (getIsPoweredByDCIN()) {
      temp = 0;
    } else if (USBPowerDelivery::negotiationComplete()) {
      temp = 1;
    } else {
      temp = 2;
    }
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
    break;
  case 7:
    // Tip resistance
    temp = getTipResistanceX10();
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
    break;
  case 8:
    // uptime
    temp = xTaskGetTickCount() / TICKS_100MS;
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
    break;
  case 9:
    // movement
    temp = lastMovementTime / TICKS_100MS;
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);

    break;
  case 10:
    // max temp
    temp = TipThermoModel::getTipMaxInC();
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
    break;
  case 11:
    // raw tip
    temp = TipThermoModel::convertTipRawADCTouV(getTipRawTemp(0), true);
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
    break;
  case 12:
    // hall sensor
    {
      int16_t hallEffectStrength = getRawHallEffect();
      if (hallEffectStrength < 0)
        hallEffectStrength = -hallEffectStrength;
      temp = hallEffectStrength;
      memcpy(buf, &temp, sizeof(temp));
      return sizeof(temp);
    }
    break;
  case 13:
    // Operating mode
    // TODO: Needs tracking
    break;
  case 14:
    // Estimated watts
    temp = x10WattHistory.average();
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
    break;
  }
  MSG("Unhandled attr read %d | %d\n", (uint32_t)attr->uuid, uuid_value);
  return 0;
}
int ble_char_read_bulk_value_callback(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  if (attr == NULL || attr->uuid == NULL) {
    return 0;
  }
  uint16_t uuid_value = ((struct bt_uuid_16 *)attr->uuid)->val;
  // Bulk is the non-const size service
  switch (uuid_value) {
  case 1:
    // Bulk data
    {
      uint32_t bulkData[] = {
          TipThermoModel::getTipInC(),                                         // Current temp
          getSettingValue(SettingsOptions::SolderingTemp),                     // Setpoint
          getHandleTemperature(0),                                             // Handle X10 Temp in C
          getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0), // Input voltage
          x10WattHistory.average(),                                            // Estimated Wattage
          X10WattsToPWM(x10WattHistory.average()),                             // Power as PWM level
          TipThermoModel::convertTipRawADCTouV(getTipRawTemp(0), true),        // Raw tip
      };
      int lenToCopy = sizeof(bulkData) - offset;
      if (lenToCopy > len) {
        lenToCopy = len;
      }
      if (lenToCopy < 0) {
        lenToCopy = 0;
      }
      memcpy(buf, ((uint8_t *)bulkData) + offset, lenToCopy);
      return lenToCopy;
    }

    break;
  case 2:
    // Accelerometer name
    // TODO: Need to store non-encoded version
    break;
  case 3:
    // Build
    // TODO: Need to store non-encoded version
    break;
  case 4:
    // Device unique id
    {
      uint64_t id = getDeviceID();
      memcpy(buf, &id, sizeof(id));
      return sizeof(id);
    }
    break;
  }
  return 0;
}
int ble_char_read_setting_value_callback(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  if (attr == NULL || attr->uuid == NULL) {
    return 0;
  }
  uint16_t uuid_value = ((struct bt_uuid_16 *)attr->uuid)->val;
  uint16_t temp       = 0;
  if (uuid_value == 0) {
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
  } else if (uuid_value <= SettingsOptions::SettingsOptionsLength) {
    temp = getSettingValue((SettingsOptions)(uuid_value - 1));
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
  }

  MSG("Unhandled attr read %d | %d\n", (uint32_t)attr->uuid, uuid_value);
  return 0;
}

int ble_char_write_setting_value_callback(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, u16_t len, u16_t offset, u8_t flags) {

  if (flags & BT_GATT_WRITE_FLAG_PREPARE) {
    // Don't use prepare write data, execute write will upload data again.
    BT_WARN("recv prepare write request\n");
    return 0;
  }
  if (attr == NULL || attr->uuid == NULL) {
    return 0;
  }

  if (flags & BT_GATT_WRITE_FLAG_CMD) {
    // Use write command data.
    BT_WARN("recv write command\n");
  } else {
    // Use write request / execute write data.
    BT_WARN("recv write request / exce write\n");
  }
  uint16_t uuid_value = ((struct bt_uuid_16 *)attr->uuid)->val;
  if (len == 2) {
    uint16_t new_value = 0;
    memcpy(&new_value, buf, sizeof(new_value));
    if (uuid_value == 0) {
      if (new_value == 1) {
        saveSettings();
        return len;
      }
    } else if (uuid_value < SettingsOptions::SettingsOptionsLength) {
      setSettingValue((SettingsOptions)(uuid_value - 1), new_value);
      return len;
    }
  }
  MSG("Unhandled attr write %d | %d\n", (uint32_t)attr->uuid, uuid_value);
  return 0;
}