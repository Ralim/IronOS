ADC 设备
=========================

简介
------------------------
ADC (Analog-to-digital Converter) 是用于将模拟形式的连续信号转换为数字形式的离散信号的一类设备,他可以将外围电路传感器产生的模拟信号转化为机器可识别的数字信号。
博流系列 MCU 中的 ADC 设备具有以下特性

- 可以选择 12/14/16 bits 转换结果输出
- ADC最大工作时钟为 2MHZ
- 支持2.0V ,3.2V 可选内部参考电压
- 支持DMA将转换结果搬运到内存
- 支持单次单通道转换、连续单通道转换、单次多通道转换和连续多通道转换模式四种模式
- 支持单端与差分两种输入模式
- 12路外部模拟通道
- 2路 DAC 内部通道
- 1路 VBAT /2通道


ADC 设备结构体定义
------------------------
.. code-block:: C

    typedef struct adc_device {
        struct device parent;
        adc_clk_div_t clk_div;
        adc_vref_t vref;
        bool continuous_conv_mode;
        bool differential_mode;
        adc_data_width_t data_width;
        adc_fifo_threshold_t fifo_threshold;
        adc_pga_gain_t gain;
    } adc_device_t;

- **parent**                继承父类属性
- **clk_div**               ADC 模块时钟内部分频
- **vref**                  参考电压选择 2.0/3.2V 可选
- **continuous_conv_mode**  是否选择为连续模式，若为连续模式，则一次 adc_start 操作， ADC 连续工作，直到 adc_stop 。否则，每 adc_start 一次，adc 仅转换一次结果。
- **differential_mode**     选择是否为差分模式，若为差分模式，则可以测量负电压。
- **data_width**            测量宽度选择，ADC 实际精度为12 bits ，但是通过 OSR 多次取平均可以达到 14bits / 16bits 的精度，注意，当选择更高的数据宽度后，频率会因为取平均而下降。详情可参考枚举信息。
- **fifo_threshold**        此参数影响 DMA 搬运阈值以及 ADC FIFO 中断阈值
- **gain**                  ADC 对输入信号的增益选择
- 其他待补充

ADC 设备参数配置表
------------------------

每一个 ADC 设备都有一个参数配置宏,宏定义位于 ``bsp/board/xxx`` 目录下 ``peripheral_config.h`` 文件,变量定义位于 ``hal_adc.c`` 中，因此无需用户自己定义变量。当用户打开对应设备的宏，该设备的配置才生效。例如打开宏 ``BSP_USING_ADC0`` 即生效，同时 ``ADC`` 设备就可以进行注册和使用了。

.. code-block:: C

    /*参数配置宏*/
    #if defined(BSP_USING_ADC0)
    #ifndef ADC0_CONFIG
    #define ADC0_CONFIG                                        \
        {                                                      \
            .clk_div = ADC_CLOCK_DIV_32,                       \
            .vref = ADC_VREF_3P2V,                             \
            .continuous_conv_mode = DISABLE,                   \
            .differential_mode = DISABLE,                      \
            .data_width = ADC_DATA_WIDTH_16B_WITH_256_AVERAGE, \
            .fifo_threshold = ADC_FIFO_THRESHOLD_1BYTE,        \
            .gain = ADC_GAIN_1                                 \
        }
    #endif
    #endif

    /*变量定义*/
    static adc_device_t adcx_device[ADC_MAX_INDEX] = {
    #ifdef BSP_USING_ADC0
        ADC0_CONFIG,
    #endif
    };

.. note:: 上述配置可以通过 ``ADC_DEV(dev)->xxx`` 进行修改，只能在调用 ``device_open`` 之前使用。

ADC 设备接口
------------------------

ADC 设备接口全部遵循标准设备驱动管理层提供的接口。

**adc_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``adc_register`` 用来注册 ADC 设备标准驱动接口，在注册之前需要打开对应 ADC 设备的宏定义。例如定义宏 ``BSP_USING_ADC0`` 方可使用 ``ADC0`` 设备,注册完成以后才可以使用其他接口，如果没有定义宏，则无法使用 ``ADC0`` 设备。

.. code-block:: C

    int adc_register(enum adc_index_type index, const char *name);

- **index** 要注册的设备索引
- **flag** 默认可读可写属性

``index`` 用来选择 ADC 设备配置，一个 index 对应一个 ADC 设备配置，比如 ``ADC0_INDEX`` 对应 ``ADC0_CONFIG`` 配置，``index`` 有如下可选类型

.. code-block:: C

    enum adc_index_type
    {
    #ifdef BSP_USING_ADC0
        ADC0_INDEX,
    #endif
        ADC_MAX_INDEX
    };

**device_open**
^^^^^^^^^^^^^^^^

``device_open`` 用于打开一个 ADC 设备，实际调用 ``adc_open``。

.. code-block:: C

    int device_open(struct device *dev, uint16_t oflag);

- **dev** 设备句柄
- **oflag** 设备的打开方式
- **return**    错误码，0表示成功，其他表示失败

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

``device_close`` 用于关闭一个 ADC 设备，实际调用 ``adc_close``。

.. code-block:: C

    int device_close(struct device *dev);

- **dev** 设备句柄
- **return**    错误码，0表示成功，其他表示失败

**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` 用于对 ADC 设备的进行控制和参数的修改，实际调用 ``adc_control``。

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);

- **dev** 设备句柄
- **cmd** 设备控制命令
- **args** 控制参数
- **return** 不同的控制命令返回的意义不同。

ADC 设备除了标准的控制命令，还具有自己特殊的控制命令。

.. code-block:: C

    #define DEVICE_CTRL_ADC_CHANNEL_START    0x10
    #define DEVICE_CTRL_ADC_CHANNEL_STOP     0x11
    #define DEVICE_CTRL_ADC_CHANNEL_CONFIG   0x12
    #define DEVICE_CTRL_ADC_VBAT_ON          0x13
    #define DEVICE_CTRL_ADC_VBAT_OFF         0x14
    #define DEVICE_CTRL_ADC_TSEN_ON          0x15
    #define DEVICE_CTRL_ADC_TSEN_OFF         0x16
    #define DEVICE_CTRL_ADC_DATA_PARSE       0x17

``args`` 根据不同的 ``cmd`` 传入不同，具体如下：

.. list-table:: table1
    :widths: 15 10 30
    :header-rows: 1

    * - cmd
      - args
      - description
    * - DEVICE_CTRL_SET_INT
      - adc_it_type
      - 开启 ADC 设备中断
    * - DEVICE_CTRL_CLR_INT
      - adc_it_type
      - 关闭 ADC 设备中断
    * - DEVICE_CTRL_CONFIG
      - ADC_param_cfg_t
      - 修改 ADC 配置
    * - DEVICE_CTRL_ADC_CHANNEL_CONFIG
      - adc_channel_cfg_t
      - 修改 ADC 通道配置
    * - DEVICE_CTRL_ATTACH_RX_DMA
      - struct device*
      - 链接接收 DMA 设备
    * - DEVICE_CTRL_ADC_CHANNEL_START
      - NULL
      - 开始/继续 ADC 转换
    * - DEVICE_CTRL_ADC_CHANNEL_STOP
      - NULL
      - 停止 ADC 转换
    * - DEVICE_CTRL_ADC_VBAT_ON
      - NULL
      - 打开内部 VDD 测量电路
    * - DEVICE_CTRL_ADC_VBAT_OFF
      - NULL
      - 关闭内部 VDD 测量电路
    * - DEVICE_CTRL_ADC_TSEN_ON
      - NULL
      - 打开内部温度测量电路(需硬件支持)
    * - DEVICE_CTRL_ADC_TSEN_OFF
      - NULL
      - 关闭内部温度测量电路(需硬件支持)
    * - DEVICE_CTRL_ADC_DATA_PARSE
      - adc_data_parse_t
      - 对原始数据进行解析，解析成电压形式

**device_read**
^^^^^^^^^^^^^^^^

``device_read`` 用于 ADC 设备数据的接收，接收方式根据打开方式可以是轮询、中断、dma。实际调用 ``adc_read``。

.. code-block:: C

    int device_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size);

- **dev** 设备句柄
- **pos** 无作用
- **buffer** 要读入的 buffer 缓冲区
- **size** 要读入的长度
- **return** 错误码，0 表示读入成功，其他表示错误

**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` 用于注册一个ADC阈值中断回调函数。

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

    enum ADC_event_type
    {
        ADC_EVENT_FIFO_READY,
        ADC_EVENT_OVERRUN,
        ADC_EVENT_UNDERRUN,
    };