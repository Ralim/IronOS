#ifndef HCI_ONCHIP_H_
#define HCI_ONCHIP_H_

enum {
    BT_HCI_CMD,
    BT_HCI_ACL_DATA,
    BT_HCI_CMD_CMP_EVT,
    BT_HCI_CMD_STAT_EVT,
    BT_HCI_LE_EVT,
    BT_HCI_EVT
};

typedef struct {
    uint16_t opcode;
    uint8_t *params;
    uint8_t param_len;
} bl_hci_cmd_struct;

typedef struct {
    /// connection handle
    uint16_t conhdl;
    /// broadcast and packet boundary flag
    uint8_t pb_bc_flag;
    /// length of the data
    uint16_t len;
    uint8_t *buffer;
} bl_hci_acl_data_tx;

typedef struct {
    union {
        bl_hci_cmd_struct hci_cmd;
        bl_hci_acl_data_tx acl_data;
    } p;
} hci_pkt_struct;

typedef void (*bt_hci_recv_cb)(uint8_t pkt_type, uint16_t src_id, uint8_t *param, uint8_t param_len);

uint8_t bt_onchiphci_interface_init(bt_hci_recv_cb cb);
int8_t bt_onchiphci_send(uint8_t pkt_type, uint16_t dest_id, hci_pkt_struct *pkt);
uint8_t bt_onchiphci_hanlde_rx_acl(void *param, uint8_t *host_buf_data);

#endif
