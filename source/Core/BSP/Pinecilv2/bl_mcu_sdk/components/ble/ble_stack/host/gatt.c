/* gatt.c - Generic Attribute Profile handling */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <atomic.h>
#include <errno.h>
#include <misc/byteorder.h>
#include <misc/util.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr.h>

#include <settings.h>

#if defined(CONFIG_BT_GATT_CACHING)
#include <aes.h>
#include <ccm_mode.h>
#include <cmac_mode.h>
#include <constants.h>
#include <utils.h>
#endif /* CONFIG_BT_GATT_CACHING */
#include <bluetooth.h>
#include <gatt.h>
#include <hci_driver.h>
#include <hci_host.h>
#include <uuid.h>
#if defined(BFLB_BLE)
#include "ble_config.h"
#include <util.h>
#endif

#if defined(CONFIG_BT_STACK_PTS)
extern u8_t event_flag;
#endif

#ifdef BT_DBG_ENABLED
#undef BT_DBG_ENABLED
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_DEBUG_GATT)
#else
#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BT_DEBUG_GATT)
#endif

#define LOG_MODULE_NAME bt_gatt
#include "log.h"

#include "att_internal.h"
#include "conn_internal.h"
#include "gatt_internal.h"
#include "hci_core.h"
#include "keys.h"
#include "l2cap_internal.h"
#include "settings.h"
#include "smp.h"

#define SC_TIMEOUT      K_MSEC(10)
#define CCC_STORE_DELAY K_SECONDS(1)

#define DB_HASH_TIMEOUT K_MSEC(10)

static u16_t last_static_handle;

/* Persistent storage format for GATT CCC */
struct ccc_store {
  u16_t handle;
  u16_t value;
};

#if defined(CONFIG_BT_GATT_CLIENT)
static sys_slist_t subscriptions;
#endif /* CONFIG_BT_GATT_CLIENT */

static const u16_t gap_appearance = CONFIG_BT_DEVICE_APPEARANCE;

#if defined(CONFIG_BT_GATT_DYNAMIC_DB)
static sys_slist_t db;
#endif /* CONFIG_BT_GATT_DYNAMIC_DB */

static atomic_t init;

#if defined(BFLB_BLE_MTU_CHANGE_CB)
bt_gatt_mtu_changed_cb_t gatt_mtu_changed_cb;
#endif

static ssize_t read_name(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  const char *name = bt_get_name();

  return bt_gatt_attr_read(conn, attr, buf, len, offset, name, strlen(name));
}

#if defined(CONFIG_BT_DEVICE_NAME_GATT_WRITABLE)

static ssize_t write_name(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, u16_t len, u16_t offset, u8_t flags) {
  char value[CONFIG_BT_DEVICE_NAME_MAX] = {};

  if (offset) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
  }

  if (len >= sizeof(value)) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
  }

  memcpy(value, buf, len);

  bt_set_name(value);

  return len;
}

#endif /* CONFIG_BT_DEVICE_NAME_GATT_WRITABLE */

static ssize_t read_appearance(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  u16_t appearance = sys_cpu_to_le16(gap_appearance);

  return bt_gatt_attr_read(conn, attr, buf, len, offset, &appearance, sizeof(appearance));
}

#if defined(CONFIG_BT_GAP_PERIPHERAL_PREF_PARAMS)
/* This checks if the range entered is valid */
BUILD_ASSERT(!(CONFIG_BT_PERIPHERAL_PREF_MIN_INT > 3200 && CONFIG_BT_PERIPHERAL_PREF_MIN_INT < 0xffff));
BUILD_ASSERT(!(CONFIG_BT_PERIPHERAL_PREF_MAX_INT > 3200 && CONFIG_BT_PERIPHERAL_PREF_MAX_INT < 0xffff));
BUILD_ASSERT(!(CONFIG_BT_PERIPHERAL_PREF_TIMEOUT > 3200 && CONFIG_BT_PERIPHERAL_PREF_TIMEOUT < 0xffff));
BUILD_ASSERT((CONFIG_BT_PERIPHERAL_PREF_MIN_INT == 0xffff) || (CONFIG_BT_PERIPHERAL_PREF_MIN_INT <= CONFIG_BT_PERIPHERAL_PREF_MAX_INT));

static ssize_t read_ppcp(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  struct __packed {
    u16_t min_int;
    u16_t max_int;
    u16_t latency;
    u16_t timeout;
  } ppcp;

  ppcp.min_int = sys_cpu_to_le16(CONFIG_BT_PERIPHERAL_PREF_MIN_INT);
  ppcp.max_int = sys_cpu_to_le16(CONFIG_BT_PERIPHERAL_PREF_MAX_INT);
  ppcp.latency = sys_cpu_to_le16(CONFIG_BT_PERIPHERAL_PREF_SLAVE_LATENCY);
  ppcp.timeout = sys_cpu_to_le16(CONFIG_BT_PERIPHERAL_PREF_TIMEOUT);

  return bt_gatt_attr_read(conn, attr, buf, len, offset, &ppcp, sizeof(ppcp));
}
#endif

#if defined(CONFIG_BT_CENTRAL) && defined(CONFIG_BT_PRIVACY)
static ssize_t read_central_addr_res(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  u8_t central_addr_res = BT_GATT_CENTRAL_ADDR_RES_SUPP;

  return bt_gatt_attr_read(conn, attr, buf, len, offset, &central_addr_res, sizeof(central_addr_res));
}
#endif /* CONFIG_BT_CENTRAL && CONFIG_BT_PRIVACY */

#if defined(BFLB_BLE_DISABLE_STATIC_ATTR)
static struct bt_gatt_attr gap_attrs[] = {
#else
BT_GATT_SERVICE_DEFINE(_2_gap_svc,
#endif
    BT_GATT_PRIMARY_SERVICE(BT_UUID_GAP),

#if defined(CONFIG_BT_DEVICE_NAME_GATT_WRITABLE)
    /* Require pairing for writes to device name */
    BT_GATT_CHARACTERISTIC(BT_UUID_GAP_DEVICE_NAME, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_name, write_name, bt_dev.name),
#else
                       BT_GATT_CHARACTERISTIC(BT_UUID_GAP_DEVICE_NAME, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_name, NULL, NULL),
#endif /* CONFIG_BT_DEVICE_NAME_GATT_WRITABLE */
    BT_GATT_CHARACTERISTIC(BT_UUID_GAP_APPEARANCE, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_appearance, NULL, NULL),
#if defined(CONFIG_BT_CENTRAL) && defined(CONFIG_BT_PRIVACY)
    BT_GATT_CHARACTERISTIC(BT_UUID_CENTRAL_ADDR_RES, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_central_addr_res, NULL, NULL),
#endif /* CONFIG_BT_CENTRAL && CONFIG_BT_PRIVACY */
#if defined(CONFIG_BT_GAP_PERIPHERAL_PREF_PARAMS)
    BT_GATT_CHARACTERISTIC(BT_UUID_GAP_PPCP, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_ppcp, NULL, NULL),
#endif
#if defined(BFLB_BLE_DISABLE_STATIC_ATTR)
};
#else
);
#endif

#if defined(BFLB_BLE_DISABLE_STATIC_ATTR)
static struct bt_gatt_service gap_svc = BT_GATT_SERVICE(gap_attrs);
#endif

struct sc_data {
  u16_t start;
  u16_t end;
} __packed;

struct gatt_sc_cfg {
  u8_t         id;
  bt_addr_le_t peer;
  struct {
    u16_t start;
    u16_t end;
  } data;
};

#define SC_CFG_MAX (CONFIG_BT_MAX_PAIRED + CONFIG_BT_MAX_CONN)
static struct gatt_sc_cfg sc_cfg[SC_CFG_MAX];
BUILD_ASSERT(sizeof(struct sc_data) == sizeof(sc_cfg[0].data));

static struct gatt_sc_cfg *find_sc_cfg(u8_t id, bt_addr_le_t *addr) {
  BT_DBG("id: %u, addr: %s", id, bt_addr_le_str(addr));

  for (size_t i = 0; i < ARRAY_SIZE(sc_cfg); i++) {
    if (id == sc_cfg[i].id && !bt_addr_le_cmp(&sc_cfg[i].peer, addr)) {
      return &sc_cfg[i];
    }
  }

  return NULL;
}

static void sc_store(struct gatt_sc_cfg *cfg) {
  char key[BT_SETTINGS_KEY_MAX];
  int  err;

  if (cfg->id) {
    char id_str[4];

    u8_to_dec(id_str, sizeof(id_str), cfg->id);
    bt_settings_encode_key(key, sizeof(key), "sc", &cfg->peer, id_str);
  } else {
    bt_settings_encode_key(key, sizeof(key), "sc", &cfg->peer, NULL);
  }

  err = settings_save_one(key, (u8_t *)&cfg->data, sizeof(cfg->data));
  if (err) {
    BT_ERR("failed to store SC (err %d)", err);
    return;
  }

  BT_DBG("stored SC for %s (%s, 0x%04x-0x%04x)", bt_addr_le_str(&cfg->peer), log_strdup(key), cfg->data.start, cfg->data.end);
}

static void sc_clear(struct gatt_sc_cfg *cfg) {
  BT_DBG("peer %s", bt_addr_le_str(&cfg->peer));

  if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
    bool modified = false;

    if (cfg->data.start || cfg->data.end) {
      modified = true;
    }

    if (modified && bt_addr_le_is_bonded(cfg->id, &cfg->peer)) {
      char key[BT_SETTINGS_KEY_MAX];
      int  err;

      if (cfg->id) {
        char id_str[4];

        u8_to_dec(id_str, sizeof(id_str), cfg->id);
        bt_settings_encode_key(key, sizeof(key), "sc", &cfg->peer, id_str);
      } else {
        bt_settings_encode_key(key, sizeof(key), "sc", &cfg->peer, NULL);
      }

      err = settings_delete(key);
      if (err) {
        BT_ERR("failed to delete SC (err %d)", err);
      } else {
        BT_DBG("deleted SC for %s (%s)", bt_addr_le_str(&cfg->peer), log_strdup(key));
      }
    }
  }

  memset(cfg, 0, sizeof(*cfg));
}

static void sc_reset(struct gatt_sc_cfg *cfg) {
  BT_DBG("peer %s", bt_addr_le_str(&cfg->peer));

  memset(&cfg->data, 0, sizeof(cfg->data));

  if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
    sc_store(cfg);
  }
}

static bool update_range(u16_t *start, u16_t *end, u16_t new_start, u16_t new_end) {
  BT_DBG("start 0x%04x end 0x%04x new_start 0x%04x new_end 0x%04x", *start, *end, new_start, new_end);

  /* Check if inside existing range */
  if (new_start >= *start && new_end <= *end) {
    return false;
  }

  /* Update range */
  if (*start > new_start) {
    *start = new_start;
  }

  if (*end < new_end) {
    *end = new_end;
  }

  return true;
}

static void sc_save(u8_t id, bt_addr_le_t *peer, u16_t start, u16_t end) {
  struct gatt_sc_cfg *cfg;
  bool                modified = false;

  BT_DBG("peer %s start 0x%04x end 0x%04x", bt_addr_le_str(peer), start, end);

  cfg = find_sc_cfg(id, peer);
  if (!cfg) {
    /* Find and initialize a free sc_cfg entry */
    cfg = find_sc_cfg(BT_ID_DEFAULT, BT_ADDR_LE_ANY);
    if (!cfg) {
      BT_ERR("unable to save SC: no cfg left");
      return;
    }

    cfg->id = id;
    bt_addr_le_copy(&cfg->peer, peer);
  }

  /* Check if there is any change stored */
  if (!(cfg->data.start || cfg->data.end)) {
    cfg->data.start = start;
    cfg->data.end   = end;
    modified        = true;
    goto done;
  }

  modified = update_range(&cfg->data.start, &cfg->data.end, start, end);

done:
  if (IS_ENABLED(CONFIG_BT_SETTINGS) && modified && bt_addr_le_is_bonded(cfg->id, &cfg->peer)) {
    sc_store(cfg);
  }
}

static bool sc_ccc_cfg_write(struct bt_conn *conn, const struct bt_gatt_attr *attr, u16_t value) {
  BT_DBG("value 0x%04x", value);

  if (value == BT_GATT_CCC_INDICATE) {
    /* Create a new SC configuration entry if subscribed */
    sc_save(conn->id, &conn->le.dst, 0, 0);
  } else {
    struct gatt_sc_cfg *cfg;

    /* Clear SC configuration if unsubscribed */
    cfg = find_sc_cfg(conn->id, &conn->le.dst);
    if (cfg) {
      sc_clear(cfg);
    }
  }

  return true;
}

static struct _bt_gatt_ccc sc_ccc = BT_GATT_CCC_INITIALIZER(NULL, sc_ccc_cfg_write, NULL);

#if defined(CONFIG_BT_GATT_CACHING)
enum {
  CF_CHANGE_AWARE, /* Client is changed aware */
  CF_OUT_OF_SYNC,  /* Client is out of sync */

  /* Total number of flags - must be at the end of the enum */
  CF_NUM_FLAGS,
};

#define CF_ROBUST_CACHING(_cfg) (_cfg->data[0] & BIT(0))

struct gatt_cf_cfg {
  u8_t         id;
  bt_addr_le_t peer;
  u8_t         data[1];
  ATOMIC_DEFINE(flags, CF_NUM_FLAGS);
};

#define CF_CFG_MAX (CONFIG_BT_MAX_PAIRED + CONFIG_BT_MAX_CONN)
static struct gatt_cf_cfg cf_cfg[CF_CFG_MAX] = {};

static struct gatt_cf_cfg *find_cf_cfg(struct bt_conn *conn) {
  int i;

  for (i = 0; i < ARRAY_SIZE(cf_cfg); i++) {
    if (!conn) {
      if (!bt_addr_le_cmp(&cf_cfg[i].peer, BT_ADDR_LE_ANY)) {
        return &cf_cfg[i];
      }
    } else if (!bt_conn_addr_le_cmp(conn, &cf_cfg[i].peer)) {
      return &cf_cfg[i];
    }
  }

  return NULL;
}

static ssize_t cf_read(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  struct gatt_cf_cfg *cfg;
  u8_t                data[1] = {};

  cfg = find_cf_cfg(conn);
  if (cfg) {
    memcpy(data, cfg->data, sizeof(data));
  }

  return bt_gatt_attr_read(conn, attr, buf, len, offset, data, sizeof(data));
}

static bool cf_set_value(struct gatt_cf_cfg *cfg, const u8_t *value, u16_t len) {
  u16_t i;
  u8_t  last_byte = 1U;
  u8_t  last_bit  = 1U;

  /* Validate the bits */
  for (i = 0U; i < len && i < last_byte; i++) {
    u8_t chg_bits = value[i] ^ cfg->data[i];
    u8_t bit;

    for (bit = 0U; bit < last_bit; bit++) {
      /* A client shall never clear a bit it has set */
      if ((BIT(bit) & chg_bits) && (BIT(bit) & cfg->data[i])) {
        return false;
      }
    }
  }

  /* Set the bits for each octect */
  for (i = 0U; i < len && i < last_byte; i++) {
    cfg->data[i] |= value[i] & ((1 << last_bit) - 1);
    BT_DBG("byte %u: data 0x%02x value 0x%02x", i, cfg->data[i], value[i]);
  }

  return true;
}

static ssize_t cf_write(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, u16_t len, u16_t offset, u8_t flags) {
  struct gatt_cf_cfg *cfg;
  const u8_t         *value = buf;

  if (offset > sizeof(cfg->data)) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
  }

  if (offset + len > sizeof(cfg->data)) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
  }

  cfg = find_cf_cfg(conn);
  if (!cfg) {
    cfg = find_cf_cfg(NULL);
  }

  if (!cfg) {
    BT_WARN("No space to store Client Supported Features");
    return BT_GATT_ERR(BT_ATT_ERR_INSUFFICIENT_RESOURCES);
  }

  BT_DBG("handle 0x%04x len %u", attr->handle, len);

  if (!cf_set_value(cfg, value, len)) {
    return BT_GATT_ERR(BT_ATT_ERR_VALUE_NOT_ALLOWED);
  }

  bt_addr_le_copy(&cfg->peer, &conn->le.dst);
  atomic_set_bit(cfg->flags, CF_CHANGE_AWARE);

  return len;
}

static u8_t           db_hash[16];
struct k_delayed_work db_hash_work;

struct gen_hash_state {
  struct tc_cmac_struct state;
  int                   err;
};

static u8_t gen_hash_m(const struct bt_gatt_attr *attr, void *user_data) {
  struct gen_hash_state *state = user_data;
  struct bt_uuid_16     *u16;
  u8_t                   data[16];
  ssize_t                len;
  u16_t                  value;

  if (attr->uuid->type != BT_UUID_TYPE_16) {
    return BT_GATT_ITER_CONTINUE;
  }

  u16 = (struct bt_uuid_16 *)attr->uuid;

  switch (u16->val) {
  /* Attributes to hash: handle + UUID + value */
  case 0x2800: /* GATT Primary Service */
  case 0x2801: /* GATT Secondary Service */
  case 0x2802: /* GATT Include Service */
  case 0x2803: /* GATT Characteristic */
  case 0x2900: /* GATT Characteristic Extended Properties */
    value = sys_cpu_to_le16(attr->handle);
    if (tc_cmac_update(&state->state, (uint8_t *)&value, sizeof(attr->handle)) == TC_CRYPTO_FAIL) {
      state->err = -EINVAL;
      return BT_GATT_ITER_STOP;
    }

    value = sys_cpu_to_le16(u16->val);
    if (tc_cmac_update(&state->state, (uint8_t *)&value, sizeof(u16->val)) == TC_CRYPTO_FAIL) {
      state->err = -EINVAL;
      return BT_GATT_ITER_STOP;
    }

    len = attr->read(NULL, attr, data, sizeof(data), 0);
    if (len < 0) {
      state->err = len;
      return BT_GATT_ITER_STOP;
    }

    if (tc_cmac_update(&state->state, data, len) == TC_CRYPTO_FAIL) {
      state->err = -EINVAL;
      return BT_GATT_ITER_STOP;
    }

    break;
  /* Attributes to hash: handle + UUID */
  case 0x2901: /* GATT Characteristic User Descriptor */
  case 0x2902: /* GATT Client Characteristic Configuration */
  case 0x2903: /* GATT Server Characteristic Configuration */
  case 0x2904: /* GATT Characteristic Presentation Format */
  case 0x2905: /* GATT Characteristic Aggregated Format */
    value = sys_cpu_to_le16(attr->handle);
    if (tc_cmac_update(&state->state, (uint8_t *)&value, sizeof(attr->handle)) == TC_CRYPTO_FAIL) {
      state->err = -EINVAL;
      return BT_GATT_ITER_STOP;
    }

    value = sys_cpu_to_le16(u16->val);
    if (tc_cmac_update(&state->state, (uint8_t *)&value, sizeof(u16->val)) == TC_CRYPTO_FAIL) {
      state->err = -EINVAL;
      return BT_GATT_ITER_STOP;
    }
    break;
  default:
    return BT_GATT_ITER_CONTINUE;
  }

  return BT_GATT_ITER_CONTINUE;
}

static void db_hash_store(void) {
  int err;

  err = settings_save_one("bt/hash", &db_hash, sizeof(db_hash));
  if (err) {
    BT_ERR("Failed to save Database Hash (err %d)", err);
  }

  BT_DBG("Database Hash stored");
}

static void db_hash_gen(bool store) {
  u8_t                           key[16] = {};
  struct tc_aes_key_sched_struct sched;
  struct gen_hash_state          state;

  if (tc_cmac_setup(&state.state, key, &sched) == TC_CRYPTO_FAIL) {
    BT_ERR("Unable to setup AES CMAC");
    return;
  }

  bt_gatt_foreach_attr(0x0001, 0xffff, gen_hash_m, &state);

  if (tc_cmac_final(db_hash, &state.state) == TC_CRYPTO_FAIL) {
    BT_ERR("Unable to calculate hash");
    return;
  }

  /**
   * Core 5.1 does not state the endianess of the hash.
   * However Vol 3, Part F, 3.3.1 says that multi-octet Characteristic
   * Values shall be LE unless otherwise defined. PTS expects hash to be
   * in little endianess as well. bt_smp_aes_cmac calculates the hash in
   * big endianess so we have to swap.
   */
  sys_mem_swap(db_hash, sizeof(db_hash));

#if !defined(BFLB_BLE)
  BT_HEXDUMP_DBG(db_hash, sizeof(db_hash), "Hash: ");
#endif

  if (IS_ENABLED(CONFIG_BT_SETTINGS) && store) {
    db_hash_store();
  }
}

static void db_hash_process(struct k_work *work) { db_hash_gen(true); }

static ssize_t db_hash_read(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  /* Check if db_hash is already pending in which case it shall be
   * generated immediately instead of waiting the work to complete.
   */
  if (k_delayed_work_remaining_get(&db_hash_work)) {
    k_delayed_work_cancel(&db_hash_work);
    db_hash_gen(true);
  }

  /* BLUETOOTH CORE SPECIFICATION Version 5.1 | Vol 3, Part G page 2347:
   * 2.5.2.1 Robust Caching
   * A connected client becomes change-aware when...
   * The client reads the Database Hash characteristic and then the server
   * receives another ATT request from the client.
   */
  bt_gatt_change_aware(conn, true);

  return bt_gatt_attr_read(conn, attr, buf, len, offset, db_hash, sizeof(db_hash));
}

static void clear_cf_cfg(struct gatt_cf_cfg *cfg) {
  bt_addr_le_copy(&cfg->peer, BT_ADDR_LE_ANY);
  memset(cfg->data, 0, sizeof(cfg->data));
  atomic_set(cfg->flags, 0);
}

static void remove_cf_cfg(struct bt_conn *conn) {
  struct gatt_cf_cfg *cfg;

  cfg = find_cf_cfg(conn);
  if (!cfg) {
    return;
  }

  /* BLUETOOTH CORE SPECIFICATION Version 5.1 | Vol 3, Part G page 2405:
   * For clients with a trusted relationship, the characteristic value
   * shall be persistent across connections. For clients without a
   * trusted relationship the characteristic value shall be set to the
   * default value at each connection.
   */
  if (!bt_addr_le_is_bonded(conn->id, &conn->le.dst)) {
    clear_cf_cfg(cfg);
  } else {
    /* Update address in case it has changed */
    bt_addr_le_copy(&cfg->peer, &conn->le.dst);
  }
}
#endif /* CONFIG_BT_GATT_CACHING */

#if defined(BFLB_BLE_DISABLE_STATIC_ATTR)
static struct bt_gatt_attr gatt_attrs[] = {
#else
BT_GATT_SERVICE_DEFINE(_1_gatt_svc,
#endif
    BT_GATT_PRIMARY_SERVICE(BT_UUID_GATT),

#if defined(CONFIG_BT_GATT_SERVICE_CHANGED)
    /* Bluetooth 5.0, Vol3 Part G:
     * The Service Changed characteristic Attribute Handle on the server
     * shall not change if the server has a trusted relationship with any
     * client.
     */
    BT_GATT_CHARACTERISTIC(BT_UUID_GATT_SC, BT_GATT_CHRC_INDICATE, BT_GATT_PERM_NONE, NULL, NULL, NULL),

    BT_GATT_CCC_MANAGED(&sc_ccc, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

#if defined(CONFIG_BT_GATT_CACHING)
    BT_GATT_CHARACTERISTIC(BT_UUID_GATT_CLIENT_FEATURES, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, cf_read, cf_write, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_GATT_DB_HASH, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, db_hash_read, NULL, NULL),
#endif /* CONFIG_BT_GATT_CACHING */
#endif /* CONFIG_BT_GATT_SERVICE_CHANGED */
#if defined(BFLB_BLE_DISABLE_STATIC_ATTR)
};
#else
);
#endif

#if defined(BFLB_BLE_DISABLE_STATIC_ATTR)
static struct bt_gatt_service gatt_svc = BT_GATT_SERVICE(gatt_attrs);
#endif

#if defined(CONFIG_BT_GATT_DYNAMIC_DB)
static u8_t found_attr(const struct bt_gatt_attr *attr, void *user_data) {
  const struct bt_gatt_attr **found = user_data;

  *found = attr;

  return BT_GATT_ITER_STOP;
}

static const struct bt_gatt_attr *find_attr(uint16_t handle) {
  const struct bt_gatt_attr *attr = NULL;

  bt_gatt_foreach_attr(handle, handle, found_attr, &attr);

  return attr;
}

static void gatt_insert(struct bt_gatt_service *svc, u16_t last_handle) {
  struct bt_gatt_service *tmp, *prev = NULL;

  if (last_handle == 0 || svc->attrs[0].handle > last_handle) {
    sys_slist_append(&db, &svc->node);
    return;
  }

  /* DB shall always have its service in ascending order */
  SYS_SLIST_FOR_EACH_CONTAINER(&db, tmp, node) {
    if (tmp->attrs[0].handle > svc->attrs[0].handle) {
      if (prev) {
        sys_slist_insert(&db, &prev->node, &svc->node);
      } else {
        sys_slist_prepend(&db, &svc->node);
      }
      return;
    }

    prev = tmp;
  }
}

static int gatt_register(struct bt_gatt_service *svc) {
  struct bt_gatt_service *last;
  u16_t                   handle, last_handle;
  struct bt_gatt_attr    *attrs = svc->attrs;
  u16_t                   count = svc->attr_count;

  if (sys_slist_is_empty(&db)) {
    handle      = last_static_handle;
    last_handle = 0;
    goto populate;
  }

  last        = SYS_SLIST_PEEK_TAIL_CONTAINER(&db, last, node);
  handle      = last->attrs[last->attr_count - 1].handle;
  last_handle = handle;

populate:
  /* Populate the handles and append them to the list */
  for (; attrs && count; attrs++, count--) {
    if (!attrs->handle) {
      /* Allocate handle if not set already */
      attrs->handle = ++handle;
    } else if (attrs->handle > handle) {
      /* Use existing handle if valid */
      handle = attrs->handle;
    } else if (find_attr(attrs->handle)) {
      /* Service has conflicting handles */
      BT_ERR("Unable to register handle 0x%04x", attrs->handle);
      return -EINVAL;
    }

    BT_DBG("attr %p handle 0x%04x uuid %s perm 0x%02x", attrs, attrs->handle, bt_uuid_str(attrs->uuid), attrs->perm);
  }

  gatt_insert(svc, last_handle);

  return 0;
}
#endif /* CONFIG_BT_GATT_DYNAMIC_DB */

enum {
  SC_RANGE_CHANGED,    /* SC range changed */
  SC_INDICATE_PENDING, /* SC indicate pending */

  /* Total number of flags - must be at the end of the enum */
  SC_NUM_FLAGS,
};

static struct gatt_sc {
  struct bt_gatt_indicate_params params;
  u16_t                          start;
  u16_t                          end;
  struct k_delayed_work          work;
  ATOMIC_DEFINE(flags, SC_NUM_FLAGS);
} gatt_sc;

static void sc_indicate_rsp(struct bt_conn *conn, const struct bt_gatt_attr *attr, u8_t err) {
#if defined(CONFIG_BT_GATT_CACHING)
  struct gatt_cf_cfg *cfg;
#endif

  BT_DBG("err 0x%02x", err);

  atomic_clear_bit(gatt_sc.flags, SC_INDICATE_PENDING);

  /* Check if there is new change in the meantime */
  if (atomic_test_bit(gatt_sc.flags, SC_RANGE_CHANGED)) {
    /* Reschedule without any delay since it is waiting already */
    k_delayed_work_submit(&gatt_sc.work, K_NO_WAIT);
  }

#if defined(CONFIG_BT_GATT_CACHING)
  /* BLUETOOTH CORE SPECIFICATION Version 5.1 | Vol 3, Part G page 2347:
   * 2.5.2.1 Robust Caching
   * A connected client becomes change-aware when...
   * The client receives and confirms a Service Changed indication.
   */
  cfg = find_cf_cfg(conn);
  if (cfg && CF_ROBUST_CACHING(cfg)) {
    atomic_set_bit(cfg->flags, CF_CHANGE_AWARE);
    BT_DBG("%s change-aware", bt_addr_le_str(&cfg->peer));
  }
#endif
}

static void sc_process(struct k_work *work) {
  struct gatt_sc *sc = CONTAINER_OF(work, struct gatt_sc, work);
  u16_t           sc_range[2];

  __ASSERT(!atomic_test_bit(sc->flags, SC_INDICATE_PENDING), "Indicate already pending");

  BT_DBG("start 0x%04x end 0x%04x", sc->start, sc->end);

  sc_range[0] = sys_cpu_to_le16(sc->start);
  sc_range[1] = sys_cpu_to_le16(sc->end);

  atomic_clear_bit(sc->flags, SC_RANGE_CHANGED);
  sc->start = 0U;
  sc->end   = 0U;
#if defined(BFLB_BLE_DISABLE_STATIC_ATTR)
  sc->params.attr = &gatt_attrs[2];
#else
  sc->params.attr = &_1_gatt_svc.attrs[2];
#endif
  sc->params.func = sc_indicate_rsp;
  sc->params.data = &sc_range[0];
  sc->params.len  = sizeof(sc_range);

  if (bt_gatt_indicate(NULL, &sc->params)) {
    /* No connections to indicate */
    return;
  }

  atomic_set_bit(sc->flags, SC_INDICATE_PENDING);
}

#if defined(CONFIG_BT_STACK_PTS)
int service_change_test(struct bt_gatt_indicate_params *params, const struct bt_conn *con) {
  u16_t sc_range[2];

  if (!params->attr) {
#if defined(BFLB_BLE_DISABLE_STATIC_ATTR)
    params->attr = &gatt_attrs[2];
#else
    params->attr = &_1_gatt_svc.attrs[2];
#endif
  }
  sc_range[0] = 0x000e;
  sc_range[1] = 0x001e;

  params->data = &sc_range[0];
  params->len  = sizeof(sc_range);

  if (bt_gatt_indicate(con, params)) {
    /* No connections to indicate */
    return;
  }
}
#endif

#if defined(CONFIG_BT_SETTINGS_CCC_STORE_ON_WRITE)
static struct gatt_ccc_store {
  struct bt_conn       *conn_list[CONFIG_BT_MAX_CONN];
  struct k_delayed_work work;
} gatt_ccc_store;

static bool gatt_ccc_conn_is_queued(struct bt_conn *conn) { return (conn == gatt_ccc_store.conn_list[bt_conn_index(conn)]); }

static void gatt_ccc_conn_unqueue(struct bt_conn *conn) {
  u8_t index = bt_conn_index(conn);

  if (gatt_ccc_store.conn_list[index] != NULL) {
    bt_conn_unref(gatt_ccc_store.conn_list[index]);
    gatt_ccc_store.conn_list[index] = NULL;
  }
}

static bool gatt_ccc_conn_queue_is_empty(void) {
  for (size_t i = 0; i < CONFIG_BT_MAX_CONN; i++) {
    if (gatt_ccc_store.conn_list[i]) {
      return false;
    }
  }

  return true;
}

static void ccc_delayed_store(struct k_work *work) {
  struct gatt_ccc_store *ccc_store = CONTAINER_OF(work, struct gatt_ccc_store, work);

  for (size_t i = 0; i < CONFIG_BT_MAX_CONN; i++) {
    struct bt_conn *conn = ccc_store->conn_list[i];

    if (!conn) {
      continue;
    }

    if (bt_addr_le_is_bonded(conn->id, &conn->le.dst)) {
      bt_gatt_store_ccc(conn->id, &conn->le.dst);
      bt_conn_unref(conn);
      ccc_store->conn_list[i] = NULL;
    }
  }
}
#endif

void bt_gatt_init(void) {
  if (!atomic_cas(&init, 0, 1)) {
    return;
  }

#if defined(BFLB_BLE_DISABLE_STATIC_ATTR)
  /* Register mandatory services */
  gatt_register(&gap_svc);
  gatt_register(&gatt_svc);

#else
  Z_STRUCT_SECTION_FOREACH(bt_gatt_service_static, svc) { last_static_handle += svc->attr_count; }
#endif

#if defined(CONFIG_BT_GATT_CACHING)
  k_delayed_work_init(&db_hash_work, db_hash_process);

  /* Submit work to Generate initial hash as there could be static
   * services already in the database.
   */
  k_delayed_work_submit(&db_hash_work, DB_HASH_TIMEOUT);
#endif /* CONFIG_BT_GATT_CACHING */

  if (IS_ENABLED(CONFIG_BT_GATT_SERVICE_CHANGED)) {
    k_delayed_work_init(&gatt_sc.work, sc_process);
    if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
      /* Make sure to not send SC indications until SC
       * settings are loaded
       */
      atomic_set_bit(gatt_sc.flags, SC_INDICATE_PENDING);
    }
  }

#if defined(CONFIG_BT_SETTINGS_CCC_STORE_ON_WRITE)
  k_delayed_work_init(&gatt_ccc_store.work, ccc_delayed_store);
#endif
}

#if defined(BFLB_BLE)
void bt_gatt_deinit(void) {
#if defined(CONFIG_BT_GATT_CACHING)
  k_delayed_work_del_timer(&db_hash_work);
#endif

  if (IS_ENABLED(CONFIG_BT_GATT_SERVICE_CHANGED)) {
    k_delayed_work_del_timer(&gatt_sc.work);
  }

#if defined(CONFIG_BT_SETTINGS_CCC_STORE_ON_WRITE)
  k_delayed_work_del_timer(&gatt_ccc_store.work);
#endif
}
#endif

#if defined(CONFIG_BT_GATT_DYNAMIC_DB) || (defined(CONFIG_BT_GATT_CACHING) && defined(CONFIG_BT_SETTINGS))
static void sc_indicate(u16_t start, u16_t end) {
  BT_DBG("start 0x%04x end 0x%04x", start, end);

#if defined(BFLB_BLE_PATCH_SET_SCRANGE_CHAGD_ONLY_IN_CONNECTED_STATE)
  struct bt_conn *conn = bt_conn_lookup_state_le(NULL, BT_CONN_CONNECTED);
  if (conn) {
#endif
    if (!atomic_test_and_set_bit(gatt_sc.flags, SC_RANGE_CHANGED)) {
      gatt_sc.start = start;
      gatt_sc.end   = end;
      goto submit;
    }
#if defined(BFLB_BLE_PATCH_SET_SCRANGE_CHAGD_ONLY_IN_CONNECTED_STATE)
  }
#endif

  if (!update_range(&gatt_sc.start, &gatt_sc.end, start, end)) {
    return;
  }

submit:
  if (atomic_test_bit(gatt_sc.flags, SC_INDICATE_PENDING)) {
    BT_DBG("indicate pending, waiting until complete...");
    return;
  }

#if defined(BFLB_BLE_PATCH_SET_SCRANGE_CHAGD_ONLY_IN_CONNECTED_STATE)
  if (conn) {
#endif
    /* Reschedule since the range has changed */
    k_delayed_work_submit(&gatt_sc.work, SC_TIMEOUT);
#if defined(BFLB_BLE_PATCH_SET_SCRANGE_CHAGD_ONLY_IN_CONNECTED_STATE)
    bt_conn_unref(conn);
  }
#endif
}
#endif /* BT_GATT_DYNAMIC_DB || (BT_GATT_CACHING && BT_SETTINGS) */

#if defined(CONFIG_BT_GATT_DYNAMIC_DB)
static void db_changed(void) {
#if defined(CONFIG_BT_GATT_CACHING)
  int i;

  k_delayed_work_submit(&db_hash_work, DB_HASH_TIMEOUT);

  for (i = 0; i < ARRAY_SIZE(cf_cfg); i++) {
    struct gatt_cf_cfg *cfg = &cf_cfg[i];

    if (!bt_addr_le_cmp(&cfg->peer, BT_ADDR_LE_ANY)) {
      continue;
    }

    if (CF_ROBUST_CACHING(cfg)) {
      /* Core Spec 5.1 | Vol 3, Part G, 2.5.2.1 Robust Caching
       *... the database changes again before the client
       * becomes change-aware in which case the error response
       * shall be sent again.
       */
      atomic_clear_bit(cfg->flags, CF_OUT_OF_SYNC);
      if (atomic_test_and_clear_bit(cfg->flags, CF_CHANGE_AWARE)) {
        BT_DBG("%s change-unaware", bt_addr_le_str(&cfg->peer));
      }
    }
  }
#endif
}

int bt_gatt_service_register(struct bt_gatt_service *svc) {
  int err;

  __ASSERT(svc, "invalid parameters\n");
  __ASSERT(svc->attrs, "invalid parameters\n");
  __ASSERT(svc->attr_count, "invalid parameters\n");

  /* Init GATT core services */
  bt_gatt_init();

  /* Do no allow to register mandatory services twice */
  if (!bt_uuid_cmp(svc->attrs[0].uuid, BT_UUID_GAP) || !bt_uuid_cmp(svc->attrs[0].uuid, BT_UUID_GATT)) {
    return -EALREADY;
  }

  err = gatt_register(svc);
  if (err < 0) {
    return err;
  }

  sc_indicate(svc->attrs[0].handle, svc->attrs[svc->attr_count - 1].handle);

  db_changed();

  return 0;
}

int bt_gatt_service_unregister(struct bt_gatt_service *svc) {
  __ASSERT(svc, "invalid parameters\n");

  if (!sys_slist_find_and_remove(&db, &svc->node)) {
    return -ENOENT;
  }

  sc_indicate(svc->attrs[0].handle, svc->attrs[svc->attr_count - 1].handle);

  db_changed();

  return 0;
}
#endif /* CONFIG_BT_GATT_DYNAMIC_DB */

ssize_t bt_gatt_attr_read(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t buf_len, u16_t offset, const void *value, u16_t value_len) {
  u16_t len;
#if defined(CONFIG_BT_STACK_PTS)
  u8_t *data = NULL;
  u8_t  i    = 0;
#endif

  if (offset > value_len) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
  }

  len = MIN(buf_len, value_len - offset);

  BT_DBG("handle 0x%04x offset %u length %u", attr->handle, offset, len);

  memcpy(buf, (u8_t *)value + offset, len);

#if defined(CONFIG_BT_STACK_PTS)
  /* PTS sends a request to iut read all primary services it contains.
   * Set event flags to avoid comflicts when other test cases need to add reference codes.
   */
  if (event_flag == att_read_by_group_type_ind) {
    data = (u8_t *)buf;
    for (i = 0; i < len; i++) {
      BT_PTS("%s:handle = [0x%04x], data[%d] = [0x%x]\r\n", __func__, attr->handle, i, data[i]);
    }
  }
#endif

  return len;
}

ssize_t bt_gatt_attr_read_service(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  struct bt_uuid *uuid = attr->user_data;

  if (uuid->type == BT_UUID_TYPE_16) {
    u16_t uuid16 = sys_cpu_to_le16(BT_UUID_16(uuid)->val);

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &uuid16, 2);
  }

  return bt_gatt_attr_read(conn, attr, buf, len, offset, BT_UUID_128(uuid)->val, 16);
}

struct gatt_incl {
  u16_t start_handle;
  u16_t end_handle;
  u16_t uuid16;
} __packed;

static u8_t get_service_handles(const struct bt_gatt_attr *attr, void *user_data) {
  struct gatt_incl *include = user_data;

  /* Stop if attribute is a service */
  if (!bt_uuid_cmp(attr->uuid, BT_UUID_GATT_PRIMARY) || !bt_uuid_cmp(attr->uuid, BT_UUID_GATT_SECONDARY)) {
    return BT_GATT_ITER_STOP;
  }

  include->end_handle = attr->handle;

  return BT_GATT_ITER_CONTINUE;
}

#if !defined(BFLB_BLE_DISABLE_STATIC_ATTR)
static u16_t find_static_attr(const struct bt_gatt_attr *attr) {
  u16_t handle = 1;

  Z_STRUCT_SECTION_FOREACH(bt_gatt_service_static, static_svc) {
    for (int i = 0; i < static_svc->attr_count; i++, handle++) {
      if (attr == &static_svc->attrs[i]) {
        return handle;
      }
    }
  }

  return 0;
}
#endif

ssize_t bt_gatt_attr_read_included(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  struct bt_gatt_attr *incl = attr->user_data;
#if defined(BFLB_BLE_DISABLE_STATIC_ATTR)
  u16_t handle = incl->handle;
#else
  u16_t handle = incl->handle ?: find_static_attr(incl);
#endif
  struct bt_uuid  *uuid = incl->user_data;
  struct gatt_incl pdu;
  u8_t             value_len;

  /* first attr points to the start handle */
  pdu.start_handle = sys_cpu_to_le16(handle);
  value_len        = sizeof(pdu.start_handle) + sizeof(pdu.end_handle);

  /*
   * Core 4.2, Vol 3, Part G, 3.2,
   * The Service UUID shall only be present when the UUID is a
   * 16-bit Bluetooth UUID.
   */
  if (uuid->type == BT_UUID_TYPE_16) {
    pdu.uuid16 = sys_cpu_to_le16(BT_UUID_16(uuid)->val);
    value_len += sizeof(pdu.uuid16);
  }

  /* Lookup for service end handle */
  bt_gatt_foreach_attr(handle + 1, 0xffff, get_service_handles, &pdu);

  return bt_gatt_attr_read(conn, attr, buf, len, offset, &pdu, value_len);
}

struct gatt_chrc {
  u8_t  properties;
  u16_t value_handle;
  union {
    u16_t uuid16;
    u8_t  uuid[16];
  };
} __packed;

uint16_t bt_gatt_attr_value_handle(const struct bt_gatt_attr *attr) {
  u16_t handle = 0;

  if ((attr != NULL) && (attr->read == bt_gatt_attr_read_chrc)) {
    struct bt_gatt_chrc *chrc = attr->user_data;

    handle = chrc->value_handle;
#if !defined(BFLB_BLE_DISABLE_STATIC_ATTR)
    if (handle == 0) {
      /* Fall back to Zephyr value handle policy */
      handle = (attr->handle ?: find_static_attr(attr)) + 1U;
    }
#endif
  }

  return handle;
}

ssize_t bt_gatt_attr_read_chrc(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  struct bt_gatt_chrc *chrc = attr->user_data;
  struct gatt_chrc     pdu;
  u8_t                 value_len;

  pdu.properties = chrc->properties;
  /* BLUETOOTH SPECIFICATION Version 4.2 [Vol 3, Part G] page 534:
   * 3.3.2 Characteristic Value Declaration
   * The Characteristic Value declaration contains the value of the
   * characteristic. It is the first Attribute after the characteristic
   * declaration. All characteristic definitions shall have a
   * Characteristic Value declaration.
   */
  pdu.value_handle = sys_cpu_to_le16(bt_gatt_attr_value_handle(attr));

  value_len = sizeof(pdu.properties) + sizeof(pdu.value_handle);

  if (chrc->uuid->type == BT_UUID_TYPE_16) {
    pdu.uuid16 = sys_cpu_to_le16(BT_UUID_16(chrc->uuid)->val);
    value_len += 2U;
  } else {
    memcpy(pdu.uuid, BT_UUID_128(chrc->uuid)->val, 16);
    value_len += 16U;
  }

  return bt_gatt_attr_read(conn, attr, buf, len, offset, &pdu, value_len);
}

static u8_t gatt_foreach_iter(const struct bt_gatt_attr *attr, u16_t start_handle, u16_t end_handle, const struct bt_uuid *uuid, const void *attr_data, uint16_t *num_matches, bt_gatt_attr_func_t func,
                              void *user_data) {
  u8_t result;

  /* Stop if over the requested range */
  if (attr->handle > end_handle) {
    return BT_GATT_ITER_STOP;
  }

  /* Check if attribute handle is within range */
  if (attr->handle < start_handle) {
    return BT_GATT_ITER_CONTINUE;
  }

  /* Match attribute UUID if set */
  if (uuid && bt_uuid_cmp(uuid, attr->uuid)) {
    return BT_GATT_ITER_CONTINUE;
  }

  /* Match attribute user_data if set */
  if (attr_data && attr_data != attr->user_data) {
    return BT_GATT_ITER_CONTINUE;
  }

  *num_matches -= 1;

  result = func(attr, user_data);

  if (!*num_matches) {
    return BT_GATT_ITER_STOP;
  }

  return result;
}

static void foreach_attr_type_dyndb(u16_t start_handle, u16_t end_handle, const struct bt_uuid *uuid, const void *attr_data, uint16_t num_matches, bt_gatt_attr_func_t func, void *user_data) {
#if defined(CONFIG_BT_GATT_DYNAMIC_DB)
  int i;

  struct bt_gatt_service *svc;

  SYS_SLIST_FOR_EACH_CONTAINER(&db, svc, node) {
    struct bt_gatt_service *next;

    next = SYS_SLIST_PEEK_NEXT_CONTAINER(svc, node);
    if (next) {
      /* Skip ahead if start is not within service handles */
      if (next->attrs[0].handle <= start_handle) {
        continue;
      }
    }

    for (i = 0; i < svc->attr_count; i++) {
      struct bt_gatt_attr *attr = &svc->attrs[i];

      if (gatt_foreach_iter(attr, start_handle, end_handle, uuid, attr_data, &num_matches, func, user_data) == BT_GATT_ITER_STOP) {
        return;
      }
    }
  }
#endif /* CONFIG_BT_GATT_DYNAMIC_DB */
}

void bt_gatt_foreach_attr_type(u16_t start_handle, u16_t end_handle, const struct bt_uuid *uuid, const void *attr_data, uint16_t num_matches, bt_gatt_attr_func_t func, void *user_data) {
  int i;

  if (!num_matches) {
    num_matches = UINT16_MAX;
  }
#if !defined(BFLB_BLE_DISABLE_STATIC_ATTR)
  if (start_handle <= last_static_handle) {
    u16_t handle = 1;

    Z_STRUCT_SECTION_FOREACH(bt_gatt_service_static, static_svc) {
      /* Skip ahead if start is not within service handles */
      if (handle + static_svc->attr_count < start_handle) {
        handle += static_svc->attr_count;
        continue;
      }

      for (i = 0; i < static_svc->attr_count; i++, handle++) {
        struct bt_gatt_attr attr;

        memcpy(&attr, &static_svc->attrs[i], sizeof(attr));

        attr.handle = handle;

        if (gatt_foreach_iter(&attr, start_handle, end_handle, uuid, attr_data, &num_matches, func, user_data) == BT_GATT_ITER_STOP) {
          return;
        }
      }
    }
  }
#endif
  /* Iterate over dynamic db */
  foreach_attr_type_dyndb(start_handle, end_handle, uuid, attr_data, num_matches, func, user_data);
}

static u8_t find_next(const struct bt_gatt_attr *attr, void *user_data) {
  struct bt_gatt_attr **next = user_data;

  *next = (struct bt_gatt_attr *)attr;

  return BT_GATT_ITER_STOP;
}

struct bt_gatt_attr *bt_gatt_attr_next(const struct bt_gatt_attr *attr) {
  struct bt_gatt_attr *next = NULL;
#if defined(BFLB_BLE_DISABLE_STATIC_ATTR)
  u16_t handle = attr->handle;
#else
  u16_t handle = attr->handle ?: find_static_attr(attr);
#endif
  bt_gatt_foreach_attr(handle + 1, handle + 1, find_next, &next);

  return next;
}

static void clear_ccc_cfg(struct bt_gatt_ccc_cfg *cfg) {
  bt_addr_le_copy(&cfg->peer, BT_ADDR_LE_ANY);
  cfg->id    = 0U;
  cfg->value = 0U;
}

static struct bt_gatt_ccc_cfg *find_ccc_cfg(const struct bt_conn *conn, struct _bt_gatt_ccc *ccc) {
  for (size_t i = 0; i < ARRAY_SIZE(ccc->cfg); i++) {
    if (conn) {
      if (conn->id == ccc->cfg[i].id && !bt_conn_addr_le_cmp(conn, &ccc->cfg[i].peer)) {
        return &ccc->cfg[i];
      }
    } else if (!bt_addr_le_cmp(&ccc->cfg[i].peer, BT_ADDR_LE_ANY)) {
      return &ccc->cfg[i];
    }
  }

  return NULL;
}

ssize_t bt_gatt_attr_read_ccc(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  struct _bt_gatt_ccc          *ccc = attr->user_data;
  const struct bt_gatt_ccc_cfg *cfg;
  u16_t                         value;

  cfg = find_ccc_cfg(conn, ccc);
  if (cfg) {
    value = sys_cpu_to_le16(cfg->value);
  } else {
    /* Default to disable if there is no cfg for the peer */
    value = 0x0000;
  }

  return bt_gatt_attr_read(conn, attr, buf, len, offset, &value, sizeof(value));
}

static void gatt_ccc_changed(const struct bt_gatt_attr *attr, struct _bt_gatt_ccc *ccc) {
  int   i;
  u16_t value = 0x0000;

  for (i = 0; i < ARRAY_SIZE(ccc->cfg); i++) {
    if (ccc->cfg[i].value > value) {
      value = ccc->cfg[i].value;
    }
  }

  BT_DBG("ccc %p value 0x%04x", ccc, value);

  if (value != ccc->value) {
    ccc->value = value;
    if (ccc->cfg_changed) {
      ccc->cfg_changed(attr, value);
    }
  }
}

ssize_t bt_gatt_attr_write_ccc(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, u16_t len, u16_t offset, u8_t flags) {
  struct _bt_gatt_ccc    *ccc = attr->user_data;
  struct bt_gatt_ccc_cfg *cfg;
  u16_t                   value;

  if (offset) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
  }

  if (!len || len > sizeof(u16_t)) {
    return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
  }

  if (len < sizeof(u16_t)) {
    value = *(u8_t *)buf;
  } else {
    value = sys_get_le16(buf);
  }

  cfg = find_ccc_cfg(conn, ccc);
  if (!cfg) {
    /* If there's no existing entry, but the new value is zero,
     * we don't need to do anything, since a disabled CCC is
     * behavioraly the same as no written CCC.
     */
    if (!value) {
      return len;
    }

    cfg = find_ccc_cfg(NULL, ccc);
    if (!cfg) {
      BT_WARN("No space to store CCC cfg");
      return BT_GATT_ERR(BT_ATT_ERR_INSUFFICIENT_RESOURCES);
    }

    bt_addr_le_copy(&cfg->peer, &conn->le.dst);
    cfg->id = conn->id;
  }

  /* Confirm write if cfg is managed by application */
  if (ccc->cfg_write && !ccc->cfg_write(conn, attr, value)) {
    return BT_GATT_ERR(BT_ATT_ERR_WRITE_NOT_PERMITTED);
  }

  cfg->value = value;

  BT_DBG("handle 0x%04x value %u", attr->handle, cfg->value);

  /* Update cfg if don't match */
  if (cfg->value != ccc->value) {
    gatt_ccc_changed(attr, ccc);

#if defined(CONFIG_BT_SETTINGS_CCC_STORE_ON_WRITE)
    if ((!gatt_ccc_conn_is_queued(conn)) && bt_addr_le_is_bonded(conn->id, &conn->le.dst)) {
      /* Store the connection with the same index it has in
       * the conns array
       */
      gatt_ccc_store.conn_list[bt_conn_index(conn)] = bt_conn_ref(conn);
      k_delayed_work_submit(&gatt_ccc_store.work, CCC_STORE_DELAY);
    }
#endif
  }

  /* Disabled CCC is the same as no configured CCC, so clear the entry */
  if (!value) {
    clear_ccc_cfg(cfg);
  }

  return len;
}

ssize_t bt_gatt_attr_read_cep(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  const struct bt_gatt_cep *value = attr->user_data;
  u16_t                     props = sys_cpu_to_le16(value->properties);

  return bt_gatt_attr_read(conn, attr, buf, len, offset, &props, sizeof(props));
}

ssize_t bt_gatt_attr_read_cud(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  const char *value = attr->user_data;

  return bt_gatt_attr_read(conn, attr, buf, len, offset, value, strlen(value));
}

ssize_t bt_gatt_attr_read_cpf(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, u16_t len, u16_t offset) {
  const struct bt_gatt_cpf *value = attr->user_data;

  return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(*value));
}

struct notify_data {
  int   err;
  u16_t type;
  union {
    struct bt_gatt_notify_params   *nfy_params;
    struct bt_gatt_indicate_params *ind_params;
  };
};

static int gatt_notify(struct bt_conn *conn, u16_t handle, struct bt_gatt_notify_params *params) {
  struct net_buf       *buf;
  struct bt_att_notify *nfy;

#if defined(CONFIG_BT_GATT_ENFORCE_CHANGE_UNAWARE)
  /* BLUETOOTH CORE SPECIFICATION Version 5.1 | Vol 3, Part G page 2350:
   * Except for the Handle Value indication, the  server shall not send
   * notifications and indications to such a client until it becomes
   * change-aware.
   */
  if (!bt_gatt_change_aware(conn, false)) {
    return -EAGAIN;
  }
#endif

  buf = bt_att_create_pdu(conn, BT_ATT_OP_NOTIFY, sizeof(*nfy) + params->len);
  if (!buf) {
    BT_WARN("No buffer available to send notification");
    return -ENOMEM;
  }

  BT_DBG("conn %p handle 0x%04x", conn, handle);

  nfy         = net_buf_add(buf, sizeof(*nfy));
  nfy->handle = sys_cpu_to_le16(handle);

  net_buf_add(buf, params->len);
  memcpy(nfy->value, params->data, params->len);

  return bt_att_send(conn, buf, params->func, params->user_data);
}

static void gatt_indicate_rsp(struct bt_conn *conn, u8_t err, const void *pdu, u16_t length, void *user_data) {
  struct bt_gatt_indicate_params *params = user_data;

  params->func(conn, params->attr, err);
}

static int gatt_send(struct bt_conn *conn, struct net_buf *buf, bt_att_func_t func, void *params, bt_att_destroy_t destroy) {
  int err;

  if (params) {
    struct bt_att_req *req = params;
    req->buf               = buf;
    req->func              = func;
    req->destroy           = destroy;

    err = bt_att_req_send(conn, req);
  } else {
    err = bt_att_send(conn, buf, NULL, NULL);
  }

  if (err) {
    BT_ERR("Error sending ATT PDU: %d", err);
  }

  return err;
}

static int gatt_indicate(struct bt_conn *conn, u16_t handle, struct bt_gatt_indicate_params *params) {
  struct net_buf         *buf;
  struct bt_att_indicate *ind;

#if defined(CONFIG_BT_GATT_ENFORCE_CHANGE_UNAWARE)
  /* BLUETOOTH CORE SPECIFICATION Version 5.1 | Vol 3, Part G page 2350:
   * Except for the Handle Value indication, the  server shall not send
   * notifications and indications to such a client until it becomes
   * change-aware.
   */
  if (!(params->func && (params->func == sc_indicate_rsp || params->func == sc_restore_rsp)) && !bt_gatt_change_aware(conn, false)) {
    return -EAGAIN;
  }
#endif

  buf = bt_att_create_pdu(conn, BT_ATT_OP_INDICATE, sizeof(*ind) + params->len);
  if (!buf) {
    BT_WARN("No buffer available to send indication");
    return -ENOMEM;
  }

  BT_DBG("conn %p handle 0x%04x", conn, handle);

  ind         = net_buf_add(buf, sizeof(*ind));
  ind->handle = sys_cpu_to_le16(handle);

  net_buf_add(buf, params->len);
  memcpy(ind->value, params->data, params->len);

  if (!params->func) {
    return gatt_send(conn, buf, NULL, NULL, NULL);
  }

  return gatt_send(conn, buf, gatt_indicate_rsp, params, NULL);
}

static u8_t notify_cb(const struct bt_gatt_attr *attr, void *user_data) {
  struct notify_data  *data = user_data;
  struct _bt_gatt_ccc *ccc;
  size_t               i;

  /* Check attribute user_data must be of type struct _bt_gatt_ccc */
  if (attr->write != bt_gatt_attr_write_ccc) {
    return BT_GATT_ITER_CONTINUE;
  }

  ccc = attr->user_data;

  /* Save Service Changed data if peer is not connected */
  if (IS_ENABLED(CONFIG_BT_GATT_SERVICE_CHANGED) && ccc == &sc_ccc) {
    for (i = 0; i < ARRAY_SIZE(sc_cfg); i++) {
      struct gatt_sc_cfg *cfg = &sc_cfg[i];
      struct bt_conn     *conn;

      if (!bt_addr_le_cmp(&cfg->peer, BT_ADDR_LE_ANY)) {
        continue;
      }

      conn = bt_conn_lookup_state_le(&cfg->peer, BT_CONN_CONNECTED);
      if (!conn) {
        struct sc_data *sc;

        sc = (struct sc_data *)data->ind_params->data;
        sc_save(cfg->id, &cfg->peer, sys_le16_to_cpu(sc->start), sys_le16_to_cpu(sc->end));
        continue;
      }
      bt_conn_unref(conn);
    }
  }

  /* Notify all peers configured */
  for (i = 0; i < ARRAY_SIZE(ccc->cfg); i++) {
    struct bt_gatt_ccc_cfg *cfg = &ccc->cfg[i];
    struct bt_conn         *conn;
    int                     err;

    /* Check if config value matches data type since consolidated
     * value may be for a different peer.
     */
    if (cfg->value != data->type) {
      continue;
    }

    conn = bt_conn_lookup_addr_le(cfg->id, &cfg->peer);
    if (!conn) {
      continue;
    }

    if (conn->state != BT_CONN_CONNECTED) {
      bt_conn_unref(conn);
      continue;
    }

    /* Confirm match if cfg is managed by application */
    if (ccc->cfg_match && !ccc->cfg_match(conn, attr)) {
      bt_conn_unref(conn);
      continue;
    }

    if (data->type == BT_GATT_CCC_INDICATE) {
      err = gatt_indicate(conn, attr->handle - 1, data->ind_params);
    } else {
      err = gatt_notify(conn, attr->handle - 1, data->nfy_params);
    }

    bt_conn_unref(conn);

    if (err < 0) {
      return BT_GATT_ITER_STOP;
    }

    data->err = 0;
  }

  return BT_GATT_ITER_CONTINUE;
}

static u8_t match_uuid(const struct bt_gatt_attr *attr, void *user_data) {
  const struct bt_gatt_attr **found = user_data;

  *found = attr;

  return BT_GATT_ITER_STOP;
}

int bt_gatt_notify_cb(struct bt_conn *conn, struct bt_gatt_notify_params *params) {
  struct notify_data         data;
  const struct bt_gatt_attr *attr;
  u16_t                      handle;

  __ASSERT(params, "invalid parameters\n");
  __ASSERT(params->attr, "invalid parameters\n");

  attr = params->attr;

  if (conn && conn->state != BT_CONN_CONNECTED) {
    return -ENOTCONN;
  }
#if !defined(BFLB_BLE_DISABLE_STATIC_ATTR)
  handle = attr->handle ?: find_static_attr(attr);
#endif
  if (!handle) {
    return -ENOENT;
  }

  /* Lookup UUID if it was given */
  if (params->uuid) {
    attr = NULL;

    bt_gatt_foreach_attr_type(handle, 0xffff, params->uuid, NULL, 1, match_uuid, &attr);
    if (!attr) {
      return -ENOENT;
    }
#if !defined(BFLB_BLE_DISABLE_STATIC_ATTR)
    handle = attr->handle ?: find_static_attr(attr);
#endif
    if (!handle) {
      return -ENOENT;
    }
  }

  /* Check if attribute is a characteristic then adjust the handle */
  if (!bt_uuid_cmp(attr->uuid, BT_UUID_GATT_CHRC)) {
    struct bt_gatt_chrc *chrc = attr->user_data;

    if (!(chrc->properties & BT_GATT_CHRC_NOTIFY)) {
      return -EINVAL;
    }

    handle = bt_gatt_attr_value_handle(attr);
  }

  if (conn) {
    return gatt_notify(conn, handle, params);
  }

  data.err        = -ENOTCONN;
  data.type       = BT_GATT_CCC_NOTIFY;
  data.nfy_params = params;

  bt_gatt_foreach_attr_type(handle, 0xffff, BT_UUID_GATT_CCC, NULL, 1, notify_cb, &data);

  return data.err;
}

int bt_gatt_indicate(struct bt_conn *conn, struct bt_gatt_indicate_params *params) {
  struct notify_data         data;
  const struct bt_gatt_attr *attr;
  u16_t                      handle;

  __ASSERT(params, "invalid parameters\n");
  __ASSERT(params->attr, "invalid parameters\n");

  attr = params->attr;

  if (conn && conn->state != BT_CONN_CONNECTED) {
    return -ENOTCONN;
  }
#if !defined(BFLB_BLE_DISABLE_STATIC_ATTR)
  handle = attr->handle ?: find_static_attr(attr);
#endif
  if (!handle) {
    return -ENOENT;
  }

  /* Lookup UUID if it was given */
  if (params->uuid) {
    attr = NULL;

    bt_gatt_foreach_attr_type(handle, 0xffff, params->uuid, NULL, 1, match_uuid, &attr);
    if (!attr) {
      return -ENOENT;
    }
#if !defined(BFLB_BLE_DISABLE_STATIC_ATTR)
    handle = attr->handle ?: find_static_attr(attr);
#endif
    if (!handle) {
      return -ENOENT;
    }
  }

  /* Check if attribute is a characteristic then adjust the handle */
  if (!bt_uuid_cmp(attr->uuid, BT_UUID_GATT_CHRC)) {
    struct bt_gatt_chrc *chrc = params->attr->user_data;

    if (!(chrc->properties & BT_GATT_CHRC_INDICATE)) {
      return -EINVAL;
    }

    handle = bt_gatt_attr_value_handle(params->attr);
  }

  if (conn) {
    return gatt_indicate(conn, handle, params);
  }

  data.err        = -ENOTCONN;
  data.type       = BT_GATT_CCC_INDICATE;
  data.ind_params = params;

  bt_gatt_foreach_attr_type(handle, 0xffff, BT_UUID_GATT_CCC, NULL, 1, notify_cb, &data);

  return data.err;
}

u16_t bt_gatt_get_mtu(struct bt_conn *conn) { return bt_att_get_mtu(conn); }

u8_t bt_gatt_check_perm(struct bt_conn *conn, const struct bt_gatt_attr *attr, u8_t mask) {
  if ((mask & BT_GATT_PERM_READ) && (!(attr->perm & BT_GATT_PERM_READ_MASK) || !attr->read)) {
    return BT_ATT_ERR_READ_NOT_PERMITTED;
  }

  if ((mask & BT_GATT_PERM_WRITE) && (!(attr->perm & BT_GATT_PERM_WRITE_MASK) || !attr->write)) {
    return BT_ATT_ERR_WRITE_NOT_PERMITTED;
  }

  mask &= attr->perm;
  if (mask & BT_GATT_PERM_AUTHEN_MASK) {
    if (bt_conn_get_security(conn) < BT_SECURITY_L3) {
      return BT_ATT_ERR_AUTHENTICATION;
    }
  }

  if ((mask & BT_GATT_PERM_ENCRYPT_MASK)) {
#if defined(CONFIG_BT_SMP)
    if (!conn->encrypt) {
      return BT_ATT_ERR_INSUFFICIENT_ENCRYPTION;
    }
#else
    return BT_ATT_ERR_INSUFFICIENT_ENCRYPTION;
#endif /* CONFIG_BT_SMP */
  }

  return 0;
}

static void sc_restore_rsp(struct bt_conn *conn, const struct bt_gatt_attr *attr, u8_t err) {
#if defined(CONFIG_BT_GATT_CACHING)
  struct gatt_cf_cfg *cfg;
#endif

  BT_DBG("err 0x%02x", err);

#if defined(CONFIG_BT_GATT_CACHING)
  /* BLUETOOTH CORE SPECIFICATION Version 5.1 | Vol 3, Part G page 2347:
   * 2.5.2.1 Robust Caching
   * A connected client becomes change-aware when...
   * The client receives and confirms a Service Changed indication.
   */
  cfg = find_cf_cfg(conn);
  if (cfg && CF_ROBUST_CACHING(cfg)) {
    atomic_set_bit(cfg->flags, CF_CHANGE_AWARE);
    BT_DBG("%s change-aware", bt_addr_le_str(&cfg->peer));
  }
#endif
}

static struct bt_gatt_indicate_params sc_restore_params[CONFIG_BT_MAX_CONN];

static void sc_restore(struct bt_conn *conn) {
  struct gatt_sc_cfg *cfg;
  u16_t               sc_range[2];
  u8_t                index;

  cfg = find_sc_cfg(conn->id, &conn->le.dst);
  if (!cfg) {
    BT_DBG("no SC data found");
    return;
  }

  if (!(cfg->data.start || cfg->data.end)) {
    return;
  }

  BT_DBG("peer %s start 0x%04x end 0x%04x", bt_addr_le_str(&cfg->peer), cfg->data.start, cfg->data.end);

  sc_range[0] = sys_cpu_to_le16(cfg->data.start);
  sc_range[1] = sys_cpu_to_le16(cfg->data.end);

  index = bt_conn_index(conn);
#if defined(BFLB_BLE_DISABLE_STATIC_ATTR)
  sc_restore_params[index].attr = &gatt_attrs[2];
#else
  sc_restore_params[index].attr = &_1_gatt_svc.attrs[2];
#endif
  sc_restore_params[index].func = sc_restore_rsp;
  sc_restore_params[index].data = &sc_range[0];
  sc_restore_params[index].len  = sizeof(sc_range);

  if (bt_gatt_indicate(conn, &sc_restore_params[index])) {
    BT_ERR("SC restore indication failed");
  }

  /* Reset config data */
  sc_reset(cfg);
}

struct conn_data {
  struct bt_conn *conn;
  bt_security_t   sec;
};

static u8_t update_ccc(const struct bt_gatt_attr *attr, void *user_data) {
  struct conn_data    *data = user_data;
  struct bt_conn      *conn = data->conn;
  struct _bt_gatt_ccc *ccc;
  size_t               i;
  u8_t                 err;

  /* Check attribute user_data must be of type struct _bt_gatt_ccc */
  if (attr->write != bt_gatt_attr_write_ccc) {
    return BT_GATT_ITER_CONTINUE;
  }

  ccc = attr->user_data;

  for (i = 0; i < ARRAY_SIZE(ccc->cfg); i++) {
    /* Ignore configuration for different peer */
    if (bt_conn_addr_le_cmp(conn, &ccc->cfg[i].peer)) {
      continue;
    }

    /* Check if attribute requires encryption/authentication */
    err = bt_gatt_check_perm(conn, attr, BT_GATT_PERM_WRITE_MASK);
    if (err) {
      bt_security_t sec;

      if (err == BT_ATT_ERR_WRITE_NOT_PERMITTED) {
        BT_WARN("CCC %p not writable", attr);
        continue;
      }

      sec = BT_SECURITY_L2;

      if (err == BT_ATT_ERR_AUTHENTICATION) {
        sec = BT_SECURITY_L3;
      }

      /* Check if current security is enough */
      if (IS_ENABLED(CONFIG_BT_SMP) && bt_conn_get_security(conn) < sec) {
        if (data->sec < sec) {
          data->sec = sec;
        }
        continue;
      }
    }

    if (ccc->cfg[i].value) {
      gatt_ccc_changed(attr, ccc);
      if (IS_ENABLED(CONFIG_BT_GATT_SERVICE_CHANGED) && ccc == &sc_ccc) {
        sc_restore(conn);
      }
      return BT_GATT_ITER_CONTINUE;
    }
  }

  return BT_GATT_ITER_CONTINUE;
}

static u8_t disconnected_cb(const struct bt_gatt_attr *attr, void *user_data) {
  struct bt_conn      *conn = user_data;
  struct _bt_gatt_ccc *ccc;
  bool                 value_used;
  size_t               i;

  /* Check attribute user_data must be of type struct _bt_gatt_ccc */
  if (attr->write != bt_gatt_attr_write_ccc) {
    return BT_GATT_ITER_CONTINUE;
  }

  ccc = attr->user_data;

  /* If already disabled skip */
  if (!ccc->value) {
    return BT_GATT_ITER_CONTINUE;
  }

  /* Checking if all values are disabled */
  value_used = false;

  for (i = 0; i < ARRAY_SIZE(ccc->cfg); i++) {
    struct bt_gatt_ccc_cfg *cfg = &ccc->cfg[i];

    /* Ignore configurations with disabled value */
    if (!cfg->value) {
      continue;
    }

    if (conn->id != cfg->id || bt_conn_addr_le_cmp(conn, &cfg->peer)) {
      struct bt_conn *tmp;

      /* Skip if there is another peer connected */
      tmp = bt_conn_lookup_addr_le(cfg->id, &cfg->peer);
      if (tmp) {
        if (tmp->state == BT_CONN_CONNECTED) {
          value_used = true;
        }

        bt_conn_unref(tmp);
      }
    } else {
      /* Clear value if not paired */
      if (!bt_addr_le_is_bonded(conn->id, &conn->le.dst)) {
        clear_ccc_cfg(cfg);
      } else {
        /* Update address in case it has changed */
        bt_addr_le_copy(&cfg->peer, &conn->le.dst);
      }
    }
  }

  /* If all values are now disabled, reset value while disconnected */
  if (!value_used) {
    ccc->value = 0U;
    if (ccc->cfg_changed) {
      ccc->cfg_changed(attr, ccc->value);
    }

    BT_DBG("ccc %p reseted", ccc);
  }

  return BT_GATT_ITER_CONTINUE;
}

bool bt_gatt_is_subscribed(struct bt_conn *conn, const struct bt_gatt_attr *attr, u16_t ccc_value) {
  const struct _bt_gatt_ccc *ccc;

  __ASSERT(conn, "invalid parameter\n");
  __ASSERT(attr, "invalid parameter\n");

  if (conn->state != BT_CONN_CONNECTED) {
    return false;
  }

  /* Check if attribute is a characteristic declaration */
  if (!bt_uuid_cmp(attr->uuid, BT_UUID_GATT_CHRC)) {
    struct bt_gatt_chrc *chrc = attr->user_data;

    if (!(chrc->properties & (BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_INDICATE))) {
      /* Characteristic doesn't support subscription */
      return false;
    }

    attr = bt_gatt_attr_next(attr);
  }

  /* Check if attribute is a characteristic value */
  if (bt_uuid_cmp(attr->uuid, BT_UUID_GATT_CCC) != 0) {
    attr = bt_gatt_attr_next(attr);
  }

  /* Check if the attribute is the CCC Descriptor */
  if (bt_uuid_cmp(attr->uuid, BT_UUID_GATT_CCC) != 0) {
    return false;
  }

  ccc = attr->user_data;

  /* Check if the connection is subscribed */
  for (size_t i = 0; i < BT_GATT_CCC_MAX; i++) {
    if (conn->id == ccc->cfg[i].id && !bt_conn_addr_le_cmp(conn, &ccc->cfg[i].peer) && (ccc_value & ccc->cfg[i].value)) {
      return true;
    }
  }

  return false;
}

#if defined(CONFIG_BT_GATT_CLIENT)
void bt_gatt_notification(struct bt_conn *conn, u16_t handle, const void *data, u16_t length) {
  struct bt_gatt_subscribe_params *params, *tmp;

  BT_DBG("handle 0x%04x length %u", handle, length);

  SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&subscriptions, params, tmp, node) {
    if (bt_conn_addr_le_cmp(conn, &params->_peer) || handle != params->value_handle) {
      continue;
    }

    if (params->notify(conn, params, data, length) == BT_GATT_ITER_STOP) {
      bt_gatt_unsubscribe(conn, params);
    }
  }
}

static void update_subscription(struct bt_conn *conn, struct bt_gatt_subscribe_params *params) {
  if (params->_peer.type == BT_ADDR_LE_PUBLIC) {
    return;
  }

  /* Update address */
  bt_addr_le_copy(&params->_peer, &conn->le.dst);
}

static void gatt_subscription_remove(struct bt_conn *conn, sys_snode_t *prev, struct bt_gatt_subscribe_params *params) {
  /* Remove subscription from the list*/
  sys_slist_remove(&subscriptions, prev, &params->node);

  params->notify(conn, params, NULL, 0);
}

static void remove_subscriptions(struct bt_conn *conn) {
  struct bt_gatt_subscribe_params *params, *tmp;
  sys_snode_t                     *prev = NULL;

  /* Lookup existing subscriptions */
  SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&subscriptions, params, tmp, node) {
    if (bt_conn_addr_le_cmp(conn, &params->_peer)) {
      prev = &params->node;
      continue;
    }

    if (!bt_addr_le_is_bonded(conn->id, &conn->le.dst) || (atomic_test_bit(params->flags, BT_GATT_SUBSCRIBE_FLAG_VOLATILE))) {
      /* Remove subscription */
      params->value = 0U;
      gatt_subscription_remove(conn, prev, params);
    } else {
      update_subscription(conn, params);
      prev = &params->node;
    }
  }
}

static void gatt_mtu_rsp(struct bt_conn *conn, u8_t err, const void *pdu, u16_t length, void *user_data) {
  struct bt_gatt_exchange_params *params = user_data;

  params->func(conn, err, params);
}
#if defined(CONFIG_BLE_AT_CMD)
int bt_at_gatt_exchange_mtu(struct bt_conn *conn, struct bt_gatt_exchange_params *params, u16_t mtu_size) {
  struct bt_att_exchange_mtu_req *req;
  struct net_buf                 *buf;
  u16_t                           mtu;

  __ASSERT(conn, "invalid parameter\n");
  __ASSERT(params && params->func, "invalid parameters\n");

  if (conn->state != BT_CONN_CONNECTED) {
    return -ENOTCONN;
  }

  buf = bt_att_create_pdu(conn, BT_ATT_OP_MTU_REQ, sizeof(*req));
  if (!buf) {
    return -ENOMEM;
  }

  mtu = mtu_size;

#if defined(CONFIG_BLE_AT_CMD)
  set_mtu_size(mtu);
#endif

  BT_DBG("Client MTU %u", mtu);

  req      = net_buf_add(buf, sizeof(*req));
  req->mtu = sys_cpu_to_le16(mtu);

  return gatt_send(conn, buf, gatt_mtu_rsp, params, NULL);
}
#endif

int bt_gatt_exchange_mtu(struct bt_conn *conn, struct bt_gatt_exchange_params *params) {
  struct bt_att_exchange_mtu_req *req;
  struct net_buf                 *buf;
  u16_t                           mtu;

  __ASSERT(conn, "invalid parameter\n");
  __ASSERT(params && params->func, "invalid parameters\n");

  if (conn->state != BT_CONN_CONNECTED) {
    return -ENOTCONN;
  }

  buf = bt_att_create_pdu(conn, BT_ATT_OP_MTU_REQ, sizeof(*req));
  if (!buf) {
    return -ENOMEM;
  }

  mtu = BT_ATT_MTU;

  BT_DBG("Client MTU %u", mtu);

  req      = net_buf_add(buf, sizeof(*req));
  req->mtu = sys_cpu_to_le16(mtu);

  return gatt_send(conn, buf, gatt_mtu_rsp, params, NULL);
}

static void gatt_discover_next(struct bt_conn *conn, u16_t last_handle, struct bt_gatt_discover_params *params) {
  /* Skip if last_handle is not set */
  if (!last_handle) {
    goto discover;
  }

  /* Continue from the last found handle */
  params->start_handle = last_handle;
  if (params->start_handle < UINT16_MAX) {
    params->start_handle++;
  } else {
    goto done;
  }

  /* Stop if over the range or the requests */
  if (params->start_handle > params->end_handle) {
    goto done;
  }

discover:
  /* Discover next range */
  if (!bt_gatt_discover(conn, params)) {
    return;
  }

done:
  params->func(conn, NULL, params);
}

static void gatt_find_type_rsp(struct bt_conn *conn, u8_t err, const void *pdu, u16_t length, void *user_data) {
  const struct bt_att_find_type_rsp *rsp    = pdu;
  struct bt_gatt_discover_params    *params = user_data;
  u8_t                               i;
  u16_t                              end_handle = 0U, start_handle;

  BT_DBG("err 0x%02x", err);

  if (err) {
    goto done;
  }

  /* Parse attributes found */
  for (i = 0U; length >= sizeof(rsp->list[i]); i++, length -= sizeof(rsp->list[i])) {
    struct bt_gatt_attr        attr = {};
    struct bt_gatt_service_val value;

    start_handle = sys_le16_to_cpu(rsp->list[i].start_handle);
    end_handle   = sys_le16_to_cpu(rsp->list[i].end_handle);

    BT_DBG("start_handle 0x%04x end_handle 0x%04x", start_handle, end_handle);

    if (params->type == BT_GATT_DISCOVER_PRIMARY) {
      attr.uuid = BT_UUID_GATT_PRIMARY;
    } else {
      attr.uuid = BT_UUID_GATT_SECONDARY;
    }

    value.end_handle = end_handle;
    value.uuid       = params->uuid;

    attr.handle    = start_handle;
    attr.user_data = &value;

    if (params->func(conn, &attr, params) == BT_GATT_ITER_STOP) {
      return;
    }
  }

  /* Stop if could not parse the whole PDU */
  if (length > 0) {
    goto done;
  }

  gatt_discover_next(conn, end_handle, params);

  return;
done:
  params->func(conn, NULL, params);
}

static int gatt_find_type(struct bt_conn *conn, struct bt_gatt_discover_params *params) {
  struct net_buf              *buf;
  struct bt_att_find_type_req *req;
  struct bt_uuid              *uuid;

  buf = bt_att_create_pdu(conn, BT_ATT_OP_FIND_TYPE_REQ, sizeof(*req));
  if (!buf) {
    return -ENOMEM;
  }

  req               = net_buf_add(buf, sizeof(*req));
  req->start_handle = sys_cpu_to_le16(params->start_handle);
  req->end_handle   = sys_cpu_to_le16(params->end_handle);

  if (params->type == BT_GATT_DISCOVER_PRIMARY) {
    uuid = BT_UUID_GATT_PRIMARY;
  } else {
    uuid = BT_UUID_GATT_SECONDARY;
  }

  req->type = sys_cpu_to_le16(BT_UUID_16(uuid)->val);

  BT_DBG("uuid %s start_handle 0x%04x end_handle 0x%04x", bt_uuid_str(params->uuid), params->start_handle, params->end_handle);

  switch (params->uuid->type) {
  case BT_UUID_TYPE_16:
    net_buf_add_le16(buf, BT_UUID_16(params->uuid)->val);
    break;
  case BT_UUID_TYPE_128:
    net_buf_add_mem(buf, BT_UUID_128(params->uuid)->val, 16);
    break;
  default:
    BT_ERR("Unknown UUID type %u", params->uuid->type);
    net_buf_unref(buf);
    return -EINVAL;
  }

  return gatt_send(conn, buf, gatt_find_type_rsp, params, NULL);
}

static void read_included_uuid_cb(struct bt_conn *conn, u8_t err, const void *pdu, u16_t length, void *user_data) {
  struct bt_gatt_discover_params *params = user_data;
  struct bt_gatt_include          value;
  struct bt_gatt_attr            *attr;
  union {
    struct bt_uuid     uuid;
    struct bt_uuid_128 u128;
  } u;

  if (length != 16U) {
    BT_ERR("Invalid data len %u", length);
    params->func(conn, NULL, params);
    return;
  }

  value.start_handle = params->_included.start_handle;
  value.end_handle   = params->_included.end_handle;
  value.uuid         = &u.uuid;
  u.uuid.type        = BT_UUID_TYPE_128;
  memcpy(u.u128.val, pdu, length);

  BT_DBG("handle 0x%04x uuid %s start_handle 0x%04x "
         "end_handle 0x%04x\n",
         params->_included.attr_handle, bt_uuid_str(&u.uuid), value.start_handle, value.end_handle);

  /* Skip if UUID is set but doesn't match */
  if (params->uuid && bt_uuid_cmp(&u.uuid, params->uuid)) {
    goto next;
  }

  attr         = (&(struct bt_gatt_attr){
              .uuid      = BT_UUID_GATT_INCLUDE,
              .user_data = &value,
  });
  attr->handle = params->_included.attr_handle;

  if (params->func(conn, attr, params) == BT_GATT_ITER_STOP) {
    return;
  }
next:
  gatt_discover_next(conn, params->start_handle, params);

  return;
}

static int read_included_uuid(struct bt_conn *conn, struct bt_gatt_discover_params *params) {
  struct net_buf         *buf;
  struct bt_att_read_req *req;

  buf = bt_att_create_pdu(conn, BT_ATT_OP_READ_REQ, sizeof(*req));
  if (!buf) {
    return -ENOMEM;
  }

  req         = net_buf_add(buf, sizeof(*req));
  req->handle = sys_cpu_to_le16(params->_included.start_handle);

  BT_DBG("handle 0x%04x", params->_included.start_handle);

  return gatt_send(conn, buf, read_included_uuid_cb, params, NULL);
}

static u16_t parse_include(struct bt_conn *conn, const void *pdu, struct bt_gatt_discover_params *params, u16_t length) {
  const struct bt_att_read_type_rsp *rsp    = pdu;
  u16_t                              handle = 0U;
  struct bt_gatt_include             value;
  union {
    struct bt_uuid     uuid;
    struct bt_uuid_16  u16;
    struct bt_uuid_128 u128;
  } u;

  /* Data can be either in UUID16 or UUID128 */
  switch (rsp->len) {
  case 8: /* UUID16 */
    u.uuid.type = BT_UUID_TYPE_16;
    break;
  case 6: /* UUID128 */
    /* BLUETOOTH SPECIFICATION Version 4.2 [Vol 3, Part G] page 550
     * To get the included service UUID when the included service
     * uses a 128-bit UUID, the Read Request is used.
     */
    u.uuid.type = BT_UUID_TYPE_128;
    break;
  default:
    BT_ERR("Invalid data len %u", rsp->len);
    goto done;
  }

  /* Parse include found */
  for (length--, pdu = rsp->data; length >= rsp->len; length -= rsp->len, pdu = (const u8_t *)pdu + rsp->len) {
    struct bt_gatt_attr      *attr;
    const struct bt_att_data *data = pdu;
    struct gatt_incl         *incl = (void *)data->value;

    handle = sys_le16_to_cpu(data->handle);
    /* Handle 0 is invalid */
    if (!handle) {
      goto done;
    }

    /* Convert include data, bt_gatt_incl and gatt_incl
     * have different formats so the conversion have to be done
     * field by field.
     */
    value.start_handle = sys_le16_to_cpu(incl->start_handle);
    value.end_handle   = sys_le16_to_cpu(incl->end_handle);

    switch (u.uuid.type) {
    case BT_UUID_TYPE_16:
      value.uuid = &u.uuid;
      u.u16.val  = sys_le16_to_cpu(incl->uuid16);
      break;
    case BT_UUID_TYPE_128:
      params->_included.attr_handle  = handle;
      params->_included.start_handle = value.start_handle;
      params->_included.end_handle   = value.end_handle;

      return read_included_uuid(conn, params);
    }

    BT_DBG("handle 0x%04x uuid %s start_handle 0x%04x "
           "end_handle 0x%04x\n",
           handle, bt_uuid_str(&u.uuid), value.start_handle, value.end_handle);

    /* Skip if UUID is set but doesn't match */
    if (params->uuid && bt_uuid_cmp(&u.uuid, params->uuid)) {
      continue;
    }

    attr         = (&(struct bt_gatt_attr){
                .uuid      = BT_UUID_GATT_INCLUDE,
                .user_data = &value,
    });
    attr->handle = handle;

    if (params->func(conn, attr, params) == BT_GATT_ITER_STOP) {
      return 0;
    }
  }

  /* Whole PDU read without error */
  if (length == 0U && handle) {
    return handle;
  }

done:
  params->func(conn, NULL, params);
  return 0;
}

#define BT_GATT_CHRC(_uuid, _handle, _props)                                                                                                                                                           \
  BT_GATT_ATTRIBUTE(BT_UUID_GATT_CHRC, BT_GATT_PERM_READ, bt_gatt_attr_read_chrc, NULL,                                                                                                                \
                    (&(struct bt_gatt_chrc){                                                                                                                                                           \
                        .uuid         = _uuid,                                                                                                                                                         \
                        .value_handle = _handle,                                                                                                                                                       \
                        .properties   = _props,                                                                                                                                                        \
                    }))

static u16_t parse_characteristic(struct bt_conn *conn, const void *pdu, struct bt_gatt_discover_params *params, u16_t length) {
  const struct bt_att_read_type_rsp *rsp    = pdu;
  u16_t                              handle = 0U;
  union {
    struct bt_uuid     uuid;
    struct bt_uuid_16  u16;
    struct bt_uuid_128 u128;
  } u;

  /* Data can be either in UUID16 or UUID128 */
  switch (rsp->len) {
  case 7: /* UUID16 */
    u.uuid.type = BT_UUID_TYPE_16;
    break;
  case 21: /* UUID128 */
    u.uuid.type = BT_UUID_TYPE_128;
    break;
  default:
    BT_ERR("Invalid data len %u", rsp->len);
    goto done;
  }

  /* Parse characteristics found */
  for (length--, pdu = rsp->data; length >= rsp->len; length -= rsp->len, pdu = (const u8_t *)pdu + rsp->len) {
    struct bt_gatt_attr      *attr;
    const struct bt_att_data *data = pdu;
    struct gatt_chrc         *chrc = (void *)data->value;

    handle = sys_le16_to_cpu(data->handle);
    /* Handle 0 is invalid */
    if (!handle) {
      goto done;
    }

    switch (u.uuid.type) {
    case BT_UUID_TYPE_16:
      u.u16.val = sys_le16_to_cpu(chrc->uuid16);
      break;
    case BT_UUID_TYPE_128:
      memcpy(u.u128.val, chrc->uuid, sizeof(chrc->uuid));
      break;
    }

    BT_DBG("handle 0x%04x uuid %s properties 0x%02x", handle, bt_uuid_str(&u.uuid), chrc->properties);

#if defined(CONFIG_BT_STACK_PTS)
    if (event_flag != gatt_discover_chara) {
      /* Skip if UUID is set but doesn't match */
      if (params->uuid && bt_uuid_cmp(&u.uuid, params->uuid)) {
        continue;
      }
    }
#else
    /* Skip if UUID is set but doesn't match */
    if (params->uuid && bt_uuid_cmp(&u.uuid, params->uuid)) {
      continue;
    }
#endif

    attr         = (&(struct bt_gatt_attr)BT_GATT_CHRC(&u.uuid, chrc->value_handle, chrc->properties));
    attr->handle = handle;

    if (params->func(conn, attr, params) == BT_GATT_ITER_STOP) {
      return 0;
    }
  }

  /* Whole PDU read without error */
  if (length == 0U && handle) {
    return handle;
  }

done:
  params->func(conn, NULL, params);
  return 0;
}

static void gatt_read_type_rsp(struct bt_conn *conn, u8_t err, const void *pdu, u16_t length, void *user_data) {
  struct bt_gatt_discover_params *params = user_data;
  u16_t                           handle;

  BT_DBG("err 0x%02x", err);

  if (err) {
    params->func(conn, NULL, params);
    return;
  }

  if (params->type == BT_GATT_DISCOVER_INCLUDE) {
    handle = parse_include(conn, pdu, params, length);
  } else {
    handle = parse_characteristic(conn, pdu, params, length);
  }

  if (!handle) {
    return;
  }

  gatt_discover_next(conn, handle, params);
}

static int gatt_read_type(struct bt_conn *conn, struct bt_gatt_discover_params *params) {
  struct net_buf              *buf;
  struct bt_att_read_type_req *req;

  buf = bt_att_create_pdu(conn, BT_ATT_OP_READ_TYPE_REQ, sizeof(*req));
  if (!buf) {
    return -ENOMEM;
  }

  req               = net_buf_add(buf, sizeof(*req));
  req->start_handle = sys_cpu_to_le16(params->start_handle);
  req->end_handle   = sys_cpu_to_le16(params->end_handle);

  if (params->type == BT_GATT_DISCOVER_INCLUDE) {
    net_buf_add_le16(buf, BT_UUID_16(BT_UUID_GATT_INCLUDE)->val);
  } else {
    net_buf_add_le16(buf, BT_UUID_16(BT_UUID_GATT_CHRC)->val);
  }

  BT_DBG("start_handle 0x%04x end_handle 0x%04x", params->start_handle, params->end_handle);

  return gatt_send(conn, buf, gatt_read_type_rsp, params, NULL);
}

static u16_t parse_service(struct bt_conn *conn, const void *pdu, struct bt_gatt_discover_params *params, u16_t length) {
  const struct bt_att_read_group_rsp *rsp = pdu;
  u16_t                               start_handle, end_handle = 0U;
  union {
    struct bt_uuid     uuid;
    struct bt_uuid_16  u16;
    struct bt_uuid_128 u128;
  } u;

  /* Data can be either in UUID16 or UUID128 */
  switch (rsp->len) {
  case 6: /* UUID16 */
    u.uuid.type = BT_UUID_TYPE_16;
    break;
  case 20: /* UUID128 */
    u.uuid.type = BT_UUID_TYPE_128;
    break;
  default:
    BT_ERR("Invalid data len %u", rsp->len);
    goto done;
  }

  /* Parse services found */
  for (length--, pdu = rsp->data; length >= rsp->len; length -= rsp->len, pdu = (const u8_t *)pdu + rsp->len) {
    struct bt_gatt_attr             attr = {};
    struct bt_gatt_service_val      value;
    const struct bt_att_group_data *data = pdu;

    start_handle = sys_le16_to_cpu(data->start_handle);
    if (!start_handle) {
      goto done;
    }

    end_handle = sys_le16_to_cpu(data->end_handle);
    if (!end_handle || end_handle < start_handle) {
      goto done;
    }

    switch (u.uuid.type) {
    case BT_UUID_TYPE_16:
      memcpy(&u.u16.val, data->value, sizeof(u.u16.val));
      u.u16.val = sys_le16_to_cpu(u.u16.val);
      break;
    case BT_UUID_TYPE_128:
      memcpy(u.u128.val, data->value, sizeof(u.u128.val));
      break;
    }

    BT_DBG("start_handle 0x%04x end_handle 0x%04x uuid %s", start_handle, end_handle, bt_uuid_str(&u.uuid));

    if (params->type == BT_GATT_DISCOVER_PRIMARY) {
      attr.uuid = BT_UUID_GATT_PRIMARY;
    } else {
      attr.uuid = BT_UUID_GATT_SECONDARY;
    }

    value.end_handle = end_handle;
    value.uuid       = &u.uuid;

    attr.handle    = start_handle;
    attr.user_data = &value;

    if (params->func(conn, &attr, params) == BT_GATT_ITER_STOP) {
      return 0;
    }
  }

  /* Whole PDU read without error */
  if (length == 0U && end_handle) {
    return end_handle;
  }

done:
  params->func(conn, NULL, params);
  return 0;
}

static void gatt_read_group_rsp(struct bt_conn *conn, u8_t err, const void *pdu, u16_t length, void *user_data) {
  struct bt_gatt_discover_params *params = user_data;
  u16_t                           handle;

  BT_DBG("err 0x%02x", err);

  if (err) {
    params->func(conn, NULL, params);
    return;
  }

  handle = parse_service(conn, pdu, params, length);
  if (!handle) {
    return;
  }

  gatt_discover_next(conn, handle, params);
}

static int gatt_read_group(struct bt_conn *conn, struct bt_gatt_discover_params *params) {
  struct net_buf               *buf;
  struct bt_att_read_group_req *req;

  buf = bt_att_create_pdu(conn, BT_ATT_OP_READ_GROUP_REQ, sizeof(*req));
  if (!buf) {
    return -ENOMEM;
  }

  req               = net_buf_add(buf, sizeof(*req));
  req->start_handle = sys_cpu_to_le16(params->start_handle);
  req->end_handle   = sys_cpu_to_le16(params->end_handle);

  if (params->type == BT_GATT_DISCOVER_PRIMARY) {
    net_buf_add_le16(buf, BT_UUID_16(BT_UUID_GATT_PRIMARY)->val);
  } else {
    net_buf_add_le16(buf, BT_UUID_16(BT_UUID_GATT_SECONDARY)->val);
  }

  BT_DBG("start_handle 0x%04x end_handle 0x%04x", params->start_handle, params->end_handle);

  return gatt_send(conn, buf, gatt_read_group_rsp, params, NULL);
}

static void gatt_find_info_rsp(struct bt_conn *conn, u8_t err, const void *pdu, u16_t length, void *user_data) {
  const struct bt_att_find_info_rsp *rsp    = pdu;
  struct bt_gatt_discover_params    *params = user_data;
  u16_t                              handle = 0U;
  u16_t                              len;
  union {
    const struct bt_att_info_16  *i16;
    const struct bt_att_info_128 *i128;
  } info;
  union {
    struct bt_uuid     uuid;
    struct bt_uuid_16  u16;
    struct bt_uuid_128 u128;
  } u;
  int  i;
  bool skip = false;

  BT_DBG("err 0x%02x", err);

  if (err) {
    goto done;
  }

  /* Data can be either in UUID16 or UUID128 */
  switch (rsp->format) {
  case BT_ATT_INFO_16:
    u.uuid.type = BT_UUID_TYPE_16;
    len         = sizeof(*info.i16);
    break;
  case BT_ATT_INFO_128:
    u.uuid.type = BT_UUID_TYPE_128;
    len         = sizeof(*info.i128);
    break;
  default:
    BT_ERR("Invalid format %u", rsp->format);
    goto done;
  }

  length--;

  /* Check if there is a least one descriptor in the response */
  if (length < len) {
    goto done;
  }

  /* Parse descriptors found */
  for (i = length / len, pdu = rsp->info; i != 0; i--, pdu = (const u8_t *)pdu + len) {
    struct bt_gatt_attr *attr;

    info.i16 = pdu;
    handle   = sys_le16_to_cpu(info.i16->handle);

    if (skip) {
      skip = false;
      continue;
    }

    switch (u.uuid.type) {
    case BT_UUID_TYPE_16:
      u.u16.val = sys_le16_to_cpu(info.i16->uuid);
      break;
    case BT_UUID_TYPE_128:
      memcpy(u.u128.val, info.i128->uuid, 16);
      break;
    }

    BT_DBG("handle 0x%04x uuid %s", handle, bt_uuid_str(&u.uuid));

    /* Skip if UUID is set but doesn't match */
    if (params->uuid && bt_uuid_cmp(&u.uuid, params->uuid)) {
      continue;
    }

    if (params->type == BT_GATT_DISCOVER_DESCRIPTOR) {
      /* Skip attributes that are not considered
       * descriptors.
       */
      if (!bt_uuid_cmp(&u.uuid, BT_UUID_GATT_PRIMARY) || !bt_uuid_cmp(&u.uuid, BT_UUID_GATT_SECONDARY) || !bt_uuid_cmp(&u.uuid, BT_UUID_GATT_INCLUDE)) {
        continue;
      }

      /* If Characteristic Declaration skip ahead as the next
       * entry must be its value.
       */
      if (!bt_uuid_cmp(&u.uuid, BT_UUID_GATT_CHRC)) {
        skip = true;
        continue;
      }
    }

    attr         = (&(struct bt_gatt_attr)BT_GATT_DESCRIPTOR(&u.uuid, 0, NULL, NULL, NULL));
    attr->handle = handle;

    if (params->func(conn, attr, params) == BT_GATT_ITER_STOP) {
      return;
    }
  }

  gatt_discover_next(conn, handle, params);

  return;

done:
  params->func(conn, NULL, params);
}

static int gatt_find_info(struct bt_conn *conn, struct bt_gatt_discover_params *params) {
  struct net_buf              *buf;
  struct bt_att_find_info_req *req;

  buf = bt_att_create_pdu(conn, BT_ATT_OP_FIND_INFO_REQ, sizeof(*req));
  if (!buf) {
    return -ENOMEM;
  }

  req               = net_buf_add(buf, sizeof(*req));
  req->start_handle = sys_cpu_to_le16(params->start_handle);
  req->end_handle   = sys_cpu_to_le16(params->end_handle);

  BT_DBG("start_handle 0x%04x end_handle 0x%04x", params->start_handle, params->end_handle);

  return gatt_send(conn, buf, gatt_find_info_rsp, params, NULL);
}

int bt_gatt_discover(struct bt_conn *conn, struct bt_gatt_discover_params *params) {
  __ASSERT(conn, "invalid parameters\n");
  __ASSERT(params && params->func, "invalid parameters\n");
  __ASSERT((params->start_handle && params->end_handle), "invalid parameters\n");
  __ASSERT((params->start_handle <= params->end_handle), "invalid parameters\n");

  if (conn->state != BT_CONN_CONNECTED) {
    return -ENOTCONN;
  }

  switch (params->type) {
  case BT_GATT_DISCOVER_PRIMARY:
  case BT_GATT_DISCOVER_SECONDARY:
    if (params->uuid) {
      return gatt_find_type(conn, params);
    }
    return gatt_read_group(conn, params);
  case BT_GATT_DISCOVER_INCLUDE:
  case BT_GATT_DISCOVER_CHARACTERISTIC:
    return gatt_read_type(conn, params);
  case BT_GATT_DISCOVER_DESCRIPTOR:
    /* Only descriptors can be filtered */
    if (params->uuid && (!bt_uuid_cmp(params->uuid, BT_UUID_GATT_PRIMARY) || !bt_uuid_cmp(params->uuid, BT_UUID_GATT_SECONDARY) || !bt_uuid_cmp(params->uuid, BT_UUID_GATT_INCLUDE) ||
                         !bt_uuid_cmp(params->uuid, BT_UUID_GATT_CHRC))) {
      return -EINVAL;
    }

#if defined(BFLB_BLE)
    __attribute__((fallthrough));
#endif

    /* Fallthrough. */
  case BT_GATT_DISCOVER_ATTRIBUTE:
    return gatt_find_info(conn, params);
  default:
    BT_ERR("Invalid discovery type: %u", params->type);
  }

  return -EINVAL;
}

static void parse_read_by_uuid(struct bt_conn *conn, struct bt_gatt_read_params *params, const void *pdu, u16_t length) {
  const struct bt_att_read_type_rsp *rsp = pdu;

  /* Parse values found */
  for (length--, pdu = rsp->data; length; length -= rsp->len, pdu = (const u8_t *)pdu + rsp->len) {
    const struct bt_att_data *data = pdu;
    u16_t                     handle;
    u16_t                     len;

    handle = sys_le16_to_cpu(data->handle);

    /* Handle 0 is invalid */
    if (!handle) {
      BT_ERR("Invalid handle");
      return;
    }

    len = rsp->len > length ? length - 2 : rsp->len - 2;

    BT_DBG("handle 0x%04x len %u value %u", handle, rsp->len, len);

    /* Update start_handle */
    params->by_uuid.start_handle = handle;

    if (params->func(conn, 0, params, data->value, len) == BT_GATT_ITER_STOP) {
      return;
    }

    /* Check if long attribute */
    if (rsp->len > length) {
      break;
    }

    /* Stop if it's the last handle to be read */
    if (params->by_uuid.start_handle == params->by_uuid.end_handle) {
      params->func(conn, 0, params, NULL, 0);
      return;
    }

    params->by_uuid.start_handle++;
  }

  /* Continue reading the attributes */
  if (bt_gatt_read(conn, params) < 0) {
    params->func(conn, BT_ATT_ERR_UNLIKELY, params, NULL, 0);
  }
}

static void gatt_read_rsp(struct bt_conn *conn, u8_t err, const void *pdu, u16_t length, void *user_data) {
  struct bt_gatt_read_params *params = user_data;

  BT_DBG("err 0x%02x", err);

  if (err || !length) {
    params->func(conn, err, params, NULL, 0);
    return;
  }

  if (!params->handle_count) {
    parse_read_by_uuid(conn, params, pdu, length);
    return;
  }

  if (params->func(conn, 0, params, pdu, length) == BT_GATT_ITER_STOP) {
    return;
  }

  /*
   * Core Spec 4.2, Vol. 3, Part G, 4.8.1
   * If the Characteristic Value is greater than (ATT_MTU - 1) octets
   * in length, the Read Long Characteristic Value procedure may be used
   * if the rest of the Characteristic Value is required.
   */
  if (length < (bt_att_get_mtu(conn) - 1)) {
    params->func(conn, 0, params, NULL, 0);
    return;
  }

  params->single.offset += length;

  /* Continue reading the attribute */
  if (bt_gatt_read(conn, params) < 0) {
    params->func(conn, BT_ATT_ERR_UNLIKELY, params, NULL, 0);
  }
}

static int gatt_read_blob(struct bt_conn *conn, struct bt_gatt_read_params *params) {
  struct net_buf              *buf;
  struct bt_att_read_blob_req *req;

  buf = bt_att_create_pdu(conn, BT_ATT_OP_READ_BLOB_REQ, sizeof(*req));
  if (!buf) {
    return -ENOMEM;
  }

  req         = net_buf_add(buf, sizeof(*req));
  req->handle = sys_cpu_to_le16(params->single.handle);
  req->offset = sys_cpu_to_le16(params->single.offset);

  BT_DBG("handle 0x%04x offset 0x%04x", params->single.handle, params->single.offset);

  return gatt_send(conn, buf, gatt_read_rsp, params, NULL);
}

static int gatt_read_uuid(struct bt_conn *conn, struct bt_gatt_read_params *params) {
  struct net_buf              *buf;
  struct bt_att_read_type_req *req;

  buf = bt_att_create_pdu(conn, BT_ATT_OP_READ_TYPE_REQ, sizeof(*req));
  if (!buf) {
    return -ENOMEM;
  }

  req               = net_buf_add(buf, sizeof(*req));
  req->start_handle = sys_cpu_to_le16(params->by_uuid.start_handle);
  req->end_handle   = sys_cpu_to_le16(params->by_uuid.end_handle);

  if (params->by_uuid.uuid->type == BT_UUID_TYPE_16) {
    net_buf_add_le16(buf, BT_UUID_16(params->by_uuid.uuid)->val);
  } else {
    net_buf_add_mem(buf, BT_UUID_128(params->by_uuid.uuid)->val, 16);
  }

  BT_DBG("start_handle 0x%04x end_handle 0x%04x uuid %s", params->by_uuid.start_handle, params->by_uuid.end_handle, bt_uuid_str(params->by_uuid.uuid));

  return gatt_send(conn, buf, gatt_read_rsp, params, NULL);
}

#if defined(CONFIG_BT_GATT_READ_MULTIPLE)
static void gatt_read_multiple_rsp(struct bt_conn *conn, u8_t err, const void *pdu, u16_t length, void *user_data) {
  struct bt_gatt_read_params *params = user_data;

  BT_DBG("err 0x%02x", err);

  if (err || !length) {
    params->func(conn, err, params, NULL, 0);
    return;
  }

  params->func(conn, 0, params, pdu, length);

  /* mark read as complete since read multiple is single response */
  params->func(conn, 0, params, NULL, 0);
}

static int gatt_read_multiple(struct bt_conn *conn, struct bt_gatt_read_params *params) {
  struct net_buf *buf;
  u8_t            i;

  buf = bt_att_create_pdu(conn, BT_ATT_OP_READ_MULT_REQ, params->handle_count * sizeof(u16_t));
  if (!buf) {
    return -ENOMEM;
  }

  for (i = 0U; i < params->handle_count; i++) {
    net_buf_add_le16(buf, params->handles[i]);
  }

  return gatt_send(conn, buf, gatt_read_multiple_rsp, params, NULL);
}
#else
static int gatt_read_multiple(struct bt_conn *conn, struct bt_gatt_read_params *params) { return -ENOTSUP; }
#endif /* CONFIG_BT_GATT_READ_MULTIPLE */

int bt_gatt_read(struct bt_conn *conn, struct bt_gatt_read_params *params) {
  struct net_buf         *buf;
  struct bt_att_read_req *req;

  __ASSERT(conn, "invalid parameters\n");
  __ASSERT(params && params->func, "invalid parameters\n");

  if (conn->state != BT_CONN_CONNECTED) {
    return -ENOTCONN;
  }

  if (params->handle_count == 0) {
    return gatt_read_uuid(conn, params);
  }

  if (params->handle_count > 1) {
    return gatt_read_multiple(conn, params);
  }

  if (params->single.offset) {
    return gatt_read_blob(conn, params);
  }

  buf = bt_att_create_pdu(conn, BT_ATT_OP_READ_REQ, sizeof(*req));
  if (!buf) {
    return -ENOMEM;
  }

  req         = net_buf_add(buf, sizeof(*req));
  req->handle = sys_cpu_to_le16(params->single.handle);

  BT_DBG("handle 0x%04x", params->single.handle);

  return gatt_send(conn, buf, gatt_read_rsp, params, NULL);
}

static void gatt_write_rsp(struct bt_conn *conn, u8_t err, const void *pdu, u16_t length, void *user_data) {
  struct bt_gatt_write_params *params = user_data;

  BT_DBG("err 0x%02x", err);

  params->func(conn, err, params);
}

int bt_gatt_write_without_response_cb(struct bt_conn *conn, u16_t handle, const void *data, u16_t length, bool sign, bt_gatt_complete_func_t func, void *user_data) {
  struct net_buf          *buf;
  struct bt_att_write_cmd *cmd;

  __ASSERT(conn, "invalid parameters\n");
  __ASSERT(handle, "invalid parameters\n");

  if (conn->state != BT_CONN_CONNECTED) {
    return -ENOTCONN;
  }

#if defined(CONFIG_BT_SMP)
  if (conn->encrypt) {
    /* Don't need to sign if already encrypted */
    sign = false;
  }
#endif

  if (sign) {
    buf = bt_att_create_pdu(conn, BT_ATT_OP_SIGNED_WRITE_CMD, sizeof(*cmd) + length + 12);
  } else {
    buf = bt_att_create_pdu(conn, BT_ATT_OP_WRITE_CMD, sizeof(*cmd) + length);
  }
  if (!buf) {
    return -ENOMEM;
  }

  cmd         = net_buf_add(buf, sizeof(*cmd));
  cmd->handle = sys_cpu_to_le16(handle);
  memcpy(cmd->value, data, length);
  net_buf_add(buf, length);

  BT_DBG("handle 0x%04x length %u", handle, length);

  return bt_att_send(conn, buf, func, user_data);
}

static int gatt_exec_write(struct bt_conn *conn, struct bt_gatt_write_params *params) {
  struct net_buf               *buf;
  struct bt_att_exec_write_req *req;

  buf = bt_att_create_pdu(conn, BT_ATT_OP_EXEC_WRITE_REQ, sizeof(*req));
  if (!buf) {
    return -ENOMEM;
  }

  req = net_buf_add(buf, sizeof(*req));
#if defined(CONFIG_BT_STACK_PTS)
  if (event_flag == gatt_cancel_write_req) {
    req->flags = BT_ATT_FLAG_CANCEL;
  } else {
    req->flags = BT_ATT_FLAG_EXEC;
  }
#else
  req->flags = BT_ATT_FLAG_EXEC;
#endif

  BT_DBG("");

  return gatt_send(conn, buf, gatt_write_rsp, params, NULL);
}

static void gatt_prepare_write_rsp(struct bt_conn *conn, u8_t err, const void *pdu, u16_t length, void *user_data) {
  struct bt_gatt_write_params *params = user_data;

  BT_DBG("err 0x%02x", err);

  /* Don't continue in case of error */
  if (err) {
    params->func(conn, err, params);
    return;
  }
  /* If there is no more data execute */
  if (!params->length) {
    gatt_exec_write(conn, params);
    return;
  }

  /* Write next chunk */
  bt_gatt_write(conn, params);
}

static int gatt_prepare_write(struct bt_conn *conn, struct bt_gatt_write_params *params)

{
  struct net_buf                  *buf;
  struct bt_att_prepare_write_req *req;
  u16_t                            len;

  len = MIN(params->length, bt_att_get_mtu(conn) - sizeof(*req) - 1);

  buf = bt_att_create_pdu(conn, BT_ATT_OP_PREPARE_WRITE_REQ, sizeof(*req) + len);
  if (!buf) {
    return -ENOMEM;
  }

  req         = net_buf_add(buf, sizeof(*req));
  req->handle = sys_cpu_to_le16(params->handle);
  req->offset = sys_cpu_to_le16(params->offset);
  memcpy(req->value, params->data, len);
  net_buf_add(buf, len);

  /* Update params */
  params->offset += len;
  params->data = (const u8_t *)params->data + len;
  params->length -= len;

  BT_DBG("handle 0x%04x offset %u len %u", params->handle, params->offset, params->length);

  return gatt_send(conn, buf, gatt_prepare_write_rsp, params, NULL);
}

#if defined(CONFIG_BT_STACK_PTS)
int bt_gatt_prepare_write(struct bt_conn *conn, struct bt_gatt_write_params *params) { return gatt_prepare_write(conn, params); }
#endif

int bt_gatt_write(struct bt_conn *conn, struct bt_gatt_write_params *params) {
  struct net_buf          *buf;
  struct bt_att_write_req *req;

  __ASSERT(conn, "invalid parameters\n");
  __ASSERT(params && params->func, "invalid parameters\n");
  __ASSERT(params->handle, "invalid parameters\n");

  if (conn->state != BT_CONN_CONNECTED) {
    return -ENOTCONN;
  }

  /* Use Prepare Write if offset is set or Long Write is required */
  if (params->offset || params->length > (bt_att_get_mtu(conn) - sizeof(*req) - 1)) {
    return gatt_prepare_write(conn, params);
  }

  buf = bt_att_create_pdu(conn, BT_ATT_OP_WRITE_REQ, sizeof(*req) + params->length);
  if (!buf) {
    return -ENOMEM;
  }

  req         = net_buf_add(buf, sizeof(*req));
  req->handle = sys_cpu_to_le16(params->handle);
  memcpy(req->value, params->data, params->length);
  net_buf_add(buf, params->length);

  BT_DBG("handle 0x%04x length %u", params->handle, params->length);

  return gatt_send(conn, buf, gatt_write_rsp, params, NULL);
}

static void gatt_subscription_add(struct bt_conn *conn, struct bt_gatt_subscribe_params *params) {
  bt_addr_le_copy(&params->_peer, &conn->le.dst);

  /* Prepend subscription */
  sys_slist_prepend(&subscriptions, &params->node);
}

static void gatt_write_ccc_rsp(struct bt_conn *conn, u8_t err, const void *pdu, u16_t length, void *user_data) {
  struct bt_gatt_subscribe_params *params = user_data;

  BT_DBG("err 0x%02x", err);

  atomic_clear_bit(params->flags, BT_GATT_SUBSCRIBE_FLAG_WRITE_PENDING);

  /* if write to CCC failed we remove subscription and notify app */
  if (err) {
    sys_snode_t *node, *tmp, *prev = NULL;
    UNUSED(prev);

    SYS_SLIST_FOR_EACH_NODE_SAFE(&subscriptions, node, tmp) {
      if (node == &params->node) {
        gatt_subscription_remove(conn, tmp, params);
        break;
      }

      prev = node;
    }
  } else if (!params->value) {
    /* Notify with NULL data to complete unsubscribe */
    params->notify(conn, params, NULL, 0);
  }
#if defined(BFLB_BLE_PATCH_NOTIFY_WRITE_CCC_RSP)
  else {
    params->notify(conn, params, NULL, 0);
  }
#endif
}

static int gatt_write_ccc(struct bt_conn *conn, u16_t handle, u16_t value, bt_att_func_t func, struct bt_gatt_subscribe_params *params) {
  struct net_buf          *buf;
  struct bt_att_write_req *req;

  buf = bt_att_create_pdu(conn, BT_ATT_OP_WRITE_REQ, sizeof(*req) + sizeof(u16_t));
  if (!buf) {
    return -ENOMEM;
  }

  req         = net_buf_add(buf, sizeof(*req));
  req->handle = sys_cpu_to_le16(handle);
  net_buf_add_le16(buf, value);

  BT_DBG("handle 0x%04x value 0x%04x", handle, value);

  atomic_set_bit(params->flags, BT_GATT_SUBSCRIBE_FLAG_WRITE_PENDING);

  return gatt_send(conn, buf, func, params, NULL);
}

int bt_gatt_subscribe(struct bt_conn *conn, struct bt_gatt_subscribe_params *params) {
  struct bt_gatt_subscribe_params *tmp;
  bool                             has_subscription = false;

  __ASSERT(conn, "invalid parameters\n");
  __ASSERT(params && params->notify, "invalid parameters\n");
  __ASSERT(params->value, "invalid parameters\n");
  __ASSERT(params->ccc_handle, "invalid parameters\n");

  if (conn->state != BT_CONN_CONNECTED) {
    return -ENOTCONN;
  }

  /* Lookup existing subscriptions */
  SYS_SLIST_FOR_EACH_CONTAINER(&subscriptions, tmp, node) {
    /* Fail if entry already exists */
    if (tmp == params) {
      return -EALREADY;
    }

    /* Check if another subscription exists */
    if (!bt_conn_addr_le_cmp(conn, &tmp->_peer) && tmp->value_handle == params->value_handle && tmp->value >= params->value) {
      has_subscription = true;
    }
  }

  /* Skip write if already subscribed */
  if (!has_subscription) {
    int err;

    err = gatt_write_ccc(conn, params->ccc_handle, params->value, gatt_write_ccc_rsp, params);
    if (err) {
      return err;
    }
  }

  /*
   * Add subscription before write complete as some implementation were
   * reported to send notification before reply to CCC write.
   */
  gatt_subscription_add(conn, params);

  return 0;
}

int bt_gatt_unsubscribe(struct bt_conn *conn, struct bt_gatt_subscribe_params *params) {
  struct bt_gatt_subscribe_params *tmp, *next;
  bool                             has_subscription = false, found = false;
  sys_snode_t                     *prev = NULL;

  __ASSERT(conn, "invalid parameters\n");
  __ASSERT(params, "invalid parameters\n");

  if (conn->state != BT_CONN_CONNECTED) {
    return -ENOTCONN;
  }

  /* Lookup existing subscriptions */
  SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&subscriptions, tmp, next, node) {
    /* Remove subscription */
    if (params == tmp) {
      found = true;
      sys_slist_remove(&subscriptions, prev, &tmp->node);
      /* Attempt to cancel if write is pending */
      if (atomic_test_bit(params->flags, BT_GATT_SUBSCRIBE_FLAG_WRITE_PENDING)) {
        bt_gatt_cancel(conn, params);
      }
      continue;
    } else {
      prev = &tmp->node;
    }

    /* Check if there still remains any other subscription */
    if (!bt_conn_addr_le_cmp(conn, &tmp->_peer) && tmp->value_handle == params->value_handle) {
      has_subscription = true;
    }
  }

  if (!found) {
    return -EINVAL;
  }

  if (has_subscription) {
    /* Notify with NULL data to complete unsubscribe */
    params->notify(conn, params, NULL, 0);
    return 0;
  }

  params->value = 0x0000;

  return gatt_write_ccc(conn, params->ccc_handle, params->value, gatt_write_ccc_rsp, params);
}

void bt_gatt_cancel(struct bt_conn *conn, void *params) { bt_att_req_cancel(conn, params); }

static void add_subscriptions(struct bt_conn *conn) {
  struct bt_gatt_subscribe_params *params;

  /* Lookup existing subscriptions */
  SYS_SLIST_FOR_EACH_CONTAINER(&subscriptions, params, node) {
    if (bt_conn_addr_le_cmp(conn, &params->_peer)) {
      continue;
    }

    /* Force write to CCC to workaround devices that don't track
     * it properly.
     */
    gatt_write_ccc(conn, params->ccc_handle, params->value, gatt_write_ccc_rsp, params);
  }
}

#endif /* CONFIG_BT_GATT_CLIENT */

#define CCC_STORE_MAX 48

static struct bt_gatt_ccc_cfg *ccc_find_cfg(struct _bt_gatt_ccc *ccc, const bt_addr_le_t *addr, u8_t id) {
  for (size_t i = 0; i < ARRAY_SIZE(ccc->cfg); i++) {
    if (id == ccc->cfg[i].id && !bt_addr_le_cmp(&ccc->cfg[i].peer, addr)) {
      return &ccc->cfg[i];
    }
  }

  return NULL;
}

struct addr_with_id {
  const bt_addr_le_t *addr;
  u8_t                id;
};

struct ccc_load {
  struct addr_with_id addr_with_id;
  struct ccc_store   *entry;
  size_t              count;
};

static void ccc_clear(struct _bt_gatt_ccc *ccc, const bt_addr_le_t *addr, u8_t id) {
  struct bt_gatt_ccc_cfg *cfg;

  cfg = ccc_find_cfg(ccc, addr, id);
  if (!cfg) {
    BT_DBG("Unable to clear CCC: cfg not found");
    return;
  }

  clear_ccc_cfg(cfg);
}

static u8_t ccc_load(const struct bt_gatt_attr *attr, void *user_data) {
  struct ccc_load        *load = user_data;
  struct _bt_gatt_ccc    *ccc;
  struct bt_gatt_ccc_cfg *cfg;

  /* Check if attribute is a CCC */
  if (attr->write != bt_gatt_attr_write_ccc) {
    return BT_GATT_ITER_CONTINUE;
  }

  ccc = attr->user_data;

  /* Clear if value was invalidated */
  if (!load->entry) {
    ccc_clear(ccc, load->addr_with_id.addr, load->addr_with_id.id);
    return BT_GATT_ITER_CONTINUE;
  } else if (!load->count) {
    return BT_GATT_ITER_STOP;
  }

  /* Skip if value is not for the given attribute */
  if (load->entry->handle != attr->handle) {
    /* If attribute handle is bigger then it means
     * the attribute no longer exists and cannot
     * be restored.
     */
    if (load->entry->handle < attr->handle) {
      BT_DBG("Unable to restore CCC: handle 0x%04x cannot be"
             " found",
             load->entry->handle);
      goto next;
    }
    return BT_GATT_ITER_CONTINUE;
  }

  BT_DBG("Restoring CCC: handle 0x%04x value 0x%04x", load->entry->handle, load->entry->value);

  cfg = ccc_find_cfg(ccc, load->addr_with_id.addr, load->addr_with_id.id);
  if (!cfg) {
    cfg = ccc_find_cfg(ccc, BT_ADDR_LE_ANY, 0);
    if (!cfg) {
      BT_DBG("Unable to restore CCC: no cfg left");
      goto next;
    }
    bt_addr_le_copy(&cfg->peer, load->addr_with_id.addr);
    cfg->id = load->addr_with_id.id;
  }

  cfg->value = load->entry->value;

next:
  load->entry++;
  load->count--;

  return load->count ? BT_GATT_ITER_CONTINUE : BT_GATT_ITER_STOP;
}

#if defined(BFLB_BLE)
static int ccc_set(const char *key, u8_t id, bt_addr_le_t *addr)
#else
static int ccc_set(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
#endif
{
  if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
    struct ccc_store ccc_store[CCC_STORE_MAX];
    struct ccc_load  load;
#if defined(BFLB_BLE)
    size_t len;
    int    err;
#else
    bt_addr_le_t addr;
    const char  *next;
    int          len, err;
#endif

#if defined(BFLB_BLE)
    err = bt_settings_get_bin(key, (u8_t *)ccc_store, CCC_STORE_MAX, &len);
    if (err) {
      return err;
    }

    load.addr_with_id.id   = id;
    load.addr_with_id.addr = addr;
    load.entry             = ccc_store;
    load.count             = len / sizeof(*ccc_store);
#else
    settings_name_next(name, &next);

    if (!name) {
      BT_ERR("Insufficient number of arguments");
      return -EINVAL;
    } else if (!next) {
      load.addr_with_id.id = BT_ID_DEFAULT;
    } else {
      load.addr_with_id.id = strtol(next, NULL, 10);
    }

    err = bt_settings_decode_key(name, &addr);
    if (err) {
      BT_ERR("Unable to decode address %s", log_strdup(name));
      return -EINVAL;
    }

    load.addr_with_id.addr = &addr;

    if (len_rd) {
      len = read_cb(cb_arg, ccc_store, sizeof(ccc_store));

      if (len < 0) {
        BT_ERR("Failed to decode value (err %d)", len);
        return len;
      }

      load.entry = ccc_store;
      load.count = len / sizeof(*ccc_store);

      for (int i = 0; i < load.count; i++) {
        BT_DBG("Read CCC: handle 0x%04x value 0x%04x", ccc_store[i].handle, ccc_store[i].value);
      }
    } else {
      load.entry = NULL;
      load.count = 0;
    }
#endif

    bt_gatt_foreach_attr(0x0001, 0xffff, ccc_load, &load);

    BT_DBG("Restored CCC for id:%x"
           "PRIu8"
           " addr:%s",
           load.addr_with_id.id, bt_addr_le_str(load.addr_with_id.addr));
  }

  return 0;
}

#if !defined(BFLB_BLE)
#if !IS_ENABLED(CONFIG_BT_SETTINGS_CCC_LAZY_LOADING)
/* Only register the ccc_set settings handler when not loading on-demand */
SETTINGS_STATIC_HANDLER_DEFINE(bt_ccc, "bt/ccc", NULL, ccc_set, NULL, NULL);
#endif /* CONFIG_BT_SETTINGS_CCC_LAZY_LOADING */
#endif

#if !defined(BFLB_BLE)
static int ccc_set_direct(const char *key, size_t len, settings_read_cb read_cb, void *cb_arg, void *param) {
  if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
    const char *name;

    BT_DBG("key: %s", log_strdup((const char *)param));

    /* Only "bt/ccc" settings should ever come here */
    if (!settings_name_steq((const char *)param, "bt/ccc", &name)) {
      BT_ERR("Invalid key");
      return -EINVAL;
    }

    return ccc_set(name, len, read_cb, cb_arg);
  }
  return 0;
}
#endif

#if defined(BFLB_BLE)
#if defined(CONFIG_BT_GATT_SERVICE_CHANGED)
#if defined(CONFIG_BT_SETTINGS)
static int sc_set(u8_t id, bt_addr_le_t *addr);
static int sc_commit(void);
#endif
#endif
#endif
void bt_gatt_connected(struct bt_conn *conn) {
  struct conn_data data;

  BT_DBG("conn %p", conn);

  data.conn = conn;
  data.sec  = BT_SECURITY_L1;

  /* Load CCC settings from backend if bonded */
  if (IS_ENABLED(CONFIG_BT_SETTINGS_CCC_LAZY_LOADING) && bt_addr_le_is_bonded(conn->id, &conn->le.dst)) {
    char key[BT_SETTINGS_KEY_MAX];

    if (conn->id) {
      char id_str[4];

      u8_to_dec(id_str, sizeof(id_str), conn->id);
      bt_settings_encode_key(key, sizeof(key), "ccc", &conn->le.dst, id_str);
    } else {
      bt_settings_encode_key(key, sizeof(key), "ccc", &conn->le.dst, NULL);
    }
#if defined(BFLB_BLE)
    ccc_set(key, conn->id, &conn->le.dst);
#else
    settings_load_subtree_direct(key, ccc_set_direct, (void *)key);
#endif
  }

  bt_gatt_foreach_attr(0x0001, 0xffff, update_ccc, &data);

  /* BLUETOOTH CORE SPECIFICATION Version 5.1 | Vol 3, Part C page 2192:
   *
   * 10.3.1.1 Handling of GATT indications and notifications
   *
   * A client requests a server to send indications and notifications
   * by appropriately configuring the server via a Client Characteristic
   * Configuration Descriptor. Since the configuration is persistent
   * across a disconnection and reconnection, security requirements must
   * be checked against the configuration upon a reconnection before
   * sending indications or notifications. When a server reconnects to a
   * client to send an indication or notification for which security is
   * required, the server shall initiate or request encryption with the
   * client prior to sending an indication or notification. If the client
   * does not have an LTK indicating that the client has lost the bond,
   * enabling encryption will fail.
   */
  if (IS_ENABLED(CONFIG_BT_SMP) && bt_conn_get_security(conn) < data.sec) {
    bt_conn_set_security(conn, data.sec);
  }

#if defined(CONFIG_BT_GATT_CLIENT)
  add_subscriptions(conn);
#endif /* CONFIG_BT_GATT_CLIENT */

#if defined(BFLB_BLE)
#if defined(CONFIG_BT_GATT_SERVICE_CHANGED)
#if defined(CONFIG_BT_SETTINGS)
  sc_set(conn->id, &conn->le.dst);
  sc_commit();
#endif
#endif
#endif
}

void bt_gatt_encrypt_change(struct bt_conn *conn) {
  struct conn_data data;

  BT_DBG("conn %p", conn);

  data.conn = conn;
  data.sec  = BT_SECURITY_L1;

  bt_gatt_foreach_attr(0x0001, 0xffff, update_ccc, &data);
}

bool bt_gatt_change_aware(struct bt_conn *conn, bool req) {
#if defined(CONFIG_BT_GATT_CACHING)
  struct gatt_cf_cfg *cfg;

  cfg = find_cf_cfg(conn);
  if (!cfg || !CF_ROBUST_CACHING(cfg)) {
    return true;
  }

  if (atomic_test_bit(cfg->flags, CF_CHANGE_AWARE)) {
    return true;
  }

  /* BLUETOOTH CORE SPECIFICATION Version 5.1 | Vol 3, Part G page 2350:
   * If a change-unaware client sends an ATT command, the server shall
   * ignore it.
   */
  if (!req) {
    return false;
  }

  /* BLUETOOTH CORE SPECIFICATION Version 5.1 | Vol 3, Part G page 2347:
   * 2.5.2.1 Robust Caching
   * A connected client becomes change-aware when...
   * The server sends the client a response with the error code set to
   * Database Out Of Sync and then the server receives another ATT
   * request from the client.
   */
  if (atomic_test_bit(cfg->flags, CF_OUT_OF_SYNC)) {
    atomic_clear_bit(cfg->flags, CF_OUT_OF_SYNC);
    atomic_set_bit(cfg->flags, CF_CHANGE_AWARE);
    BT_DBG("%s change-aware", bt_addr_le_str(&cfg->peer));
    return true;
  }

  atomic_set_bit(cfg->flags, CF_OUT_OF_SYNC);

  return false;
#else
  return true;
#endif
}

static int bt_gatt_store_cf(struct bt_conn *conn) {
#if defined(CONFIG_BT_GATT_CACHING)
  struct gatt_cf_cfg *cfg;
  char                key[BT_SETTINGS_KEY_MAX];
  char               *str;
  size_t              len;
  int                 err;

  cfg = find_cf_cfg(conn);
  if (!cfg) {
    /* No cfg found, just clear it */
    BT_DBG("No config for CF");
    str = NULL;
    len = 0;
  } else {
    str = (char *)cfg->data;
    len = sizeof(cfg->data);

    if (conn->id) {
      char id_str[4];

      u8_to_dec(id_str, sizeof(id_str), conn->id);
      bt_settings_encode_key(key, sizeof(key), "cf", &conn->le.dst, id_str);
    }
  }

  if (!cfg || !conn->id) {
    bt_settings_encode_key(key, sizeof(key), "cf", &conn->le.dst, NULL);
  }

#if defined(BFLB_BLE)
  err = settings_save_one(key, (u8_t *)str, len);
#else
  err = settings_save_one(key, str, len);
#endif
  if (err) {
    BT_ERR("Failed to store Client Features (err %d)", err);
    return err;
  }

  BT_DBG("Stored CF for %s (%s)", bt_addr_le_str(&conn->le.dst), log_strdup(key));
#endif /* CONFIG_BT_GATT_CACHING */
  return 0;
}

void bt_gatt_disconnected(struct bt_conn *conn) {
  BT_DBG("conn %p", conn);
  bt_gatt_foreach_attr(0x0001, 0xffff, disconnected_cb, conn);

#if defined(CONFIG_BT_SETTINGS_CCC_STORE_ON_WRITE)
  gatt_ccc_conn_unqueue(conn);

  if (gatt_ccc_conn_queue_is_empty()) {
    k_delayed_work_cancel(&gatt_ccc_store.work);
  }
#endif

  if (IS_ENABLED(CONFIG_BT_SETTINGS) && bt_addr_le_is_bonded(conn->id, &conn->le.dst)) {
    bt_gatt_store_ccc(conn->id, &conn->le.dst);
    bt_gatt_store_cf(conn);
  }

#if defined(CONFIG_BT_GATT_CLIENT)
  remove_subscriptions(conn);
#endif /* CONFIG_BT_GATT_CLIENT */

#if defined(CONFIG_BT_GATT_CACHING)
  remove_cf_cfg(conn);
#endif
}

#if defined(BFLB_BLE_MTU_CHANGE_CB)
void bt_gatt_mtu_changed(struct bt_conn *conn, u16_t mtu) {
  if (gatt_mtu_changed_cb) {
    gatt_mtu_changed_cb(conn, (int)mtu);
  }
}

void bt_gatt_register_mtu_callback(bt_gatt_mtu_changed_cb_t cb) { gatt_mtu_changed_cb = cb; }
#endif

#if defined(CONFIG_BT_SETTINGS)

struct ccc_save {
  struct addr_with_id addr_with_id;
  struct ccc_store    store[CCC_STORE_MAX];
  size_t              count;
};

static u8_t ccc_save(const struct bt_gatt_attr *attr, void *user_data) {
  struct ccc_save        *save = user_data;
  struct _bt_gatt_ccc    *ccc;
  struct bt_gatt_ccc_cfg *cfg;

  /* Check if attribute is a CCC */
  if (attr->write != bt_gatt_attr_write_ccc) {
    return BT_GATT_ITER_CONTINUE;
  }

  ccc = attr->user_data;

  /* Check if there is a cfg for the peer */
  cfg = ccc_find_cfg(ccc, save->addr_with_id.addr, save->addr_with_id.id);
  if (!cfg) {
    return BT_GATT_ITER_CONTINUE;
  }

  BT_DBG("Storing CCCs handle 0x%04x value 0x%04x", attr->handle, cfg->value);

  save->store[save->count].handle = attr->handle;
  save->store[save->count].value  = cfg->value;
  save->count++;

  return BT_GATT_ITER_CONTINUE;
}

int bt_gatt_store_ccc(u8_t id, const bt_addr_le_t *addr) {
  struct ccc_save save;
  char            key[BT_SETTINGS_KEY_MAX];
  size_t          len;
  char           *str;
  int             err;

  save.addr_with_id.addr = addr;
  save.addr_with_id.id   = id;
  save.count             = 0;

  bt_gatt_foreach_attr(0x0001, 0xffff, ccc_save, &save);

  if (id) {
    char id_str[4];

    u8_to_dec(id_str, sizeof(id_str), id);
    bt_settings_encode_key(key, sizeof(key), "ccc", (bt_addr_le_t *)addr, id_str);
  } else {
    bt_settings_encode_key(key, sizeof(key), "ccc", (bt_addr_le_t *)addr, NULL);
  }

  if (save.count) {
    str = (char *)save.store;
    len = save.count * sizeof(*save.store);
  } else {
    /* No entries to encode, just clear */
    str = NULL;
    len = 0;
  }

  err = settings_save_one(key, (const u8_t *)str, len);
  if (err) {
    BT_ERR("Failed to store CCCs (err %d)", err);
    return err;
  }

  BT_DBG("Stored CCCs for %s (%s)", bt_addr_le_str(addr), log_strdup(key));
  if (len) {
    for (int i = 0; i < save.count; i++) {
      BT_DBG("  CCC: handle 0x%04x value 0x%04x", save.store[i].handle, save.store[i].value);
    }
  } else {
    BT_DBG("  CCC: NULL");
  }

  return 0;
}

static u8_t remove_peer_from_attr(const struct bt_gatt_attr *attr, void *user_data) {
  const struct addr_with_id *addr_with_id = user_data;
  struct _bt_gatt_ccc       *ccc;
  struct bt_gatt_ccc_cfg    *cfg;

  /* Check if attribute is a CCC */
  if (attr->write != bt_gatt_attr_write_ccc) {
    return BT_GATT_ITER_CONTINUE;
  }

  ccc = attr->user_data;

  /* Check if there is a cfg for the peer */
  cfg = ccc_find_cfg(ccc, addr_with_id->addr, addr_with_id->id);
  if (cfg) {
    memset(cfg, 0, sizeof(*cfg));
  }

  return BT_GATT_ITER_CONTINUE;
}

static int bt_gatt_clear_ccc(u8_t id, const bt_addr_le_t *addr) {
  char                key[BT_SETTINGS_KEY_MAX];
  struct addr_with_id addr_with_id = {
      .addr = addr,
      .id   = id,
  };

  if (id) {
    char id_str[4];

    u8_to_dec(id_str, sizeof(id_str), id);
    bt_settings_encode_key(key, sizeof(key), "ccc", (bt_addr_le_t *)addr, id_str);
  } else {
    bt_settings_encode_key(key, sizeof(key), "ccc", (bt_addr_le_t *)addr, NULL);
  }

  bt_gatt_foreach_attr(0x0001, 0xffff, remove_peer_from_attr, &addr_with_id);

  return settings_delete(key);
}

#if defined(CONFIG_BT_GATT_CACHING)
static struct gatt_cf_cfg *find_cf_cfg_by_addr(const bt_addr_le_t *addr) {
  int i;

  for (i = 0; i < ARRAY_SIZE(cf_cfg); i++) {
    if (!bt_addr_le_cmp(addr, &cf_cfg[i].peer)) {
      return &cf_cfg[i];
    }
  }

  return NULL;
}
#endif /* CONFIG_BT_GATT_CACHING */

static int bt_gatt_clear_cf(u8_t id, const bt_addr_le_t *addr) {
#if defined(CONFIG_BT_GATT_CACHING)
  char                key[BT_SETTINGS_KEY_MAX];
  struct gatt_cf_cfg *cfg;

  if (id) {
    char id_str[4];

    u8_to_dec(id_str, sizeof(id_str), id);
    bt_settings_encode_key(key, sizeof(key), "cf", (bt_addr_le_t *)addr, id_str);
  } else {
    bt_settings_encode_key(key, sizeof(key), "cf", (bt_addr_le_t *)addr, NULL);
  }

  cfg = find_cf_cfg_by_addr(addr);
  if (cfg) {
    clear_cf_cfg(cfg);
  }

  return settings_delete(key);
#endif /* CONFIG_BT_GATT_CACHING */
  return 0;
}

static int sc_clear_by_addr(u8_t id, const bt_addr_le_t *addr) {
  if (IS_ENABLED(CONFIG_BT_GATT_SERVICE_CHANGED)) {
    struct gatt_sc_cfg *cfg;

    cfg = find_sc_cfg(id, (bt_addr_le_t *)addr);
    if (cfg) {
      sc_clear(cfg);
    }
  }
  return 0;
}

static void bt_gatt_clear_subscriptions(const bt_addr_le_t *addr) {
#if defined(CONFIG_BT_GATT_CLIENT)
  struct bt_gatt_subscribe_params *params, *tmp;
  sys_snode_t                     *prev = NULL;

  SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&subscriptions, params, tmp, node) {
    if (bt_addr_le_cmp(addr, &params->_peer)) {
      prev = &params->node;
      continue;
    }
    params->value = 0U;
    gatt_subscription_remove(NULL, prev, params);
  }
#endif /* CONFIG_BT_GATT_CLIENT */
}

int bt_gatt_clear(u8_t id, const bt_addr_le_t *addr) {
  int err;

  err = bt_gatt_clear_ccc(id, addr);
  if (err < 0) {
    return err;
  }

  err = sc_clear_by_addr(id, addr);
  if (err < 0) {
    return err;
  }

  err = bt_gatt_clear_cf(id, addr);
  if (err < 0) {
    return err;
  }

  bt_gatt_clear_subscriptions(addr);

  return 0;
}

#if defined(CONFIG_BT_GATT_SERVICE_CHANGED)
#if defined(BFLB_BLE)
static int sc_set(u8_t id, bt_addr_le_t *addr)
#else
static int sc_set(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg)
#endif
{
  struct gatt_sc_cfg *cfg;
#if !defined(BFLB_BLE)
  u8_t         id;
  bt_addr_le_t addr;
  int          len, err;
  const char  *next;
#endif

#if defined(BFLB_BLE)
  int  err;
  char key[BT_SETTINGS_KEY_MAX];

  cfg = find_sc_cfg(id, addr);
  if (!cfg) {
    /* Find and initialize a free sc_cfg entry */
    cfg = find_sc_cfg(BT_ID_DEFAULT, BT_ADDR_LE_ANY);
    if (!cfg) {
      BT_ERR("Unable to restore SC: no cfg left");
      return -ENOMEM;
    }

    cfg->id = id;
    bt_addr_le_copy(&cfg->peer, addr);
  }

  if (id) {
    char id_str[4];

    u8_to_dec(id_str, sizeof(id_str), id);
    bt_settings_encode_key(key, sizeof(key), "sc", addr, id_str);
  } else {
    bt_settings_encode_key(key, sizeof(key), "sc", addr, NULL);
  }

  err = bt_settings_get_bin(key, (u8_t *)cfg, sizeof(*cfg), NULL);
  if (err) {
    memset(cfg, 0, sizeof(*cfg));
  }
  return err;
#else
  if (!name) {
    BT_ERR("Insufficient number of arguments");
    return -EINVAL;
  }

  err = bt_settings_decode_key(name, &addr);
  if (err) {
    BT_ERR("Unable to decode address %s", log_strdup(name));
    return -EINVAL;
  }

  settings_name_next(name, &next);

  if (!next) {
    id = BT_ID_DEFAULT;
  } else {
    id = strtol(next, NULL, 10);
  }

  cfg = find_sc_cfg(id, &addr);
  if (!cfg && len_rd) {
    /* Find and initialize a free sc_cfg entry */
    cfg = find_sc_cfg(BT_ID_DEFAULT, BT_ADDR_LE_ANY);
    if (!cfg) {
      BT_ERR("Unable to restore SC: no cfg left");
      return -ENOMEM;
    }

    cfg->id = id;
    bt_addr_le_copy(&cfg->peer, &addr);
  }

  if (len_rd) {
    len = read_cb(cb_arg, &cfg->data, sizeof(cfg->data));
    if (len < 0) {
      BT_ERR("Failed to decode value (err %d)", len);
      return len;
    }
    BT_DBG("Read SC: len %d", len);

    BT_DBG("Restored SC for %s", bt_addr_le_str(&addr));
  } else if (cfg) {
    /* Clear configuration */
    memset(cfg, 0, sizeof(*cfg));

    BT_DBG("Removed SC for %s", bt_addr_le_str(&addr));
  }

  return 0;
#endif
}

static int sc_commit(void) {
  atomic_clear_bit(gatt_sc.flags, SC_INDICATE_PENDING);

  if (atomic_test_bit(gatt_sc.flags, SC_RANGE_CHANGED)) {
    /* Schedule SC indication since the range has changed */
    k_delayed_work_submit(&gatt_sc.work, SC_TIMEOUT);
  }

  return 0;
}

#if !defined(BFLB_BLE)
SETTINGS_STATIC_HANDLER_DEFINE(bt_sc, "bt/sc", NULL, sc_set, sc_commit, NULL);
#endif
#endif /* CONFIG_BT_GATT_SERVICE_CHANGED */

#if defined(CONFIG_BT_GATT_CACHING)
static int cf_set(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg) {
  struct gatt_cf_cfg *cfg;
  bt_addr_le_t        addr;
  int                 len, err;

  if (!name) {
    BT_ERR("Insufficient number of arguments");
    return -EINVAL;
  }

  err = bt_settings_decode_key(name, &addr);
  if (err) {
    BT_ERR("Unable to decode address %s", log_strdup(name));
    return -EINVAL;
  }

  cfg = find_cf_cfg_by_addr(&addr);
  if (!cfg) {
    cfg = find_cf_cfg(NULL);
    if (!cfg) {
      BT_ERR("Unable to restore CF: no cfg left");
      return 0;
    }
  }

  if (len_rd) {
    len = read_cb(cb_arg, cfg->data, sizeof(cfg->data));
    if (len < 0) {
      BT_ERR("Failed to decode value (err %d)", len);
      return len;
    }

    BT_DBG("Read CF: len %d", len);
  } else {
    clear_cf_cfg(cfg);
  }

  BT_DBG("Restored CF for %s", bt_addr_le_str(&addr));

  return 0;
}

SETTINGS_STATIC_HANDLER_DEFINE(bt_cf, "bt/cf", NULL, cf_set, NULL, NULL);

static u8_t stored_hash[16];

static int db_hash_set(const char *name, size_t len_rd, settings_read_cb read_cb, void *cb_arg) {
  int len;

  len = read_cb(cb_arg, stored_hash, sizeof(stored_hash));
  if (len < 0) {
    BT_ERR("Failed to decode value (err %d)", len);
    return len;
  }

  BT_HEXDUMP_DBG(stored_hash, sizeof(stored_hash), "Stored Hash: ");

  return 0;
}

static int db_hash_commit(void) {
  /* Stop work and generate the hash */
  if (k_delayed_work_remaining_get(&db_hash_work)) {
    k_delayed_work_cancel(&db_hash_work);
    db_hash_gen(false);
  }

  /* Check if hash matches then skip SC update */
  if (!memcmp(stored_hash, db_hash, sizeof(stored_hash))) {
    BT_DBG("Database Hash matches");
    k_delayed_work_cancel(&gatt_sc.work);
    return 0;
  }

  BT_HEXDUMP_DBG(db_hash, sizeof(db_hash), "New Hash: ");

  /**
   * GATT database has been modified since last boot, likely due to
   * a firmware update or a dynamic service that was not re-registered on
   * boot. Indicate Service Changed to all bonded devices for the full
   * database range to invalidate client-side cache and force discovery on
   * reconnect.
   */
  sc_indicate(0x0001, 0xffff);

  /* Hash did not match overwrite with current hash */
  db_hash_store();

  return 0;
}

SETTINGS_STATIC_HANDLER_DEFINE(bt_hash, "bt/hash", NULL, db_hash_set, db_hash_commit, NULL);
#endif /*CONFIG_BT_GATT_CACHING */
#endif /* CONFIG_BT_SETTINGS */
