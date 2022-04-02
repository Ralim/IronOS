.. _ble-index:

BLE
==================

Introduction
-------------------
- Features
    + HOST
        - GAP support peripheral and Central, Observer and Broadcaster
        - GATT support server and Client
        - Support pairing with the secure connection feature in Bluetooth 4.2
        - Support permanent storage of Bluetooth specific settings and data
    + mesh
        - TODO

- The architecture of the BLE protocol stack:
                        .. figure:: img/image1.png

    + There are 3 main layers, which together form a complete Bluetooth low power protocol stack
        - Host: Under the application program, it is composed of multiple (non-real-time) networks and transmission protocols, enabling the application program to communicate with peer devices in a standard and interoperable manner
        - Controller：The controller implements the link layer (LE LL), which is a low-level real-time protocol that provides standard interoperability for over-the-air communication with the radio. LL handles the reception and transmission of packets, guarantees the transfer of data, and handles all LL control procedures
        - Radio Hardware：Implement analog and digital baseband functions, allowing link layer firmware to transmit and receive in the 2.4GHz band of the spectrum

- Master Host:
                        .. figure:: img/image2.png

    * The Bluetooth Host layer implements all high-level protocols and configuration files, the most important thing is that it provides high-level APIs for applications
        - HCI: Host and controller interface
        - L2CAP: Logical Link Control and Adaptation Protocol
        - GATT: Generic Attribute Profile
        - GAP: Generic Access Profile
        - SMP: Security Manager Specification

- Application
    * The application layer contains the necessary protocol stack parameter settings and api reference. We analyze the two devices separately from the Bluetooth slave and the Bluetooth master
        * Bluetooth slave
            - Hardware and basic service initialization
            - Set broadcast parameters: broadcast data, broadcast interval, scan response, etc
            - Profile settings: add slave services, feature values, and set callback functions to receive host data, etc
            - Set pairing parameters (optional)
            - Start the broadcast, start running
            - Waiting for related events, and event processing, such as receiving data from the host, being linked, etc
        * Bluetooth host
            - Related hardware and basic service initialization
            - Set scan parameters
            - Set connection parameters
            - Set pairing parameters (optional)
            - Start the protocol stack and start running
            - Wait for related events, and event processing, such as scan events, Notify events from slaves, etc

API
----------

- API introduction

``void ble_controller_init(uint8_t task_priority)``

::

    /**
    * function      Controller layer initialization
    * @param[in]    task_priority: task priority
    * @return       None
    */

``int hci_driver_init(void)``

::

    /**
    * function      HCI interface driver initialization
    * @param[in]    None
    * @return       0: success, !=0: fail
    */

``int bt_enable(bt_ready_cb_t cb)``

::

    /**
    * function      BLE enable
    * @param[in]    cb: Call the callback function if successful
    * @return       0: success, !=0: fail
    */

``int bt_le_adv_start(const struct bt_le_adv_param *param,const struct bt_data *ad, size_t ad_len,``
                            ``const struct bt_data *sd, size_t sd_len)``

::

    /**
    * function      Turn on BLE broadcast
    *
    * @param[in]    param:  Pointer to broadcast configuration parameter
    * @param[in]    ad:     Pointer to the data in the broadcast packet
    * @param[in]    ad_len: The length of the data in the broadcast packet
    * @param[in]    sd:     Pointer to scan response packet data
    * @param[in]    sd_len: Scan response packet data length
    * @return               0: success, !=0: fail
    */


``int bt_le_adv_update_data(const struct bt_data *ad, size_t ad_len,const struct bt_data *sd, size_t sd_len)``


::

    /**
    * function      Update BLE broadcast data
    * @param[in]    ad:     Pointer to the data in the broadcast packet
    * @param[in]    ad_len: The length of the data in the broadcast packet
    * @param[in]    sd:     Pointer to scan response packet data
    * @param[in]    sd_len: Scan response packet data length
    * @return               0: success, !=0: fail
    */

``int bt_le_adv_stop(void)``


::

    /**
    * function      Stop BLE broadcast
    * @param[in]    None
    * @return       0: success, !=0: fail
    */


``int bt_le_scan_start(const struct bt_le_scan_param *param, bt_le_scan_cb_t cb)``

::

    /**
    * function      Turn on BLE scanning
    * @param[in]    param: Pointer to scan parameter
    * @param[in]    cb:    Scan callback function
    * @return              0: success, !=0: fail
    */

``int bt_le_scan_stop(void)``

::

    /**
    * function      Stop BLE scanning
    * @param[in]    None
    * @return       0: success, !=0: fail
    */

``int bt_le_whitelist_add(const bt_addr_le_t *addr)``

::

    /**
    * function      Add devices to the whitelist by address
    * @param[in]    addr: Pointer to the address of the device that needs to be added
    * @return       0: success, !=0: fail
    */

``int bt_le_whitelist_rem(const bt_addr_le_t *addr)``

::


    /**
    * function      Remove the device from the whitelist
    * @param[in]    addr: Pointer to the address of the device that needs to be removed
    * @return       0: success, !=0: fail
    */

``int bt_le_whitelist_clear(void)``


::

    /**
    * function      Clear the whitelist list
    * @param[in]    None
    * @return       0: success, !=0: fail
    */

``int bt_le_set_chan_map(u8_t chan_map[5])``

::

    /**
    * function      Set (LE) channel mapping
    * @param[in]    chan_map: channel array
    * @return       0: success, !=0: fail
    */

``int bt_unpair(u8_t id, const bt_addr_le_t *addr)``

::

    /**
    * function      Clear pairing information
    * @param[in]    id: Local ID (mostly just the default BT ID)
    * @param[in]    addr: Remote device address, NULL or BT_ADDR_LE_ANY to clear all remote devices
    * @return       0: success, !=0: fail
    */

``int bt_conn_get_info(const struct bt_conn *conn, struct bt_conn_info *info)``

::

    /**
    * function      Get the information of the currently connected device
    * @param[in]    conn: Pointer to the current connection
    * @param[in]    info: Pointer to the information of the currently connected device
    * @return       0: success, !=0: fail
    */


``int bt_conn_get_remote_dev_info(struct bt_conn_info *info)``

::

    /**
    * function      Get information about connected devices
    * @param[in]    info: Pointer to the information of the currently connected device
    * @return       Number of connected devices
    */

``int bt_conn_le_param_update(struct bt_conn *conn,const struct bt_le_conn_param *param)``

::

    /**
    * function      Update connection parameters
    * @param[in]    conn: Pointer to the current connection
    * @param[in]    param: Pointer to connection parameter
    * @return       0: success, !=0: fail
    */

``int bt_conn_disconnect(struct bt_conn *conn, u8_t reason)``

::

    /**
    * function      Disconnect current connection
    * @param[in]    conn: pointer to the current connection
    * @param[in]    reason: the reason for disconnecting the current connection
    * @return       0: success, !=0: fail
    */

``struct bt_conn *bt_conn_create_le(const bt_addr_le_t *peer,const struct bt_le_conn_param *param)``

::

    /**
    * function      Create connection
    * @param[in]    peer: The pointer of the device address that needs to be connected
    * @param[in]    param: Pointer to connection parameter
    * @return       Success: a valid connection object, otherwise it fails
    */


``int bt_conn_create_auto_le(const struct bt_le_conn_param *param)``

::

    /**
    * function      Automatically create and connect to devices in the whitelist
    * @param[in]    param: pointer to connection parameter
    * @return       0: success, !=0: fail
    */

``int bt_conn_create_auto_stop(void)``

::

    /**
    * function      Stop automatic creation and connect to devices in the whitelist
    * @param[in]    None
    * @return       0: success, !=0: fail
    */

``int bt_le_set_auto_conn(const bt_addr_le_t *addr,const struct bt_le_conn_param *param)``

::

    /**
    * function      Automatically create a connection to the remote device
    * @param[in]    addr: Remote device address pointer
    * @param[in]    param: Pointer to connection parameter
    * @return       0: success, !=0: fail
    */

``struct bt_conn *bt_conn_create_slave_le(const bt_addr_le_t *peer,const struct bt_le_adv_param *param)``

::

    /**
    * function      Initiate a directed broadcast packet to the remote device
    * @param[in]    peer: Remote device address pointer
    * @param[in]    param: Pointer to broadcast parameters
    * @return       Success: a valid connection object, otherwise it fails
    */

``int bt_conn_set_security(struct bt_conn *conn, bt_security_t sec)``

::

    /**
    * function      Set the connection security level
    * @param[in]    conn: Pointer to the connection object
    * @param[in]    sec: Security level
    * @return       0: success, !=0: fail
    */

``bt_security_t bt_conn_get_security(struct bt_conn *conn)``

::

    /**
    * function      Get the security level of the current connection
    * @param[in]    conn: Pointer to the connection object
    * @return       Security Level
    */


``u8_t bt_conn_enc_key_size(struct bt_conn *conn)``

::

    /**
    * function      Get the size of the encryption key of the current connection
    * @param[in]    conn: Pointer to the connection object
    * @return       The size of the encryption key
    */


``void bt_conn_cb_register(struct bt_conn_cb *cb)``

::

    /**
    * function      Register connection callback function
    * @param[in]    cb: Connection callback function
    * @return       None
    */

``void bt_set_bondable(bool enable)``

::

    /**
    * function      Set/clear the binding flag in the SMP pairing request/response to the data authentication request
    * @param[in]    enable: 1: enable, 0: disable
    * @return       None
    */

``int bt_conn_auth_cb_register(const struct bt_conn_auth_cb *cb)``

::

    /**
    * function      Register authentication callback function
    * @param[in]    cb: Callback function pointer
    * @return       0: success, !=0: failure
    */

``int bt_conn_auth_passkey_entry(struct bt_conn *conn, unsigned int passkey)``

::

    /**
    * function      Reply with the key
    * @param[in]    conn: Connect object pointer
    * @param[in]    passkey: the key entered
    * @return       0: success, !=0: failure
    */

``int bt_conn_auth_cancel(struct bt_conn *conn)``

::

    /**
    * function      Cancel the authentication process
    * @param[in]    conn: Connection object pointer
    * @return       0: success, !=0: failure
    */

``int bt_conn_auth_passkey_confirm(struct bt_conn *conn)``

::

    /**
    * function      If the password matches, reply to the other side
    * @param[in]    conn: Pointer to connection object
    * @return       0: success, !=0: failure
    */

``int bt_conn_auth_pincode_entry(struct bt_conn *conn, const char *pin)``

::

    /**
    * function      Reply with PIN code
    * @param[in]    conn: Pointer to connection object
    * @param[in]    pin: PIN code pointer
    * @return       0: success, !=0: failure
    */

``int bt_le_read_rssi(u16_t handle,int8_t *rssi)``

::

    /**
    * function      Read the RSSI value of the opposite device
    * @param[in]    handle: The handle value of the connection
    * @param[in]    rssi: rssi pointer
    * @return       0: success, !=0: failure
    */

``int bt_get_local_address(bt_addr_le_t *adv_addr)``

::

    /**
    * function      Read the address of the machine
    * @param[in]    adv_addr: Pointer to address
    * @return       0: success, !=0: failure
    */

``int bt_set_tx_pwr(int8_t power)``

::

    /**
    * function      Set the transmit power of this device
    * @param[in]    power: power value
    * @return       0: success, !=0: failure
    */

Data structure reference
-----------------------------

``bt_le_adv_param``\ data structure:

.. code:: c

    /** LE Advertising Parameters. */
    struct bt_le_adv_param {
        /** Local identity */
        u8_t  id;

        /** Bit-field of advertising options */
        u8_t  options;

        /** Minimum Advertising Interval (N * 0.625) */
        u16_t interval_min;

        /** Maximum Advertising Interval (N * 0.625) */
        u16_t interval_max;

        #if defined(CONFIG_BT_STACK_PTS)
        u8_t  addr_type;
        #endif
    };

This data structure is used to configure broadcast parameters, including local identification, broadcast option bit fields, broadcast gaps, etc. The broadcast option bit fields have the following enumerated type parameters to choose from:

.. code:: c

    enum {
        /** Convenience value when no options are specified. */
        BT_LE_ADV_OPT_NONE = 0,

        /** Advertise as connectable. Type of advertising is determined by
            * providing SCAN_RSP data and/or enabling local privacy support.
            */
        BT_LE_ADV_OPT_CONNECTABLE = BIT(0),

        /** Don't try to resume connectable advertising after a connection.
            *  This option is only meaningful when used together with
            *  BT_LE_ADV_OPT_CONNECTABLE. If set the advertising will be stopped
            *  when bt_le_adv_stop() is called or when an incoming (slave)
            *  connection happens. If this option is not set the stack will
            *  take care of keeping advertising enabled even as connections
            *  occur.
            */
        BT_LE_ADV_OPT_ONE_TIME = BIT(1),

        /** Advertise using the identity address as the own address.
            *  @warning This will compromise the privacy of the device, so care
            *           must be taken when using this option.
            */
        BT_LE_ADV_OPT_USE_IDENTITY = BIT(2),

        /** Advertise using GAP device name */
        BT_LE_ADV_OPT_USE_NAME = BIT(3),

        /** Use low duty directed advertising mode, otherwise high duty mode
            *  will be used. This option is only effective when used with
            *  bt_conn_create_slave_le().
            */
        BT_LE_ADV_OPT_DIR_MODE_LOW_DUTY = BIT(4),

        /** Enable use of Resolvable Private Address (RPA) as the target address
            *  in directed advertisements when CONFIG_BT_PRIVACY is not enabled.
            *  This is required if the remote device is privacy-enabled and
            *  supports address resolution of the target address in directed
            *  advertisement.
            *  It is the responsibility of the application to check that the remote
            *  device supports address resolution of directed advertisements by
            *  reading its Central Address Resolution characteristic.
            */
        BT_LE_ADV_OPT_DIR_ADDR_RPA = BIT(5),

        /** Use whitelist to filter devices that can request scan response
            *  data.
            */
        BT_LE_ADV_OPT_FILTER_SCAN_REQ = BIT(6),

        /** Use whitelist to filter devices that can connect. */
        BT_LE_ADV_OPT_FILTER_CONN = BIT(7),
    };

If you need to send a broadcast packet, the configuration can be as follows:

.. code:: c

    param.id = 0;
    param.options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_NAME | BT_LE_ADV_OPT_ONE_TIME);
    param.interval_min = 0x00a0;
    param.interval_max = 0x00f0;

``bt_data``\ data structure:

.. code:: c

    struct bt_data {
        u8_t type;
        u8_t data_len;
        const u8_t *data;
    };

This data structure is used to fill the data in the broadcast packet, the specific data packet type can refer to the following:

.. code:: c

    Service UUID
    Local Name
    Flags
    Manufacturer Specific Data
    TX Power Level
    Secure Simple Pairing OOB
    Security Manager OOB
    Security Manager TK Value
    Slave Connection Interval Range
    Service Solicitation
    Service Data
    Appearance
    Public Target Address
    Random Target Address
    Advertising Interval
    LE Bluetooth Device Address
    LE Role
    Uniform Resource Identifier
    LE Supported Features
    Channel Map Update Indication

Use this data structure to configure a broadcast packet data, as shown below:

.. code:: c

    struct bt_data ad_discov[] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
        BT_DATA(BT_DATA_NAME_COMPLETE, "BL602-BLE-DEV", 13),
    };

``bt_le_scan_param``\ data structure:

.. code:: c

    /** LE scan parameters */
    struct bt_le_scan_param {
        /** Scan type (BT_LE_SCAN_TYPE_ACTIVE or BT_LE_SCAN_TYPE_PASSIVE) */
        u8_t  type;

        /** Bit-field of scanning filter options. */
        u8_t  filter_dup;

        /** Scan interval (N * 0.625 ms) */
        u16_t interval;

        /** Scan window (N * 0.625 ms) */
        u16_t window;
    };

This data structure is used to fill scan parameters,
type: There are two types of scan types: BT_LE_SCAN_TYPE_ACTIVE (0x01) and BT_LE_SCAN_TYPE_PASSIVE (0x00)
filter_dup: 0x00, except for targeted advertisements, accept all broadcast and scan responses, 0x01, only receive broadcast and scan responses from devices in the whitelist
interval: Scan interval
window: Scan window

If the scan request is enabled, it can be configured as follows:

.. code:: c

    scan_param.type = BT_LE_SCAN_TYPE_PASSIVE
    scan_param.filter_dup = 0x00
    interval=BT_GAP_SCAN_SLOW_INTERVAL_1
    window=BT_GAP_SCAN_SLOW_WINDOW_1


``bt_le_conn_param``\ data structure:

.. code:: c

    /** Connection parameters for LE connections */
    struct bt_le_conn_param {
        u16_t interval_min;
        u16_t interval_max;
        u16_t latency;
        u16_t timeout;

        #if defined(CONFIG_BT_STACK_PTS)
        u8_t  own_address_type;
        #endif
    };

This data structure is used to fill in the connection parameters, interval_min: the minimum value of the connection interval (0x0018), interval_max: the maximum value of the connection interval (0x0028),
latency: The maximum slave latency allowed for connection events
timeout: The time for the connection to time out

Configure the data structure as follows:

.. code:: c

    interval_min=BT_GAP_INIT_CONN_INT_MIN(0x0018)
    interval_max=BT_GAP_INIT_CONN_INT_MAX(0x0028)
    latency=0
    timeout=400

``bt_conn``\ data structure:

.. code:: c

    struct bt_conn {
        u16_t			handle;
        u8_t			type;
        u8_t			role;

        ATOMIC_DEFINE(flags, BT_CONN_NUM_FLAGS);

        /* Which local identity address this connection uses */
        u8_t                    id;

    #if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
        bt_security_t		sec_level;
        bt_security_t		required_sec_level;
        u8_t			encrypt;
    #endif /* CONFIG_BT_SMP || CONFIG_BT_BREDR */

        /* Connection error or reason for disconnect */
        u8_t			err;

        bt_conn_state_t		state;

        u16_t		        rx_len;
        struct net_buf		*rx;

        /* Sent but not acknowledged TX packets with a callback */
        sys_slist_t		tx_pending;
        /* Sent but not acknowledged TX packets without a callback before
        * the next packet (if any) in tx_pending.
        */
        u32_t                   pending_no_cb;

        /* Completed TX for which we need to call the callback */
        sys_slist_t		tx_complete;
        struct k_work           tx_complete_work;


        /* Queue for outgoing ACL data */
        struct k_fifo		tx_queue;

        /* Active L2CAP channels */
        sys_slist_t		channels;

        atomic_t		ref;

        /* Delayed work for connection update and other deferred tasks */
        struct k_delayed_work	update_work;

        union {
            struct bt_conn_le	le;
    #if defined(CONFIG_BT_BREDR)
            struct bt_conn_br	br;
            struct bt_conn_sco	sco;
    #endif
        };

    #if defined(CONFIG_BT_REMOTE_VERSION)
        struct bt_conn_rv {
            u8_t  version;
            u16_t manufacturer;
            u16_t subversion;
        } rv;
    #endif
    };

This data structure is the current connection data structure, which includes the parameters related to the BLE connection. After the connection is successful, the data structure can be called by the user.

