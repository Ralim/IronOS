#ifndef _FS2711_DEFINE_HPP_
#define _FS2711_DEFINE_HPP_

#define FS2711_WRITE_ADDR 0x5A
#define FS2711_READ_ADDR  0x5B

#define FS2711_ADDR 0x5A

#define FS2711_PDO_FIX 0
#define FS2711_PDO_PPS 1

#define FS2711_MAX_5V  1
#define FS2711_MAX_9V  3
#define FS2711_MAX_12V 7
#define FS2711_MAX_20V 15

#define FS2711_ENABLE  0x1
#define FS2711_DISABLE 0x2

// Protocol Selection
#define FS2711_PROTOCOL_QC2A 4
#define FS2711_PROTOCOL_QC2B 5
#define FS2711_PROTOCOL_QC3A 6
#define FS2711_PROTOCOL_QC3B 7
#define FS2711_PROTOCOL_PPS  20
#define FS2711_PROTOCOL_PD   21

#define FS2711_PROTOCOL_PPS_CURRENT 0xDE

#define FS2711_PROTOCOL_QC_MAX_VOLT 0xC0

#define FS2711_REG_SCAN_START      0x40 // Protocol Scan
#define FS2711_REG_ENABLE_PROTOCOL 0x41 // Enable Protocol
#define FS2711_REG_SELECT_PROTOCOL 0x42 // Select Protocol
#define FS2711_REG_ENABLE_VOLTAGE  0x43 // Enable Voltage
#define FS2711_REG_PDO_IDX         0x46 // Requests Protocol Index
#define FS2711_REG_SWEEP           0x47 // Requests a voltage sweep?
#define FS2711_REG_PORT_RESET      0x49 // Port Reset
#define FS2711_REG_SYSTEM_RESET    0x4A // System Reset
#define FS2711_REG_DPDM            0x51 // DPDM
#define FS2711_REG_MODE_SET        0xA0 // Mode set
#define FS2711_REG_STATE0          0xB1 // PD:A_SNK PD:A_SRC (PD:pe_ready POM:crc_success) (PD:soft_reset POM:crc_fail) (PD:hard_rest POM:resp_fail) PD:hardreset_found VIVO
#define FS2711_REG_STATE1          0xB2 // scan_done pdo_updated vooc_recv_cmd vivo_pom_tx_finish vivo_pom_rx_finish huawei_comm_fail huawei_op_finish

// Used to calculate PDO Objects
#define FS2711_REG_PDO_B0 0xC0
#define FS2711_REG_PDO_B1 0xC1
#define FS2711_REG_PDO_B2 0xC2
#define FS2711_REG_PDO_B3 0xC3

#define FS2711_REG_VOLT_CFG_B0 0xF4
#define FS2711_REG_VOLT_CFG_B1 0xF5
#define FS2711_REG_VOLT_CFG_B2 0xF6
#define FS2711_REG_VOLT_CFG_B3 0xF7

// 0xF0 ~ 0xF1 16 bits
#define FS2711_REG_MASK 0xF0
// 0xF4 ~ 0xF7 32 bits
#define FS2711_REG_PROTOCOL_VOLT 0xF4
// 0xF8 ~ 0xFB 24 bits
#define FS2711_REG_PROTOCOL_EXISTS 0xF8

#define FS2711_SWEEP_SAW  0
#define FS2711_SWEEP_TRI  1
#define FS2711_SWEEP_STEP 2

#define FS2711_STATE_SCAN_DONE   0x01
#define FS2711_STATE_PDO_UPDATE  0x02
#define FS2711_STATE_PD_SNK      0x40
#define FS2711_STATE_PD_SRC      0x80
#define FS2711_STATE_PD_PE_READY 0x100
#define FS2711_STATE_DISABLE     0x800
#endif
