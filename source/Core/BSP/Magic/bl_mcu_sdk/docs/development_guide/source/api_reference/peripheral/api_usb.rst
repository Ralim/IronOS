USB 设备
=========================

简介
------------------------

USB 通信串行总线（Universal Serial Bus）是一种外部总线标准，用于规范电脑和外部设备的连接和通讯，是应用在 PC 领域的接口技术。博流 BL70X 系列 MCU 中 USB 设备具有以下特性：

- 支持 USB1.1 全速设备
- 支持 8 个双向端点：EP0 可配置为控制/批量/中断/同步端点，EP1-EP7 可配置为批量/中断/同步端点
- 每个端点均包含 TX、RX 两个方向的 FIFO，FIFO 深度 64 字节，并且支持 DMA
- 支持内部 transceiver
- 支持 suspend/resume
- 支持 LPM
- 支持多种 USB 中断配置

USB 设备结构体定义
------------------------

.. code-block:: C

    typedef struct usb_dc_device {
        struct device parent;
        uint8_t id;
        usb_dc_ep_state_t in_ep[8];
        usb_dc_ep_state_t out_ep[8];
        void *tx_dma;
        void *rx_dma;
    } usb_dc_device_t;

- **parent**        继承父类属性
- **id**            USB id，0 表示 USB0
- **in_ep**         USB 输入端点的参数
- **out_ep**        USB 输出端点的参数
- **tx_dma**        附加的发送 dma 句柄
- **rx_dma**        附加的接收 dma 句柄


``in_ep`` ``out_ep`` 的结构如下

.. code-block:: C

    typedef struct
    {
        uint8_t ep_ena;
        uint32_t is_stalled;
        struct usb_dc_ep_cfg ep_cfg;
    } usb_dc_ep_state_t;

- **ep_ena**        继承父类属性
- **is_stalled**    挂起状态，0表示非挂起状态，1表示挂起状态
- **ep_cfg**        端点的属性


``ep_cfg`` 的结构如下

.. code-block:: C

    struct usb_dc_ep_cfg {
        uint8_t ep_addr;
        uint16_t ep_mps;
        uint8_t ep_type;
    };

- **ep_addr**        端点的地址
- **ep_mps**         端点最大 packet 长度
- **ep_type**        端点传输类型


.. note:: 以上参数无需用户配置

USB 设备接口
------------------------

USB 设备接口遵循标准设备驱动管理层提供的接口。

**usb_dc_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``usb_dc_register`` 用来注册一个 USB 从设备标准驱动接口，在注册之前需要打开对应 USB 设备的宏定义,例如定义宏 ``BSP_USING_USB`` 方可使用 USB 设备。注册完成以后才可以使用其他接口，如果没有定义宏，则无法使用 USB 设备。

.. code-block:: C

    int usb_dc_register(enum usb_index_type index, const char *name);

- **index**     要注册的设备索引
- **name**      为注册的设备命名
- **return**    错误码，0表示成功，其他表示失败

**device_open**
^^^^^^^^^^^^^^^^

``device_open`` 用于打开一个 USB 设备，实际调用 ``usb_open``。

.. code-block:: C

    int device_open(struct device *dev, uint16_t oflag);

- **dev**       设备句柄
- **oflag**     设备的打开方式
- **return**    错误码，0表示成功，其他表示失败

**device_close**
^^^^^^^^^^^^^^^^

``device_close`` 用于关闭一个 USB 设备，实际调用 ``usb_close``。

.. code-block:: C

    int device_close(struct device *dev);

- **dev**       设备句柄
- **return**    错误码，0表示成功，其他表示失败

**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` 用于对 USB 设备的进行控制和参数的修改，实际调用 ``usb_control``。

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);

- **dev**       设备句柄
- **cmd**       设备控制命令
- **args**      控制参数
- **return**    错误码，0表示成功，其他表示失败

USB 设备除了标准的控制命令，还具有私有的控制命令。

.. list-table:: table1
    :widths: 15 10 30
    :header-rows: 1

    * - cmd
      - args
      - description
    * - DEVICE_CTRL_SET_INT
      - uint32_t
      - 开启 USB 中断
    * - DEVICE_CTRL_USB_DC_SET_ACK
      - uint32_t
      - 设置 USB 设备 ACK 状态
    * - DEVICE_CTRL_USB_DC_ENUM_ON
      - NULL
      - 打开 USB 设备枚举
    * - DEVICE_CTRL_USB_DC_ENUM_OFF
      - NULL
      - 关闭 USB 设备枚举
    * - DEVICE_CTRL_USB_DC_GET_EP_TX_FIFO_CNT
      - uint32_t
      - 设置 USB 设备发送 FIFO 的个数
    * - DEVICE_CTRL_USB_DC_GET_EP_RX_FIFO_CNT
      - uint32_t
      - 设置 USB 设备接收 FIFO 的个数
    * - DEVICE_CTRL_ATTACH_TX_DMA
      - device*
      - 设置 USB 设备发送 dma 句柄
    * - DEVICE_CTRL_ATTACH_RX_DMA
      - device*
      - 设置 USB 设备接收 dma 句柄
    * - DEVICE_CTRL_USB_DC_SET_TX_DMA
      - uint32_t
      - 开启 USB 设备通过 dma 发送
    * - DEVICE_CTRL_USB_DC_SET_RX_DMA
      - uint32_t
      - 开启 USB 设备通过 dma 接收

**device_write**
^^^^^^^^^^^^^^^^

``device_write`` 用于 USB 设备数据的发送,当前只支持同步传输 dma 发送，实际调用 ``usb_write``。

.. code-block:: C

    int device_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size);

- **dev** 设备句柄
- **pos** 无作用
- **buffer** 要写入的 buffer 缓冲区
- **size** 要写入的长度
- **return**    错误码，0表示成功，其他表示失败

**device_read**
^^^^^^^^^^^^^^^^

``device_read`` 用于 USB 设备数据的接收,当前只支持同步传输 dma 接收。实际调用 ``usb_read``。

.. code-block:: C

    int device_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size);

- **dev** 设备句柄
- **pos** 无作用
- **buffer** 要读入的 buffer 缓冲区
- **size** 要读入的长度
- **return**    错误码，0表示成功，其他表示失败

**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` 用于注册一个 USB 设备中断回调函数。

.. code-block:: C

    int device_set_callback(struct device *dev, void (*callback)(struct device *dev, void *args, uint32_t size, uint32_t event));

- **dev** 设备句柄
- **callback** 要注册的中断回调函数
- **return**    错误码，0表示成功，其他表示失败

    - **dev** 设备句柄
    - **args** 接收发送缓冲区，数据类型为 uint8_t*
    - **size** 传输长度
    - **event** 中断事件类型

``event`` 类型如下

.. code-block:: C

    enum usb_dc_event_type {
        /** USB error reported by the controller */
        USB_DC_EVENT_ERROR,
        /** USB reset */
        USB_DC_EVENT_RESET,
        /** Start of Frame received */
        USB_DC_EVENT_SOF,
        /** USB connection established, hardware enumeration is completed */
        USB_DC_EVENT_CONNECTED,
        /** USB configuration done */
        USB_DC_EVENT_CONFIGURED,
        /** USB connection suspended by the HOST */
        USB_DC_EVENT_SUSPEND,
        /** USB connection lost */
        USB_DC_EVENT_DISCONNECTED,
        /** USB connection resumed by the HOST */
        USB_DC_EVENT_RESUME,

        /** setup packet received */
        USB_DC_EVENT_SETUP_NOTIFY,
        /** ep0 in packet received */
        USB_DC_EVENT_EP0_IN_NOTIFY,
        /** ep0 out packet received */
        USB_DC_EVENT_EP0_OUT_NOTIFY,
        /** ep in packet except ep0 received */
        USB_DC_EVENT_EP_IN_NOTIFY,
        /** ep out packet except ep0 received */
        USB_DC_EVENT_EP_OUT_NOTIFY,
        /** Initial USB connection status */
        USB_DC_EVENT_UNKNOWN
    };

**usb_dc_send_from_ringbuffer**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``usb_dc_send_from_ringbuffer`` 用于从 ringbuffer 中读取数据并通过某个端点将数据发送出去。

.. code-block:: C

    int usb_dc_send_from_ringbuffer(struct device *dev, Ring_Buffer_Type *rb, uint8_t ep);

- **dev**       设备指针
- **rb**        rinbuffer 结构体指针
- **ep**        端点地址
- **return**    错误码，0表示成功，其他表示失败

**usb_dc_receive_to_ringbuffer**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``usb_dc_receive_to_ringbuffer`` 用于从某个端点接收数据并保存到 ringbuffer 中。

.. code-block:: C

    int usb_dc_receive_to_ringbuffer(struct device *dev, Ring_Buffer_Type *rb, uint8_t ep);

- **dev**       设备指针
- **rb**        rinbuffer 结构体指针
- **ep**        端点地址
- **return**    错误码，0表示成功，其他表示失败

.. important:: 以下函数为 USB Device 协议栈需要实现的 porting 接口，用户无需在应用层调用。

.. _usb_dc_set_address:

**usb_dc_set_address**
^^^^^^^^^^^^^^^^^^^^^^^^

``usb_dc_set_address`` 用于 USB 设备地址的配置。

.. code-block:: C

    int usb_dc_set_address(const uint8_t addr);

- **addr**      USB 设备的地址
- **return**    错误码，0表示成功，其他表示失败

.. _usb_dc_ep_open:

**usb_dc_ep_open**
^^^^^^^^^^^^^^^^^^^^^^^^

``usb_dc_ep_open`` 用于 USB 设备端点的开启。

.. code-block:: C

    int usb_dc_ep_open(const struct usbd_endpoint_cfg *ep_cfg);

- **ep_cfg**    端点的属性
- **return**    错误码，0表示成功，其他表示失败

.. _usb_dc_ep_close:

**usb_dc_ep_close**
^^^^^^^^^^^^^^^^^^^^^^^^

``usb_dc_ep_close`` 用于 USB 设备端点的关闭。

.. code-block:: C

    int usb_dc_ep_close(const uint8_t ep);

- **ep**        端点地址
- **return**    错误码，0表示成功，其他表示失败

.. _usb_dc_ep_set_stall:

**usb_dc_ep_set_stall**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``usb_dc_ep_set_stall`` 用于 USB 设备挂起状态的设置，挂起状态下无法收发数据。

.. code-block:: C

    int usb_dc_ep_set_stall(const uint8_t ep);

- **ep**        端点地址
- **return**    错误码，0表示成功，其他表示失败

.. _usb_dc_ep_clear_stall:

**usb_dc_ep_clear_stall**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``usb_dc_ep_clear_stall`` 用于端点挂起状态的清除，非挂起状态可以进行收发数据。

.. code-block:: C

    int usb_dc_ep_clear_stall(const uint8_t ep);

- **ep**        端点地址
- **return**    错误码，0表示成功，其他表示失败

.. _usb_dc_ep_is_stalled:

**usb_dc_ep_is_stalled**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``usb_dc_ep_is_stalled`` 用于 USB 设备挂起状态的查询。

.. code-block:: C

    int usb_dc_ep_is_stalled(const uint8_t ep, uint8_t *stalled);

- **ep**        端点id
- **stalled**   保存挂起状态的地址， 0 表示非挂起状态， 1 表示挂起状态
- **return**    错误码，0表示成功，其他表示失败

.. _usb_dc_ep_write:

**usb_dc_ep_write**
^^^^^^^^^^^^^^^^^^^^^^^^

``usb_dc_ep_write`` 用于向某个端点发送数据。

.. code-block:: C

    int usb_dc_ep_write(const uint8_t ep, const uint8_t *data, uint32_t data_len, uint32_t *ret_bytes);

- **ep**        端点地址
- **data**      要发送的数据地址
- **data_len**  要发送数据的长度
- **ret_bytes** 实际成功发生的数据长度
- **return**    错误码，0表示成功，其他表示失败

.. _usb_dc_ep_read:

**usb_dc_ep_read**
^^^^^^^^^^^^^^^^^^^^^^^^

``usb_dc_ep_read`` 用于从某个端点接收数据。

.. code-block:: C

    int usb_dc_ep_read(const uint8_t ep, uint8_t *data, uint32_t max_data_len, uint32_t *read_bytes);

- **ep**        端点地址
- **data**      要接收数据的地址
- **data_len**  要接收数据的长度，不能大于最大包长
- **ret_bytes** 实际成功接收的数据长度
- **return**    错误码，0表示成功，其他表示失败
