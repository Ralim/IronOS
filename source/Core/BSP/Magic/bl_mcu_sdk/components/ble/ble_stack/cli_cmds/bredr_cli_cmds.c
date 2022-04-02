/** @file
 * @brief Bluetooth BR/EDR shell module
 *
 * Provide some BR/EDR commands that can be useful to applications.
 */

#include <stdlib.h>
#include <string.h>
#include <byteorder.h>
#include <bluetooth.h>
#include <hci_host.h>
#include <conn.h>
#include <l2cap.h>

#include "cli.h"

#if PCM_PRINTF
#include "oi_codec_sbc.h"
#include "a2dp.h"
#endif

static void bredr_connected(struct bt_conn *conn, u8_t err);
static void bredr_disconnected(struct bt_conn *conn, u8_t reason);

static bool init = false;
static struct bt_conn_info conn_info;
static struct bt_conn *default_conn = NULL;

static struct bt_conn_cb conn_callbacks = {
    .connected = bredr_connected,
    .disconnected = bredr_disconnected,
};

#if PCM_PRINTF
static void pcm(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
#endif
static void bredr_init(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void bredr_write_eir(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void bredr_discoverable(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void bredr_connectable(char *p_write_buffer, int write_buffer_len, int argc, char **argv);

const struct cli_command bredr_cmd_set[] STATIC_CLI_CMD_ATTRIBUTE = {
#if PCM_PRINTF
    { "pcm", "", pcm },
#endif
    { "bredr_init", "", bredr_init },
    { "bredr_eir", "", bredr_write_eir },
    { "bredr_connectable", "", bredr_connectable },
    { "bredr_discoverable", "", bredr_discoverable },

};

#if PCM_PRINTF
extern OI_BYTE sbc_frame[];
extern OI_UINT32 sbc_frame_len;
OI_INT16 pcm_data[1024];
OI_UINT32 pcm_bytes = sizeof(pcm_data);
OI_INT16 cool_edit[600000];
OI_UINT32 byte_index = 0;
static void pcm(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    //a2dp_sbc_decode_init();
    //a2dp_sbc_decode_process(sbc_frame, sbc_frame_len);

    printf("pcm data count: %d \n", byte_index);

    OI_UINT32 samps = byte_index / sizeof(OI_INT16);

    printf("SAMPLES:    %d\n", samps);
    printf("BITSPERSAMPLE:  16\n");
    printf("CHANNELS:   2\n");
    printf("SAMPLERATE: 44100\n");
    printf("NORMALIZED: FALSE\n");

    for (int i = 0; i < samps; i++) {
        printf("%d\n", cool_edit[i]);
    }
}
#endif

static void bredr_init(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (init) {
        printf("bredr has initialized\n");
        return;
    }

    default_conn = NULL;
    bt_conn_cb_register(&conn_callbacks);

    init = true;
    printf("bredr init successfully\n");
}

static void bredr_connected(struct bt_conn *conn, u8_t err)
{
    char addr[BT_ADDR_STR_LEN];

    bt_conn_get_info(conn, &conn_info);
    bt_addr_to_str(conn_info.br.dst, addr, sizeof(addr));

    if (err) {
        printf("bredr failed to connect to %s (%u) \r\n", addr, err);
        return;
    }

    printf("bredr connected: %s \r\n", addr);

    if (!default_conn) {
        default_conn = conn;
    }
}

static void bredr_disconnected(struct bt_conn *conn, u8_t reason)
{
    char addr[BT_ADDR_STR_LEN];

    bt_conn_get_info(conn, &conn_info);
    bt_addr_to_str(conn_info.br.dst, addr, sizeof(addr));

    printf("bredr disconnected: %s (reason %u) \r\n", addr, reason);

    if (default_conn == conn) {
        default_conn = NULL;
    }
}

static void bredr_write_eir(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;
    char *name = "Bouffalolab-bl606p-classic-bt";
    uint8_t rec = 1;
    uint8_t data[32] = { 0 };

    data[0] = 30;
    data[1] = 0x09;
    memcpy(data + 2, name, strlen(name));

    for (int i = 0; i < strlen(name); i++) {
        printf("0x%02x ", data[2 + i]);
    }
    printf("\n");

    err = bt_br_write_eir(rec, data);
    if (err) {
        printf("BR/EDR write EIR failed, (err %d)\n", err);
    } else {
        printf("BR/EDR write EIR done.\n");
    }
}

static void bredr_discoverable(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;
    uint8_t action;

    if (argc != 2) {
        printf("Number of parameters is not correct\n");
        return;
    }

    get_uint8_from_string(&argv[1], &action);

    if (action == 1) {
        err = bt_br_set_discoverable(true);
    } else if (action == 0) {
        err = bt_br_set_discoverable(false);
    } else {
        printf("Arg1 is invalid\n");
        return;
    }

    if (err) {
        printf("BR/EDR set discoverable failed, (err %d)\n", err);
    } else {
        printf("BR/EDR set discoverable done.\n");
    }
}

static void bredr_connectable(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;
    uint8_t action;

    if (argc != 2) {
        printf("Number of parameters is not correct\n");
        return;
    }

    get_uint8_from_string(&argv[1], &action);

    if (action == 1) {
        err = bt_br_set_connectable(true);
    } else if (action == 0) {
        err = bt_br_set_connectable(false);
    } else {
        printf("Arg1 is invalid\n");
        return;
    }

    if (err) {
        printf("BR/EDR set connectable failed, (err %d)\n", err);
    } else {
        printf("BR/EDR set connectable done.\n");
    }
}

int bredr_cli_register(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //aos_cli_register_commands(bredr_cmd_set, sizeof(bredr_cmd_set)/sizeof(bredr_cmd_set[0]));
    return 0;
}
