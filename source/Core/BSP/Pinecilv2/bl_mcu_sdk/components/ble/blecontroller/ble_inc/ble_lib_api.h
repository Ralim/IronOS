#ifndef BLE_LIB_API_H_
#define BLE_LIB_API_H_

#include <stdbool.h>
#include <stdint.h>

void ble_controller_init(uint8_t task_priority);
void ble_controller_deinit(void);
#if defined(CFG_BT_RESET)
void ble_controller_reset(void);
#endif
char *ble_controller_get_lib_ver(void);

// if 0, success.
// if -1, fail,
int8_t ble_controller_set_scan_filter_table_size(uint8_t size);

// return sleep duration, in unit of 1/32768s
// if 0, means not allow sleep
// if -1, means allow sleep, but there is no end of sleep interrupt (ble core deep sleep is not enabled)
int32_t ble_controller_sleep(void);
void ble_controller_sleep_restore(void);
bool ble_controller_sleep_is_ongoing(void);

void ble_controller_set_tx_pwr(int ble_tx_power);
void ble_rf_set_tx_channel(uint16_t tx_channel);

#if defined(CONFIG_BLE_MFG)
enum {
    BLE_TEST_TX = 0x00,
    BLE_TEST_RX,
    BLE_TEST_RXTX,
    BLE_TEST_END
};

///HCI LE Receiver Test Command parameters structure
struct le_rx_test_cmd {
    ///RX frequency for Rx test
    uint8_t rx_freq;
};

///HCI LE Transmitter Test Command parameters structure
struct le_tx_test_cmd {
    ///TX frequency for Tx test
    uint8_t tx_freq;
    ///TX test data length
    uint8_t test_data_len;
    ///TX test payload type - see enum
    uint8_t pk_payload_type;
};

struct le_enhanced_rx_test_cmd {
    ///RX frequency for Rx test
    uint8_t rx_freq;
    ///RX PHY for Rx test
    uint8_t rx_phy;
    ///Modulation index: Assume transmitter will have a standard or stable modulation index
    uint8_t modulation_index;
};

///HCI LE Enhanced Transmitter Test Command parameters structure
struct le_enhanced_tx_test_cmd {
    ///TX frequency for Tx test
    uint8_t tx_freq;
    ///TX test data length
    uint8_t test_data_len;
    ///TX test payload type - see enum
    uint8_t pk_payload_type;
    ///TX PHY for Rx test
    uint8_t tx_phy;
};

int le_rx_test_cmd_handler(uint16_t src_id, void *param, bool from_hci);
int le_tx_test_cmd_handler(uint16_t src_id, void *param, bool from_hci);
int le_test_end_cmd_handler(bool from_hci);
uint8_t le_get_direct_test_type(void);

#if defined(CONFIG_BLE_MFG_HCI_CMD)
int reset_cmd_handler(void);
#endif
#endif

#endif
