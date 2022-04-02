DMA 设备
=========================

简介
------------------------

DMA(Direct Memory Access) 是一种内存存取技术，可以独立地直接读写系统内存，而不需处理器介入处理。在同等
程度的处理器负担下，DMA 是一种快速的数据传送方式。博流系列 MCU 中 DMA 设备具有以下特性：

- 控制器有 8 组独立专用通道
- 四种传输方向：内存至内存、内存至外设、外设至内存、外设到外设
- LLI 链表工作模式。

DMA 设备结构体定义
------------------------

.. code-block:: C

    typedef struct dma_device
    {
        struct device parent;
        uint8_t id;
        uint8_t ch;
        uint8_t transfer_mode;
        uint8_t direction;
        uint32_t src_req;
        uint32_t dst_req;
        uint8_t src_addr_inc;
        uint8_t dst_addr_inc;
        uint8_t src_burst_size;
        uint8_t dst_burst_size;
        uint8_t src_width;
        uint8_t dst_width;
        dma_lli_ctrl_t *lli_cfg;
    } dma_device_t;

- **parent**        继承父类属性
- **id**            DMA id号，默认0，当前只有一个DMA
- **ch**            通道号
- **transfer_mode** 传输模式
- **direction**     传输方向
- **src_req**       源请求
- **dst_req**       目标请求
- **src_addr_inc**  源地址自增
- **dst_addr_inc**  目标地址自增
- **src_burst_size** 源突发字节数
- **dst_burst_size** 目标突发字节数
- **src_width**     源传输位宽
- **dst_width**     目标传输位宽
- **lli_cfg**       用来存储dma通道的一些信息，用户不用管

``transfer_mode`` 提供以下类型

.. code-block:: C

    #define DMA_LLI_ONCE_MODE     0
    #define DMA_LLI_CYCLE_MODE    1

``direction`` 提供以下类型

.. code-block:: C

    typedef enum {
        DMA_MEMORY_TO_MEMORY = 0,                        /*!< DMA transfer tyep:memory to memory */
        DMA_MEMORY_TO_PERIPH,                            /*!< DMA transfer tyep:memory to peripheral */
        DMA_PERIPH_TO_MEMORY,                            /*!< DMA transfer tyep:peripheral to memory */
        DMA_PERIPH_TO_PERIPH,                            /*!< DMA transfer tyep:peripheral to peripheral */
    }dma_transfer_dir_type;

``src_req`` 提供以下类型

.. code-block:: C

    #define DMA_REQUEST_NONE        0x00000000 /*!< DMA request peripheral:None */
    #define DMA_REQUEST_UART0_RX    0x00000000 /*!< DMA request peripheral:UART0 RX */
    #define DMA_REQUEST_UART0_TX    0x00000001 /*!< DMA request peripheral:UART0 TX */
    #define DMA_REQUEST_UART1_RX    0x00000002 /*!< DMA request peripheral:UART1 RX */
    #define DMA_REQUEST_UART1_TX    0x00000003 /*!< DMA request peripheral:UART1 TX */
    #define DMA_REQUEST_I2C0_RX     0x00000006 /*!< DMA request peripheral:I2C RX */
    #define DMA_REQUEST_I2C0_TX     0x00000007 /*!< DMA request peripheral:I2C TX */
    #define DMA_REQUEST_SPI0_RX     0x0000000A /*!< DMA request peripheral:SPI RX */
    #define DMA_REQUEST_SPI0_TX     0x0000000B /*!< DMA request peripheral:SPI TX */
    #define DMA_REQUEST_I2S_RX      0x00000014 /*!< DMA request peripheral:I2S RX */
    #define DMA_REQUEST_I2S_TX      0x00000015 /*!< DMA request peripheral:I2S TX */
    #define DMA_REQUEST_ADC0        0x00000016 /*!< DMA request peripheral:ADC0 */
    #define DMA_REQUEST_DAC0        0x00000017 /*!< DMA request peripheral:DAC0 */
    #define DMA_REQUEST_USB_EP0     0x00000018 /*!< DMA request peripheral:USB EP0*/
    #define DMA_REQUEST_USB_EP1     0x00000019 /*!< DMA request peripheral:USB EP1*/
    #define DMA_REQUEST_USB_EP2     0x0000001A /*!< DMA request peripheral:USB EP2*/
    #define DMA_REQUEST_USB_EP3     0x0000001B /*!< DMA request peripheral:USB EP3*/
    #define DMA_REQUEST_USB_EP4     0x0000001C /*!< DMA request peripheral:USB EP4*/
    #define DMA_REQUEST_USB_EP5     0x0000001D /*!< DMA request peripheral:USB EP5*/
    #define DMA_REQUEST_USB_EP6     0x0000001E /*!< DMA request peripheral:USB EP6*/
    #define DMA_REQUEST_USB_EP7     0x0000001F /*!< DMA request peripheral:USB EP7 */

``dst_req`` 提供以下类型

.. code-block:: C

    #define DMA_REQUEST_NONE        0x00000000 /*!< DMA request peripheral:None */
    #define DMA_REQUEST_UART0_RX    0x00000000 /*!< DMA request peripheral:UART0 RX */
    #define DMA_REQUEST_UART0_TX    0x00000001 /*!< DMA request peripheral:UART0 TX */
    #define DMA_REQUEST_UART1_RX    0x00000002 /*!< DMA request peripheral:UART1 RX */
    #define DMA_REQUEST_UART1_TX    0x00000003 /*!< DMA request peripheral:UART1 TX */
    #define DMA_REQUEST_I2C0_RX     0x00000006 /*!< DMA request peripheral:I2C RX */
    #define DMA_REQUEST_I2C0_TX     0x00000007 /*!< DMA request peripheral:I2C TX */
    #define DMA_REQUEST_SPI0_RX     0x0000000A /*!< DMA request peripheral:SPI RX */
    #define DMA_REQUEST_SPI0_TX     0x0000000B /*!< DMA request peripheral:SPI TX */
    #define DMA_REQUEST_I2S_RX      0x00000014 /*!< DMA request peripheral:I2S RX */
    #define DMA_REQUEST_I2S_TX      0x00000015 /*!< DMA request peripheral:I2S TX */
    #define DMA_REQUEST_ADC0        0x00000016 /*!< DMA request peripheral:ADC0 */
    #define DMA_REQUEST_DAC0        0x00000017 /*!< DMA request peripheral:DAC0 */
    #define DMA_REQUEST_USB_EP0     0x00000018 /*!< DMA request peripheral:USB EP0*/
    #define DMA_REQUEST_USB_EP1     0x00000019 /*!< DMA request peripheral:USB EP1*/
    #define DMA_REQUEST_USB_EP2     0x0000001A /*!< DMA request peripheral:USB EP2*/
    #define DMA_REQUEST_USB_EP3     0x0000001B /*!< DMA request peripheral:USB EP3*/
    #define DMA_REQUEST_USB_EP4     0x0000001C /*!< DMA request peripheral:USB EP4*/
    #define DMA_REQUEST_USB_EP5     0x0000001D /*!< DMA request peripheral:USB EP5*/
    #define DMA_REQUEST_USB_EP6     0x0000001E /*!< DMA request peripheral:USB EP6*/
    #define DMA_REQUEST_USB_EP7     0x0000001F /*!< DMA request peripheral:USB EP7 */

``src_addr_inc`` 提供以下类型

.. code-block:: C

    #define DMA_ADDR_INCREMENT_DISABLE 0 /*!< Addr increment mode disable */
    #define DMA_ADDR_INCREMENT_ENABLE  1 /*!< Addr increment mode enable  */

``dst_addr_inc`` 提供以下类型

.. code-block:: C

    #define DMA_ADDR_INCREMENT_DISABLE 0 /*!< Addr increment mode disable */
    #define DMA_ADDR_INCREMENT_ENABLE  1 /*!< Addr increment mode enable  */

``src_burst_size`` 提供以下类型

.. code-block:: C

    #define DMA_BURST_1BYTE     0
    #define DMA_BURST_4BYTE     1
    #define DMA_BURST_8BYTE     2
    #define DMA_BURST_16BYTE    3

``dst_burst_size`` 提供以下类型

.. code-block:: C

    #define DMA_BURST_1BYTE     0
    #define DMA_BURST_4BYTE     1
    #define DMA_BURST_8BYTE     2
    #define DMA_BURST_16BYTE    3

``src_width`` 提供以下类型

.. code-block:: C

    #define DMA_TRANSFER_WIDTH_8BIT  0
    #define DMA_TRANSFER_WIDTH_16BIT 1
    #define DMA_TRANSFER_WIDTH_32BIT 2

``dst_width`` 提供以下类型

.. code-block:: C

    #define DMA_TRANSFER_WIDTH_8BIT  0
    #define DMA_TRANSFER_WIDTH_16BIT 1
    #define DMA_TRANSFER_WIDTH_32BIT 2

DMA 设备参数配置表
------------------------

每一个 DMA 设备都有一个参数配置宏,宏定义位于 ``bsp/board/xxx`` 目录下 ``peripheral_config.h`` 文件,变量定义位于 ``hal_dma.c`` 中，因此无需用户自己定义变量。当用户打开对应设备的宏，该设备的配置才生效。例如打开宏 ``BSP_USING_DMA0_CH0`` ，``DMA0_CH0_CONFIG`` 即生效，同时DMA 通道0设备就可以进行注册和使用了。

.. code-block:: C

    /*参数配置宏*/
    #if defined(BSP_USING_DMA0_CH0)
    #ifndef DMA0_CH0_CONFIG
    #define DMA0_CH0_CONFIG \
    {   \
        .id = 0,                                   \
        .ch = 7,                                   \
        .direction = DMA_MEMORY_TO_MEMORY,         \
        .transfer_mode = DMA_LLI_ONCE_MODE,        \
        .src_req = DMA_REQUEST_NONE,               \
        .dst_req = DMA_REQUEST_NONE,               \
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE, \
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE, \
        .src_burst_size = DMA_BURST_1BYTE,         \
        .dst_burst_size = DMA_BURST_1BYTE,         \
        .src_width = DMA_TRANSFER_WIDTH_8BIT,      \
        .dst_width = DMA_TRANSFER_WIDTH_8BIT,      \
    }
    #endif
    #endif


    /*变量定义*/
    static dma_device_t dmax_device[DMA_MAX_INDEX] =
    {
    #ifdef BSP_USING_DMA0_CH0
        DMA0_CH0_CONFIG,
    #endif
    #ifdef BSP_USING_DMA0_CH1
        DMA0_CH1_CONFIG,
    #endif
    #ifdef BSP_USING_DMA0_CH2
        DMA0_CH2_CONFIG,
    #endif
    #ifdef BSP_USING_DMA0_CH3
        DMA0_CH3_CONFIG,
    #endif
    #ifdef BSP_USING_DMA0_CH4
        DMA0_CH4_CONFIG,
    #endif
    #ifdef BSP_USING_DMA0_CH5
        DMA0_CH5_CONFIG,
    #endif
    #ifdef BSP_USING_DMA0_CH6
        DMA0_CH6_CONFIG,
    #endif
    #ifdef BSP_USING_DMA0_CH7
        DMA0_CH7_CONFIG,
    #endif
    };

.. note:: 上述配置可以通过 ``DMA_DEV(dev)->xxx`` 进行修改，只能在调用 ``device_open`` 之前使用。

DMA 设备接口
------------------------

DMA 设备接口全部遵循标准设备驱动管理层提供的接口。并且为了方便用户调用，将某些标准接口使用宏来重定义。

**dma_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``dma_register`` 用来注册一个 DMA 设备标准驱动接口，在注册之前需要打开对应 DMA 设备的通道宏定义。例如定义宏 ``BSP_USING_DMA_CH0`` 方可使用 ``DMA`` 设备的 0 通道,注册完成以后才可以使用其他接口，如果没有定义宏，则无法使用 ``DMA`` 设备的 0 通道。

.. code-block:: C

    int dma_register(enum dma_index_type index, const char *name);

- **index** 要注册的设备索引
- **name** 为注册的设备命名

``index`` 用来选择 DMA 设备某个通道的配置，一个 index 对应一个 DMA 设备的一个通道配置，比如 ``DMA_CH0_INDEX`` 对应 DMA 通道0 配置，``index`` 有如下可选类型

.. code-block:: C

    enum dma_index_type
    {
    #ifdef BSP_USING_DMA0_CH0
        DMA0_CH0_INDEX,
    #endif
    #ifdef BSP_USING_DMA0_CH1
        DMA0_CH1_INDEX,
    #endif
    #ifdef BSP_USING_DMA0_CH2
        DMA0_CH2_INDEX,
    #endif
    #ifdef BSP_USING_DMA0_CH3
        DMA0_CH3_INDEX,
    #endif
    #ifdef BSP_USING_DMA0_CH4
        DMA0_CH4_INDEX,
    #endif
    #ifdef BSP_USING_DMA0_CH5
        DMA0_CH5_INDEX,
    #endif
    #ifdef BSP_USING_DMA0_CH6
        DMA0_CH6_INDEX,
    #endif
    #ifdef BSP_USING_DMA0_CH7
        DMA0_CH7_INDEX,
    #endif
        DMA_MAX_INDEX
    };

**device_open**
^^^^^^^^^^^^^^^^

``device_open`` 用于打开一个 DMA 设备的一个通道，实际调用 ``dma_open``。

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

``device_close`` 用于关闭 DMA 设备的一个通道，实际调用 ``dma_close``。

.. code-block:: C

    int device_close(struct device *dev);

- **dev** 设备句柄
- **return** 错误码，0 表示关闭成功，其他表示错误

**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` 用于对 DMA 设备的一个通道进行控制和参数的修改，实际调用 ``dma_control``。

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);

- **dev** 设备句柄
- **cmd** 设备控制命令
- **args** 控制参数
- **return** 不同的控制命令返回的意义不同。

DMA 设备除了标准的控制命令，还具有自己特殊的控制命令。

.. code-block:: C

    #define DEVICE_CTRL_DMA_CHANNEL_GET_STATUS  0x10
    #define DEVICE_CTRL_DMA_CHANNEL_START       0x11
    #define DEVICE_CTRL_DMA_CHANNEL_STOP        0x12
    #define DEVICE_CTRL_DMA_CHANNEL_UPDATE      0x13

``args`` 根据不同的 ``cmd`` 传入不同，具体如下：

.. list-table:: table1
    :widths: 15 10 30
    :header-rows: 1

    * - cmd
      - args
      - description
    * - DEVICE_CTRL_SET_INT
      - NULL
      - 开启 dma 传输完成中断
    * - DEVICE_CTRL_CLR_INT
      - NULL
      - 关闭 dma 传输完成中断
    * - DEVICE_CTRL_DMA_CHANNEL_GET_STATUS
      - NULL
      - 获取 dma 通道完成状态
    * - DEVICE_CTRL_DMA_CHANNEL_START
      - NULL
      - 开启 dma 通道
    * - DEVICE_CTRL_DMA_CHANNEL_STOP
      - NULL
      - 关闭 dma 通道
    * - DEVICE_CTRL_DMA_CHANNEL_UPDATE
      - NULL
      - 更新 dma 传输配置

**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` 用于注册一个 DMA 设备的一个通道中断回调函数。

.. code-block:: C

    int device_set_callback(struct device *dev, void (*callback)(struct device *dev, void *args, uint32_t size, uint32_t event));

- **dev** 设备句柄
- **callback** 要注册的中断回调函数

    - **dev** 设备句柄
    - **args** 无用
    - **size** 无用
    - **event** 中断事件类型

``event`` 类型如下

.. code-block:: C

    enum dma_event_type
    {
        DMA_EVENT_COMPLETE,
    };


**dma_channel_start**
^^^^^^^^^^^^^^^^^^^^^^

``dma_channel_start`` 用于开启 DMA 通道。实际是调用 ``device_control`` ，其中 ``cmd`` 为 ``DEVICE_CTRL_DMA_CHANNEL_START``。

.. code-block:: C

    dma_channel_start(dev)

- **dev** 需要开启的 dma 通道句柄


**dma_channel_stop**
^^^^^^^^^^^^^^^^^^^^^^

``dma_channel_stop`` 用于关闭一个 DMA 通道。实际是调用 ``device_control`` ，其中 ``cmd`` 为 ``DEVICE_CTRL_DMA_CHANNEL_STOP``。

.. code-block:: C

    dma_channel_stop(dev)

- **dev** 需要关闭的 dma 通道句柄


**dma_channel_update**
^^^^^^^^^^^^^^^^^^^^^^^

``dma_channel_update`` 用于更新 DMA 的一个通道配置。实际是调用 ``device_control`` ，其中 ``cmd`` 为 ``DEVICE_CTRL_DMA_CHANNEL_UPDATE``。

.. code-block:: C

    dma_channel_update(dev,list)

- **dev** 需要更新的 dma 通道句柄
- **list** dma_lli_ctrl_t句柄


**dma_channel_check_busy**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``dma_channel_check_busy`` 用于查询当前使用的 DMA 通道是否传输完成。实际是调用 ``device_control`` ，其中 ``cmd`` 为 ``DEVICE_CTRL_DMA_CHANNEL_GET_STATUS``。

.. code-block:: C

    dma_channel_check_busy(dev)

- **dev** 需要查询的 dma 通道句柄
- **return** 当前 dma 状态，0为传输完成，1为未传输完成

**dma_reload**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``dma_reload`` 用于更新 DMA 一个通道的配置，相比于 ``dma_channel_update`` ,该函数无需用户传递很多参数，只需要填入源地址和目标地址，以及长度，内部会自己计算后再进行配置。此函数调用后，DMA通道是没有开启的，需要手动调用 ``dma_channel_start`` 函数。

.. code-block:: C

    int dma_reload(struct device *dev, uint32_t src_addr, uint32_t dst_addr, uint32_t transfer_size);

- **dev** 需要查询的DMA通道句柄
- **src_addr** 传输源地址
- **dst_addr** 传输目标地址
- **transfer_size** 传输字节总长度，如果传输的位数是16位、32位，这里需要进行转换成字节长度。



DMA 的效率与FIFO
------------------------

内存到内存
^^^^^^^^^^^^^^^

- DMA在搬运数据时，会以设定的位宽 ``xxx_width`` 与 设定的突发量 ``xxx_burst_size`` 去访问总线进行数据读写，在内存到内存的搬运时，一般 source 端与 destination 端的配置相同。

- 在总的数据量(data size)不变的情况下，位宽 ``xxx_width`` 越大传输速度越快，效率越高，但传输的数据总量(data size)必须是位宽 ``xxx_width`` 的整倍数，数据的地址也必须是 ``xxx_width`` 的整倍数(地址对齐)，否则会出错。

- 在连续读写时，burst 突发模式的总线利用效率比 single 单次模式高得多，因此可以尽量提高 ``xxx_burst_size``，但注意，DMA0 的每个通道只有 16Byte 的 FIFO，因此 width 乘 burst_size 的积必须小于等于 16Byte。

因此在内存到内存搬运数据时，最高效的是 ``xxx_width`` 值为 ``DMA_TRANSFER_WIDTH_32BIT``, ``xxx_burst_size`` 值为 ``DMA_BURST_4BYTE``,此时完全利用了 DMA 的FIFO，读写最快，总线占用最少，但要求数据量与地址满足对齐要求。


外设到内存 与 内存到外设
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

很多传输接口外设是存在 tx-FIFO 与 rx-FIFO 的，这些 FIFO 可以让外设的使用更加灵活方便，在与 DMA 配合使用时也可以增加效率。

- 在使用 DMA-接口外设 发送数据时，注意 DMA 的 ``des_width`` 与外设的 tx-FIFO 有效数据宽度应当一致，如使用 UART 时一般为 DMA_TRANSFER_WIDTH_8BIT。接收数据时 DMA 的 ``src_width`` 与 rx-FIFO 有效数据宽度也要一致。

- 外设端的 burst_size 对 外设的 FIFO 深度有要求，burst_size 必须小于等于 外设配置的 ``fifo_threshold`` 以保证不会出现写溢出或读溢出。

- 内存端配置的 burst_size 和 width 与外设端的可以不相等，但 burst_size 与 width 的乘积必须相等，并且小于 16Byte。内存端配置更高的 ``xxx_width`` 可以提高传输速度，减少对总线占用，但注意对数据量(data size)与地址的对齐要求。

如对于 I2S ,他的 tx 与 rx 的 FIFO 深度都为 8，I2S 最佳的 ``fifo_threshold`` 应为 4，DMA 的 ``xxx_burst_size`` 应该为 ``DMA_BURST_4BYTE``，这样能保证 I2S 的 FIFO 能留有一定余量防止出现 rx-FIFO 溢出与 tx-FIFO 欠载，又减少 DMA 了对总线的占用。

又如对于 SPI ，他的 tx 与 rx 的 FIFO 深度都为 4，若使用 burst_size 为 4 的方式传输，那么 SPI 的 ``fifo_threshold`` 只能是 4，没有冗余，若此时 CPU 在占用总线导致 DMA 传输不及时，可能会出现SPI传输间歇，在SPI从机模式下还可能出现发送欠载与接收溢出。
因此对于 SPI 而言，最佳的 ``fifo_threshold`` 应为 1，DMA 的 ``xxx_burst_size`` 应为 ``DMA_BURST_1BYTE``，此时 DMA 虽然对总线的访问效率一般，但保证了 SPI 的 FIFO 有冗余，不会出现上诉问题。