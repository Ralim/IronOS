TIMER
=========================

Introduction
------------------------

The TIMER device in BL series MCU has the following characteristics:

- Multiple clock source options
- 8-bit clock divider, the division factor is 1-256
- Two 32-bit timers
- Each timer can independently set three groups of alarm values
- Support FreeRun mode and PreLoad mode
- 16-bit watchdog
- Support write protection to prevent system abnormalities caused by incorrect settings
- Support two watchdog overflow modes, interrupt or reset

TIMER Device Structure Definition
------------------------------------

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

- **parent** inherit the properties of the parent class
- **id** timer id
- **cnt_mode** counting mode:FreeRun and preload
- **trigger** source of preload comparator
- **reload** reload value in preload mode
- **timeout1** compare source 0 timout value ,unit is us
- **timeout2** compare source 1 timout value ,unit is us
- **timeout3** compare source 2 timout value ,unit is us

``ch`` provides the following types

.. code-block:: C

    enum timer_index_type {
        TIMER0_INDEX,
        TIMER1_INDEX,
        TIMER_MAX_INDEX
    };

``cnt_mode`` provides the following types

.. code-block:: C

    enum timer_cnt_mode_type {
        TIMER_CNT_PRELOAD,
        TIMER_CNT_FREERUN,
    };

``pl_trig_src`` provides the following types

.. code-block:: C

    enum timer_preload_trigger_type {
        TIMER_PRELOAD_TRIGGER_NONE,
        TIMER_PRELOAD_TRIGGER_COMP0,
        TIMER_PRELOAD_TRIGGER_COMP1,
        TIMER_PRELOAD_TRIGGER_COMP2,
    };

TIMER Device Parameter Configuration Table
---------------------------------------------

Each TIMER has a parameter configuration macro, the macro definition is located in the ``peripheral_config.h`` file under the ``bsp/board/xxx`` directory, and the variable definition is located in ``hal_timer.c``, so the user does not need to define the variable . When the user opens the macro of the corresponding device, the configuration of the device will take effect. For example, open the macro ``BSP_USING_TIMER_CH0``, ``TIMER_CH0_CONFIG`` will take effect, and the ``TIMER_CH0_INDEX`` device can be registered and used.

.. code-block:: C

    /*Parameter configuration macro*/
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

    /*Variable definitions*/
    static timer_device_t timerx_device[TIMER_MAX_INDEX] = {
    #ifdef BSP_USING_TIMER0
        TIMER0_CONFIG,
    #endif
    #ifdef BSP_USING_TIMER1
        TIMER1_CONFIG,
    #endif
    };

.. note::
    The above configuration can be modified through ``TIMER_DEV(dev)->xxx`` and can only be used before calling ``device_open``.

TIMER Device Interface
------------------------

TIMER device interface follows which provided by the standard device driver management layer.

**timer_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``timer_register`` is used to register a TIMER device standard driver interface. Before registering, you need to open the macro definition of the corresponding TIMER device. For example, define the macro ``BSP_USING_TIMER_CH0`` to use the ``TIMER_CH0_INDEX`` device. After the registration is completed, other interfaces can be used. If the macro is not defined, the ``TIMER_CH0_INDEX`` device cannot be used.

.. code-block:: C

    int timer_register(enum timer_index_type index, const char *name);

- **index** the index of the device to be registered
- **name** Name the device

``index`` is used to select TIMER device configuration, one index corresponds to a TIMER device configuration, for example, ``TIMER_CH0_INDEX`` corresponds to ``TIMER_CH0_CONFIG`` configuration, and ``index`` has the following optional types

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

``device_open`` is used to open a TIMER device, this funtion calls ``timer_open`` actually.

.. code-block:: C

    int device_open(struct device *dev, uint16_t oflag);

- **dev** device handle
- **oflag** open method
- **return** error code, 0 means opening is successful, others mean error

``oflag`` provides the following types

.. code-block:: C

    #define DEVICE_OFLAG_STREAM_TX  0x001 /* The device is turned on in polling sending mode */
    #define DEVICE_OFLAG_STREAM_RX  0x002 /* The device is turned on in polling receiving mode */
    #define DEVICE_OFLAG_INT_TX     0x004 /* The device is turned on in interrupt sending mode */
    #define DEVICE_OFLAG_INT_RX     0x008 /* The device is turned on in interrupt receiving mode */
    #define DEVICE_OFLAG_DMA_TX     0x010 /* The device is turned on in DMA transmission mode */
    #define DEVICE_OFLAG_DMA_RX     0x020 /* The device is turned on in DMA receiving mode */

**device_close**
^^^^^^^^^^^^^^^^

``device_close`` is used to close a TIMER device,this funtion calls ``timer_open`` actually.

.. code-block:: C

    int device_close(struct device *dev);

- **dev** device handle
- **return** error code, 0 means closing is successful, others mean error

**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` is used to control and modify the parameters of the TIMER device according to commands.This funtion calls ``timer_control`` actually.

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);

- **dev** device handle
- **cmd** device control command
- **args** control parameters
- **return** different control commands return different meanings

In addition to standard control commands, TIMER device also has its own special control commands.

.. code-block:: C

    #define DEVICE_CTRL_TIMER_CH_START   0x80
    #define DEVICE_CTRL_TIMER_CH_STOP    0x81
    #define DEVICE_CTRL_GET_MATCH_STATUS 0x82

``args`` input is different depending on ``cmd``, the list is as follows:

.. list-table:: table1
    :widths: 15 10 30
    :header-rows: 1

    * - cmd
      - args
      - description
    * - DEVICE_CTRL_SET_INT
      - timer_it_type
      - Enable TIMER interrupt
    * - DEVICE_CTRL_CLR_INT
      - timer_it_type
      - Disable TIMER interrupt
    * - DEVICE_CTRL_GET_INT
      - NULL
      - Get TIMER interrupt status
    * - DEVICE_CTRL_RESUME
      - NULL
      - Enable TIMER
    * - DEVICE_CTRL_SUSPEND
      - NULL
      - Disable TIMER
    * - DEVICE_CTRL_GET_CONFIG
      - NULL
      - Get TIMER current count

**device_write**
^^^^^^^^^^^^^^^^

``device_write`` is used to config timer device timeout value.This funtion calls ``timer_write`` actually.

.. code-block:: C

    int device_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size);

- **dev** device handle
- **pos** unused
- **buffer** timer_timeout_cfg_t handle
- **size** the length of timer_timeout_cfg_t
- **return** error code, 0 means writing is successful, others mean errors

**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` is used to register a timer compare interrupt callback function.

.. code-block:: C

    int device_set_callback(struct device *dev, void (*callback)(struct device *dev, void *args, uint32_t size, uint32_t event));

- **dev** device handle
- **callback** the interrupt callback function to be registered

    - **dev** device handle
    - **args** unused
    - **size** unused
    - **event** interrupt event type

``event`` type definition is as follows:

.. code-block:: C

    enum timer_event_type {
        TIMER_EVENT_COMP0,
        TIMER_EVENT_COMP1,
        TIMER_EVENT_COMP2,
        TIMER_EVENT_UNKNOWN
    };