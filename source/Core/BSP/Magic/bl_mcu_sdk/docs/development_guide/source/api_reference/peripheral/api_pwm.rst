PWM 设备
=========================

简介
------------------------

脉冲宽度调制（Pulse width modulation，简称 PWM）是一种用数字方式实现模拟电压控制的技术。它是通过对一系列脉冲的宽度进行调制，等效出所需要的波形（包含形状以及幅值），对模拟信号电平进行数字编码，也就是说通过调节占空比的变化来调节信号、能量等的变化，占空比就是指在一个周期内，信号处于高电平的时间占据整个信号周期的百分比，例如方波的占空比就是50%。博流系列 MCU 中 DMA 设备具有以下特性：

- 支持5通道 PWM 信号生成
- 三种时钟源可选择（总线时钟 <bclk>、晶振时钟 <xtal_ck>、慢速时钟 <32k>），搭配 16-bit 时钟分频器
- 双门限值域设定，增加脉冲弹性

PWM 设备结构体定义
------------------------

.. code-block:: C

    typedef struct pwm_device {
        struct device parent;
        uint8_t ch;
        uint8_t polarity_invert_mode;
        uint16_t period;
        uint16_t threshold_low;
        uint16_t threshold_high;
        uint16_t it_pulse_count;

    } pwm_device_t;

- **parent**    继承父类属性
- **ch**        通道号，使能PWM通道0则ch为0，使能PWM通道0则ch为1，以此类推
- **polarity_invert_mode** 极性翻转使能
- **period**    PWM 周期值
- **threshold_low** PWM 低门限阈值,不能大于period
- **threshold_high** PWM 高门限阈值,不能大于period
- **it_pulse_count** 触发中断条件的周期计数值

.. note:: PWM 实际频率 = PWM 时钟源/分频/period ，period 非 PWM 实际周期，

.. note:: PWM 占空比 = (threshold_high-threshold_low)/period * 100%

PWM 设备参数配置表
------------------------

每一个 PWM 设备都有一个参数配置宏,宏定义位于 ``bsp/board/xxx`` 目录下 ``peripheral_config.h`` 文件,变量定义位于 ``hal_pwm.c`` 中，因此无需用户自己定义变量。当用户打开对应设备的宏，该设备的配置才生效。例如打开宏 ``BSP_USING_PWM_CH2`` ，``PWM_CH2_CONFIG`` 即生效，同时 ``PWM`` 设备的通道2就可以进行注册和使用了。

.. code-block:: C

    /*参数配置宏*/
    #if defined(BSP_USING_PWM_CH2)
    #ifndef PWM_CH2_CONFIG
    #define PWM_CH2_CONFIG                   \
        {                                    \
            .ch = 2,                         \
            .polarity_invert_mode = DISABLE, \
            .period = 0,                     \
            .threshold_low = 0,              \
            .threshold_high = 0,             \
            .it_pulse_count = 0,             \
        }
    #endif
    #endif


    /*变量定义*/
    static pwm_device_t pwmx_device[PWM_MAX_INDEX] = {
    #ifdef BSP_USING_PWM_CH0
        PWM_CH0_CONFIG,
    #endif
    #ifdef BSP_USING_PWM_CH1
        PWM_CH1_CONFIG,
    #endif
    #ifdef BSP_USING_PWM_CH2
        PWM_CH2_CONFIG,
    #endif
    #ifdef BSP_USING_PWM_CH3
        PWM_CH3_CONFIG,
    #endif
    #ifdef BSP_USING_PWM_CH4
        PWM_CH4_CONFIG,
    #endif
    };

.. note:: 上述配置可以通过 ``PWM_DEV(dev)->xxx`` 进行修改，只能在调用 ``device_open`` 之前使用。

PWM 设备接口
------------------------

PWM 设备接口全部遵循标准设备驱动管理层提供的接口。并且为了方便用户调用，将某些标准接口使用宏来重定义。

**pwm_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``pwm_register`` 用来注册一个 PWM 设备的一个通道的标准驱动接口，在注册之前需要打开对应 PWM 设备某个通道的宏定义,例如定义 ``BSP_USING_PWM_CH0`` 方可使用 ``PWM`` 通道0 设备。注册完成以后才可以使用其他接口，如果没有定义宏，则无法使用 PWM 设备。

.. code-block:: C

    int pwm_register(enum pwm_index_type index, const char *name);

- **index** 要注册的设备索引
- **name** 为注册的设备命名

``index`` 用来选择 PWM 设备某个通道的配置，一个 index 对应一个 PWM 设备的一个通道配置，比如 ``PWM_CH0_INDEX`` 对应 PWM 通道0 配置，``index`` 有如下可选类型

.. code-block:: C

    enum pwm_index_type
    {
    #ifdef BSP_USING_PWM_CH0
        PWM_CH0_INDEX,
    #endif
    #ifdef BSP_USING_PWM_CH1
        PWM_CH1_INDEX,
    #endif
    #ifdef BSP_USING_PWM_CH2
        PWM_CH2_INDEX,
    #endif
    #ifdef BSP_USING_PWM_CH3
        PWM_CH3_INDEX,
    #endif
    #ifdef BSP_USING_PWM_CH4
        PWM_CH4_INDEX,
    #endif
        PWM_MAX_INDEX
    };

**device_open**
^^^^^^^^^^^^^^^^

``device_open`` 用于打开一个 PWM 设备的一个通道，实际调用 ``pwm_open``。

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

``device_close`` 用于关闭一个 PWM 设备的一个通道，实际调用 ``pwm_close``。

.. code-block:: C

    int device_close(struct device *dev);

- **dev** 设备句柄
- **return** 错误码，0 表示关闭成功，其他表示错误

**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` 用于对 PWM 设备的一个通道进行控制和参数的修改，实际调用 ``pwm_control``。

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);

- **dev** 设备句柄
- **cmd** 设备控制命令
- **args** 控制参数
- **return** 不同的控制命令返回的意义不同。

PWM 设备除了标准的控制命令，还具有自己特殊的控制命令。

.. code-block:: C

    #define DEIVCE_CTRL_PWM_IT_PULSE_COUNT_CONFIG 0x10

``args`` 根据不同的 ``cmd`` 传入不同，具体如下：

.. list-table:: table1
    :widths: 15 10 30
    :header-rows: 1

    * - cmd
      - args
      - description
    * - DEVICE_CTRL_RESUME
      - NULL
      - 开启当前 PWM 通道
    * - DEVICE_CTRL_SUSPEND
      - NULL
      - 关闭当前 PWM 通道
    * - DEVICE_CTRL_PWM_FREQUENCE_CONFIG
      - uint32_t
      - 配置当前 PWM 通道周期值
    * - DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG
      - pwm_dutycycle_config_t
      - 配置当前 PWM 通道占空比
    * - DEVICE_CTRL_PWM_IT_PULSE_COUNT_CONFIG
      - uint32_t
      - 配置触发 PWM 中断周期值

**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` 用于注册一个 PWM 设备的一个通道中断回调函数。

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

    enum pwm_event_type
    {
        PWM_EVENT_COMPLETE,
    };

**pwm_channel_start**
^^^^^^^^^^^^^^^^^^^^^^

``pwm_channel_start`` 用于开启 PWM 通道。实际是调用 ``device_control`` ，其中 ``cmd`` 为 ``DEVICE_CTRL_RESUME``。

.. code-block:: C

    pwm_channel_start(dev)

- **dev** 需要开启的 pwm 通道句柄


**pwm_channel_stop**
^^^^^^^^^^^^^^^^^^^^^^

``pwm_channel_stop`` 用于关闭 PWM 通道。实际是调用 ``device_control`` ，其中 ``cmd`` 为 ``DEVICE_CTRL_SUSPEND``。

.. code-block:: C

    pwm_channel_stop(dev)

- **dev** 需要关闭的 pwm 通道句柄


**pwm_channel_freq_update**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``pwm_channel_freq_update`` 用于更新 PWM 通道的频率。实际是调用 ``device_control`` ，其中 ``cmd`` 为 ``DEIVCE_CTRL_PWM_FREQUENCE_CONFIG``。

.. code-block:: C

    pwm_channel_freq_update(dev,count)

- **dev** 需要更新的 pwm 通道句柄
- **count** 周期值，实际频率 = pwm_clk/pwm_div/count

**pwm_channel_dutycycle_update**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``pwm_channel_dutycycle_update`` 用于更新 PWM 通道的频率。实际是调用 ``device_control`` ，其中 ``cmd`` 为 ``DEIVCE_CTRL_PWM_DUTYCYCLE_CONFIG``。

.. code-block:: C

    pwm_channel_dutycycle_update(dev,cfg)

- **dev** 需要更新周期计数值的 pwm 通道句柄
- **cfg** pwm_dutycycle_config_t 句柄


**pwm_it_pulse_count_update**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``pwm_it_pulse_count_update`` 用于更新 PWM 通道的计数值，需要先使能 PWM 中断才起作用，当 pwm 计数达到设置的周期计数值则会产生中断。实际是调用 ``device_control`` ，其中 ``cmd`` 为 ``DEIVCE_CTRL_PWM_IT_PULSE_COUNT_CONFIG``。

.. code-block:: C

    pwm_it_pulse_count_update(dev,count)

- **dev** 需要更新周期计数值的 pwm 通道句柄
- **count** 周期计数值

