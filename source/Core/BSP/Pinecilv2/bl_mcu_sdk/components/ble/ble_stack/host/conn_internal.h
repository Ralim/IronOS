/** @file
 *  @brief Internal APIs for Bluetooth connection handling.
 */

/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
typedef enum __packed {
    BT_CONN_DISCONNECTED,
    BT_CONN_CONNECT_SCAN,
    BT_CONN_CONNECT_DIR_ADV,
    BT_CONN_CONNECT,
    BT_CONN_CONNECTED,
    BT_CONN_DISCONNECT,
} bt_conn_state_t;

/* bt_conn flags: the flags defined here represent connection parameters */
enum {
    BT_CONN_AUTO_CONNECT,
    BT_CONN_BR_LEGACY_SECURE,     /* 16 digits legacy PIN tracker */
    BT_CONN_USER,                 /* user I/O when pairing */
    BT_CONN_BR_PAIRING,           /* BR connection in pairing context */
    BT_CONN_BR_NOBOND,            /* SSP no bond pairing tracker */
    BT_CONN_BR_PAIRING_INITIATOR, /* local host starts authentication */
    BT_CONN_CLEANUP,              /* Disconnected, pending cleanup */
    BT_CONN_AUTO_PHY_UPDATE,      /* Auto-update PHY */
    BT_CONN_SLAVE_PARAM_UPDATE,   /* If slave param update timer fired */
    BT_CONN_SLAVE_PARAM_SET,      /* If slave param were set from app */
    BT_CONN_SLAVE_PARAM_L2CAP,    /* If should force L2CAP for CPUP */
    BT_CONN_FORCE_PAIR,           /* Pairing even with existing keys. */

    BT_CONN_AUTO_PHY_COMPLETE, /* Auto-initiated PHY procedure done */
    BT_CONN_AUTO_FEATURE_EXCH, /* Auto-initiated LE Feat done */
    BT_CONN_AUTO_VERSION_INFO, /* Auto-initiated LE version done */

    /* Total number of flags - must be at the end of the enum */
    BT_CONN_NUM_FLAGS,
};

struct bt_conn_le {
    bt_addr_le_t dst;

    bt_addr_le_t init_addr;
    bt_addr_le_t resp_addr;

    u16_t interval;
    u16_t interval_min;
    u16_t interval_max;

    u16_t latency;
    u16_t timeout;
    u16_t pending_latency;
    u16_t pending_timeout;

    u8_t features[8];

    struct bt_keys *keys;

#if defined(CONFIG_BT_STACK_PTS)
    u8_t own_adder_type;
#endif
};

#if defined(CONFIG_BT_BREDR)
/* For now reserve space for 2 pages of LMP remote features */
#define LMP_MAX_PAGES 2

struct bt_conn_br {
    bt_addr_t dst;
    u8_t remote_io_capa;
    u8_t remote_auth;
    u8_t pairing_method;
    /* remote LMP features pages per 8 bytes each */
    u8_t features[LMP_MAX_PAGES][8];

    struct bt_keys_link_key *link_key;
};

struct bt_conn_sco {
    /* Reference to ACL Connection */
    struct bt_conn *acl;
    u16_t pkt_type;
};
#endif

struct bt_conn_iso {
    /* Reference to ACL Connection */
    struct bt_conn *acl;
    /* CIG ID */
    uint8_t cig_id;
    /* CIS ID */
    uint8_t cis_id;
};

typedef void (*bt_conn_tx_cb_t)(struct bt_conn *conn, void *user_data);

struct bt_conn_tx {
    sys_snode_t node;

    bt_conn_tx_cb_t cb;
    void *user_data;

    /* Number of pending packets without a callback after this one */
    u32_t pending_no_cb;
};

struct bt_conn {
    u16_t handle;
    u8_t type;
    u8_t role;

    ATOMIC_DEFINE(flags, BT_CONN_NUM_FLAGS);

    /* Which local identity address this connection uses */
    u8_t id;

#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
    bt_security_t sec_level;
    bt_security_t required_sec_level;
    u8_t encrypt;
#endif /* CONFIG_BT_SMP || CONFIG_BT_BREDR */

    /* Connection error or reason for disconnect */
    u8_t err;

    bt_conn_state_t state;

    u16_t rx_len;
    struct net_buf *rx;

    /* Sent but not acknowledged TX packets with a callback */
    sys_slist_t tx_pending;
    /* Sent but not acknowledged TX packets without a callback before
	 * the next packet (if any) in tx_pending.
	 */
    u32_t pending_no_cb;

    /* Completed TX for which we need to call the callback */
    sys_slist_t tx_complete;
    struct k_work tx_complete_work;

    /* Queue for outgoing ACL data */
    struct k_fifo tx_queue;

    /* Active L2CAP channels */
    sys_slist_t channels;

    atomic_t ref;

    /* Delayed work for connection update and other deferred tasks */
    struct k_delayed_work update_work;

    union {
        struct bt_conn_le le;
#if defined(CONFIG_BT_BREDR)
        struct bt_conn_br br;
        struct bt_conn_sco sco;
#endif
#if defined(CONFIG_BT_AUDIO)
        struct bt_conn_iso iso;
#endif
    };

#if defined(CONFIG_BT_REMOTE_VERSION)
    struct bt_conn_rv {
        u8_t version;
        u16_t manufacturer;
        u16_t subversion;
    } rv;
#endif
};

void bt_conn_reset_rx_state(struct bt_conn *conn);

/* Process incoming data for a connection */
void bt_conn_recv(struct bt_conn *conn, struct net_buf *buf, u8_t flags);

/* Send data over a connection */
int bt_conn_send_cb(struct bt_conn *conn, struct net_buf *buf,
                    bt_conn_tx_cb_t cb, void *user_data);

static inline int bt_conn_send(struct bt_conn *conn, struct net_buf *buf)
{
    return bt_conn_send_cb(conn, buf, NULL, NULL);
}

/* Add a new LE connection */
struct bt_conn *bt_conn_add_le(u8_t id, const bt_addr_le_t *peer);

/** Connection parameters for ISO connections */
struct bt_iso_create_param {
    uint8_t id;
    uint8_t num_conns;
    struct bt_conn **conns;
    struct bt_iso_chan **chans;
};

/* Bind ISO connections parameters */
int bt_conn_bind_iso(struct bt_iso_create_param *param);

/* Connect ISO connections */
int bt_conn_connect_iso(struct bt_conn **conns, uint8_t num_conns);

/* Add a new ISO connection */
struct bt_conn *bt_conn_add_iso(struct bt_conn *acl);

/* Cleanup ISO references */
void bt_iso_cleanup(struct bt_conn *iso_conn);

/* Allocate new ISO connection */
struct bt_conn *iso_conn_new(struct bt_conn *conns, size_t size);

/* Add a new BR/EDR connection */
struct bt_conn *bt_conn_add_br(const bt_addr_t *peer);

/* Add a new SCO connection */
struct bt_conn *bt_conn_add_sco(const bt_addr_t *peer, int link_type);

/* Cleanup SCO references */
void bt_sco_cleanup(struct bt_conn *sco_conn);

/* Look up an existing sco connection by BT address */
struct bt_conn *bt_conn_lookup_addr_sco(const bt_addr_t *peer);

/* Look up an existing connection by BT address */
struct bt_conn *bt_conn_lookup_addr_br(const bt_addr_t *peer);

void bt_conn_pin_code_req(struct bt_conn *conn);
u8_t bt_conn_get_io_capa(void);
u8_t bt_conn_ssp_get_auth(const struct bt_conn *conn);
void bt_conn_ssp_auth(struct bt_conn *conn, u32_t passkey);
void bt_conn_ssp_auth_complete(struct bt_conn *conn, u8_t status);

void bt_conn_disconnect_all(u8_t id);

/* Look up an existing connection */
struct bt_conn *bt_conn_lookup_handle(u16_t handle);

/* Compare an address with bt_conn destination address */
int bt_conn_addr_le_cmp(const struct bt_conn *conn, const bt_addr_le_t *peer);

/* Helpers for identifying & looking up connections based on the the index to
 * the connection list. This is useful for O(1) lookups, but can't be used
 * e.g. as the handle since that's assigned to us by the controller.
 */
#define BT_CONN_ID_INVALID 0xff
struct bt_conn *bt_conn_lookup_id(u8_t id);

/* Look up a connection state. For BT_ADDR_LE_ANY, returns the first connection
 * with the specific state
 */
struct bt_conn *bt_conn_lookup_state_le(const bt_addr_le_t *peer,
                                        const bt_conn_state_t state);

/* Set connection object in certain state and perform action related to state */
void bt_conn_set_state(struct bt_conn *conn, bt_conn_state_t state);

int bt_conn_le_conn_update(struct bt_conn *conn,
                           const struct bt_le_conn_param *param);

void notify_remote_info(struct bt_conn *conn);

void notify_le_param_updated(struct bt_conn *conn);

void notify_le_phy_updated(struct bt_conn *conn, u8_t tx_phy, u8_t rx_phy);

bool le_param_req(struct bt_conn *conn, struct bt_le_conn_param *param);

#if defined(CONFIG_BT_SMP)
/* rand and ediv should be in BT order */
int bt_conn_le_start_encryption(struct bt_conn *conn, u8_t rand[8],
                                u8_t ediv[2], const u8_t *ltk, size_t len);

/* Notify higher layers that RPA was resolved */
void bt_conn_identity_resolved(struct bt_conn *conn);
#endif /* CONFIG_BT_SMP */

#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
/* Notify higher layers that connection security changed */
void bt_conn_security_changed(struct bt_conn *conn, enum bt_security_err err);
#endif /* CONFIG_BT_SMP || CONFIG_BT_BREDR */

/* Prepare a PDU to be sent over a connection */
#if defined(CONFIG_NET_BUF_LOG)
struct net_buf *bt_conn_create_pdu_timeout_debug(struct net_buf_pool *pool,
                                                 size_t reserve, s32_t timeout,
                                                 const char *func, int line);
#define bt_conn_create_pdu_timeout(_pool, _reserve, _timeout)   \
    bt_conn_create_pdu_timeout_debug(_pool, _reserve, _timeout, \
                                     __func__, __LINE__)

#define bt_conn_create_pdu(_pool, _reserve)                      \
    bt_conn_create_pdu_timeout_debug(_pool, _reserve, K_FOREVER, \
                                     __func__, __line__)
#else
struct net_buf *bt_conn_create_pdu_timeout(struct net_buf_pool *pool,
                                           size_t reserve, s32_t timeout);

#define bt_conn_create_pdu(_pool, _reserve) \
    bt_conn_create_pdu_timeout(_pool, _reserve, K_FOREVER)
#endif

/* Prepare a PDU to be sent over a connection */
#if defined(CONFIG_NET_BUF_LOG)
struct net_buf *bt_conn_create_frag_timeout_debug(size_t reserve, s32_t timeout,
                                                  const char *func, int line);

#define bt_conn_create_frag_timeout(_reserve, _timeout)   \
    bt_conn_create_frag_timeout_debug(_reserve, _timeout, \
                                      __func__, __LINE__)

#define bt_conn_create_frag(_reserve)                      \
    bt_conn_create_frag_timeout_debug(_reserve, K_FOREVER, \
                                      __func__, __LINE__)
#else
struct net_buf *bt_conn_create_frag_timeout(size_t reserve, s32_t timeout);

#define bt_conn_create_frag(_reserve) \
    bt_conn_create_frag_timeout(_reserve, K_FOREVER)
#endif

/* Initialize connection management */
int bt_conn_init(void);

/* Selects based on connecton type right semaphore for ACL packets */
struct k_sem *bt_conn_get_pkts(struct bt_conn *conn);

/* k_poll related helpers for the TX thread */
int bt_conn_prepare_events(struct k_poll_event events[]);

#if (BFLB_BT_CO_THREAD)
void bt_conn_process_tx(struct bt_conn *conn, struct net_buf *tx_buf);
#else
void bt_conn_process_tx(struct bt_conn *conn);
#endif

#if defined(BFLB_BLE)
/** @brief Get connection handle for a connection.
 *
 * @param conn Connection object.
 * @param conn_handle Place to store the Connection handle.
 *
 * @return 0 on success or negative error value on failure.
 */
int bt_hci_get_conn_handle(const struct bt_conn *conn, u16_t *conn_handle);
#endif
