I2C 设备
=========================

简介
------------------------

I2C (Inter-Intergrated Circuit) 是一种串行通讯总线，使用多主从架构，用来连接低速外围装置。每个器件都有一个唯
一的地址识别，并且都可以作为一个发送器或接收器。每个连接到总线的器件都可以通过唯一的地址和一直存在的主、
从机关系用软件设置地址，主机可以作为主机发送器或主机接收器。如果有两个或多个主机同时初始化，数据传输可
以通过冲突检测和仲裁防止数据被破坏。博流系列 MCU 中 I2C 设备具有以下特性：

- 灵活配置的从机地址 ``slaveAddr`` 、寄存器地址 ``subAddr``
- 可以灵活调整的时钟频率
- 支持轮询、中断、DMA 传输


I2C 设备结构体定义
------------------------

.. code-block:: C

    typedef struct i2c_device
    {
        struct device parent;
        uint8_t id;
        uint8_t mode;
        uint32_t phase;
    } i2c_device_t;

- **parent**    继承父类属性
- **ch**        i2c id，0 表示 i2c0,1 表示 i2c1
- **mode**      i2c 传输模式，0 为使用硬件 i2c，1 为使用软件 i2c，当前软件 i2c 暂时无效
- **phase**     用来计算 i2c 实际时序时钟 ，公式：i2c_clk = i2c_source_clk/(4*(phase+1))
- 其他待补充

I2C 设备参数配置表
------------------------

每一个 I2C 设备都有一个参数配置宏,宏定义位于 ``bsp/board/xxx`` 目录下 ``peripheral_config.h`` 文件,变量定义位于 ``hal_i2c.c`` 中，因此无需用户自己定义变量。当用户打开对应设备的宏，该设备的配置才生效。例如打开宏 ``BSP_USING_I2C0`` ，``I2C0_CONFIG`` 即生效，同时 ``I2C`` 设备就可以进行注册和使用了。

.. code-block:: C

    /*参数配置宏*/
    #if defined(BSP_USING_I2C0)
    #ifndef I2C0_CONFIG
    #define I2C0_CONFIG \
    {   \
    .id = 0, \
    .mode = I2C_HW_MODE,\
    .phase = 15, \
    }
    #endif
    #endif

    /*变量定义*/
    static i2c_device_t i2cx_device[I2C_MAX_INDEX] =
    {
    #ifdef BSP_USING_I2C0
        I2C0_CONFIG,
    #endif
    };

.. note:: 上述配置可以通过 ``I2C_DEV(dev)->xxx`` 进行修改，只能在调用 ``device_open`` 之前使用。


I2C 设备接口
------------------------

I2C 设备标准接口当前仅使用 ``device_open`` , 并提供标准的数据收发接口。

**i2c_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``i2c_register`` 用来注册一个 I2C 设备标准驱动接口，在注册之前需要打开对应 I2C 设备的宏定义。例如定义宏 ``BSP_USING_I2C0`` 方可使用 ``I2C0`` 设备,注册完成以后才可以使用其他接口，如果没有定义宏，则无法使用 ``I2C0`` 设备。

.. code-block:: C

    int i2c_register(enum i2c_index_type index, const char *name);

- **index** 要注册的设备索引
- **name** 为注册的设备命名

``index`` 用来选择 I2C 设备,一个 index 对应一个 I2C 设备配置，比如 ``I2C0_INDEX`` 对应 ``I2C0_CONFIG`` 配置，``index`` 有如下可选类型

.. code-block:: C

    enum i2c_index_type
    {
    #ifdef BSP_USING_I2C0
        I2C0_INDEX,
    #endif
        I2C_MAX_INDEX
    };

**device_open**
^^^^^^^^^^^^^^^^

``device_open`` 用于打开一个 I2C 设备，实际调用 ``i2c_open``。

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

**i2c_transfer**
^^^^^^^^^^^^^^^^

``i2c_transfer`` 用于设备的数据传输，形参中 ``msgs`` 中的成员 ``flags`` 指示传输的方向是写还是读，并且指定寄存器地址长度是 0、1、2。

.. code-block:: C

    int i2c_transfer(struct device *dev, i2c_msg_t msgs[], uint32_t num);

- **dev** 设备句柄
- **msgs** 需要传输的消息
- **num** 消息个数
- **return** 错误码，0 表示打开成功，其他表示错误

``i2c_msg_t`` 结构体定义如下：

.. code-block:: C

    typedef struct i2c_msg
    {
        uint8_t slaveaddr;
        uint32_t subaddr;
        uint16_t flags;
        uint16_t len;
        uint8_t *buf;
    } i2c_msg_t;

- **slaveaddr** i2c 从设备7位从机地址
- **subaddr** i2c 从设备寄存器地址
- **flags** 读写模式以及寄存器地址长度
- **len** 传输数据长度
- **buf** 数据缓冲区

其中 ``flags`` 有如下定义：

.. code-block:: C

    /*读写模式*/
    #define I2C_WR 0x0000
    #define I2C_RD 0x0001

    /*寄存器地址长度*/
    #define SUB_ADDR_0BYTE 0x0010
    #define SUB_ADDR_1BYTE 0x0020
    #define SUB_ADDR_2BYTE 0x0040