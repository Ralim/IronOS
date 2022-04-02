#include "byteorder.h"
#include "oad_service.h"
#include "oad.h"
#include "log.h"
#include "hci_core.h"
#include "conn_internal.h"

oad_upper_recv_cb upper_recv_cb;
oad_disc_cb disc_cb;
struct bt_conn *ble_oad_conn = NULL;

static void ble_oad_connected(struct bt_conn *conn, u8_t err);
static void ble_oad_disconnected(struct bt_conn *conn, u8_t reason);

static struct bt_conn_cb ble_oad_conn_callbacks = {
    .connected = ble_oad_connected,
    .disconnected = ble_oad_disconnected,
};

static void ble_oad_tx_mtu_size(struct bt_conn *conn, u8_t err,
                                struct bt_gatt_exchange_params *params)
{
    if (!err) {
        BT_WARN("ble oad echange mtu size success, mtu size: %d\n", bt_gatt_get_mtu(ble_oad_conn));
    } else {
        BT_WARN("ble oad echange mtu size failure, err: %d\n", err);
    }
}

static void ble_oad_connected(struct bt_conn *conn, u8_t err)
{
    if (err || conn->type != BT_CONN_TYPE_LE) {
        return;
    }

    int ret = -1;
    int tx_octets = 0x00fb;
    int tx_time = 0x0848;

    if (!ble_oad_conn) {
        ble_oad_conn = conn;
    }

    //set data length after connected.
    ret = bt_le_set_data_len(ble_oad_conn, tx_octets, tx_time);
    if (!ret) {
        BT_WARN("ble oad set data length success.\n");
    } else {
        BT_WARN("ble oad set data length failure, err: %d\n", ret);
    }

    //exchange mtu size after connected.
    struct bt_gatt_exchange_params *exchg_mtu = k_malloc(sizeof(struct bt_gatt_exchange_params));
    exchg_mtu->func = ble_oad_tx_mtu_size;
    ret = bt_gatt_exchange_mtu(ble_oad_conn, exchg_mtu);
    if (!ret) {
        BT_WARN("ble oad exchange mtu size pending.\n");
    } else {
        BT_WARN("ble oad exchange mtu size failure, err: %d\n", ret);
    }
}

static void ble_oad_disconnected(struct bt_conn *conn, u8_t reason)
{
    if (conn->type != BT_CONN_TYPE_LE) {
        return;
    }

    if (conn == ble_oad_conn) {
        (disc_cb)(conn, reason);
    }
}

static int oad_recv(struct bt_conn *conn,
                    const struct bt_gatt_attr *attr, const void *buf,
                    u16_t len, u16_t offset, u8_t flags)
{
    (upper_recv_cb)(conn, buf, len);

    return 0;
}

static void oad_ccc_changed(const struct bt_gatt_attr *attr, u16_t value)
{
    BT_WARN("oad ccc:value=[%d]\r\n", value);
}

static struct bt_gatt_attr oad_attrs[] = {
    BT_GATT_PRIMARY_SERVICE(BT_UUID_OAD),
    BT_GATT_CHARACTERISTIC(BT_UUID_OAD_DATA_IN,
                           BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE, NULL, oad_recv, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_OAD_DATA_OUT,
                           BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_NONE,
                           NULL, NULL, NULL),
    BT_GATT_CCC(oad_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

};

static struct bt_gatt_service oad_svc = BT_GATT_SERVICE(oad_attrs);

void bt_oad_notify(struct bt_conn *conn, const void *data, u16_t len)
{
    bt_gatt_notify(conn, &oad_attrs[3], data, len);
}

void bt_oad_register_recv_cb(oad_upper_recv_cb cb)
{
    upper_recv_cb = cb;
}

void bt_oad_register_disc_cb(oad_disc_cb cb)
{
    disc_cb = cb;
}

void bt_oad_service_enable(void)
{
    bt_gatt_service_register(&oad_svc);
    bt_conn_cb_register(&ble_oad_conn_callbacks);
}

void bt_oad_service_disable(void)
{
    bt_gatt_service_unregister(&oad_svc);
}
