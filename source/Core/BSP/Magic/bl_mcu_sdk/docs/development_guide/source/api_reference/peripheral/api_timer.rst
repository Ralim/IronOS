TIMER 设备
=========================

简介
------------------------

TIMER 通用定时器，博流系列 MCU 中 TIMER 设备具有以下特性：

- 多种时钟来源选择
- 8-bit 时钟分频器，分频系数为 1-256
- 两组 32-bit 定时器
- 每个定时器包含三组报警值设定，可独立设定每组报警值溢出时报警
- 支持 FreeRun 模式和 PreLoad 模式
- 16-bit 看门狗定时器
- 支持写入密码保护，防止误设定造成系统异常
- 支持中断或复位两种看门狗溢出方式

TIMER 设备结构体定义
------------------------

.. code-block:: C

    typedef struct timer_device {
        struct device parent;
        uint8_t id;
        enum timer_cnt_mode_type cnt_mode;
        enum timer_preload_trigger_type trigger;
        uint32_t reload;
        uint32_t timeout1;
        uint32_t timeout2;
        uint32_t timeout3;
    } timer_device_t;

- **parent**        继承父类属性
- **id**            定时器 id ，使能定时器 0 则 id 为 0 ，使能定时器 1 则 id 为 1 ，以此类推
- **cnt_mode**      计数模式：FreeRun 和 PreLoad
- **trigger**   比较器的触发源
- **reload**    重装载值,只有在 PreLoad 模式下有效
- **timeout1**  COMP0 超时时间，单位 us
- **timeout2**  COMP1 超时时间，单位 us
- **timeout3**  COMP2 超时时间，单位 us

``ch`` 提供以下类型

.. code-block:: C

    enum timer_index_type {
        TIMER0_INDEX,
        TIMER1_INDEX,
        TIMER_MAX_INDEX
    };

``cnt_mode`` 提供以下类型

.. code-block:: C

    enum timer_cnt_mode_type {
        TIMER_CNT_PRELOAD,
        TIMER_CNT_FREERUN,
    };

``trigger`` 提供以下类型

.. code-block:: C

    enum timer_preload_trigger_type {
        TIMER_PRELOAD_TRIGGER_NONE,
        TIMER_PRELOAD_TRIGGER_COMP0,
        TIMER_PRELOAD_TRIGGER_COMP1,
        TIMER_PRELOAD_TRIGGER_COMP2,
    };

TIMER 设备参数配置表
------------------------

每一个 TIMER 设备都有一个参数配置宏,宏定义位于 ``bsp/board/xxx`` 目录下 ``peripheral_config.h`` 文件,变量定义位于 ``hal_timer.c`` 中，因此无需用户自己定义变量。当用户打开对应设备的宏，该设备的配置才生效。例如打开宏 ``BSP_USING_TIMER_CH0`` ，``TIMER_CH0_CONFIG`` 即生效，同时 ``TIMER_CH0_INDEX`` 设备就可以进行注册和使用了。

.. code-block:: C

    /*参数配置宏*/
    #if defined(BSP_USING_TIMER0)
    #ifndef TIMER0_CONFIG
    #define TIMER0_CONFIG                           \
        {                                           \
            .id = 0,                                \
            .cnt_mode = TIMER_CNT_PRELOAD,          \
            .trigger = TIMER_PRELOAD_TRIGGER_COMP2, \
            .reload = 0,                            \
            .timeout1 = 1000000,                    \
            .timeout2 = 2000000,                    \
            .timeout3 = 3000000,                    \
        }
    #endif
    #endif

    #if defined(BSP_USING_TIMER1)
    #ifndef TIMER1_CONFIG
    #define TIMER1_CONFIG                           \
        {                                           \
            .id = 1,                                \
            .cnt_mode = TIMER_CNT_PRELOAD,          \
            .trigger = TIMER_PRELOAD_TRIGGER_COMP0, \
            .reload = 0,                            \
            .timeout1 = 1000000,                    \
            .timeout2 = 2000000,                    \
            .timeout3 = 3000000,                    \
        }
    #endif
    #endif

    /*变量定义*/
    static timer_device_t timerx_device[TIMER_MAX_INDEX] = {
    #ifdef BSP_USING_TIMER0
        TIMER0_CONFIG,
    #endif
    #ifdef BSP_USING_TIMER1
        TIMER1_CONFIG,
    #endif
    };

.. note:: 上述配置可以通过 ``TIMER_DEV(dev)->xxx`` 进行修改，只能在调用 ``device_open`` 之前使用。

TIMER 设备接口
------------------------

TIMER 设备接口全部遵循标准设备驱动管理层提供的接口。

**timer_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``timer_register`` 用来注册 TIMER 设备标准驱动接口，在注册之前需要打开对应 TIMER 设备的宏定义。例如定义宏 ``BSP_USING_TIMER0`` 方可使用 ``TIMER0_INDEX`` 设备,注册完成以后才可以使用其他接口，如果没有定义宏，则无法使用 ``TIMER0_INDEX`` 设备。

.. code-block:: C

    int timer_register(enum timer_index_type index, const char *name);

- **index** 要注册的设备索引
- **name** 为注册的设备命名

``index`` 用来选择 TIMER 设备配置，一个 index 对应一个 TIMER 设备配置，比如 ``TIMER0_INDEX`` 对应 ``TIMER0_CONFIG`` 配置，``index`` 有如下可选类型

.. code-block:: C

    enum timer_index_type {
    #ifdef BSP_USING_TIMER0
        TIMER0_INDEX,
    #endif
    #ifdef BSP_USING_TIMER1
        TIMER1_INDEX,
    #endif
        TIMER_MAX_INDEX
    };

**device_open**
^^^^^^^^^^^^^^^^

``device_open`` 用于打开一个 TIMER 设备，实际调用 ``timer_open``。

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

``device_close`` 用于关闭一个 TIMER 设备，实际调用 ``timer_close``。

.. code-block:: C

    int device_close(struct device *dev);

- **dev** 设备句柄
- **return** 错误码，0 表示关闭成功，其他表示错误

**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` 用于对 TIMER 设备的进行控制和参数的修改，实际调用 ``timer_control``。

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);

- **dev** 设备句柄
- **cmd** 设备控制命令
- **args** 控制参数
- **return** 不同的控制命令返回的意义不同。

``args`` 根据不同的 ``cmd`` 传入不同，具体如下：

.. list-table:: table1
    :widths: 15 10 30
    :header-rows: 1

    * - cmd
      - args
      - description
    * - DEVICE_CTRL_SET_INT
      - timer_it_type
      - 开启 TIMER 中断
    * - DEVICE_CTRL_CLR_INT
      - timer_it_type
      - 关闭 TIMER 中断
    * - DEVICE_CTRL_GET_INT
      - NULL
      - 获取当前中断状态
    * - DEVICE_CTRL_RESUME
      - NULL
      - 开启 TIMER 设备
    * - DEVICE_CTRL_SUSPEND
      - NULL
      - 关闭 TIMER 设备
    * - DEVICE_CTRL_GET_CONFIG
      - NULL
      - 获取 TIMER 当前的计数值

**device_write**
^^^^^^^^^^^^^^^^

``device_write`` 用于配置 TIMER 设备超时时间，实际调用 ``timer_write``。

.. code-block:: C

    int device_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size);

- **dev** 设备句柄
- **pos** 无作用
- **buffer** timer_timeout_cfg_t 句柄
- **size** timer_timeout_cfg_t 个数
- **return** 错误码，0 表示写入成功，其他表示错误

**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` 用于注册一个 TIMER 设备中断回调函数。

.. code-block:: C

    int device_set_callback(struct device *dev, void (*callback)(struct device *dev, void *args, uint32_t size, uint32_t event));

- **dev** 设备句柄
- **callback** 要注册的中断回调函数

    - **dev** 设备句柄
    - **args** 无
    - **size** 无
    - **event** 中断事件类型

``event`` 类型如下

.. code-block:: C

    enum timer_event_type {
        TIMER_EVENT_COMP0,
        TIMER_EVENT_COMP1,
        TIMER_EVENT_COMP2,
        TIMER_EVENT_UNKNOWN
    };