/**
 ****************************************************************************************
 *
 * @file app.c
 *
 * @brief Bouffalo Lab Ble Demo implementation
 *
 * Copyright (C) Bouffalo Lab 2018
 *
 * History: 2020-12-31 crealted by caofp 
 *
 ****************************************************************************************
 */
#include "config.h"
#include "cli.h"
#include "oad_client.h"
#include "log.h"

/*The header is 15bytes*/
#define TRANSPORT_FILE_SIZE (244 - 15)
#define MAX_ATT_SIZE        (244)
extern struct bt_conn *default_conn;

u16_t in_handle = 0;
u16_t out_handle = 0;

void oad_send_image_identity_to_servicer(struct bt_conn *conn, u8_t *data, u16_t len);
void oad_send_block_resp_to_servicer(struct bt_conn *conn, u8_t *data, u8_t len);
static void oad_start_identity(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void oad_discovery(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void oad_subscribe(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void oad_start_update(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);

const struct cli_command oadCmdSet[] STATIC_CLI_CMD_ATTRIBUTE = {
    { "oad_start_indentity", "", oad_start_identity },
    { "oad_discovery", "", oad_discovery },
    { "oad_subscribe", "", oad_subscribe },
    { "oad_start_update", "", oad_start_update },
};

void oad_send_data_to_uart(uint8_t cmd_type, uint8_t *data, uint8_t len)
{
    uint8_t final_data[128];

    if (len + 2 > 128) {
        return;
    }
    memset(final_data, 0, 128);

    final_data[0] = cmd_type;
    final_data[1] = len;
    memcpy(&final_data[2], data, len);

    BT_WARN("cmd_data:%s\r\n", bt_hex(final_data, len + 2));
}

void oad_start_to_uart(void)
{
    uint8_t req[1];
    req[0] = OAD_CMDPROC_START;

    oad_send_data_to_uart(CMDPROC_TYPE_OAD, req, sizeof(req));
}

void oad_image_identity_to_uart(struct oad_file_info *file_info)
{
    struct oad_cmdproc_req_t req;
    req.cmd_id = OAD_CMDPROC_IMAGE_IDENTITY;
    memcpy(&req.q.file_info, file_info, sizeof(*file_info));
    oad_send_data_to_uart(CMDPROC_TYPE_OAD, (uint8_t *)&req, sizeof(req));
}

void oad_block_req_to_uart(struct oad_file_info *file_info, uint32_t file_offset, uint8_t data_len)
{
    struct oad_cmdproc_req_t req;

    req.cmd_id = OAD_CMDPROC_BLOCK_REQ;
    memcpy(&req.q.block_req.file_info, file_info, sizeof(*file_info));
    req.q.block_req.file_offset = file_offset;
    req.q.block_req.data_len = data_len;
    oad_send_data_to_uart(CMDPROC_TYPE_OAD, (uint8_t *)&req, sizeof(req));
}

void oad_upgrd_end_to_uart(uint8_t status, struct oad_file_info *file_info)
{
    struct oad_cmdproc_req_t req;

    req.cmd_id = OAD_CMDPROC_UPGRD_END;
    req.q.upgrd_end.status = status;
    memcpy(&req.q.upgrd_end.file_info, file_info, sizeof(*file_info));

    oad_send_data_to_uart(CMDPROC_TYPE_OAD, (uint8_t *)&req, sizeof(req));
}

static void oad_start_identity(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t buf[50];
    int len = 0;

    if (!default_conn)
        return;

    memset(buf, 0, 50);
    get_bytearray_from_string(&argv[1], buf, 50);
    len = buf[1];
    if (len + 2 >= 50) {
        BT_WARN("Failed to receved data\r\n");
        return;
    }

    oad_send_image_identity_to_servicer(default_conn, buf, len + 2);
}

static void oad_start_update(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    uint8_t buf[MAX_ATT_SIZE];

    if (!default_conn)
        return;

    memset(buf, 0, MAX_ATT_SIZE);
    get_bytearray_from_string(&argv[1], buf, MAX_ATT_SIZE);

    oad_send_block_resp_to_servicer(default_conn, buf, MAX_ATT_SIZE);
}

static struct bt_gatt_discover_params discover_params;

static u8_t oad_discover_func(struct bt_conn *conn, const struct bt_gatt_attr *attr, struct bt_gatt_discover_params *params)
{
    struct bt_gatt_service_val *gatt_service;
    struct bt_gatt_chrc *gatt_chrc;
    struct bt_gatt_include *gatt_include;
    struct bt_uuid *uuid_out = BT_UUID_OAD_DATA_OUT;
    struct bt_uuid *uuid_in = BT_UUID_OAD_DATA_IN;
    char str[37];

    if (!attr) {
        BT_WARN("Discover complete\r\n");
        BT_WARN("in handle = (0x%x) out_handle = (0x%x)\r\n", in_handle, out_handle);
        (void)memset(params, 0, sizeof(*params));
        return BT_GATT_ITER_STOP;
    }

    switch (params->type) {
        case BT_GATT_DISCOVER_SECONDARY:
        case BT_GATT_DISCOVER_PRIMARY:
            gatt_service = attr->user_data;
            bt_uuid_to_str(gatt_service->uuid, str, sizeof(str));
            BT_WARN("Service %s found: attr handle %x, end_handle %x\r\n", str, attr->handle, gatt_service->end_handle);

            break;
        case BT_GATT_DISCOVER_CHARACTERISTIC:
            gatt_chrc = attr->user_data;
            bt_uuid_to_str(gatt_chrc->uuid, str, sizeof(str));
            BT_WARN("Characteristic %s found: attr->handle %x  chrcval->handle %x \r\n", str, attr->handle, gatt_chrc->value_handle);
            //print_chrc_props(gatt_chrc->properties);
            break;
        case BT_GATT_DISCOVER_INCLUDE:
            gatt_include = attr->user_data;
            bt_uuid_to_str(gatt_include->uuid, str, sizeof(str));
            BT_WARN("Include %s found: attr handle %x, start %x, end %x\r\n", str, attr->handle,
                    gatt_include->start_handle, gatt_include->end_handle);
            break;
        default:
            bt_uuid_to_str(attr->uuid, str, sizeof(str));

            if (!bt_uuid_cmp(attr->uuid, uuid_in)) {
                in_handle = attr->handle;
            } else if (!bt_uuid_cmp(attr->uuid, uuid_out)) {
                out_handle = attr->handle;
            }

            BT_WARN("Descriptor %s found: handle %x\r\n", str, attr->handle);
            break;
    }

    return BT_GATT_ITER_CONTINUE;
}

static void oad_discovery(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int err;
    u8_t disc_type;

    if (!default_conn) {
        BT_WARN("Not connected\r\n");
        return;
    }

    discover_params.func = oad_discover_func;
    discover_params.start_handle = 0x1;
    discover_params.end_handle = 0xffff;
    discover_params.uuid = NULL;

    get_uint8_from_string(&argv[1], &disc_type);
    BT_WARN("disc_type = [%d]\r\n", disc_type);
    if (disc_type == 0) {
        discover_params.type = BT_GATT_DISCOVER_PRIMARY;
    } else if (disc_type == 1) {
        discover_params.type = BT_GATT_DISCOVER_SECONDARY;
    } else if (disc_type == 2) {
        discover_params.type = BT_GATT_DISCOVER_INCLUDE;
    } else if (disc_type == 3) {
        discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;
    } else if (disc_type == 4) {
        discover_params.type = BT_GATT_DISCOVER_DESCRIPTOR;
    } else {
        BT_WARN("Invalid discovery type\r\n");
        return;
    }
    //discover_params.type = BT_GATT_DISCOVER_DESCRIPTOR;
    err = bt_gatt_discover(default_conn, &discover_params);
    if (err) {
        BT_WARN("Discover failed (err %d)\r\n", err);
    } else {
        BT_WARN("Discover pending\r\n");
    }
}

static struct bt_gatt_subscribe_params subscribe_params;

static u8_t oad_client_recv_data(struct bt_conn *conn,
                                 struct bt_gatt_subscribe_params *params,
                                 const void *data, u16_t length)
{
    u8_t *pdata = data;

    if (!params->value) {
        BT_WARN("Unsubscribed\r\n");
        params->value_handle = 0U;
        return BT_GATT_ITER_STOP;
    }
    BT_WARN("oad_client_recv_data:%s\r\n", bt_hex(pdata, length));
    oad_client_notify_handler(data, length);

    return BT_GATT_ITER_CONTINUE;
}

static void oad_subscribe(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int err;
    if (!default_conn) {
        BT_WARN("Not connected\r\n");
        return;
    }

    subscribe_params.ccc_handle = out_handle + 1;
    subscribe_params.value_handle = out_handle;
    subscribe_params.value = 0x01;

    subscribe_params.notify = oad_client_recv_data;

    err = bt_gatt_subscribe(default_conn, &subscribe_params);
    if (err) {
        BT_WARN("Subscribe failed (err %d)\r\n", err);
    } else {
        BT_WARN("Subscribed\r\n");
    }
}

void oad_client_notify_handler(void *buf, u16_t len)
{
    u8_t *pdata = (u8_t *)buf;
    u8_t cmd_id;
    struct oad_image_identity_t identity;
    struct oad_block_req_t req;
    struct oad_upgrd_end_t upgrd_end;

    if (!buf) {
        BT_WARN("BUF is NULL\r\n");
        return;
    }

    cmd_id = pdata[0];
    switch (cmd_id) {
        case OAD_CMD_IMAG_IDENTITY:
            if (len == sizeof(struct oad_image_identity_t) + 1) {
                memset(&identity, 0, sizeof(struct oad_image_identity_t));
                memcpy(&identity, buf + 1, sizeof(struct oad_image_identity_t));
                BT_WARN("manu_code = [0x%x] file ver = [0x%x]\r\n", identity.file_info.manu_code, identity.file_info.file_ver);
                oad_image_identity_to_uart(&(identity.file_info));
            } else {
                BT_WARN("Length (%d) sizeof (%d) errors\r\n", len, sizeof(struct oad_image_identity_t));
            }
            break;

        case OAD_CMD_IMAG_BLOCK_REQ:
            if (len == sizeof(struct oad_block_req_t) + 1) {
                memset(&req, 0, sizeof(struct oad_block_req_t));
                memcpy(&req, buf + 1, sizeof(struct oad_block_req_t));
                BT_WARN("OAD_CMD_IMAG_BLOCK_REQ: manu_code = [0x%x] file ver = [0x%x]\r\n", req.file_info.manu_code, req.file_info.file_ver);
                oad_block_req_to_uart(&(req.file_info), req.file_offset, TRANSPORT_FILE_SIZE);
            } else {
                BT_WARN("Length (%d) sizeof (%d) errors\r\n", len, sizeof(struct oad_block_req_t));
            }
            break;

        case OAD_CMD_IMAG_UPGRD_END:
            if (len == sizeof(struct oad_upgrd_end_t) + 1) {
                memset(&upgrd_end, 0, sizeof(struct oad_upgrd_end_t));
                memcpy(&upgrd_end, buf + 1, sizeof(struct oad_upgrd_end_t));
                oad_upgrd_end_to_uart(upgrd_end.status, &(upgrd_end.file_info));
            } else {
                BT_WARN("Length (%d) sizeof (%d) errors\r\n", len, sizeof(struct oad_upgrd_end_t));
            }
            break;

        default:
            break;
    }
}

void oad_send_image_identity_to_servicer(struct bt_conn *conn, u8_t *data, u16_t len)
{
    u16_t handle = in_handle;

    if (!conn)
        return;

    data[0] = OAD_CMD_IMAG_IDENTITY;
    bt_gatt_write_without_response(conn, handle, data, len, false);
}

void oad_send_block_resp_to_servicer(struct bt_conn *conn, u8_t *data, u8_t len)
{
    u16_t handle = in_handle;
    u8_t slen = 0;

    if (!conn)
        return;

    data[0] = OAD_CMD_IMAG_BLOCK_RESP;
    if (len <= bt_gatt_get_mtu(conn))
        slen = len;
    else {
        slen = bt_gatt_get_mtu(conn);
    }

    bt_gatt_write_without_response(conn, handle, data, slen, false);
}

void oad_cli_register(void)
{
    return;
}
