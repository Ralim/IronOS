UART
=========================

Introduction
------------------------

UART is a universal asynchronous transmitter-receiver, which provides a flexible way for full-duplex data exchange with external devices. The UART device in BL series MCU has the following characteristics:

- Data bit length can choose 5/6/7/8 bits
- Stop bit length can be selected 0.5/1/1.5/2 bits
- Support odd/even/no parity bit
- Support hardware flow control (RTS/CTS)
- Automatic baud rate detection
- Support LIN protocol (transceive BREAK/SYNC)
- Send/receive FIFO
- Support polling, interrupt, DMA transfer
- Unique rto interrupt

UART Device Structure Definition
----------------------------------

.. code-block:: C

    typedef struct uart_device
    {
        struct device parent;
        uint8_t id;
        uint32_t baudrate;
        uart_databits_t databits;
        uart_stopbits_t stopbits;
        uart_parity_t parity;
        uint8_t fifo_threshold;
        void* tx_dma;
        void* rx_dma;
    } uart_device_t;

- **parent** inherit the properties of the parent class
- **id** serial port id, if serial port 0 is enabled, id is 0, if serial port 1 is enabled, id is 1, and so on
- **baudrate** baud rate
- **databits** data bits
- **stopbits** stop bits
- **parity** parity
- **fifo_threshold** fifo threshold, the maximum value of different mcu is different
- **tx_dma** additional send dma handle
- **rx_dma** additional receive dma handle

``databits`` provides the following types

.. code-block:: C

    typedef enum
    {
        UART_DATA_LEN_5 = 0,  /*!< Data length is 5 bits */
        UART_DATA_LEN_6 = 1,  /*!< Data length is 6 bits */
        UART_DATA_LEN_7 = 2,  /*!< Data length is 7 bits */
        UART_DATA_LEN_8 = 3   /*!< Data length is 8 bits */
    } uart_databits_t;

``stopbits`` provides the following types

.. code-block:: C

    typedef enum
    {
        UART_STOP_ONE = 0,  /*!< One stop bit */
        UART_STOP_ONE_D_FIVE = 1,  /*!< 1.5 stop bit */
        UART_STOP_TWO = 2   /*!< Two stop bits */
    } uart_stopbits_t;

``parity`` provides the following types

.. code-block:: C

    typedef enum
    {
        UART_PAR_NONE = 0,  /*!< No parity */
        UART_PAR_ODD  = 1,  /*!< Parity bit is odd */
        UART_PAR_EVEN = 2,  /*!< Parity bit is even */
    } uart_parity_t;

UART Device Parameter Configuration Table
-------------------------------------------

Each UART device has a parameter configuration macro, the macro definition is located in the ``peripheral_config.h`` file under the ``bsp/board/xxx`` directory, and the variable definition is located in ``hal_uart.c``, so the user does not need to define variable. When the user opens the macro of the corresponding device, the configuration of the device will take effect. For example, open the macro ``BSP_USING_UART0``, ``UART0_CONFIG`` will take effect, and the ``UART0`` device can be registered and used.

.. code-block:: C

    /*Parameter configuration macro*/
    #if defined(BSP_USING_UART0)
    #ifndef UART0_CONFIG
    #define UART0_CONFIG \
    {   \
    .id = 0, \
    .baudrate = 2000000,\
    .databits = UART_DATA_LEN_8, \
    .stopbits = UART_STOP_ONE, \
    .parity = UART_PAR_NONE, \
    .fifo_threshold = 1, \
    }
    #endif
    #endif

    /*Variable definitions*/
    static uart_device_t uartx_device[UART_MAX_INDEX] =
    {
    #ifdef BSP_USING_UART0
            UART0_CONFIG,
    #endif
    #ifdef BSP_USING_UART1
            UART1_CONFIG,
    #endif
    };

.. note::
    The above configuration can be modified through ``UART_DEV(dev)->xxx`` and can only be used before calling ``device_open``.

UART Device Interface
------------------------

UART device interface follows which provided by the standard device driver management layer.

**uart_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``uart_register`` is used to register a UART device standard driver interface. Before registering, you need to open the macro definition of the corresponding UART device. For example, define the macro ``BSP_USING_UART0`` to use the ``UART0`` device. After the registration is completed, other interfaces can be used. If the macro is not defined, the ``UART0`` device cannot be used.

.. code-block:: C

    int uart_register(enum uart_index_type index, const char *name);

- **index** the index of the device to be registered
- **name** device name

``index`` is used to select UART device configuration, one index corresponds to a UART device configuration, such as ``UART0_INDEX`` corresponds to ``UART0_CONFIG`` configuration, ``index`` has the following optional types

.. code-block:: C

    enum uart_index_type
    {
    #ifdef BSP_USING_UART0
        UART0_INDEX,
    #endif
    #ifdef BSP_USING_UART1
        UART1_INDEX,
    #endif
        UART_MAX_INDEX
    };

**device_open**
^^^^^^^^^^^^^^^^

``device_open`` is used to open a UART device, this funtion calls ``uart_open`` actually.

.. code-block:: C

    int device_open(struct device *dev, uint16_t oflag);

- **dev** device handle
- **oflag** open mode
- **return** error code, 0 means opening is successful, others mean errors

``oflag`` provides the following types

.. code-block:: C

    #define DEVICE_OFLAG_STREAM_TX  0x001 /* The device is turned on in polling sending mode */
    #define DEVICE_OFLAG_STREAM_RX  0x002 /* The device is turned on in rotation receiving mode */
    #define DEVICE_OFLAG_INT_TX     0x004 /* The device is turned on in interrupt sending mode */
    #define DEVICE_OFLAG_INT_RX     0x008 /* The device is turned on in interrupt receiving mode */
    #define DEVICE_OFLAG_DMA_TX     0x010 /* The device is turned on in DMA transmission mode */
    #define DEVICE_OFLAG_DMA_RX     0x020 /* The device is turned on in DMA receiving mode */

**device_close**
^^^^^^^^^^^^^^^^

``device_close`` is used to close a UART device,this funtion calls ``uart_close`` actually.

.. code-block:: C

    int device_close(struct device *dev);

- **dev** device handle
- **return** error code, 0 means closing is successful, others mean error

**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` is used to control and modify the parameters of the UART device according to commands.This funtion calls ``uart_control`` actually.

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);

- **dev** device handle
- **cmd** device control command
- **args** control parameters
- **return** different control commands return different meanings.

In addition to standard control commands, UART device also has its own special control commands.

.. code-block:: C

    #define DEVICE_CTRL_UART_GET_TX_FIFO        0x10
    #define DEVICE_CTRL_UART_GET_RX_FIFO        0x11

``args`` input is different depending on ``cmd``, the list is as follows:

.. list-table:: table1
    :widths: 15 10 30
    :header-rows: 1

    * - cmd
      - args
      - description
    * - DEVICE_CTRL_SET_INT
      - uart_it_type
      - Enable uart device interrupt
    * - DEVICE_CTRL_CLR_INT
      - uart_it_type
      - Disable uart device interrupt
    * - DEVICE_CTRL_CONFIG
      - uart_param_cfg_t*
      - Modify the serial port configuration
    * - DEVICE_CTRL_ATTACH_TX_DMA
      - NULL
      - Link to tx dma device
    * - DEVICE_CTRL_ATTACH_RX_DMA
      - NULL
      - Link to rx dma device
    * - DEVICE_CTRL_TX_DMA_SUSPEND
      - NULL
      - Suspend uart tx dma mode
    * - DEVICE_CTRL_RX_DMA_SUSPEND
      - NULL
      - Suspend uart rx dma mode
    * - DEVICE_CTRL_TX_DMA_RESUME
      - NULL
      - Resume uart tx dma mode
    * - DEVICE_CTRL_RX_DMA_RESUME
      - NULL
      - Resume uart rx dma mode
    * - DEVICE_CTRL_UART_GET_TX_FIFO
      - uint32_t*
      - Get the number of uart tx fifo
    * - DEVICE_CTRL_UART_GET_RX_FIFO
      - uint32_t*
      - Get the number of uart rx fifo

**device_write**
^^^^^^^^^^^^^^^^

``device_write`` is used to send data. The sending mode can be polling, interrupt, dma according to the open mode.This funtion calls ``uart_write`` actually.

.. code-block:: C

    int device_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size);

- **dev** device handle
- **pos** useless
- **buffer** the buffer to be written
- **size** the length to be written
- **return** error code, 0 means writing is successful, others mean errors

**device_read**
^^^^^^^^^^^^^^^^

``device_read`` is used to receive data. The receiving mode can be polling, interrupt, dma according to the open mode.This funtion calls ``uart_read`` actually.

.. code-block:: C

    int device_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size);

- **dev** device handle
- **pos** useless
- **buffer** the buffer to be read
- **size** the length to be read
- **return** error code, 0 means successful reading, others mean errors

**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` is used to register a uart interrupt callback function.

.. code-block:: C

    int device_set_callback(struct device *dev, void (*callback)(struct device *dev, void *args, uint32_t size, uint32_t event));

- **dev** device handle
- **callback** the interrupt callback function to be registered

    - **dev** device handle
    - **args** receive and send buffer, the data type is uint8_t*
    - **size** transmission length
    - **event** interrupt event type

``event`` type definition is as follows:

.. code-block:: C

    enum uart_event_type
    {
        UART_EVENT_TX_END,
        UART_EVENT_TX_FIFO,
        UART_EVENT_RX_END,
        UART_EVENT_RX_FIFO,
        UART_EVENT_RTO,
        UART_EVENT_UNKNOWN
    };