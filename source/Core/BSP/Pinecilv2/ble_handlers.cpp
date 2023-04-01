

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

#include "../../version.h"
#include "OLED.hpp"
#include "OperatingModes.h"
#include "USBPD.h"
#include "ble_characteristics.h"
#include "ble_handlers.h"
#include "pd.h"
#include "power.hpp"
#if POW_PD
#include "USBPD.h"
#include "pd.h"
#endif

extern TickType_t    lastMovementTime;
extern OperatingMode currentMode;

int ble_char_read_status_callback(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  if (attr == NULL || attr->uuid == NULL) {
    return 0;
  }
  // Decode the uuid
  // Byte 12 has the lowest part of the first UUID chunk
  uint16_t uuid_value = ((struct bt_uuid_128 *)attr->uuid)->val[12];
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
    temp = getPowerSrc();
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
    temp = currentMode;
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
    break;
  case 14:
    // Estimated watts
    temp = x10WattHistory.average();
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
    break;
  }
  MSG((char *)"Unhandled attr read %d | %d\n", (uint32_t)attr->uuid, uuid_value);
  return 0;
}
int ble_char_read_bulk_value_callback(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  if (attr == NULL || attr->uuid == NULL) {
    return 0;
  }
  // Byte 12 has the lowest part of the first UUID chunk
  uint16_t uuid_value = ((struct bt_uuid_128 *)attr->uuid)->val[12];
  // Bulk is the non-const size service
  switch (uuid_value) {
  case 1:
    // Bulk data
    {
      uint32_t bulkData[] = {
          TipThermoModel::getTipInC(),                                         // 0  - Current temp
          getSettingValue(SettingsOptions::SolderingTemp),                     // 1  - Setpoint
          getInputVoltageX10(getSettingValue(SettingsOptions::VoltageDiv), 0), // 2  - Input voltage
          getHandleTemperature(0),                                             // 3  - Handle X10 Temp in C
          X10WattsToPWM(x10WattHistory.average()),                             // 4  - Power as PWM level
          getPowerSrc(),                                                       // 5  - power src
          getTipResistanceX10(),                                               // 6  - Tip resistance
          xTaskGetTickCount() / TICKS_100MS,                                   // 7  - uptime in deciseconds
          lastMovementTime / TICKS_100MS,                                      // 8  - last movement time (deciseconds)
          TipThermoModel::getTipMaxInC(),                                      // 9  - max temp
          TipThermoModel::convertTipRawADCTouV(getTipRawTemp(0), true),        // 10 - Raw tip in μV
          abs(getRawHallEffect()),                                             // 11 - hall sensor
          currentMode,                                                         // 12 - Operating mode
          x10WattHistory.average(),                                            // 13 - Estimated Wattage *10
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
    // FW Version
    memcpy(buf, &BUILD_VERSION, sizeof(BUILD_VERSION) - 1);
    return sizeof(BUILD_VERSION) - 1;
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
  // Byte 12 has the lowest part of the first UUID chunk
  uint16_t uuid_value = ((struct bt_uuid_128 *)attr->uuid)->val[12];
  uint16_t temp       = 0xFFFF;
  if (uuid_value <= SettingsOptions::SettingsOptionsLength) {
    temp = getSettingValue((SettingsOptions)(uuid_value));
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
  } else {
    memcpy(buf, &temp, sizeof(temp));
    return sizeof(temp);
  }

  MSG((char *)"Unhandled attr read %d | %d\n", (uint32_t)attr->uuid, uuid_value);
  return 0;
}

int ble_char_write_setting_value_callback(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, u16_t len, u16_t offset, u8_t flags) {

  if (flags & BT_GATT_WRITE_FLAG_PREPARE) {
    // Don't use prepare write data, execute write will upload data again.
    BT_WARN((char *)"recv prepare write request\n");
    return 0;
  }
  if (attr == NULL || attr->uuid == NULL) {
    return 0;
  }

  if (flags & BT_GATT_WRITE_FLAG_CMD) {
    // Use write command data.
    BT_WARN((char *)"recv write command\n");
  } else {
    // Use write request / execute write data.
    BT_WARN((char *)"recv write request / exce write\n");
  }
  uint8_t uuid_value = ((struct bt_uuid_128 *)attr->uuid)->val[12];
  if (len == 2) {
    uint16_t new_value = 0;
    memcpy(&new_value, buf, sizeof(new_value));
    if (uuid_value == 0xFF) {
      if (new_value == 1) {
        saveSettings();
        return len;
      }
    } else if (uuid_value == 0xFE) {
      if (new_value == 1) {
        resetSettings();
        return len;
      }
    } else if (uuid_value < SettingsOptions::SettingsOptionsLength) {
      setSettingValue((SettingsOptions)(uuid_value), new_value);
      switch (uuid_value) {
      case SettingsOptions::OLEDInversion:
        OLED::setInverseDisplay(getSettingValue(SettingsOptions::OLEDInversion));
        break;
      case SettingsOptions::OLEDBrightness:
        OLED::setBrightness(getSettingValue(SettingsOptions::OLEDBrightness));
        break;
      case SettingsOptions::OrientationMode:
        OLED::setRotation(getSettingValue(SettingsOptions::OrientationMode) & 1);
        break;
      default:
        break;
      }
      return len;
    }
  }
  MSG((char *)"Unhandled attr write %d | %d\n", (uint32_t)attr->uuid, uuid_value);
  return 0;
}

uint32_t getPowerSrc() {
  int sourceNumber = 0;
  if (getIsPoweredByDCIN()) {
    sourceNumber = 0;
  } else {
    // We are not powered via DC, so want to display the appropriate state for PD or QC
    bool poweredbyPD        = false;
    bool pdHasVBUSConnected = false;
#if POW_PD
    if (USBPowerDelivery::fusbPresent()) {
      // We are PD capable
      if (USBPowerDelivery::negotiationComplete()) {
        // We are powered via PD
        poweredbyPD = true;
#ifdef VBUS_MOD_TEST
        pdHasVBUSConnected = USBPowerDelivery::isVBUSConnected();
#endif
      }
    }
#endif
    if (poweredbyPD) {

      if (pdHasVBUSConnected) {
        sourceNumber = 2;
      } else {
        sourceNumber = 3;
      }
    } else {
      sourceNumber = 1;
    }
  }
  return sourceNumber;
}
