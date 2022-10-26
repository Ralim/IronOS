#ifndef BLE_CHARACTERISTICS_H_
#define BLE_CHARACTERISTICS_H_

#include "ble_config.h"

/*

 Pinecil exposes two main services; Status and settings

 Status:
 - Current setpoint temperature
 - Current live tip temperature
 - Current DC Input
 - Current Handle cold junction temperature
 - Current power level (aka pwm level)

 Settings:
 - One entry for every setting in the unit
*/

// d85efab4-168e-4a71-affd-33e27f9bc533
#define BT_UUID_SVC_LIVE_DATA BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0xd85efab4, 0x168e, 0x4a71, 0xaffd, 0x33e27f9bc533))
// f6d75f91-5a10-4eba-a233-47d3f26a907f
#define BT_UUID_SVC_SETTINGS_DATA BT_UUID_DECLARE_128(BT_UUID_128_ENCODE(0xf6d75f91, 0x5a10, 0x4eba, 0xa233, 0x47d3f26a907f))

#define BT_UUID_CHAR_BLE_LIVE_SETPOINT_TEMP BT_UUID_DECLARE_16(0x0001)
#define BT_UUID_CHAR_BLE_LIVE_LIVE_TEMP     BT_UUID_DECLARE_16(0x0002)
#define BT_UUID_CHAR_BLE_LIVE_DC_INPUT      BT_UUID_DECLARE_16(0x0003)
#define BT_UUID_CHAR_BLE_LIVE_HANDLE_TEMP   BT_UUID_DECLARE_16(0x0004)
#define BT_UUID_CHAR_BLE_LIVE_POWER_LEVEL   BT_UUID_DECLARE_16(0x0005)
#define BT_UUID_CHAR_BLE_LIVE_POWER_SRC     BT_UUID_DECLARE_16(0x0006)

#endif