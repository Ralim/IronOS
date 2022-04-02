API 概述
=========================

简介
------

**bl_mcu_sdk** 代码层次结构主要分为以下几层

- 应用层：由用户自己编写的代码
- 组件层：开源的一些组件，接口则是调用 HAL 层的接口，使用到无线功能则是调用 Wireless 层的接口
- 为适配不同 MCU 提供的 HAL 层和无线层，其中 HAL 层又分为两层
    - 设备驱动管理层：提供一套标准的接口，具体实现由外设驱动适配层实现
    - 外设驱动适配层：实现设备驱动管理层的标准接口，并且拓展自己的独特的接口
- 基于寄存器封装的标准驱动层
- 硬件层，也就是寄存器层

.. figure:: img/sw_arch.png
    :alt:

    code structure

设备驱动管理层实现
---------------------

设备驱动管理层主要针对芯片外设实现，目的是不影响用户应用层代码因为芯片驱动的不同而频繁修改。

设备驱动管理层的实现采用了面向对象的思想，首先我们将外设看成是一个设备或者是文件，秉承 **一切皆文件** 的理念，而文件又都具有标准的调用接口：``open``、``close``、``ctrl``、``write``、``read``、``callback``，不同文件类别不同（比如串口设备、ADC设备、SPI设备），并且打开的方式也不同（比如轮询、中断、DMA），由此，我们可以构建出一个对象的基类（父类）。

**基类**

.. code-block:: C

    struct device
    {
        char name[NAME_MAX];            /*name of device */
        dlist_t list;                   /*list node of device */
        enum device_status_type status; /*status of device */
        enum device_class_type type;    /*type of device */
        uint16_t oflag;                 /*oflag of device */

        int (*open)(struct device *dev, uint16_t oflag);
        int (*close)(struct device *dev);
        int (*control)(struct device *dev, int cmd, void *args);
        int (*write)(struct device *dev, uint32_t pos, const void *buffer, uint32_t size);
        int (*read)(struct device *dev, uint32_t pos, void *buffer, uint32_t size);
        void (*callback)(struct device *dev, void *args, uint32_t size, uint32_t event);
        void *handle;
    };

**基类成员：name**

给设备取名，后面会使用 ``device_find`` 找到这个设备。

**基类成员：type**

``type`` 记录当前设备的类别，可以选择的 ``type`` 类型如下。

.. code-block:: C

    enum device_class_type
    {
        DEVICE_CLASS_NONE = 0,
        DEVICE_CLASS_GPIO,
        DEVICE_CLASS_UART,
        DEVICE_CLASS_SPI,
        DEVICE_CLASS_I2C,
        DEVICE_CLASS_ADC,
        DEVICE_CLASS_DMA,
        DEVICE_CLASS_TIMER,
        DEVICE_CLASS_PWM,
        DEVICE_CLASS_SDIO,
        DEVICE_CLASS_USB,
        DEVICE_CLASS_I2S,
        DEVICE_CLASS_CAMERA,
        DEVICE_CLASS_SEC_HASH,
    } ;

**基类成员：status**

``status`` 用来记录当前设备的状态，当前提供 4 种状态。

.. code-block:: C

    enum device_status_type
    {
        DEVICE_UNREGISTER = 0,
        DEVICE_REGISTERED,
        DEVICE_OPENED,
        DEVICE_CLOSED
    } ;

**基类成员：oflag**

``oflag`` 记录 注册时填入的 flag 信息以及使用 ``device_open`` 时填入的 ``oflag`` 信息。

**基类成员：list**

设备的增加和删除使用双向链表进行存储，节省内存。

**基类成员：标准的函数指针**

为不同的外设提供了标准的函数接口，当外设实现此类接口并赋值给该成员，便能达到重写的功能。

设备驱动管理层标准接口
-----------------------

**device_register**
^^^^^^^^^^^^^^^^^^^^

``device_register`` 用于设备标准驱动的注册，并将设备信息注册到链表当中。

.. code-block:: C

    int device_register(struct device *dev, const char *name);

- dev 设备句柄。
- name 设备名称。
- return 返回错误码，0 表示注册成功，其他表示错误。

**device_unregister**
^^^^^^^^^^^^^^^^^^^^^^^

``device_unregister`` 用于设备的删除，将设备信息从链表中删除。

.. code-block:: C

    int device_unregister(const char *name);

- dev 设备句柄
- name 要删除的设备名称
- return 错误码，0 表示删除，其他表示错误

**device_find**
^^^^^^^^^^^^^^^^

``device_find`` 用于根据 ``name`` 从链表中寻找设备，并返回设备句柄的首地址。

.. code-block:: C

    struct device *device_find(const char *name);

- dev 设备句柄
- name 要查找的设备名称
- return 错误码，不为 0 表示找到的设备句柄，NULL 表示未找到该设备。


**device_open**
^^^^^^^^^^^^^^^^

``device_open`` 用于设备的打开，``oflag`` 表示以何种方式打开，目前提供 6 种打开方式。底层调用 ``dev`` 句柄中的 ``open`` 成员。

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

``device_close`` 用于设备的关闭。底层调用 ``dev`` 句柄中的 ``close`` 成员。

.. code-block:: C

    int device_close(struct device *dev);

- dev 设备句柄
- return 错误码，0 表示关闭成功，其他表示错误

**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` 用于根据命令对设备进行控制和参数的修改。底层调用 ``dev`` 句柄中的 ``control`` 成员。

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);

- dev 设备句柄
- cmd 设备控制命令
- args 控制参数
- return 不同的控制命令返回的意义不同。

``cmd`` 提供了以下标准命令，除此之外，不同外设还具有自己的命令

.. code-block:: C

    #define DEVICE_CTRL_SET_INT             0x01    /* set interrupt */
    #define DEVICE_CTRL_CLR_INT             0x02    /* clear interrupt */
    #define DEVICE_CTRL_GET_INT             0x03    /* get interrupt status*/
    #define DEVICE_CTRL_RESUME              0x04    /* resume device */
    #define DEVICE_CTRL_SUSPEND             0x05    /* suspend device */
    #define DEVICE_CTRL_CONFIG              0x06    /* config device */
    #define DEVICE_CTRL_GET_CONFIG          0x07    /* get device configuration */
    #define DEVICE_CTRL_ATTACH_TX_DMA       0x08
    #define DEVICE_CTRL_ATTACH_RX_DMA       0x09
    #define DEVICE_CTRL_TX_DMA_SUSPEND      0x0a
    #define DEVICE_CTRL_RX_DMA_SUSPEND      0x0b
    #define DEVICE_CTRL_TX_DMA_RESUME       0x0c
    #define DEVICE_CTRL_RX_DMA_RESUME       0x0d
    #define DEVICE_CTRL_RESVD1              0x0E
    #define DEVICE_CTRL_RESVD2              0x0F

**device_write**
^^^^^^^^^^^^^^^^

``device_write`` 用于数据的发送，发送方式根据打开方式可以是轮询、中断、dma。底层调用 ``dev`` 句柄中的 ``write`` 成员。

.. code-block:: C

    int device_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size);

- dev 设备句柄
- pos 不同的设备 pos 的意义不同
- buffer 要写入的 buffer 缓冲区
- size 要写入的长度
- return 错误码，0 表示写入成功，其他表示错误

**device_read**
^^^^^^^^^^^^^^^^

``device_read`` 用于数据的接收，接收方式根据打开方式可以是轮询、中断、dma。底层调用 ``dev`` 句柄中的 ``read`` 成员。

.. code-block:: C

    int device_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size);

- dev 设备句柄
- pos 不同的设备 pos 的意义不同
- buffer 要读入的 buffer 缓冲区
- size 要读入的长度
- return 错误码，0 表示读入成功，其他表示错误

**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` 用于中断回调函数的注册。底层调用 ``dev`` 句柄中的 ``callback`` 成员。

.. code-block:: C

    int device_set_callback(struct device *dev, void (*callback)(struct device *dev, void *args, uint32_t size, uint32_t event));

- dev 设备句柄
- callback 要注册的中断回调函数


    * dev 设备句柄
    * args 不同外设意义不同
    * size 传输长度
    * event 中断事件类型

外设驱动适配层实现
------------------------

**子类继承父类**

不同的外设首成员为 ``struct device`` ，这就相当于父类的继承，从而可以使用子类来访问父类成员，当使用子类修改父类成员时，便拥有了子类自己的功能。实现原理是不同结构体的首地址是该结构体中首个成员的地址。

.. code-block:: C

    typedef struct xxx_device
    {
        struct device parent;

    } xxx_device_t;

**重写标准接口**

每个外设都有一个 ``xxx_register`` 函数，用来重写标准接口。

.. code-block:: C

    dev->open = xxx_open;
    dev->close = xxx_close;
    dev->control = xxx_control;
    dev->write = xxx_write;
    dev->read = xxx_read;
