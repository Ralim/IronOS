#ifndef PINECILV2_BLE_H_
#define PINECILV2_BLE_H_

/*
 * BLE Interface for the Pinecil V2
 *
 * Exposes:
 * - Live Measurements
 * - Device Settings Names
 * - Device Settings Values
 */
#ifdef __cplusplus
extern "C" {
#endif

// Spawns the BLE stack tasks and makes the device available to be connected to via BLE.
void ble_stack_start(void);

#ifdef __cplusplus
};
#endif

#endif