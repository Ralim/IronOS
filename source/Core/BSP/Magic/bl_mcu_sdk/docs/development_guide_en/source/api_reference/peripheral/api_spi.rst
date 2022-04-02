SPI
=========================

Introduction
------------------------

Serial Peripheral Interface Bus (SPI) is a synchronous serial communication interface specification for short-range communication. The full-duplex communication mode is used between devices, which is a master-slave mode of one master and one or more slaves, and requires at least 4 wires. In fact, 3 wires are also available (when transmitting in one direction), including SDI (data input), SDO (data output), SCLK (clock), CS (chip select). The SPI devices in the BL series MCU have the following characteristics:

- It can be used as both SPI master and SPI slave.
- The transmit and receive channels each have a FIFO with a depth of 4 words
- Both master and slave devices support 4 clock formats (CPOL, CPHA)
- Both master and slave devices support 1/2/3/4 byte transmission mode
- Flexible clock configuration, up to 40M clock
- Configurable MSB/LSB priority transmission
- Receive filter function
- Timeout mechanism under slave device
- Support polling, interrupt, DMA transfer

SPI Device Structure Definition
----------------------------------

.. code-block:: C

    typedef struct spi_device
    {
        struct device parent;
        uint8_t id;
        uint32_t clk;
        uint8_t mode;
        uint8_t direction;
        uint8_t clk_polaraity;
        uint8_t clk_phase;
        uint8_t datasize;
        uint8_t fifo_threshold;
        void* tx_dma;
        void* rx_dma;
    } spi_device_t;

- **parent**        inherit the properties of the parent class
- **id**            SPI id, 0 means SPI0
- **clk**           SPI clock frequency
- **mode**          master mode or slave mode
- **direction**     transmission first mode
- **clk_polaraity** clock polarity
- **clk_phase**     clock phase
- **datasize**      data transmission bit width
- **fifo_threshold** fifo threshold, the maximum is 4
- **tx_dma**        additional send dma handle
- **rx_dma**        dditional receive dma handle

``mode`` provides the following types

.. code-block:: C

    #define SPI_SLVAE_MODE                                0
    #define SPI_MASTER_MODE                               1

``direction`` provides the following types

.. code-block:: C

    #define SPI_LSB_BYTE0_DIRECTION_FIRST 0
    #define SPI_LSB_BYTE3_DIRECTION_FIRST 1
    #define SPI_MSB_BYTE0_DIRECTION_FIRST 2
    #define SPI_MSB_BYTE3_DIRECTION_FIRST 3

``clk_polaraity`` provides the following types

.. code-block:: C

    #define SPI_POLARITY_LOW                              0
    #define SPI_POLARITY_HIGH                             1

``clk_phase`` provides the following types

.. code-block:: C

    #define SPI_PHASE_1EDGE                               0
    #define SPI_PHASE_2EDGE                               1


``datasize`` provides the following types

.. code-block:: C

    #define SPI_DATASIZE_8BIT                            0
    #define SPI_DATASIZE_16BIT                           1
    #define SPI_DATASIZE_24BIT                           2
    #define SPI_DATASIZE_32BIT                           3

SPI Device Parameter Configuration Table
------------------------------------------

Each SPI device has a parameter configuration macro, the macro definition is located in the ``peripheral_config.h`` file under the ``bsp/board/xxx`` directory, and the variable definition is located in ``hal_spi.c``, so the user does not need to define variable. When the user opens the macro of the corresponding device, the configuration of the device will take effect. For example, open the macro ``BSP_USING_SPI0``, ``SPI0_CONFIG`` will take effect, and the ``SPI0`` device can be registered and used.

.. code-block:: C

    /*Parameter configuration macro*/
    #if defined(BSP_USING_SPI0)
    #ifndef SPI0_CONFIG
    #define SPI0_CONFIG \
    {   \
    .id = 0, \
    .clk = 18000000,\
    .mode = SPI_MASTER_MODE, \
    .direction = SPI_MSB_BYTE0_DIRECTION_FIRST, \
    .clk_polaraity = SPI_POLARITY_LOW, \
    .clk_phase = SPI_PHASE_1EDGE, \
    .datasize = SPI_DATASIZE_8BIT, \
    .fifo_threshold = 1, \
    }
    #endif
    #endif

    /*Variable definition*/
    static spi_device_t spix_device[SPI_MAX_INDEX] =
    {
    #ifdef BSP_USING_SPI0
        SPI0_CONFIG,
    #endif
    };

.. note::
    The above configuration can be modified through ``SPI_DEV(dev)->xxx`` and can only be used before calling ``device_open``.


SPI Device Interface
------------------------

SPI device interface follows which provided by the standard device driver management layer.

**spi_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``spi_register`` is used to register an SPI device standard driver interface. Before registering, you need to open the macro definition of the corresponding SPI device. For example, define the macro ``BSP_USING_SPI0`` before you can use the SPI0 device. After the registration is completed, other interfaces can be used. If no macro is defined, the SPI device cannot be used.

.. code-block:: C

    int spi_register(enum spi_index_type index, const char *name);

- **index** device index to be registered
- **name** device name to be registered

``index`` is used to select SPI device configuration, one index corresponds to one SPI device configuration, for example, ``SPI0_INDEX`` corresponds to ``SPI0_CONFIG`` configuration, and ``index`` has the following optional types

.. code-block:: C

    enum spi_index_type
    {
    #ifdef BSP_USING_SPI0
        SPI0_INDEX,
    #endif
        SPI_MAX_INDEX
    };

**device_open**
^^^^^^^^^^^^^^^^

``device_open`` is used to open the device,this funtion calls ``spi_open`` actually.

.. code-block:: C

    int device_open(struct device *dev, uint16_t oflag);

- **dev** device handle
- **oflag** open mode
- **return** error code, 0 means opening is successful, others mean errors

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

``device_close`` is used to close the device,this funtion calls ``spi_close`` actually.

.. code-block:: C

    int device_close(struct device *dev);

- **dev** device handle
- **return** error code, 0 means closing is successful, others means error

**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` is used to control the device and modify parameters according to commands.This funtion calls ``spi_control`` actually.

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);

- **dev** device handle
- **cmd** device control command
- **args** control parameters
- **return** Different control commands return different meanings.

In addition to standard control commands, SPI devices also have their own special control commands.

.. code-block:: C

    #define DEVICE_CTRL_SPI_CONFIG_CLOCK       0x10

``args`` input is different depending on ``cmd``, the list is as follows:

.. list-table:: table1
    :widths: 15 10 30
    :header-rows: 1

    * - cmd
      - args
      - description
    * - DEVICE_CTRL_SET_INT
      - NULL
      - Enable spi device interrupt
    * - DEVICE_CTRL_CLR_INT
      - NULL
      - Disable spi device interrupt
    * - DEVICE_CTRL_RESUME
      - NULL
      - Resume spi device
    * - DEVICE_CTRL_SUSPEND
      - NULL
      - Suspend spi device
    * - DEVICE_CTRL_ATTACH_TX_DMA
      - NULL
      - Link to tx dma device
    * - DEVICE_CTRL_ATTACH_RX_DMA
      - NULL
      - Link to rx dma device
    * - DEVICE_CTRL_SPI_CONFIG_CLOCK
      - NULL
      - Modify SPI device clock
    * - DEVICE_CTRL_TX_DMA_SUSPEND
      - NULL
      - Suspend spi tx dma mode
    * - DEVICE_CTRL_RX_DMA_SUSPEND
      - NULL
      - Suspend spi rx dma mode
    * - DEVICE_CTRL_TX_DMA_RESUME
      - NULL
      - Resume spi tx dma mode
    * - DEVICE_CTRL_RX_DMA_RESUME
      - NULL
      - Resume spi rx dma mode

**device_write**
^^^^^^^^^^^^^^^^

``device_write`` is used to send data. The sending mode can be polling, interrupt, dma according to the open mode.This funtion calls ``spi_write`` actually.

.. code-block:: C

    int device_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size);

- **dev** device handle
- **pos** useless
- **buffer** the buffer to be written
- **size** the length to be written
- **return** error code, 0 means writing is successful, others mean errors

**device_read**
^^^^^^^^^^^^^^^^

``device_read`` is used to receive data, and the receiving mode can be polling, interrupt, dma according to the open mode.This funtion calls ``spi_read`` actually.

.. code-block:: C

    int device_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size);

- **dev** device handle
- **pos** useless
- **buffer** the buffer to be read
- **size** the length to be read
- **return** error code, 0 means successful reading, others mean errors

**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` is used to register an SPI device interrupt callback function.

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

    enum spi_event_type
    {
        SPI_EVENT_TX_FIFO,
        SPI_EVENT_RX_FIFO,
        SPI_EVENT_UNKNOWN
    };

**spi_transmit**
^^^^^^^^^^^^^^^^^^^^^^^^

``spi_transmit`` is used to send data from SPI devices.

.. code-block:: C

    int spi_transmit(struct device *dev, void *buffer, uint32_t size, uint8_t type);

- **dev** device handle
- **buffer** send data buffer
- **size** send length
- **type** send bit width type

``type`` provides the following types

.. code-block:: C

    #define SPI_TRANSFER_TYPE_8BIT    0
    #define SPI_TRANSFER_TYPE_16BIT   1
    #define SPI_TRANSFER_TPYE_24BIT   2
    #define SPI_TRANSFER_TYPE_32BIT   3

**spi_receive**
^^^^^^^^^^^^^^^^^^^^^^^^

``spi_receive`` is used to receive data from SPI devices.

.. code-block:: C

    int spi_receive(struct device *dev, void *buffer, uint32_t size, uint8_t type);

- **dev** device handle
- **buffer** receive data buffer
- **size** receiving length
- **type** bit width type

**spi_transmit_receive**
^^^^^^^^^^^^^^^^^^^^^^^^

``spi_transmit_receive`` is used to send and receive data from SPI devices.

.. code-block:: C

    int spi_transmit_receive(struct device *dev, const void *send_buf, void *recv_buf, uint32_t length, uint8_t type);

- **dev** device handle
- **send_buf** send data buffer
- **recv_buf** receive data buffer
- **length** send and receive length
- **type** bit width type
