QDEC 设备
=========================

简介
------------------------

QDEC（quadrature decoder）正交解码器，用于将双路旋转编码器产生的两组相位相差 90 度的脉冲解码为对应转速和旋转方向。博流系列 MCU 中 QDEC 设备具有以下特性：

- 拥有 3 组 QDEC 设备可使用
- 多种时钟来源选择，QDEC 的时钟源可以设置为 32KHz 或者 32MHz，当使用 32KHz 的时钟源时，可以从睡眠模式中唤醒
- 5-bit 时钟分频器，分频系数为 1-32
- 16-bit 脉冲计数器，计数范围 -32768~32767 pulse/sample
- 12 种可配置的 sample 周期（32us~131ms per sample at 1MHz）
- 16-bit 可设置的 report 周期（0~65535 sample/report）
- 内置一个可随采样进行闪烁的 LED 功能（LED on/off 0~511 us/sample）
- 支持 4 路可配置中断源（sample 中断、report 中断、error 中断、overflow 中断）
- 可配置为 PDS 的唤醒源（clock source 需要配置为 32KHz）

QDEC 设备结构体定义
------------------------

.. code-block:: C

    typedef struct qdec_device {
        struct device parent;
        uint8_t id;
        uint8_t acc_mode;
        uint8_t sample_mode;
        uint8_t sample_period;
        uint8_t report_mode;
        uint32_t report_period;
        uint8_t led_en;
        uint8_t led_swap;
        uint16_t led_period;
        uint8_t deglitch_en;
        uint8_t deglitch_strength;
    } qdec_device_t;

- parent             继承父类属性
- id                 正交解码器 id ，使用正交解码器 0 则 id 为 0 ，使用正交解码器 1 则 id 为 1 ，以此类推
- acc_mode           QDEC 计数模式：连续计数和溢出即停止 两种模式
- sample_mode        QDEC 采样模式：连续采样和完成采样即停止 两种模式
- sample_period      QDEC 采样周期选择
- report_mode        QDEC report 模式
- report_period      report 周期设置
- led_en             随动 LED 功能使能
- led_swap           led 引脚交换
- led_period         led 输出频率
- deglitch_en        消抖功能使能
- deglitch_strength  消抖长度

``id`` 提供以下类型

.. code-block:: C

    enum qdec_index_type {
        QDEC0_INDEX,
        QDEC1_INDEX,
        QDEC2_INDEX,
        QDEC_MAX_INDEX,
    };

``acc_mode`` 提供以下类型

.. code-block:: C

    enum acc_mode_type {
        QDEC_ACC_STOP_SAMPLE_IF_OVERFLOW,
        QDEC_ACC_CONTINUE_ACCUMULATE,
    };

``sample_mode`` 提供以下类型

.. code-block:: C

    enum sample_mode_type {
        QDEC_SAMPLE_SINGLE_MOD,
        QDEC_SAMPLE_CONTINUE_MOD,
    };

``report_mode`` 提供以下类型

.. code-block:: C

    enum report_mode_type {
        QDEC_REPORT_SAMPLE_CHANGE_MOD,
        QDEC_REPORT_TIME_MOD,
    };


QDEC 设备参数配置表
------------------------

每一个 QDEC 设备都有一个参数配置宏，宏定义位于 ``bsp/board/xxx`` 目录下 ``peripheral_config.h`` 文件，变量定义位于 ``hal_qdec.c`` 中，因此无需用户自己定义变量。当用户打开对应设备的宏，该设备的配置才生效。例如打开宏 ``BSP_USING_QDEC0`` ，``QDEC0_CONFIG`` 即生效，同时 ``QDEC0_INDEX`` 设备就可以进行注册和使用了。

.. code-block:: C

    /*参数配置宏*/
    #if defined(BSP_USING_QDEC0)
    #ifndef QDEC0_CONFIG
    #define QDEC0_CONFIG                               \
        {                                              \
            .id = 0,                                   \
            .acc_mode = QDEC_ACC_CONTINUE_ACCUMULATE,  \
            .sample_mode = QDEC_SAMPLE_SINGLE_MOD,     \
            .sample_period = QDEC_SAMPLE_PERIOD_256US, \
            .report_mode = QDEC_REPORT_TIME_MOD,       \
            .report_period = 2000,                     \
            .led_en = ENABLE,                          \
            .led_swap = DISABLE,                       \
            .led_period = 7,                           \
            .deglitch_en = DISABLE,                    \
            .deglitch_strength = 0x0,                  \
        }
    #endif
    #endif

    #if defined(BSP_USING_QDEC1)
    #ifndef QDEC1_CONFIG
    #define QDEC1_CONFIG                               \
        {                                              \
            .id = 1,                                   \
            .acc_mode = QDEC_ACC_CONTINUE_ACCUMULATE,  \
            .sample_mode = QDEC_SAMPLE_SINGLE_MOD,     \
            .sample_period = QDEC_SAMPLE_PERIOD_256US, \
            .report_mode = QDEC_REPORT_TIME_MOD,       \
            .report_period = 2000,                     \
            .led_en = ENABLE,                          \
            .led_swap = DISABLE,                       \
            .led_period = 7,                           \
            .deglitch_en = DISABLE,                    \
            .deglitch_strength = 0x0,                  \
        }
    #endif
    #endif

    #if defined(BSP_USING_QDEC2)
    #ifndef QDEC2_CONFIG
    #define QDEC2_CONFIG                               \
        {                                              \
            .id = 2,                                   \
            .acc_mode = QDEC_ACC_CONTINUE_ACCUMULATE,  \
            .sample_mode = QDEC_SAMPLE_SINGLE_MOD,     \
            .sample_period = QDEC_SAMPLE_PERIOD_256US, \
            .report_mode = QDEC_REPORT_TIME_MOD,       \
            .report_period = 2000,                     \
            .led_en = ENABLE,                          \
            .led_swap = DISABLE,                       \
            .led_period = 7,                           \
            .deglitch_en = DISABLE,                    \
            .deglitch_strength = 0x0,                  \
        }
    #endif
    #endif

    /*变量定义*/
    static qdec_device_t qdecx_device[QDEC_MAX_INDEX] = {
    #ifdef BSP_USING_QDEC0
        QDEC0_CONFIG,
    #endif
    #ifdef BSP_USING_QDEC1
        QDEC1_CONFIG,
    #endif
    #ifdef BSP_USING_QDEC2
        QDEC2_CONFIG,
    #endif
    };

.. note:: 上述配置可以通过 ``QDEC_DEV(dev)->xxx`` 进行修改，只能在调用 ``device_open`` 之前使用。

QDEC 设备接口
------------------------

QDEC 设备接口全部遵循标准设备驱动管理层提供的接口。

**qdec_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``qdec_register`` 用来注册 QDEC 设备标准驱动接口，在注册之前需要打开对应 QDEC 设备的宏定义。例如定义宏 ``BSP_USING_QDEC0`` 方可使用 ``QDEC0_INDEX`` 设备，注册完成以后才可以使用其他接口，如果没有定义宏，则无法使用 ``QDEC0_INDEX`` 设备。

.. code-block:: C

    int qdec_register(enum qdec_index_type index, const char *name);

- index 要注册的设备索引
- name 为注册的设备命名

``index`` 用来选择 QDEC 设备配置，一个 index 对应一个 QDEC 设备配置，比如 ``QDEC0_INDEX`` 对应 ``QDEC0_CONFIG`` 配置，``index`` 有如下可选类型

.. code-block:: C

    enum qdec_index_type {
    #ifdef BSP_USING_QDEC0
        QDEC0_INDEX,
    #endif
    #ifdef BSP_USING_QDEC1
        QDEC1_INDEX,
    #endif
    #ifdef BSP_USING_QDEC2
        QDEC2_INDEX,
    #endif
        QDEC_MAX_INDEX,
    };

**device_open**
^^^^^^^^^^^^^^^^

``device_open`` 用于 QDEC 设备的打开，``oflag`` 表示以何种方式打开。实际调用 ``qdec_open``。

.. code-block:: C

    int device_open(struct device *dev, uint16_t oflag);

- dev 设备句柄
- oflag 设备的打开方式
- return 错误码，0 表示打开成功，其他表示错误

``oflag`` 可以写入以下参数：

.. code-block:: C

    #define DEVICE_OFLAG_STREAM_TX  0x001 /* 设备以轮训发送模式打开 */
    #define DEVICE_OFLAG_STREAM_RX  0x002 /* 设备以轮训接收模式打开 */
    #define DEVICE_OFLAG_INT_TX     0x004 /* 设备以中断发送模式打开 */
    #define DEVICE_OFLAG_INT_RX     0x008 /* 设备以中断接收模式打开 */
    #define DEVICE_OFLAG_DMA_TX     0x010 /* 设备以 DMA 发送模式打开 */
    #define DEVICE_OFLAG_DMA_RX     0x020 /* 设备以 DMA 接收模式打开 */

**device_close**
^^^^^^^^^^^^^^^^

``device_close`` 用于设备的关闭。实际调用 ``qdec_close``。

.. code-block:: C

    int device_close(struct device *dev);

- dev 设备句柄
- return 错误码，0 表示关闭成功，其他表示错误

**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` 用于根据命令对 QDEC 设备进行控制和参数的修改。实际调用 ``qdec_control``。

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);

- dev 设备句柄
- cmd 设备控制命令
- args 控制参数
- return 不同的控制命令返回的意义不同。

``args`` 根据不同的 ``cmd`` 传入不同，具体如下：

.. list-table:: table1
    :widths: 15 10 30
    :header-rows: 1

    * - cmd
      - args
      - description
    * - DEVICE_CTRL_SET_INT
      - NULL
      - 开启 QDEC 中断
    * - DEVICE_CTRL_CLR_INT
      - NULL
      - 关闭 QDEC 中断
    * - DEVICE_CTRL_GET_INT
      - NULL
      - 获取当前中断状态
    * - DEVICE_CTRL_RESUME
      - NULL
      - 开启 QDEC
    * - DEVICE_CTRL_SUSPEND
      - NULL
      - 关闭 QDEC
    * - DEVICE_CTRL_GET_SAMPLE_VAL
      - NULL
      - 获取 QDEC 当前的计数值
    * - DEVICE_CTRL_GET_SAMPLE_DIR
      - NULL
      - 获取 QDEC 当前的计数方向
    * - DEVICE_CTRL_GET_ERROR_CNT
      - NULL
      - 获取 QDEC 当前的错误计数次数


**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` 用于注册一个 QDEC 中断回调函数。

.. code-block:: C

    int device_set_callback(struct device *dev, void (*callback)(struct device *dev, void *args, uint32_t size, uint32_t event));

- dev 设备句柄
- callback 要注册的中断回调函数

    - dev 设备句柄
    - args 接收发送缓冲区，数据类型为 uint8_t*
    - size 传输长度
    - event 中断事件类型

QDEC 设备 ``event`` 类型如下

.. code-block:: C

    enum qdec_event_type {
        QDEC_REPORT_EVENT,   /*!< report interrupt */
        QDEC_SAMPLE_EVENT,   /*!< sample interrupt */
        QDEC_ERROR_EVENT,    /*!< error interrupt */
        QDEC_OVERFLOW_EVENT, /*!< ACC1 and ACC2 overflow interrupt */
        QDEC_ALL_EVENT,      /*!< interrupt max num */
    };