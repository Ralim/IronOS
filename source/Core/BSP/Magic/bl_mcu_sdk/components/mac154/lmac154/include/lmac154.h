#ifndef __LMAC154_H__
#define __LMAC154_H__


#include <stdint.h>
#include <stdlib.h>
#include <string.h>


typedef void (*lmac154_isr_t)(void);


typedef enum {
    LMAC154_CHANNEL_11 = 0,
    LMAC154_CHANNEL_12,
    LMAC154_CHANNEL_13,
    LMAC154_CHANNEL_14,
    LMAC154_CHANNEL_15,
    LMAC154_CHANNEL_16,
    LMAC154_CHANNEL_17,
    LMAC154_CHANNEL_18,
    LMAC154_CHANNEL_19,
    LMAC154_CHANNEL_20,
    LMAC154_CHANNEL_21,
    LMAC154_CHANNEL_22,
    LMAC154_CHANNEL_23,
    LMAC154_CHANNEL_24,
    LMAC154_CHANNEL_25,
    LMAC154_CHANNEL_26,
}lmac154_channel_t;

typedef enum {
    LMAC154_TX_POWER_0dBm  = 0,
    LMAC154_TX_POWER_1dBm  = 1,
    LMAC154_TX_POWER_2dBm  = 2,
    LMAC154_TX_POWER_3dBm  = 3,
    LMAC154_TX_POWER_4dBm  = 4,
    LMAC154_TX_POWER_5dBm  = 5,
    LMAC154_TX_POWER_6dBm  = 6,
    LMAC154_TX_POWER_7dBm  = 7,
    LMAC154_TX_POWER_8dBm  = 8,
    LMAC154_TX_POWER_9dBm  = 9,
    LMAC154_TX_POWER_10dBm = 10,
    LMAC154_TX_POWER_11dBm = 11,
    LMAC154_TX_POWER_12dBm = 12,
    LMAC154_TX_POWER_13dBm = 13,
    LMAC154_TX_POWER_14dBm = 14,
}lmac154_tx_power_t;

typedef enum {
    LMAC154_DATA_RATE_250K = 0,
    LMAC154_DATA_RATE_500K = 1,
    LMAC154_DATA_RATE_1M   = 2,
    LMAC154_DATA_RATE_2M   = 3,
}lmac154_data_rate_t;

typedef enum {
    LMAC154_CCA_MODE_ED        = 0,
    LMAC154_CCA_MODE_CS        = 1,
    LMAC154_CCA_MODE_ED_AND_CS = 2,
    LMAC154_CCA_MODE_ED_OR_CS  = 3,
}lmac154_cca_mode_t;

typedef enum {
    LMAC154_FRAME_TYPE_BEACON = 0x01,
    LMAC154_FRAME_TYPE_DATA   = 0x02,
    LMAC154_FRAME_TYPE_ACK    = 0x04,
    LMAC154_FRAME_TYPE_CMD    = 0x08,
}lmac154_frame_type_t;

typedef enum {
    LMAC154_RF_STATE_RX       = 1,
    LMAC154_RF_STATE_RX_DOING = 2,
    LMAC154_RF_STATE_TX       = 3,
    LMAC154_RF_STATE_IDLE     = 4,
}lmac154_rf_state_t;

typedef enum {
    LMAC154_AES_MODE_ENCRYPT = 0,
    LMAC154_AES_MODE_DECRYPT = 1,
}lmac154_aes_mode_t;

typedef enum {
    LMAC154_AES_MIC_LEN_4  = 0,
    LMAC154_AES_MIC_LEN_8  = 1,
    LMAC154_AES_MIC_LEN_16 = 2,
    LMAC154_AES_MIC_LEN_12 = 3,
}lmac154_aes_mic_len_t;

typedef enum {
    LMAC154_AES_STATUS_SUCCESS         = 0,
    LMAC154_AES_STATUS_INVALID_MODE    = -1,
    LMAC154_AES_STATUS_INVALID_MIC_LEN = -2,
    LMAC154_AES_STATUS_REENTER         = -3,
    LMAC154_AES_STATUS_TIMEOUT         = -4,
}lmac154_aes_status_t;

typedef enum {
    LMAC154_FPT_STATUS_SUCCESS        = 0,
    LMAC154_FPT_STATUS_NO_RESOURCE    = -1,
    LMAC154_FPT_STATUS_ADDR_NOT_FOUND = -2,
}lmac154_fpt_status_t;

typedef enum {
    LMAC154_TX_STATUS_TX_FINISHED = 0,
    LMAC154_TX_STATUS_CSMA_FAILED = 1,
    LMAC154_TX_STATUS_TX_ABORTED  = 2,
    LMAC154_TX_STATUS_HW_ERROR    = 3,
}lmac154_tx_status_t;


/****************************************************************************//**
 * @brief  Initialize the hardware module
 *         Call this function first
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_init(void);


/****************************************************************************//**
 * @brief  Get whether lmac154 is disabled
 *
 * @param  None
 *
 * @return 1 for disabled, 0 for enabled
 *
*******************************************************************************/
uint32_t lmac154_isDisabled(void);


/****************************************************************************//**
 * @brief  Get the lmac154 interrupt handler for interrupt registration of M154_IRQn
 *
 * @param  None
 *
 * @return The lmac154 interrupt handler
 *
*******************************************************************************/
lmac154_isr_t lmac154_getInterruptHandler(void);


/****************************************************************************//**
 * @brief  Get the version of liblmac154.a in string format
 *
 * @param  None
 *
 * @return The version of liblmac154.a
 *
*******************************************************************************/
char * lmac154_getLibVersion(void);


/****************************************************************************//**
 * @brief  Enable standard or enhanced rx promiscuous mode (default disabled)
 *         The MAC sublayer accepts all frames received from the PHY layer
 *         Able to receive frames without any IFS but rx sensitivity may be degraded if enhanced_mode = 1
 *         The MPDU is unavailable in lmac154_rxDoneEvent if ignore_mpdu = 1
 *
 * @param  enhanced_mode: 0: standard mode, 1: enhanced mode
 * @param  ignore_mpdu: 0: read mpdu from registers, 1: do not read mpdu from registers
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableRxPromiscuousMode(uint8_t enhanced_mode, uint8_t ignore_mpdu);


/****************************************************************************//**
 * @brief  Disable standard or enhanced rx promiscuous mode (default disabled)
 *         The MAC sublayer only accepts frames that pass the address filtering
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableRxPromiscuousMode(void);


/****************************************************************************//**
 * @brief  Enable RX (default disabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableRx(void);


/****************************************************************************//**
 * @brief  Disable RX (default disabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableRx(void);


/****************************************************************************//**
 * @brief  Set the number of maximum retransmission times (default 0)
 *         Will perform retransmission in case ack is not received if num > 0
 *         Will raise lmac154_ackEvent(ack_received = 0) after maximum retransmission times
 *
 * @param  num: number of maximum retransmission times
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setTxRetry(uint32_t num);


/****************************************************************************//**
 * @brief  Trigger TX
 *
 * @param  DataPtr: pointer to data buffer
 * @param  length: data length in bytes
 * @param  csma: 0: without CSMA, 1: with CSMA
 *
 * @return None
 *
*******************************************************************************/
void lmac154_triggerTx(uint8_t *DataPtr, uint8_t length, uint8_t csma);


/****************************************************************************//**
 * @brief  Run tx continuous modulation (PRBS15 pattern)
 *         Call lmac154_resetTx to stop
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_runTxCM(void);


/****************************************************************************//**
 * @brief  Run tx continuous wave (single tone)
 *         Call lmac154_resetTx to stop
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_runTxCW(void);


/****************************************************************************//**
 * @brief  Reset tx state machine
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_resetTx(void);


/****************************************************************************//**
 * @brief  Set the channel (default LMAC154_CHANNEL_11)
 *
 * @param  ch_ind: channel index ranging from LMAC154_CHANNEL_11 to LMAC154_CHANNEL_26
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setChannel(lmac154_channel_t ch_ind);


/****************************************************************************//**
 * @brief  Get the channel
 *
 * @param  None
 *
 * @return The channel index ranging from LMAC154_CHANNEL_11 to LMAC154_CHANNEL_26
 *
*******************************************************************************/
lmac154_channel_t lmac154_getChannel(void);


/****************************************************************************//**
 * @brief  Get RSSI in dBm
 *
 * @param  None
 *
 * @return RSSI in dBm
 *
*******************************************************************************/
int lmac154_getRSSI(void);


/****************************************************************************//**
 * @brief  Get LQI
 *
 * @param  None
 *
 * @return LQI
 *
*******************************************************************************/
int lmac154_getLQI(void);

/****************************************************************************//**
 * @brief  Get SFD correlation
 *
 * @param  None
 *
 * @return SFD correlation
 *
*******************************************************************************/
uint8_t lmac154_getSFDCorrelation(void);

/****************************************************************************//**
 * @brief  Get the frequency offset in Hz
 *
 * @param  None
 *
 * @return The frequency offset in Hz
 *
*******************************************************************************/
int lmac154_getFrequencyOffset(void);

/****************************************************************************//**
 * @brief  Set tx power (no default value)
 *
 * @param  power_dbm: tx power ranging from LMAC154_TX_POWER_0dBm to LMAC154_TX_POWER_14dBm
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setTxPower(lmac154_tx_power_t power_dbm);


/****************************************************************************//**
 * @brief  Get tx power
 *
 * @param  None
 *
 * @return tx power ranging from LMAC154_TX_POWER_0dBm to LMAC154_TX_POWER_14dBm
 *
*******************************************************************************/
lmac154_tx_power_t lmac154_getTxPower(void);


/****************************************************************************//**
 * @brief  Set tx data rate (default LMAC154_DATA_RATE_250K)
 *         The maximum tx mpdu length is 127 bytes for 250kbps tx data rate, and 255 bytes for others
 *
 * @param  rate: LMAC154_DATA_RATE_250K, LMAC154_DATA_RATE_500K, LMAC154_DATA_RATE_1M, or LMAC154_DATA_RATE_2M
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setTxDataRate(lmac154_data_rate_t rate);


/****************************************************************************//**
 * @brief  Enable auto rx data rate (default disabled)
 *         The maximum rx mpdu length is 127 bytes for 250kbps rx data rate, and 255 bytes for others
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableAutoRxDataRate(void);


/****************************************************************************//**
 * @brief  Disable auto rx data rate (default disabled)
 *         The rx data rate is 250kbps, and the maximum rx mpdu length is 127 bytes
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableAutoRxDataRate(void);


/****************************************************************************//**
 * @brief  Get rx data rate
 *
 * @param  None
 *
 * @return LMAC154_DATA_RATE_250K, LMAC154_DATA_RATE_500K, LMAC154_DATA_RATE_1M, or LMAC154_DATA_RATE_2M
 *
*******************************************************************************/
lmac154_data_rate_t lmac154_getRxDataRate(void);


/****************************************************************************//**
 * @brief  Set CCA mode (default LMAC154_CCA_MODE_ED_AND_CS)
 *
 * @param  mode: LMAC154_CCA_MODE_ED, LMAC154_CCA_MODE_CS, LMAC154_CCA_MODE_ED_AND_CS, or LMAC154_CCA_MODE_ED_OR_CS
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setCCAMode(lmac154_cca_mode_t mode);


/****************************************************************************//**
 * @brief  Set CCA ED threshold (default -71dBm)
 *
 * @param  threshold: ED threshold ranging from -122 to 5 in dBm
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setEDThreshold(int threshold);

/****************************************************************************//**
 * @brief  Get CCA ED threshold
 *
 * @param  None
 *
 * @return ED threshold
 *
*******************************************************************************/
int lmac154_getEDThreshold(void);

/****************************************************************************//**
 * @brief  Run CCA
 *
 * @param  rssi: rssi output ranging from -122 to 5 in dBm
 *
 * @return CCA result: 0: channel idle, 1: channel busy
 *
*******************************************************************************/
uint8_t lmac154_runCCA(int *rssi);


/****************************************************************************//**
 * @brief  Set PAN ID (default 0x4321)
 *
 * @param  pid: PAN ID
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setPanId(uint16_t pid);

/****************************************************************************//**
 * @brief  Get PAN ID
 *
 * @param  None
 *
 * @return PAN ID
 *
*******************************************************************************/
uint16_t lmac154_getPanId(void);

/****************************************************************************//**
 * @brief  Set 16-bit short address (default 0x0000)
 *
 * @param  sadr: 16-bit short address
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setShortAddr(uint16_t sadr);

/****************************************************************************//**
 * @brief  Get 16-bit short address
 *
 * @param  None 
 *
 * @return 16-bit short address
 *
*******************************************************************************/
uint16_t lmac154_getShortAddr(void);

/****************************************************************************//**
 * @brief  Set 64-bit long address (default 0xACDE4800_00000002)
 *
 * @param  ladr: pointer to 64-bit long address
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setLongAddr(uint8_t *ladr);

/****************************************************************************//**
 * @brief  Get 64-bit long address
 *
 * @param  ladr: pointer to 64-bit long address
 *
 * @return None
 *
*******************************************************************************/
void lmac154_getLongAddr(uint8_t *ladr);

/****************************************************************************//**
 * @brief  Enable frame type filtering (default disabled)
 *         Will only receive specified frame types
 *
 * @param  frame_types: any combination of LMAC154_FRAME_TYPE_XXX (BEACON, DATA, ACK, or CMD), via bitwise OR ('|') operation
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableFrameTypeFiltering(uint8_t frame_types);


/****************************************************************************//**
 * @brief  Disable frame type filtering (default disabled)
 *         Will receive all valid frame types (BEACON, DATA, ACK, or CMD)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableFrameTypeFiltering(void);


/****************************************************************************//**
 * @brief  Enable crc filtering (default enabled)
 *         Will only receive frames that pass crc check
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableCrcFiltering(void);


/****************************************************************************//**
 * @brief  Disable crc filtering (default enabled)
 *         Will receive all frames ignoring crc check
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableCrcFiltering(void);


/****************************************************************************//**
 * @brief  Set external PA
 *         Restriction: txpin % 5 != rxpin % 5
 *
 * @param  txpin: tx pin ranging from 0 to 31
 * @param  rxpin: rx pin ranging from 0 to 31
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setExtPA(uint8_t txpin, uint8_t rxpin);


/****************************************************************************//**
 * @brief  Get RF state
 *
 * @param  None
 *
 * @return LMAC154_RF_STATE_RX, LMAC154_RF_STATE_RX_DOING, LMAC154_RF_STATE_TX, or LMAC154_RF_STATE_IDLE
 *
*******************************************************************************/
lmac154_rf_state_t lmac154_getRFState(void);


/****************************************************************************//**
 * @brief  Enable the coexistence of zigbee and other wireless modules (default disabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableCoex(void);


/****************************************************************************//**
 * @brief  Disable the coexistence of zigbee and other wireless modules (default disabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableCoex(void);


/****************************************************************************//**
 * @brief  Enable auto transmission of ack frame by hardware (default enabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableHwAutoTxAck(void);


/****************************************************************************//**
 * @brief  Disable auto transmission of ack frame by hardware (default enabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableHwAutoTxAck(void);


/****************************************************************************//**
 * @brief  Enable lmac154_ackEvent (default enabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableAckEvent(void);


/****************************************************************************//**
 * @brief  Disable lmac154_ackEvent (default enabled)
 *         Will raise lmac154_rxDoneEvent instead when ack is received
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableAckEvent(void);


/****************************************************************************//**
 * @brief  Enable lmac154_rxStartEvent (default disabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enableRxStartEvent(void);


/****************************************************************************//**
 * @brief  Disable lmac154_rxStartEvent (default disabled)
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disableRxStartEvent(void);

/****************************************************************************//**
 * @brief  Get whether rx promiscuous mode is enabled
 *
 * @param  None
 *
 * @return 1 for enabled; 0 for disabled
 *
*******************************************************************************/
uint32_t lmac154_isRxPromiscuousModeEnabled(void);

/****************************************************************************//**
 * @brief  Get the receiving or received mpdu length in bytes (crc included)
 *
 * @param  None
 *
 * @return The receiving or received mpdu length
 *
*******************************************************************************/
uint8_t lmac154_getRxLength(void);


/****************************************************************************//**
 * @brief  Read rx data (crc not included)
 *
 * @param  buf: pointer to data buffer
 * @param  offset: data offset ranging from 0 to 252 in bytes
 * @param  len: data length ranging from 1 to 253-offset in bytes
 *
 * @return None
 *
*******************************************************************************/
void lmac154_readRxData(uint8_t *buf, uint8_t offset, uint8_t len);


/****************************************************************************//**
 * @brief  Read rx crc
 *
 * @param  crc: byte array to carry rx crc
 *
 * @return None
 *
*******************************************************************************/
void lmac154_readRxCrc(uint8_t crc[2]);


/****************************************************************************//**
 * @brief  Set the maximum wait time for ack frame (default 864us)
 *
 * @param  time_us: maximum wait time ranging from 1 to 1023 in us
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setAckWaitTime(uint16_t time_us);


/****************************************************************************//**
 * @brief  Set the maximum and minimum CSMA-CA backoff exponent
 *
 * @param  max_be: maximum BE ranging from 3 to 8, default 5
 * @param  min_be: minimum BE ranging from 0 to max_be, default 3
 *
 * @return None
 *
*******************************************************************************/
void lmac154_setBE(uint8_t max_be, uint8_t min_be);


/****************************************************************************//**
 * @brief  Run AES CCM
 *
 * @param  mode: LMAC154_AES_MODE_ENCRYPT or LMAC154_AES_MODE_DECRYPT
 * @param  a_data: pointer to additional authentication data buffer
 * @param  a_len: length of additional authentication data in bytes
 * @param  nonce: pointer to 13-byte nonce buffer
 * @param  key: pointer to 4-word key buffer
 * @param  mic_len: LMAC154_MIC_LEN_4, LMAC154_MIC_LEN_8, LMAC154_MIC_LEN_12, or LMAC154_MIC_LEN_16
 * @param  mic: pointer to mic buffer
 * @param  input_data: pointer to input data buffer
 * @param  output_data: pointer to output data buffer
 * @param  len: length of input data in bytes
 *
 * @return The error code
 *
*******************************************************************************/
lmac154_aes_status_t lmac154_runAESCCM(lmac154_aes_mode_t mode, 
                                       const uint8_t *a_data, 
                                       uint8_t a_len, 
                                       const uint8_t *nonce, 
                                       const uint32_t *key, 
                                       lmac154_aes_mic_len_t mic_len, 
                                       uint32_t *mic, 
                                       const uint32_t *input_data, 
                                       uint32_t *output_data, 
                                       uint8_t len);




// hardware frame pending table (256 bytes)
// shared by short addresses and long addresses (supports up to 128 short addresses or 32 long addresses or mixed)
// used by hardware to set the frame pending bit of hw auto tx ack
// if the address is not found in the table (e.g. not set or removed), the frame pending bit of hw auto tx ack is 1


/****************************************************************************//**
 * @brief  Add (or update) the key-value pair {sadr: pending} to (in) the frame pending table
 *
 * @param  sadr: 16-bit short address
 * @param  pending: value of the corresponding frame pending bit
 *
 * @return LMAC154_FPT_STATUS_SUCCESS or LMAC154_FPT_STATUS_NO_RESOURCE
 *
*******************************************************************************/
lmac154_fpt_status_t lmac154_fptSetShortAddrPending(uint16_t sadr, uint8_t pending);


/****************************************************************************//**
 * @brief  Add (or update) the key-value pair {ladr: pending} to (in) the frame pending table
 *
 * @param  ladr: pointer to 64-bit long address
 * @param  pending: value of the corresponding frame pending bit
 *
 * @return LMAC154_FPT_STATUS_SUCCESS or LMAC154_FPT_STATUS_NO_RESOURCE
 *
*******************************************************************************/
lmac154_fpt_status_t lmac154_fptSetLongAddrPending(uint8_t *ladr, uint8_t pending);


/****************************************************************************//**
 * @brief  Get the corresponding frame pending bit of the short address in the frame pending table
 *
 * @param  sadr: 16-bit short address
 * @param  pending: value of the corresponding frame pending bit
 *
 * @return LMAC154_FPT_STATUS_SUCCESS or LMAC154_FPT_STATUS_ADDR_NOT_FOUND
 *
*******************************************************************************/
lmac154_fpt_status_t lmac154_fptGetShortAddrPending(uint16_t sadr, uint8_t *pending);


/****************************************************************************//**
 * @brief  Get the corresponding frame pending bit of the long address in the frame pending table
 *
 * @param  ladr: pointer to 64-bit long address
 * @param  pending: value of the corresponding frame pending bit
 *
 * @return LMAC154_FPT_STATUS_SUCCESS or LMAC154_FPT_STATUS_ADDR_NOT_FOUND
 *
*******************************************************************************/
lmac154_fpt_status_t lmac154_fptGetLongAddrPending(uint8_t *ladr, uint8_t *pending);


/****************************************************************************//**
 * @brief  Remove the key-value pair {sadr: pending} from the frame pending table
 *
 * @param  sadr: 16-bit short address
 *
 * @return LMAC154_FPT_STATUS_SUCCESS or LMAC154_FPT_STATUS_ADDR_NOT_FOUND
 *
*******************************************************************************/
lmac154_fpt_status_t lmac154_fptRemoveShortAddr(uint16_t sadr);


/****************************************************************************//**
 * @brief  Remove the key-value pair {ladr: pending} from the frame pending table
 *
 * @param  ladr: pointer to 64-bit long address
 *
 * @return LMAC154_FPT_STATUS_SUCCESS or LMAC154_FPT_STATUS_ADDR_NOT_FOUND
 *
*******************************************************************************/
lmac154_fpt_status_t lmac154_fptRemoveLongAddr(uint8_t *ladr);


/****************************************************************************//**
 * @brief  Get short address list in the frame pending table
 *
 * @param  list: pointer to list buffer
 * @param  entry_num: number of entries
 *
 * @return None
 *
*******************************************************************************/
void lmac154_fpt_GetShortAddrList(void *list, uint8_t *entry_num);


/****************************************************************************//**
 * @brief  Get long address list in the frame pending table
 *
 * @param  list: pointer to list buffer
 * @param  entry_num: number of entries
 *
 * @return None
 *
*******************************************************************************/
void lmac154_fptGetLongAddrList(void *list, uint8_t *entry_num);


/****************************************************************************//**
 * @brief  Set frame pending bit according to frame pending table, or force to set to 1
 *
 * @param  force: 0: set according to frame pending table (default), 1: force to set to 1
 *
 * @return None
 *
*******************************************************************************/
void lmac154_fptForcePending(uint8_t force);


/****************************************************************************//**
 * @brief  Clear the frame pending table
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_fptClear(void);


/****************************************************************************//**
 * @brief  Print the frame pending table using specified print function
 *
 * @param  print_func: user specified print function
 *
 * @return None
 *
*******************************************************************************/
void lmac154_fptDump(int print_func(const char *fmt, ...));




// functions below are callback functions running in the interrupt context
// should be implemented by user if needed


/****************************************************************************//**
 * @brief  Tx Done Event
 *         Will be raised after calling lmac154_triggerTx
 *
 * @param  tx_status: LMAC154_TX_STATUS_XXX (TX_FINISHED, CSMA_FAILED, TX_ABORTED, or HW_ERROR)
 *
 * @return None
 *
*******************************************************************************/
void lmac154_txDoneEvent(lmac154_tx_status_t tx_status);


/****************************************************************************//**
 * @brief  Ack Event
 *         Will be raised after calling lmac154_triggerTx(AR bit = 1)
 *
 * @param  ack_received: 0: no ack, 1: ack received
 * @param  frame_pending: frame pending bit in ack frame
 * @param  seq_num: sequence number in ack frame
 *
 * @return None
 *
*******************************************************************************/
void lmac154_ackEvent(uint8_t ack_received, uint8_t frame_pending, uint8_t seq_num);

/****************************************************************************//**
 * @brief  Ack Event with ack frame
 *         Will be raised after calling lmac154_triggerTx(AR bit = 1)
 *
 * @param  ack_received: 0: no ack, 1: ack received
 * @param  rx_buf: pointer to the ack frame buffer
 * @param  len: ACK frame length
 *
 * @return None
 *
*******************************************************************************/
void lmac154_ackFrameEvent(uint8_t ack_received, uint8_t *rx_buf, uint8_t len);

/****************************************************************************//**
 * @brief  Rx Done Event
 *         Will be raised when a frame is received
 *
 * @param  rx_buf: pointer to rx buffer
 * @param  rx_len: received mpdu length in bytes (crc included)
 * @param  crc_fail: 0: crc pass, 1: crc fail
 *
 * @return None
 *
*******************************************************************************/
void lmac154_rxDoneEvent(uint8_t *rx_buf, uint8_t rx_len, uint8_t crc_fail);


/****************************************************************************//**
 * @brief  Rx Start Event
 *         Will be raised when rx on MAC sublayer starts
 *         lmac154_getRxLength is available because it is obtained from PHY layer
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_rxStartEvent(void);

/****************************************************************************//**
 * @brief  Hardware auto tx ack done event
 *        
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void lmac154_hwAutoTxAckDoneEvent(void);

/****************************************************************************//**
 * @brief  Enable second stack for dual stack
 *
 * @return None
 *
*******************************************************************************/
void lmac154_enable2ndStack(void);

/****************************************************************************//**
 * @brief  Disable second stack for dual stack, which is disable by default
 *
 * @return None
 *
*******************************************************************************/
void lmac154_disable2ndStack(void);

/****************************************************************************//**
 * @brief  Check whether the second stack is enabled
 *
 * @return None
 *
*******************************************************************************/
uint32_t lmac154_is2ndStackEnabled(void);

/****************************************************************************//**
 * @brief  Set second PAN ID (default 0x4321) for dual stack
 *
 * @param  pid: PAN ID
 *
 * @return None
 *
*******************************************************************************/
void lmac154_set2ndPanId(uint16_t pid);

/****************************************************************************//**
 * @brief  Set second 16-bit short address (default 0x0000)  for dual stack
 *
 * @param  sadr: 16-bit short address
 *
 * @return None
 *
*******************************************************************************/
void lmac154_set2ndShortAddr(uint16_t sadr);

/****************************************************************************//**
 * @brief  Set second 64-bit long address (default 0xACDE4800_00000002) 
 *         for dual stack
 *
 * @param  ladr: pointer to 64-bit long address
 *
 * @return None
 *
*******************************************************************************/
void lmac154_set2ndLongAddr(uint8_t *ladr);

#endif
