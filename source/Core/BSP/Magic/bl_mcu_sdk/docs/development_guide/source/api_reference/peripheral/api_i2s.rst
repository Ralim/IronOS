I2S 设备
=========================

简介
------------------------

I2S (Inter—IC Sound) 总线, 又称集成电路内置音频总线，为数字音频设备之间的音频数据传输而制定的一种总线标准，
该总线专门用于音频设备之间的数据传输，广泛应用于各种多媒体系统。博流系列 MCU 中 I2S 设备具有以下特性：

-  支持主模式以及从模式
-  支持 Left-justified/Right-justified/DSP 等数据格式
-  支持 8/16/24/32 比特数据宽度
-  除单声道/双声道模式之外，同时支持四声道模式
-  支持动态静音切换功能
-  数据发送 FIFO 的宽度为 32 位，深度 16
-  数据接收 FIFO 的宽度为 32 位，深度 16

I2S 设备结构体定义
------------------------

.. code-block:: C

    typedef struct i2s_device {
        struct      device parent;
        uint8_t     id;
        i2s_mode_t          iis_mode;
        interface_mode_t    interface_mode;
        uint32_t            sampl_freq_hz;
        i2s_channel_num_t   channel_num;
        i2s_frame_size_t    frame_size;
        i2s_data_size_t     data_size;
        uint8_t     fifo_threshold;
        void        *tx_dma;
        void        *rx_dma;
    } i2s_device_t;

- **parent** 继承父类属性
- **id** 外设 id 号, 0 表示 I2S0
- **iis_mode** 主从模式选择
- **interface_mode**  协议类型
- **sampl_freq_hz**  音频数据采样率
- **channel_num**  音频通道数，非 DSP 模式下最高 2 通道
- **frame_size**  帧格式长度，即每个通道音频的 BCLK 个数
- **data_size** 实际的有效音频数据位宽，不能大于 frame_size
- **fifo_threshold**  收发数据的 fifo 深度阈值
- **tx_dma**  附加的发送 dma 句柄
- **rx_dma**  附加的接收 dma 句柄

``iis_mode`` 提供以下类型

.. code-block:: C

    typedef enum {
        I2S_MODE_MASTER = 0, /*!< I2S as master */
        I2S_MODE_SLAVE,      /*!< I2S as slave */
    } i2s_mode_t;

``interface_mode`` 提供以下类型

.. code-block:: C

    typedef enum {
        I2S_MODE_STD,   /*!< I2S STD Mode */
        I2S_MODE_LEFT,  /*!< Left-Justified Mode */
        I2S_MODE_RIGHT, /*!< Right-Justified Mode */
        I2S_MODE_DSP_A, /*!< DSP/PCM Mode A*/
        I2S_MODE_DSP_B, /*!< DSP/PCM Mode B*/
    } interface_mode_t;

``channel_num`` 提供以下类型

.. code-block:: C

    typedef enum {
        I2S_FS_CHANNELS_NUM_MONO = 1, /*!< I2S frame is for 1 channels */
        I2S_FS_CHANNELS_NUM_2 = 2,    /*!< I2S frame is for 2 channels */
        I2S_FS_CHANNELS_NUM_3 = 3,    /*!< I2S frame is for 3 channels, DSP mode only, frame_size must equal data_size*/
        I2S_FS_CHANNELS_NUM_4 = 4,    /*!< I2S frame is for 4 channels, DSP mode only, frame_size must equal data_size*/
    } i2s_channel_num_t;

``frame_size`` 提供以下类型

.. code-block:: C

    typedef enum {
        I2S_FRAME_LEN_8 = 1,  /*!< I2S frame size 8 bits */
        I2S_FRAME_LEN_16 = 2, /*!< I2S frame size 16 bits */
        I2S_FRAME_LEN_24 = 3, /*!< I2S frame size 24 bits */
        I2S_FRAME_LEN_32 = 4, /*!< I2S frame size 32 bits */
    } i2s_frame_size_t;

``data_size`` 提供以下类型

.. code-block:: C

    typedef enum {
        I2S_DATA_LEN_8 = 1,  /*!< I2S data size 8 bits */
        I2S_DATA_LEN_16 = 2, /*!< I2S data size 16 bits */
        I2S_DATA_LEN_24 = 3, /*!< I2S data size 24 bits */
        I2S_DATA_LEN_32 = 4, /*!< I2S data size 32 bits */
    } i2s_data_size_t;

I2S 设备参数配置表
------------------------

每一个 I2S 设备都有一个参数配置宏,宏定义位于 ``bsp/board/xxx`` 目录下 ``peripheral_config.h`` 文件,
因此无需用户自己定义变量。当用户打开对应设备的宏，该设备的配置才生效。
例如打开宏 ``BSP_USING_I2S0`` ， ``I2S0_CONFIG`` 即生效，同时 ``I2S0`` 设备就可以进行注册和使用了。

.. code-block:: C

    #if defined(BSP_USING_I2S0)
    #ifndef I2S0_CONFIG
    #define I2S0_CONFIG                              \
        {                                            \
            .id = 0,                                 \
            .iis_mode = I2S_MODE_MASTER,             \
            .interface_mode = I2S_MODE_LEFT,         \
            .sampl_freq_hz = 16 * 1000,              \
            .channel_num = I2S_FS_CHANNELS_NUM_MONO, \
            .frame_size = I2S_FRAME_LEN_16,          \
            .data_size = I2S_DATA_LEN_16,            \
            .fifo_threshold = 8,                     \
        }
    #endif
    #endif

    static i2s_device_t i2sx_device[I2S_MAX_INDEX] = {
    #ifdef BSP_USING_I2S0
        I2S0_CONFIG,
    #endif
    };

.. note:: 上述配置可以通过 ``I2S_DEV(dev)->xxx`` 进行修改，只能在调用 ``device_open`` 之前使用。

I2S 设备接口
------------------------

I2S 设备接口全部遵循标准设备驱动管理层提供的接口。

**i2s_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``i2s_register`` 用来注册一个 I2S 标准驱动接口，在注册之前需要打开对应 I2S 设备的宏定义,例如定义宏 ``BSP_USING_I2S0`` 方可使用 I2S0 设备。注册完成以后才可以使用其他接口，如果没有定义宏，则无法使用 I2S 设备。

.. code-block:: C

    int i2s_register(enum i2s_index_type index, const char *name);

- index 要注册的设备索引
- name 为注册的设备命名

``index`` 用来选择 I2S 设备配置，一个 index 对应一个 I2S 设备配置，比如 ``I2S0_INDEX`` 对应 ``I2S0_CONFIG`` 配置。index 有如下可选类型

.. code-block:: C

    enum i2s_index_type {
    #ifdef BSP_USING_I2S0
        I2S0_INDEX,
    #endif
        I2S_MAX_INDEX
    };

**device_open**
^^^^^^^^^^^^^^^^

``device_open`` 用于开启一个 I2S 设备，实际调用 ``i2s_open``。

.. code-block:: C

    int device_open(struct device *dev, uint16_t oflag);


- **dev** 设备句柄
- **oflag** 设备的打开方式
- **return** 错误码，0 表示打开成功，其他表示错误

``oflag`` 提供以下类型

.. code-block:: C

    #define DEVICE_OFLAG_STREAM_TX  0x001 /* 设备以轮训发送模式打开 */
    #define DEVICE_OFLAG_STREAM_RX  0x002 /* 设备以轮训接收模式打开 */
    #define DEVICE_OFLAG_DMA_TX     0x010 /* 设备以 DMA 发送模式打开 */
    #define DEVICE_OFLAG_DMA_RX     0x020 /* 设备以 DMA 接收模式打开 */

**device_close**
^^^^^^^^^^^^^^^^

``device_close`` 用于关闭一个 I2S 设备，实际调用 ``i2s_close``。

.. code-block:: C

    int device_close(struct device *dev);

- **dev** 设备句柄
- **return** 错误码，0 表示关闭成功，其他表示错误

**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` 用于对 I2S 设备的进行控制和参数的修改，实际调用 ``i2s_control``。

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);

- **dev** 设备句柄
- **cmd** 设备控制命令
- **args** 控制参数
- **return** 不同的控制命令返回的意义不同。

I2S 设备除了标准的控制命令，还具有自己特殊的控制命令。

.. code-block:: C

    #define I2S_GET_TX_FIFO_CMD           0x10
    #define I2S_GET_RX_FIFO_CMD           0x11

``args`` 根据不同的 ``cmd`` 传入不同，具体如下：

.. list-table:: table1
    :widths: 15 10 30
    :header-rows: 1

    * - cmd
      - args
      - description
    * - DEVICE_CTRL_I2S_ATTACH_TX_DMA
      - struct device*
      - 链接发送 dma 设备
    * - DEVICE_CTRL_I2S_ATTACH_RX_DMA
      - struct device*
      - 链接接收 dma 设备
    * - DEVICE_CTRL_GET_CONFIG
      - I2S_GET_TX_FIFO_CMD
      - 获取 I2S 发送 fifo 未发数据量
    * - DEVICE_CTRL_GET_CONFIG
      - I2S_GET_RX_FIFO_CMD
      - 获取 I2S 接受 fifo 已收数据量

**device_write**
^^^^^^^^^^^^^^^^

``device_write`` 用于 I2S 设备数据的发送，考虑到效率，目前仅支持 DMA 模式发送。

.. code-block:: C

    int device_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size);

- **dev** 设备句柄
- **pos** 无作用
- **buffer** 要写入的 buffer 缓冲区
- **size** 要写入的长度
- **return** 错误码，0 表示写入成功，其他表示错误

**device_read**
^^^^^^^^^^^^^^^^

``device_read`` 用于 I2S 设备数据的接收，考虑到效率，目前仅支持 DMA 模式接收。

.. code-block:: C

    int device_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size);

- **dev** 设备句柄
- **pos** 无作用
- **buffer** 要读入的 buffer 缓冲区
- **size** 要读入的长度
- **return** 错误码，0 表示读入成功，其他表示错误

