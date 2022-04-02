SPI 设备
=========================

简介
------------------------

串行外设接口（Serial Peripheral Interface Bus, SPI）是一种用于短程通信的同步串行通信接口规范，装置之间使用全
双工模式通信，是一个主机和一个或多个从机的主从模式。需要至少 4 根线，事实上 3 根也可以（单向传输时）, 包括
SDI（数据输入）、SDO（数据输出）、SCLK（时钟）、CS（片选）。博流系列 MCU 中 SPI 设备具有以下特性：

- 既可以作为 SPI 主机也可以作为 SPI 从机。
- 发送和接收通道各有深度为 4 个字的 FIFO
- 主从设备都支持 4 种时钟格式（CPOL，CPHA）
- 主从设备都支持 1/2/3/4 字节传输模式
- 灵活的时钟配置，最高可支持 40M 时钟
- 可配置 MSB/LSB 优先传输
- 接收过滤功能
- 从设备下的超时机制
- 支持轮询、中断、DMA 传输

SPI 设备结构体定义
------------------------

.. code-block:: C

    typedef struct spi_device
    {
        struct device parent;
        uint8_t id;
        uint32_t clk;
        uint8_t mode;
        uint8_t direction;
        uint8_t clk_polaraity;
        uint8_t clk_phase;
        uint8_t datasize;
        uint8_t fifo_threshold;
        void* tx_dma;
        void* rx_dma;
    } spi_device_t;

- **parent**        继承父类属性
- **id**            SPI id，0 表示 SPI0
- **clk**           SPI 时钟频率
- **mode**          主机模式或者从机模式
- **direction**     传输先行模式
- **clk_polaraity** 时钟极性
- **clk_phase**     时钟相位
- **datasize**      数据传输位宽
- **fifo_threshold** fifo 阈值, 最大为 4
- **tx_dma**        附加的发送 dma 句柄
- **rx_dma**        附加的接收 dma 句柄

``mode`` 提供以下类型

.. code-block:: C

    #define SPI_SLVAE_MODE                                0
    #define SPI_MASTER_MODE                               1

``direction`` 提供以下类型

.. code-block:: C

    #define SPI_LSB_BYTE0_DIRECTION_FIRST 0
    #define SPI_LSB_BYTE3_DIRECTION_FIRST 1
    #define SPI_MSB_BYTE0_DIRECTION_FIRST 2
    #define SPI_MSB_BYTE3_DIRECTION_FIRST 3

``clk_polaraity`` 提供以下类型

.. code-block:: C

    #define SPI_POLARITY_LOW                              0
    #define SPI_POLARITY_HIGH                             1

``clk_phase`` 提供以下类型

.. code-block:: C

    #define SPI_PHASE_1EDGE                               0
    #define SPI_PHASE_2EDGE                               1


``datasize`` 提供以下类型

.. code-block:: C

    #define SPI_DATASIZE_8BIT                            0
    #define SPI_DATASIZE_16BIT                           1
    #define SPI_DATASIZE_24BIT                           2
    #define SPI_DATASIZE_32BIT                           3

SPI 设备参数配置表
------------------------

每一个 SPI 设备都有一个参数配置宏,宏定义位于 ``bsp/board/xxx`` 目录下 ``peripheral_config.h`` 文件,变量定义位于 ``hal_spi.c`` 中，因此无需用户自己定义变量。当用户打开对应设备的宏，该设备的配置才生效。例如打开宏 ``BSP_USING_SPI0`` ，``SPI0_CONFIG`` 即生效，同时 ``SPI0`` 设备就可以进行注册和使用了。

.. code-block:: C

    /*参数配置宏*/
    #if defined(BSP_USING_SPI0)
    #ifndef SPI0_CONFIG
    #define SPI0_CONFIG \
    {   \
    .id = 0, \
    .clk = 18000000,\
    .mode = SPI_MASTER_MODE, \
    .direction = SPI_MSB_BYTE0_DIRECTION_FIRST, \
    .clk_polaraity = SPI_POLARITY_LOW, \
    .clk_phase = SPI_PHASE_1EDGE, \
    .datasize = SPI_DATASIZE_8BIT, \
    .fifo_threshold = 1, \
    }
    #endif
    #endif

    /*变量定义*/
    static spi_device_t spix_device[SPI_MAX_INDEX] =
    {
    #ifdef BSP_USING_SPI0
        SPI0_CONFIG,
    #endif
    };

.. note:: 上述配置可以通过 ``SPI_DEV(dev)->xxx`` 进行修改，只能在调用 ``device_open`` 之前使用。


SPI 设备接口
------------------------

SPI 设备接口全部遵循标准设备驱动管理层提供的接口。

**spi_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``spi_register`` 用来注册 SPI 设备标准驱动接口，在注册之前需要打开对应 SPI 设备的宏定义,例如定义宏 ``BSP_USING_SPI0`` 方可使用 SPI0 设备。注册完成以后才可以使用其他接口，如果没有定义宏，则无法使用 SPI 设备。

.. code-block:: C

    int spi_register(enum spi_index_type index, const char *name);

- **index** 要注册的设备索引
- **name** 为注册的设备命名

``index`` 用来选择 SPI 设备配置，一个 index 对应一个 SPI 设备配置，比如 ``SPI0_INDEX`` 对应 ``SPI0_CONFIG`` 配置，``index`` 有如下可选类型

.. code-block:: C

    enum spi_index_type
    {
    #ifdef BSP_USING_SPI0
        SPI0_INDEX,
    #endif
        SPI_MAX_INDEX
    };

**device_open**
^^^^^^^^^^^^^^^^

``device_open`` 用于打开一个 SPI 设备，实际调用 ``spi_open``。

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

``device_close`` 用于关闭一个 SPI 设备，实际调用 ``spi_close``。

.. code-block:: C

    int device_close(struct device *dev);

- **dev** 设备句柄
- **return** 错误码，0 表示关闭成功，其他表示错误

**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` 用于对 SPI 设备进行控制和参数的修改，实际调用 ``spi_control``。

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);

- **dev** 设备句柄
- **cmd** 设备控制命令
- **args** 控制参数
- **return** 不同的控制命令返回的意义不同。

SPI 设备除了标准的控制命令，还具有自己特殊的控制命令。

.. code-block:: C

    #define DEVICE_CTRL_SPI_CONFIG_CLOCK       0x10

``args`` 根据不同的 ``cmd`` 传入不同，具体如下：

.. list-table:: table1
    :widths: 15 10 30
    :header-rows: 1

    * - cmd
      - args
      - description
    * - DEVICE_CTRL_SET_INT
      - NULL
      - 开启 spi 设备中断
    * - DEVICE_CTRL_CLR_INT
      - NULL
      - 关闭 spi 设备中断
    * - DEVICE_CTRL_RESUME
      - NULL
      - 恢复 spi 设备
    * - DEVICE_CTRL_SUSPEND
      - NULL
      - 挂起 spi 设备
    * - DEVICE_CTRL_ATTACH_TX_DMA
      - NULL
      - 链接发送 dma 设备
    * - DEVICE_CTRL_ATTACH_RX_DMA
      - NULL
      - 链接接收 dma 设备
    * - DEVICE_CTRL_SPI_CONFIG_CLOCK
      - NULL
      - 修改 spi 设备时钟
    * - DEVICE_CTRL_TX_DMA_SUSPEND
      - NULL
      - 挂起spi tx dma模式
    * - DEVICE_CTRL_RX_DMA_SUSPEND
      - NULL
      - 挂起spi rx dma模式
    * - DEVICE_CTRL_TX_DMA_RESUME
      - NULL
      - 恢复spi tx dma模式
    * - DEVICE_CTRL_RX_DMA_RESUME
      - NULL
      - 恢复spi rx dma模式

**device_write**
^^^^^^^^^^^^^^^^

``device_write`` 用于 SPI 设备数据的发送，发送方式根据打开方式可以是轮询、中断、dma。实际调用 ``spi_write``。

.. code-block:: C

    int device_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size);

- **dev** 设备句柄
- **pos** 无作用
- **buffer** 要写入的 buffer 缓冲区
- **size** 要写入的长度
- **return** 错误码，0 表示写入成功，其他表示错误

**device_read**
^^^^^^^^^^^^^^^^

``device_read`` 用于 SPI 设备数据的接收，接收方式根据打开方式可以是轮询、中断、dma。实际调用 ``spi_read``。

.. code-block:: C

    int device_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size);

- **dev** 设备句柄
- **pos** 无作用
- **buffer** 要读入的 buffer 缓冲区
- **size** 要读入的长度
- **return** 错误码，0 表示读入成功，其他表示错误

**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` 用于注册一个 SPI 设备中断回调函数。

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

    enum spi_event_type
    {
        SPI_EVENT_TX_FIFO,
        SPI_EVENT_RX_FIFO,
        SPI_EVENT_UNKNOWN
    };

**spi_transmit**
^^^^^^^^^^^^^^^^^^^^^^^^

``spi_transmit`` 用于 SPI 设备数据的发送。

.. code-block:: C

    int spi_transmit(struct device *dev, void *buffer, uint32_t size, uint8_t type);

- **dev** 设备句柄
- **buffer** 发送数据缓冲区
- **size** 发送长度
- **type** 发送位宽类型

``type`` 提供以下类型

.. code-block:: C

    #define SPI_TRANSFER_TYPE_8BIT    0
    #define SPI_TRANSFER_TYPE_16BIT   1
    #define SPI_TRANSFER_TPYE_24BIT   2
    #define SPI_TRANSFER_TYPE_32BIT   3

**spi_receive**
^^^^^^^^^^^^^^^^^^^^^^^^

``spi_receive`` 用于 SPI 设备数据的接收。

.. code-block:: C

    int spi_receive(struct device *dev, void *buffer, uint32_t size, uint8_t type);

- **dev** 设备句柄
- **buffer** 接收数据缓冲区
- **size** 接收长度
- **type** 位宽类型

**spi_transmit_receive**
^^^^^^^^^^^^^^^^^^^^^^^^

``spi_transmit_receive`` 用于 SPI 设备数据的发送和接收。

.. code-block:: C

    int spi_transmit_receive(struct device *dev, const void *send_buf, void *recv_buf, uint32_t length, uint8_t type);

- **dev** 设备句柄
- **send_buf** 发送数据缓冲区
- **recv_buf** 接收数据缓冲区
- **length** 收发长度
- **type** 位宽类型
