/*
 * Copyright (c) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <zephyr.h>
#include <bluetooth.h>
#include <conn.h>

#define BT_DBG_ENABLED  IS_ENABLED(CONFIG_BT_DEBUG_SETTINGS)
#define LOG_MODULE_NAME bt_settings
#include "log.h"

#include "hci_core.h"
#include "settings.h"
#include "keys.h"
#include "gatt.h"
#if defined(BFLB_BLE)
#include <stdlib.h>
#if defined(CONFIG_BT_SETTINGS)
#include "easyflash.h"
#endif
#include <FreeRTOS.h>
#include "portable.h"
#endif

#if defined(CONFIG_BT_SETTINGS_USE_PRINTK)
void bt_settings_encode_key(char *path, size_t path_size, const char *subsys,
                            bt_addr_le_t *addr, const char *key)
{
    if (key) {
        snprintk(path, path_size,
                 "bt/%s/%02x%02x%02x%02x%02x%02x%u/%s", subsys,
                 addr->a.val[5], addr->a.val[4], addr->a.val[3],
                 addr->a.val[2], addr->a.val[1], addr->a.val[0],
                 addr->type, key);
    } else {
        snprintk(path, path_size,
                 "bt/%s/%02x%02x%02x%02x%02x%02x%u", subsys,
                 addr->a.val[5], addr->a.val[4], addr->a.val[3],
                 addr->a.val[2], addr->a.val[1], addr->a.val[0],
                 addr->type);
    }

    BT_DBG("Encoded path %s", log_strdup(path));
}
#else
void bt_settings_encode_key(char *path, size_t path_size, const char *subsys,
                            bt_addr_le_t *addr, const char *key)
{
    size_t len = 3;

    /* Skip if path_size is less than 3; strlen("bt/") */
    if (len < path_size) {
        /* Key format:
		 *  "bt/<subsys>/<addr><type>/<key>", "/<key>" is optional
		 */
        strcpy(path, "bt/");
        strncpy(&path[len], subsys, path_size - len);
        len = strlen(path);
        if (len < path_size) {
            path[len] = '/';
            len++;
        }

        for (s8_t i = 5; i >= 0 && len < path_size; i--) {
            len += bin2hex(&addr->a.val[i], 1, &path[len],
                           path_size - len);
        }

        if (len < path_size) {
            /* Type can be either BT_ADDR_LE_PUBLIC or
			 * BT_ADDR_LE_RANDOM (value 0 or 1)
			 */
            path[len] = '0' + addr->type;
            len++;
        }

        if (key && len < path_size) {
            path[len] = '/';
            len++;
            strncpy(&path[len], key, path_size - len);
            len += strlen(&path[len]);
        }

        if (len >= path_size) {
            /* Truncate string */
            path[path_size - 1] = '\0';
        }
    } else if (path_size > 0) {
        *path = '\0';
    }

    BT_DBG("Encoded path %s", log_strdup(path));
}
#endif

#if !defined(BFLB_BLE)
int bt_settings_decode_key(const char *key, bt_addr_le_t *addr)
{
    if (settings_name_next(key, NULL) != 13) {
        return -EINVAL;
    }

    if (key[12] == '0') {
        addr->type = BT_ADDR_LE_PUBLIC;
    } else if (key[12] == '1') {
        addr->type = BT_ADDR_LE_RANDOM;
    } else {
        return -EINVAL;
    }

    for (u8_t i = 0; i < 6; i++) {
        hex2bin(&key[i * 2], 2, &addr->a.val[5 - i], 1);
    }

    BT_DBG("Decoded %s as %s", log_strdup(key), bt_addr_le_str(addr));

    return 0;
}

static int set(const char *name, size_t len_rd, settings_read_cb read_cb,
               void *cb_arg)
{
    ssize_t len;
    const char *next;

    if (!name) {
        BT_ERR("Insufficient number of arguments");
        return -ENOENT;
    }

    len = settings_name_next(name, &next);

    if (!strncmp(name, "id", len)) {
        /* Any previously provided identities supersede flash */
        if (atomic_test_bit(bt_dev.flags, BT_DEV_PRESET_ID)) {
            BT_WARN("Ignoring identities stored in flash");
            return 0;
        }

        len = read_cb(cb_arg, &bt_dev.id_addr, sizeof(bt_dev.id_addr));
        if (len < sizeof(bt_dev.id_addr[0])) {
            if (len < 0) {
                BT_ERR("Failed to read ID address from storage"
                       " (err %zu)",
                       len);
            } else {
                BT_ERR("Invalid length ID address in storage");
                BT_HEXDUMP_DBG(&bt_dev.id_addr, len,
                               "data read");
            }
            (void)memset(bt_dev.id_addr, 0,
                         sizeof(bt_dev.id_addr));
            bt_dev.id_count = 0U;
        } else {
            int i;

            bt_dev.id_count = len / sizeof(bt_dev.id_addr[0]);
            for (i = 0; i < bt_dev.id_count; i++) {
                BT_DBG("ID[%d] %s", i,
                       bt_addr_le_str(&bt_dev.id_addr[i]));
            }
        }

        return 0;
    }

#if defined(CONFIG_BT_DEVICE_NAME_DYNAMIC)
    if (!strncmp(name, "name", len)) {
        len = read_cb(cb_arg, &bt_dev.name, sizeof(bt_dev.name) - 1);
        if (len < 0) {
            BT_ERR("Failed to read device name from storage"
                   " (err %zu)",
                   len);
        } else {
            bt_dev.name[len] = '\0';

            BT_DBG("Name set to %s", log_strdup(bt_dev.name));
        }
        return 0;
    }
#endif

#if defined(CONFIG_BT_PRIVACY)
    if (!strncmp(name, "irk", len)) {
        len = read_cb(cb_arg, bt_dev.irk, sizeof(bt_dev.irk));
        if (len < sizeof(bt_dev.irk[0])) {
            if (len < 0) {
                BT_ERR("Failed to read IRK from storage"
                       " (err %zu)",
                       len);
            } else {
                BT_ERR("Invalid length IRK in storage");
                (void)memset(bt_dev.irk, 0, sizeof(bt_dev.irk));
            }
        } else {
            int i, count;

            count = len / sizeof(bt_dev.irk[0]);
            for (i = 0; i < count; i++) {
                BT_DBG("IRK[%d] %s", i,
                       bt_hex(bt_dev.irk[i], 16));
            }
        }

        return 0;
    }
#endif /* CONFIG_BT_PRIVACY */

    return -ENOENT;
}

#define ID_DATA_LEN(array) (bt_dev.id_count * sizeof(array[0]))

static void save_id(struct k_work *work)
{
    int err;
    BT_INFO("Saving ID");
    err = settings_save_one("bt/id", &bt_dev.id_addr,
                            ID_DATA_LEN(bt_dev.id_addr));
    if (err) {
        BT_ERR("Failed to save ID (err %d)", err);
    }

#if defined(CONFIG_BT_PRIVACY)
    err = settings_save_one("bt/irk", bt_dev.irk, ID_DATA_LEN(bt_dev.irk));
    if (err) {
        BT_ERR("Failed to save IRK (err %d)", err);
    }
#endif
}

K_WORK_DEFINE(save_id_work, save_id);
#endif //!BFLB_BLE
#if defined(BFLB_BLE)
#if defined(CONFIG_BT_SETTINGS)
bool ef_ready_flag = false;
int bt_check_if_ef_ready()
{
    int err = 0;

    if (!ef_ready_flag) {
        err = easyflash_init();
        if (!err)
            ef_ready_flag = true;
    }

    return err;
}

int bt_settings_set_bin(const char *key, const uint8_t *value, size_t length)
{
    const char *lookup = "0123456789abcdef";
    char *str_value;
    int err;

    err = bt_check_if_ef_ready();
    if (err)
        return err;

    str_value = pvPortMalloc(length * 2 + 1);

    BT_ASSERT(str_value != NULL);

    for (size_t i = 0; i < length; i++) {
        str_value[(i * 2) + 0] = lookup[(value[i] >> 4) & 0x0F];
        str_value[(i * 2) + 1] = lookup[value[i] & 0x0F];
    }
    str_value[length * 2] = '\0';

    err = ef_set_env(key, (const char *)str_value);

    vPortFree(str_value);

    return err;
}

int bt_settings_get_bin(const char *key, u8_t *value, size_t exp_len, size_t *real_len)
{
    char *str_value;
    size_t str_value_len;
    char rand[3];
    int err;

    err = bt_check_if_ef_ready();
    if (err)
        return err;

    str_value = ef_get_env(key);
    if (str_value == NULL) {
        return -1;
    }

    str_value_len = strlen(str_value);

    if ((str_value_len % 2) != 0 || (exp_len > 0 && str_value_len > exp_len * 2)) {
        return -1;
    }

    if (real_len)
        *real_len = str_value_len / 2;

    for (size_t i = 0; i < str_value_len / 2; i++) {
        strncpy(rand, str_value + 2 * i, 2);
        rand[2] = '\0';
        value[i] = strtol(rand, NULL, 16);
    }

    return 0;
}

int settings_delete(const char *key)
{
    return ef_del_env(key);
}

int settings_save_one(const char *key, const u8_t *value, size_t length)
{
    return bt_settings_set_bin(key, value, length);
}
#endif //CONFIG_BT_SETTINGS
#endif

void bt_settings_save_id(void)
{
#if defined(BFLB_BLE)
#if defined(CONFIG_BT_SETTINGS)
    if (bt_check_if_ef_ready())
        return;
    bt_settings_set_bin(NV_LOCAL_ID_ADDR, (const u8_t *)&bt_dev.id_addr[0], sizeof(bt_addr_le_t) * CONFIG_BT_ID_MAX);
#if defined(CONFIG_BT_PRIVACY)
    bt_settings_set_bin(NV_LOCAL_IRK, (const u8_t *)&bt_dev.irk[0], 16 * CONFIG_BT_ID_MAX);
#endif //CONFIG_BT_PRIVACY
#endif //CONFIG_BT_SETTINGS
#else
    k_work_submit(&save_id_work);
#endif
}

#if defined(BFLB_BLE)
#if defined(CONFIG_BT_SETTINGS)
void bt_settings_save_name(void)
{
    if (bt_check_if_ef_ready())
        return;

    ef_set_env(NV_LOCAL_NAME, bt_dev.name);
}

void bt_local_info_load(void)
{
    if (bt_check_if_ef_ready())
        return;
#if defined(CONFIG_BT_DEVICE_NAME_DYNAMIC)
    char *dev_name;
    uint8_t len;
    dev_name = ef_get_env(NV_LOCAL_NAME);
    if (dev_name != NULL) {
        len = ((strlen(dev_name) + 1) < CONFIG_BT_DEVICE_NAME_MAX) ? (strlen(dev_name) + 1) : CONFIG_BT_DEVICE_NAME_MAX;
        memcpy(bt_dev.name, dev_name, len);
    }
#endif
    bt_settings_get_bin(NV_LOCAL_ID_ADDR, (u8_t *)&bt_dev.id_addr[0], sizeof(bt_addr_le_t) * CONFIG_BT_ID_MAX, NULL);
#if defined(CONFIG_BT_PRIVACY)
    bt_settings_get_bin(NV_LOCAL_IRK, (u8_t *)&bt_dev.irk[0][0], 16 * CONFIG_BT_ID_MAX, NULL);
#endif
}
#endif //CONFIG_BT_SETTINGS
#endif

#if !defined(BFLB_BLE)
static int commit(void)
{
    BT_DBG("");

#if defined(CONFIG_BT_DEVICE_NAME_DYNAMIC)
    if (bt_dev.name[0] == '\0') {
        bt_set_name(CONFIG_BT_DEVICE_NAME);
    }
#endif
    if (!bt_dev.id_count) {
        int err;

        err = bt_setup_id_addr();
        if (err) {
            BT_ERR("Unable to setup an identity address");
            return err;
        }
    }

    /* Make sure that the identities created by bt_id_create after
	 * bt_enable is saved to persistent storage. */
    if (!atomic_test_bit(bt_dev.flags, BT_DEV_PRESET_ID)) {
        bt_settings_save_id();
    }

    if (!atomic_test_bit(bt_dev.flags, BT_DEV_READY)) {
        bt_finalize_init();
    }

    return 0;
}

SETTINGS_STATIC_HANDLER_DEFINE(bt, "bt", NULL, set, commit, NULL);

#endif //!BFLB_BLE

int bt_settings_init(void)
{
#if defined(BFLB_BLE)
    return 0;
#else
    int err;

    BT_DBG("");

    err = settings_subsys_init();
    if (err) {
        BT_ERR("settings_subsys_init failed (err %d)", err);
        return err;
    }

    return 0;
#endif
}
