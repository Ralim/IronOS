#ifndef __BLE_OAD_H__
#define __BLE_OAD_H__
#include "gatt.h"
#include "Conn_internal.h"
#include "oad.h"

enum {
    OAD_CMDPROC_START = 0x00,
    OAD_CMDPROC_IMAGE_IDENTITY,
    OAD_CMDPROC_BLOCK_REQ,
    OAD_CMDPROC_BLOCK_RESP,
    OAD_CMDPROC_UPGRD_END,
};

struct oad_cmdproc_block_req_t {
    struct oad_file_info file_info;
    uint32_t file_offset;
    uint8_t data_len;
} __packed;

struct oad_cmdproc_upgrd_end_t {
    uint8_t status;
    struct oad_file_info file_info;
} __packed;

struct oad_cmdproc_req_t {
    uint8_t cmd_id;
    union {
        struct oad_file_info file_info;
        struct oad_cmdproc_block_req_t block_req;
        struct oad_cmdproc_upgrd_end_t upgrd_end;
    } q;
} __packed;

enum {
    CMDPROC_TYPE_OAD = 0x00,
    CMDPROC_TYPE_MAX
};

void oad_client_notify_handler(void *buf, u16_t len);
void oad_send_image_identity_to_servicer(struct bt_conn *conn, u8_t *data, u16_t len);
void oad_send_block_resp_to_servicer(struct bt_conn *conn, u8_t *data, u8_t len);
void oad_cli_register(void);

#endif
