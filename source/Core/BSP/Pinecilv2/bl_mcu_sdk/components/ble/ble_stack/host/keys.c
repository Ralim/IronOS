/* keys.c - Bluetooth key handling */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <string.h>
#include <stdlib.h>
#include <atomic.h>
#include <misc/util.h>

#include <bluetooth.h>
#include <conn.h>
#include <hci_host.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_DEBUG_KEYS)
#include "log.h"

#include "rpa.h"
#include "gatt_internal.h"
#include "hci_core.h"
#include "smp.h"
#include "settings.h"
#include "keys.h"
#if defined(BFLB_BLE)
#if defined(CONFIG_BT_SETTINGS)
#include "easyflash.h"
#endif
#endif

static struct bt_keys key_pool[CONFIG_BT_MAX_PAIRED];

#define BT_KEYS_STORAGE_LEN_COMPAT (BT_KEYS_STORAGE_LEN - sizeof(uint32_t))

#if IS_ENABLED(CONFIG_BT_KEYS_OVERWRITE_OLDEST)
static u32_t aging_counter_val;
static struct bt_keys *last_keys_updated;
#endif /* CONFIG_BT_KEYS_OVERWRITE_OLDEST */

struct bt_keys *bt_keys_get_addr(u8_t id, const bt_addr_le_t *addr)
{
    struct bt_keys *keys;
    int i;
    size_t first_free_slot = ARRAY_SIZE(key_pool);

    BT_DBG("%s", bt_addr_le_str(addr));

    for (i = 0; i < ARRAY_SIZE(key_pool); i++) {
        keys = &key_pool[i];

        if (keys->id == id && !bt_addr_le_cmp(&keys->addr, addr)) {
            return keys;
        }

        if (first_free_slot == ARRAY_SIZE(key_pool) &&
            (!bt_addr_le_cmp(&keys->addr, BT_ADDR_LE_ANY) ||
             !keys->enc_size)) {
            first_free_slot = i;
        }
    }

#if IS_ENABLED(CONFIG_BT_KEYS_OVERWRITE_OLDEST)
    if (first_free_slot == ARRAY_SIZE(key_pool)) {
        struct bt_keys *oldest = &key_pool[0];

        for (i = 1; i < ARRAY_SIZE(key_pool); i++) {
            struct bt_keys *current = &key_pool[i];

            if (current->aging_counter < oldest->aging_counter) {
                oldest = current;
            }
        }

        bt_unpair(oldest->id, &oldest->addr);
        if (!bt_addr_le_cmp(&oldest->addr, BT_ADDR_LE_ANY)) {
            first_free_slot = oldest - &key_pool[0];
        }
    }

#endif /* CONFIG_BT_KEYS_OVERWRITE_OLDEST */
    if (first_free_slot < ARRAY_SIZE(key_pool)) {
        keys = &key_pool[first_free_slot];
        keys->id = id;
        bt_addr_le_copy(&keys->addr, addr);
#if IS_ENABLED(CONFIG_BT_KEYS_OVERWRITE_OLDEST)
        keys->aging_counter = ++aging_counter_val;
        last_keys_updated = keys;
#endif /* CONFIG_BT_KEYS_OVERWRITE_OLDEST */
        BT_DBG("created %p for %s", keys, bt_addr_le_str(addr));
        return keys;
    }

    BT_DBG("unable to create keys for %s", bt_addr_le_str(addr));

    return NULL;
}

void bt_foreach_bond(u8_t id, void (*func)(const struct bt_bond_info *info, void *user_data),
                     void *user_data)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(key_pool); i++) {
        struct bt_keys *keys = &key_pool[i];

        if (keys->keys && keys->id == id) {
            struct bt_bond_info info;

            bt_addr_le_copy(&info.addr, &keys->addr);
            func(&info, user_data);
        }
    }
}

void bt_keys_foreach(int type, void (*func)(struct bt_keys *keys, void *data),
                     void *data)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(key_pool); i++) {
        if ((key_pool[i].keys & type)) {
            func(&key_pool[i], data);
        }
    }
}

struct bt_keys *bt_keys_find(int type, u8_t id, const bt_addr_le_t *addr)
{
    int i;

    BT_DBG("type %d %s", type, bt_addr_le_str(addr));

    for (i = 0; i < ARRAY_SIZE(key_pool); i++) {
        if ((key_pool[i].keys & type) && key_pool[i].id == id &&
            !bt_addr_le_cmp(&key_pool[i].addr, addr)) {
            return &key_pool[i];
        }
    }

    return NULL;
}

struct bt_keys *bt_keys_get_type(int type, u8_t id, const bt_addr_le_t *addr)
{
    struct bt_keys *keys;

    BT_DBG("type %d %s", type, bt_addr_le_str(addr));

    keys = bt_keys_find(type, id, addr);
    if (keys) {
        return keys;
    }

    keys = bt_keys_get_addr(id, addr);
    if (!keys) {
        return NULL;
    }

    bt_keys_add_type(keys, type);

    return keys;
}

struct bt_keys *bt_keys_find_irk(u8_t id, const bt_addr_le_t *addr)
{
    int i;

    BT_DBG("%s", bt_addr_le_str(addr));

    if (!bt_addr_le_is_rpa(addr)) {
        return NULL;
    }

    for (i = 0; i < ARRAY_SIZE(key_pool); i++) {
        if (!(key_pool[i].keys & BT_KEYS_IRK)) {
            continue;
        }

        if (key_pool[i].id == id &&
            !bt_addr_cmp(&addr->a, &key_pool[i].irk.rpa)) {
            BT_DBG("cached RPA %s for %s",
                   bt_addr_str(&key_pool[i].irk.rpa),
                   bt_addr_le_str(&key_pool[i].addr));
            return &key_pool[i];
        }
    }

    for (i = 0; i < ARRAY_SIZE(key_pool); i++) {
        if (!(key_pool[i].keys & BT_KEYS_IRK)) {
            continue;
        }

        if (key_pool[i].id != id) {
            continue;
        }

        if (bt_rpa_irk_matches(key_pool[i].irk.val, &addr->a)) {
            BT_DBG("RPA %s matches %s",
                   bt_addr_str(&key_pool[i].irk.rpa),
                   bt_addr_le_str(&key_pool[i].addr));

            bt_addr_copy(&key_pool[i].irk.rpa, &addr->a);

            return &key_pool[i];
        }
    }

    BT_DBG("No IRK for %s", bt_addr_le_str(addr));

    return NULL;
}

struct bt_keys *bt_keys_find_addr(u8_t id, const bt_addr_le_t *addr)
{
    int i;

    BT_DBG("%s", bt_addr_le_str(addr));

    for (i = 0; i < ARRAY_SIZE(key_pool); i++) {
        if (key_pool[i].id == id &&
            !bt_addr_le_cmp(&key_pool[i].addr, addr)) {
            return &key_pool[i];
        }
    }

    return NULL;
}

#if defined(CONFIG_BLE_AT_CMD)
bt_addr_le_t *bt_get_keys_address(u8_t id)
{
    bt_addr_le_t addr;

    memset(&addr, 0, sizeof(bt_addr_le_t));
    if (id < ARRAY_SIZE(key_pool)) {
        if (bt_addr_le_cmp(&key_pool[id].addr, &addr)) {
            return &key_pool[id].addr;
        }
    }

    return NULL;
}
#endif

void bt_keys_add_type(struct bt_keys *keys, int type)
{
    keys->keys |= type;
}

void bt_keys_clear(struct bt_keys *keys)
{
#if defined(BFLB_BLE)
    if (keys->keys & BT_KEYS_IRK) {
        bt_id_del(keys);
    }

    memset(keys, 0, sizeof(*keys));

#if defined(CONFIG_BT_SETTINGS)
    ef_del_env(NV_KEY_POOL);
#endif
#else
    BT_DBG("%s (keys 0x%04x)", bt_addr_le_str(&keys->addr), keys->keys);

    if (keys->keys & BT_KEYS_IRK) {
        bt_id_del(keys);
    }

    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
        char key[BT_SETTINGS_KEY_MAX];

        /* Delete stored keys from flash */
        if (keys->id) {
            char id[4];

            u8_to_dec(id, sizeof(id), keys->id);
            bt_settings_encode_key(key, sizeof(key), "keys",
                                   &keys->addr, id);
        } else {
            bt_settings_encode_key(key, sizeof(key), "keys",
                                   &keys->addr, NULL);
        }

        BT_DBG("Deleting key %s", log_strdup(key));
        settings_delete(key);
    }

    (void)memset(keys, 0, sizeof(*keys));
#endif
}

static void keys_clear_id(struct bt_keys *keys, void *data)
{
    u8_t *id = data;

    if (*id == keys->id) {
        if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
            bt_gatt_clear(*id, &keys->addr);
        }

        bt_keys_clear(keys);
    }
}

void bt_keys_clear_all(u8_t id)
{
    bt_keys_foreach(BT_KEYS_ALL, keys_clear_id, &id);
}

#if defined(CONFIG_BT_SETTINGS)
int bt_keys_store(struct bt_keys *keys)
{
#if defined(BFLB_BLE)
    int err;
    err = bt_settings_set_bin(NV_KEY_POOL, (const u8_t *)&key_pool[0], sizeof(key_pool));
    return err;
#else
    char val[BT_SETTINGS_SIZE(BT_KEYS_STORAGE_LEN)];
    char key[BT_SETTINGS_KEY_MAX];
    char *str;
    int err;

    str = settings_str_from_bytes(keys->storage_start, BT_KEYS_STORAGE_LEN,
                                  val, sizeof(val));
    if (!str) {
        BT_ERR("Unable to encode bt_keys as value");
        return -EINVAL;
    }

    if (keys->id) {
        char id[4];

        u8_to_dec(id, sizeof(id), keys->id);
        bt_settings_encode_key(key, sizeof(key), "keys", &keys->addr,
                               id);
    } else {
        bt_settings_encode_key(key, sizeof(key), "keys", &keys->addr,
                               NULL);
    }

    err = settings_save_one(key, keys->storage_start, BT_KEYS_STORAGE_LEN);
    if (err) {
        BT_ERR("Failed to save keys (err %d)", err);
        return err;
    }

    BT_DBG("Stored keys for %s (%s)", bt_addr_le_str(&keys->addr),
           log_strdup(key));

    return 0;
#endif //BFLB_BLE
}

#if !defined(BFLB_BLE)
static int keys_set(const char *name, size_t len_rd, settings_read_cb read_cb,
                    void *cb_arg)
{
    struct bt_keys *keys;
    bt_addr_le_t addr;
    u8_t id;
    size_t len;
    int err;
    char val[BT_KEYS_STORAGE_LEN];
    const char *next;

    if (!name) {
        BT_ERR("Insufficient number of arguments");
        return -EINVAL;
    }

    len = read_cb(cb_arg, val, sizeof(val));
    if (len < 0) {
        BT_ERR("Failed to read value (err %zu)", len);
        return -EINVAL;
    }

    BT_DBG("name %s val %s", log_strdup(name),
           (len) ? bt_hex(val, sizeof(val)) : "(null)");

    err = bt_settings_decode_key(name, &addr);
    if (err) {
        BT_ERR("Unable to decode address %s", name);
        return -EINVAL;
    }

    settings_name_next(name, &next);

    if (!next) {
        id = BT_ID_DEFAULT;
    } else {
        id = strtol(next, NULL, 10);
    }

    if (!len) {
        keys = bt_keys_find(BT_KEYS_ALL, id, &addr);
        if (keys) {
            (void)memset(keys, 0, sizeof(*keys));
            BT_DBG("Cleared keys for %s", bt_addr_le_str(&addr));
        } else {
            BT_WARN("Unable to find deleted keys for %s",
                    bt_addr_le_str(&addr));
        }

        return 0;
    }

    keys = bt_keys_get_addr(id, &addr);
    if (!keys) {
        BT_ERR("Failed to allocate keys for %s", bt_addr_le_str(&addr));
        return -ENOMEM;
    }
    if (len != BT_KEYS_STORAGE_LEN) {
        do {
            /* Load shorter structure for compatibility with old
			 * records format with no counter.
			 */
            if (IS_ENABLED(CONFIG_BT_KEYS_OVERWRITE_OLDEST) &&
                len == BT_KEYS_STORAGE_LEN_COMPAT) {
                BT_WARN("Keys for %s have no aging counter",
                        bt_addr_le_str(&addr));
                memcpy(keys->storage_start, val, len);
                continue;
            }

            BT_ERR("Invalid key length %zu != %zu", len,
                   BT_KEYS_STORAGE_LEN);
            bt_keys_clear(keys);

            return -EINVAL;
        } while (0);
    } else {
        memcpy(keys->storage_start, val, len);
    }

    BT_DBG("Successfully restored keys for %s", bt_addr_le_str(&addr));
#if IS_ENABLED(CONFIG_BT_KEYS_OVERWRITE_OLDEST)
    if (aging_counter_val < keys->aging_counter) {
        aging_counter_val = keys->aging_counter;
    }
#endif /* CONFIG_BT_KEYS_OVERWRITE_OLDEST */
    return 0;
}
#endif //!(BFLB_BLE)

static void id_add(struct bt_keys *keys, void *user_data)
{
    bt_id_add(keys);
}

#if defined(BFLB_BLE)
int keys_commit(void)
#else
static int keys_commit(void)
#endif
{
    BT_DBG("");

    /* We do this in commit() rather than add() since add() may get
	 * called multiple times for the same address, especially if
	 * the keys were already removed.
	 */
    bt_keys_foreach(BT_KEYS_IRK, id_add, NULL);

    return 0;
}

//SETTINGS_STATIC_HANDLER_DEFINE(bt_keys, "bt/keys", NULL, keys_set, keys_commit,
//			       NULL);

#if defined(BFLB_BLE)
int bt_keys_load(void)
{
    return bt_settings_get_bin(NV_KEY_POOL, (u8_t *)&key_pool[0], sizeof(key_pool), NULL);
}
#endif

#endif /* CONFIG_BT_SETTINGS */

#if IS_ENABLED(CONFIG_BT_KEYS_OVERWRITE_OLDEST)
void bt_keys_update_usage(u8_t id, const bt_addr_le_t *addr)
{
    struct bt_keys *keys = bt_keys_find_addr(id, addr);

    if (!keys) {
        return;
    }

    if (last_keys_updated == keys) {
        return;
    }

    keys->aging_counter = ++aging_counter_val;
    last_keys_updated = keys;

    BT_DBG("Aging counter for %s is set to %u", bt_addr_le_str(addr),
           keys->aging_counter);

    if (IS_ENABLED(CONFIG_BT_KEYS_SAVE_AGING_COUNTER_ON_PAIRING)) {
        bt_keys_store(keys);
    }
}

#endif /* CONFIG_BT_KEYS_OVERWRITE_OLDEST */
