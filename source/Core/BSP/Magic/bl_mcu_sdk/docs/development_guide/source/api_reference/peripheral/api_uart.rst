UART 设备
=========================

简介
------------------------

UART 全称（Universal Asynchronous Receiver/Transmitter）通用异步收发传输器，提供了与外部设备进行全双工数据交换的灵活方式。博流系列 MCU 中 UART 设备具有以下特性：

- 数据位长度可选择 5/6/7/8 比特
- 停止位长度可选择 0.5/1/1.5/2 比特
- 支持奇/偶/无校验比特
- 支持硬件流控（RTS/CTS）
- 自动波特率检测
- 支持 LIN 协议（收发 BREAK/SYNC）
- 硬件字节发送/接收 FIFO
- 支持轮询、中断、DMA传输
- 特有的 rto 中断

UART 设备结构体定义
------------------------

.. code-block:: C

    typedef struct uart_device
    {
        struct device parent;
        uint8_t id;
        uint32_t baudrate;
        uart_databits_t databits;
        uart_stopbits_t stopbits;
        uart_parity_t parity;
        uint8_t fifo_threshold;
        void* tx_dma;
        void* rx_dma;
    } uart_device_t;

- **parent**    继承父类属性
- **id**        串口 id，使能串口0则id为0，使能串口1则id为1，以此类推
- **baudrate**  波特率
- **databits**  数据位
- **stopbits**  停止位
- **parity**    校验位
- **fifo_threshold** fifo 阈值,不同 mcu 最大值不同
- **tx_dma**    附加的发送 dma 句柄
- **rx_dma**    附加的接收 dma 句柄

``databits`` 提供以下类型

.. code-block:: C

    typedef enum
    {
        UART_DATA_LEN_5 = 0,  /*!< Data length is 5 bits */
        UART_DATA_LEN_6 = 1,  /*!< Data length is 6 bits */
        UART_DATA_LEN_7 = 2,  /*!< Data length is 7 bits */
        UART_DATA_LEN_8 = 3   /*!< Data length is 8 bits */
    } uart_databits_t;

``stopbits`` 提供以下类型

.. code-block:: C

    typedef enum
    {
        UART_STOP_ONE = 0,  /*!< One stop bit */
        UART_STOP_ONE_D_FIVE = 1,  /*!< 1.5 stop bit */
        UART_STOP_TWO = 2   /*!< Two stop bits */
    } uart_stopbits_t;

``parity`` 提供以下类型

.. code-block:: C

    typedef enum
    {
        UART_PAR_NONE = 0,  /*!< No parity */
        UART_PAR_ODD  = 1,  /*!< Parity bit is odd */
        UART_PAR_EVEN = 2,  /*!< Parity bit is even */
    } uart_parity_t;

UART 设备参数配置表
------------------------

每一个 UART 设备都有一个参数配置宏,宏定义位于 ``bsp/board/xxx`` 目录下 ``peripheral_config.h`` 文件,变量定义位于 ``hal_uart.c`` 中，因此无需用户自己定义变量。当用户打开对应设备的宏，该设备的配置才生效。例如打开宏 ``BSP_USING_UART0`` ，``UART0_CONFIG`` 即生效，同时 ``UART0`` 设备就可以进行注册和使用了。

.. code-block:: C

    /*参数配置宏*/
    #if defined(BSP_USING_UART0)
    #ifndef UART0_CONFIG
    #define UART0_CONFIG \
    {   \
    .id = 0, \
    .baudrate = 2000000,\
    .databits = UART_DATA_LEN_8, \
    .stopbits = UART_STOP_ONE, \
    .parity = UART_PAR_NONE, \
    .fifo_threshold = 1, \
    }
    #endif
    #endif

    /*变量定义*/
    static uart_device_t uartx_device[UART_MAX_INDEX] =
    {
    #ifdef BSP_USING_UART0
            UART0_CONFIG,
    #endif
    #ifdef BSP_USING_UART1
            UART1_CONFIG,
    #endif
    };

.. note:: 上述配置可以通过 ``UART_DEV(dev)->xxx`` 进行修改，只能在调用 ``device_open`` 之前使用。

UART 设备接口
------------------------

UART 设备接口全部遵循标准设备驱动管理层提供的接口。

**uart_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``uart_register`` 用来注册 UART 设备标准驱动接口，在注册之前需要打开对应 UART 设备的宏定义。例如定义宏 ``BSP_USING_UART0`` 方可使用 ``UART0`` 设备,注册完成以后才可以使用其他接口，如果没有定义宏，则无法使用 ``UART0`` 设备。

.. code-block:: C

    int uart_register(enum uart_index_type index, const char *name);

- **index** 要注册的设备索引
- **name** 为注册的设备命名

``index`` 用来选择 UART 设备配置，一个 index 对应一个 UART 设备配置，比如 ``UART0_INDEX`` 对应 ``UART0_CONFIG`` 配置，``index`` 有如下可选类型

.. code-block:: C

    enum uart_index_type
    {
    #ifdef BSP_USING_UART0
        UART0_INDEX,
    #endif
    #ifdef BSP_USING_UART1
        UART1_INDEX,
    #endif
        UART_MAX_INDEX
    };

**device_open**
^^^^^^^^^^^^^^^^

``device_open`` 用于打开 UART 设备，实际调用 ``uart_open``。

.. code-block:: C

    int device_open(struct device *dev, uint16_t oflag);

- **dev** 设备句柄
- **oflag** 设备的打开方式
- **return** 错误码，0 表示打开成功，其他表示错误

``oflag`` 提供以下类型

.. code-block:: C

    #define DEVICE_OFLAG_STREAM_TX  0x001 /* 设备以轮训发送模式打开 */
    #define DEVICE_OFLAG_STREAM_RX  0x002 /* 设备以轮训接收模式打开 */
    #define DEVICE_OFLAG_INT_TX     0x004 /* 设备以中断发送模式打开 */
    #define DEVICE_OFLAG_INT_RX     0x008 /* 设备以中断接收模式打开 */
    #define DEVICE_OFLAG_DMA_TX     0x010 /* 设备以 DMA 发送模式打开 */
    #define DEVICE_OFLAG_DMA_RX     0x020 /* 设备以 DMA 接收模式打开 */

**device_close**
^^^^^^^^^^^^^^^^

``device_close`` 用于关闭 UART 设备，实际调用 ``uart_close``。

.. code-block:: C

    int device_close(struct device *dev);

- **dev** 设备句柄
- **return** 错误码，0 表示关闭成功，其他表示错误

**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` 用于对 UART 设备的进行控制和参数的修改，实际调用 ``uart_control``。

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);

- **dev** 设备句柄
- **cmd** 设备控制命令
- **args** 控制参数
- **return** 不同的控制命令返回的意义不同。

串口设备除了标准的控制命令，还具有自己特殊的控制命令。

.. code-block:: C

    #define DEVICE_CTRL_UART_GET_TX_FIFO        0x10
    #define DEVICE_CTRL_UART_GET_RX_FIFO        0x11

``args`` 根据不同的 ``cmd`` 传入不同，具体如下：

.. list-table:: table1
    :widths: 15 10 30
    :header-rows: 1

    * - cmd
      - args
      - description
    * - DEVICE_CTRL_SET_INT
      - uart_it_type
      - 开启 uart 设备中断
    * - DEVICE_CTRL_CLR_INT
      - uart_it_type
      - 关闭 uart 设备中断
    * - DEVICE_CTRL_CONFIG
      - uart_param_cfg_t*
      - 修改串口配置
    * - DEVICE_CTRL_ATTACH_TX_DMA
      - NULL
      - 链接发送 dma 设备
    * - DEVICE_CTRL_ATTACH_RX_DMA
      - NULL
      - 链接接收 dma 设备
    * - DEVICE_CTRL_TX_DMA_SUSPEND
      - NULL
      - 挂起uart tx dma模式
    * - DEVICE_CTRL_RX_DMA_SUSPEND
      - NULL
      - 挂起uart rx dma模式
    * - DEVICE_CTRL_TX_DMA_RESUME
      - NULL
      - 恢复uart tx dma模式
    * - DEVICE_CTRL_RX_DMA_RESUME
      - NULL
      - 恢复uart rx dma模式
    * - DEVICE_CTRL_UART_GET_TX_FIFO
      - uint32_t*
      - 获取uart tx fifo 数据个数
    * - DEVICE_CTRL_UART_GET_RX_FIFO
      - uint32_t*
      - 获取uart rx fifo 数据个数

**device_write**
^^^^^^^^^^^^^^^^

``device_write`` 用于 UART 设备数据的发送，发送方式根据打开方式可以是轮询、中断、dma。实际调用 ``uart_write``。

.. code-block:: C

    int device_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size);

- **dev** 设备句柄
- **pos** 无作用
- **buffer** 要写入的 buffer 缓冲区
- **size** 要写入的长度
- **return** 错误码，0 表示写入成功，其他表示错误

**device_read**
^^^^^^^^^^^^^^^^

``device_read`` 用于 UART 设备数据的接收，接收方式根据打开方式可以是轮询、中断、dma。实际调用 ``uart_read``。

.. code-block:: C

    int device_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size);

- **dev** 设备句柄
- **pos** 无作用
- **buffer** 要读入的 buffer 缓冲区
- **size** 要读入的长度
- **return** 错误码，0 表示读入成功，其他表示错误

**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` 用于注册一个 UART 设备中断回调函数。

.. code-block:: C

    int device_set_callback(struct device *dev, void (*callback)(struct device *dev, void *args, uint32_t size, uint32_t event));

- **dev** 设备句柄
- **callback** 要注册的中断回调函数

    - **dev** 设备句柄
    - **args** 接收发送缓冲区，数据类型为 uint8_t*
    - **size** 传输长度
    - **event** 中断事件类型

``event`` 类型如下

.. code-block:: C

    enum uart_event_type
    {
        UART_EVENT_TX_END,
        UART_EVENT_TX_FIFO,
        UART_EVENT_RX_END,
        UART_EVENT_RX_FIFO,
        UART_EVENT_RTO,
        UART_EVENT_UNKNOWN
    };