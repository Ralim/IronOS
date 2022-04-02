DMA
=========================

Introduction
------------------------

DMA is a memory access technology that can directly read and write system memory independently without processor intervention. Under the same degree of processor burden, DMA is a fast data transfer method. The DMA device in BL series MCU has the following characteristics:

- 8 independent dedicated channels
- Four transmission directions: memory to memory, memory to peripheral, peripheral to memory, peripheral to peripheral
- LLI linked list

DMA Device Structure Definition
----------------------------------

.. code-block:: C

    typedef struct dma_device
    {
        struct device parent;
        uint8_t id;
        uint8_t ch;
        uint8_t direction;
        uint8_t transfer_mode;
        uint32_t src_req;
        uint32_t dst_req;
        uint8_t src_burst_size;
        uint8_t dst_burst_size;
        uint8_t src_width;
        uint8_t dst_width;
        dma_lli_ctrl_t *lli_cfg;
    } dma_device_t;

- **parent**            inherits the properties of the parent class
- **id**                DMA id number, default 0, currently there is only one DMA
- **ch**                channel number
- **direction**         transmission direction
- **transfer_mode**     transfer mode
- **src_req**           source request
- **dst_req**           destination request
- **src_burst_size**    source burst bytes
- **dst_burst_size**    destination number of burst bytes
- **src_width**         source transmission bit width
- **dst_width**         destination transmission bit width
- **lli_cfg**           used to store some information of the dma channel, the user does not need to worry about it

``direction`` provides the following types

.. code-block:: C

    typedef enum {
        DMA_MEMORY_TO_MEMORY = 0,                        /*!< DMA transfer tyep:memory to memory */
        DMA_MEMORY_TO_PERIPH,                            /*!< DMA transfer tyep:memory to peripheral */
        DMA_PERIPH_TO_MEMORY,                            /*!< DMA transfer tyep:peripheral to memory */
        DMA_PERIPH_TO_PERIPH,                            /*!< DMA transfer tyep:peripheral to peripheral */
    }dma_transfer_dir_type;

``transfer_mode`` provides the following types

.. code-block:: C

    #define DMA_LLI_ONCE_MODE     0
    #define DMA_LLI_CYCLE_MODE    1

``src_req`` provides the following types

.. code-block:: C

    #define DMA_REQUEST_NONE        0x00000000 /*!< DMA request peripheral:None */
    #define DMA_REQUEST_UART0_RX    0x00000000 /*!< DMA request peripheral:UART0 RX */
    #define DMA_REQUEST_UART0_TX    0x00000001 /*!< DMA request peripheral:UART0 TX */
    #define DMA_REQUEST_UART1_RX    0x00000002 /*!< DMA request peripheral:UART1 RX */
    #define DMA_REQUEST_UART1_TX    0x00000003 /*!< DMA request peripheral:UART1 TX */
    #define DMA_REQUEST_I2C0_RX     0x00000006 /*!< DMA request peripheral:I2C RX */
    #define DMA_REQUEST_I2C0_TX     0x00000007 /*!< DMA request peripheral:I2C TX */
    #define DMA_REQUEST_SPI0_RX     0x0000000A /*!< DMA request peripheral:SPI RX */
    #define DMA_REQUEST_SPI0_TX     0x0000000B /*!< DMA request peripheral:SPI TX */
    #define DMA_REQUEST_I2S_RX      0x00000014 /*!< DMA request peripheral:I2S RX */
    #define DMA_REQUEST_I2S_TX      0x00000015 /*!< DMA request peripheral:I2S TX */
    #define DMA_REQUEST_ADC0        0x00000016 /*!< DMA request peripheral:ADC0 */
    #define DMA_REQUEST_DAC0        0x00000017 /*!< DMA request peripheral:DAC0 */
    #define DMA_REQUEST_USB_EP0     0x00000018 /*!< DMA request peripheral:USB EP0*/
    #define DMA_REQUEST_USB_EP1     0x00000019 /*!< DMA request peripheral:USB EP1*/
    #define DMA_REQUEST_USB_EP2     0x0000001A /*!< DMA request peripheral:USB EP2*/
    #define DMA_REQUEST_USB_EP3     0x0000001B /*!< DMA request peripheral:USB EP3*/
    #define DMA_REQUEST_USB_EP4     0x0000001C /*!< DMA request peripheral:USB EP4*/
    #define DMA_REQUEST_USB_EP5     0x0000001D /*!< DMA request peripheral:USB EP5*/
    #define DMA_REQUEST_USB_EP6     0x0000001E /*!< DMA request peripheral:USB EP6*/
    #define DMA_REQUEST_USB_EP7     0x0000001F /*!< DMA request peripheral:USB EP7 */

``dst_req`` provides the following types

.. code-block:: C

    #define DMA_REQUEST_NONE        0x00000000 /*!< DMA request peripheral:None */
    #define DMA_REQUEST_UART0_RX    0x00000000 /*!< DMA request peripheral:UART0 RX */
    #define DMA_REQUEST_UART0_TX    0x00000001 /*!< DMA request peripheral:UART0 TX */
    #define DMA_REQUEST_UART1_RX    0x00000002 /*!< DMA request peripheral:UART1 RX */
    #define DMA_REQUEST_UART1_TX    0x00000003 /*!< DMA request peripheral:UART1 TX */
    #define DMA_REQUEST_I2C0_RX     0x00000006 /*!< DMA request peripheral:I2C RX */
    #define DMA_REQUEST_I2C0_TX     0x00000007 /*!< DMA request peripheral:I2C TX */
    #define DMA_REQUEST_SPI0_RX     0x0000000A /*!< DMA request peripheral:SPI RX */
    #define DMA_REQUEST_SPI0_TX     0x0000000B /*!< DMA request peripheral:SPI TX */
    #define DMA_REQUEST_I2S_RX      0x00000014 /*!< DMA request peripheral:I2S RX */
    #define DMA_REQUEST_I2S_TX      0x00000015 /*!< DMA request peripheral:I2S TX */
    #define DMA_REQUEST_ADC0        0x00000016 /*!< DMA request peripheral:ADC0 */
    #define DMA_REQUEST_DAC0        0x00000017 /*!< DMA request peripheral:DAC0 */
    #define DMA_REQUEST_USB_EP0     0x00000018 /*!< DMA request peripheral:USB EP0*/
    #define DMA_REQUEST_USB_EP1     0x00000019 /*!< DMA request peripheral:USB EP1*/
    #define DMA_REQUEST_USB_EP2     0x0000001A /*!< DMA request peripheral:USB EP2*/
    #define DMA_REQUEST_USB_EP3     0x0000001B /*!< DMA request peripheral:USB EP3*/
    #define DMA_REQUEST_USB_EP4     0x0000001C /*!< DMA request peripheral:USB EP4*/
    #define DMA_REQUEST_USB_EP5     0x0000001D /*!< DMA request peripheral:USB EP5*/
    #define DMA_REQUEST_USB_EP6     0x0000001E /*!< DMA request peripheral:USB EP6*/
    #define DMA_REQUEST_USB_EP7     0x0000001F /*!< DMA request peripheral:USB EP7 */

``src_burst_size`` provides the following types

.. code-block:: C

    #define DMA_BURST_1BYTE     0
    #define DMA_BURST_4BYTE     1
    #define DMA_BURST_8BYTE     2
    #define DMA_BURST_16BYTE    3

``dst_burst_size`` provides the following types

.. code-block:: C

    #define DMA_BURST_1BYTE     0
    #define DMA_BURST_4BYTE     1
    #define DMA_BURST_8BYTE     2
    #define DMA_BURST_16BYTE    3

``src_width`` provides the following types

.. code-block:: C

    #define DMA_TRANSFER_WIDTH_8BIT  0
    #define DMA_TRANSFER_WIDTH_16BIT 1
    #define DMA_TRANSFER_WIDTH_32BIT 2

``dst_width``: provide the following types

.. code-block:: C

    #define DMA_TRANSFER_WIDTH_8BIT  0
    #define DMA_TRANSFER_WIDTH_16BIT 1
    #define DMA_TRANSFER_WIDTH_32BIT 2

DMA Device Parameter Configuration Table
------------------------------------------------

Each DMA device has a parameter configuration macro, the macro definition is located in the ``peripheral_config.h`` file under the ``bsp/board/xxx`` directory, and the variable definition is located in ``hal_dma.c``, so there is no need for the user himself Define variables. When the user opens the macro of the corresponding device, the configuration of the device will take effect. For example, open the macro ``BSP_USING_DMA0_CH0``, ``DMA0_CH0_CONFIG`` will take effect, and the DMA channel 0 device can be registered and used.

.. code-block:: C

    /*Parameter configuration macro*/
    #if defined(BSP_USING_DMA0_CH0)
    #ifndef DMA0_CH0_CONFIG
    #define DMA0_CH0_CONFIG \
    {   \
     .id = 0, \
     .ch = 0,\
     .direction = DMA_MEMORY_TO_MEMORY,\
     .transfer_mode = DMA_LLI_ONCE_MODE, \
     .src_req = DMA_REQUEST_NONE, \
     .dst_req = DMA_REQUEST_NONE, \
     .src_width = DMA_TRANSFER_WIDTH_32BIT , \
     .dst_width = DMA_TRANSFER_WIDTH_32BIT , \
    }
    #endif
    #endif


    /*Variable definitions*/
    static dma_device_t dmax_device[DMA_MAX_INDEX] =
    {
    #ifdef BSP_USING_DMA0_CH0
        DMA0_CH0_CONFIG,
    #endif
    #ifdef BSP_USING_DMA0_CH1
        DMA0_CH1_CONFIG,
    #endif
    #ifdef BSP_USING_DMA0_CH2
        DMA0_CH2_CONFIG,
    #endif
    #ifdef BSP_USING_DMA0_CH3
        DMA0_CH3_CONFIG,
    #endif
    #ifdef BSP_USING_DMA0_CH4
        DMA0_CH4_CONFIG,
    #endif
    #ifdef BSP_USING_DMA0_CH5
        DMA0_CH5_CONFIG,
    #endif
    #ifdef BSP_USING_DMA0_CH6
        DMA0_CH6_CONFIG,
    #endif
    #ifdef BSP_USING_DMA0_CH7
        DMA0_CH7_CONFIG,
    #endif
    };

.. note::
    The above configuration can be modified through ``DMA_DEV(dev)->xxx`` and can only be used before calling ``device_open``.

DMA Device Interface
------------------------

The DMA device interface follows which provided by the standard device driver management layer. In order to facilitate the user to call, some standard interfaces are redefined using macros.

**dma_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``dma_register`` is used to register a DMA device standard driver interface. Before registering, you need to open the channel macro definition of the corresponding DMA device. For example, after defining the macro ``BSP_USING_DMA_CH0``, the 0 channel of the ``DMA`` device can be used. After the registration is completed, other interfaces can be used. If the macro is not defined, the 0 channel of the ``DMA`` device cannot be used.

.. code-block:: C

    int dma_register(enum dma_index_type index, const char *name);

- **index** device index to be registered
- **name** device name to be registered

``index`` is used to select the configuration of a certain channel of DMA, an index corresponds to a channel configuration of a DMA, for example, ``DMA_CH0_INDEX`` corresponds to the configuration of DMA channel 0, and ``index`` has the following optional types

.. code-block:: C

    enum dma_index_type
    {
    #ifdef BSP_USING_DMA0_CH0
        DMA0_CH0_INDEX,
    #endif
    #ifdef BSP_USING_DMA0_CH1
        DMA0_CH1_INDEX,
    #endif
    #ifdef BSP_USING_DMA0_CH2
        DMA0_CH2_INDEX,
    #endif
    #ifdef BSP_USING_DMA0_CH3
        DMA0_CH3_INDEX,
    #endif
    #ifdef BSP_USING_DMA0_CH4
        DMA0_CH4_INDEX,
    #endif
    #ifdef BSP_USING_DMA0_CH5
        DMA0_CH5_INDEX,
    #endif
    #ifdef BSP_USING_DMA0_CH6
        DMA0_CH6_INDEX,
    #endif
    #ifdef BSP_USING_DMA0_CH7
        DMA0_CH7_INDEX,
    #endif
        DMA_MAX_INDEX
    };

**device_open**
^^^^^^^^^^^^^^^^

``device_open`` is used to open a channel of a dma device,this funtion calls ``dma_open`` actually.

.. code-block:: C

    int device_open(struct device *dev, uint16_t oflag);

- **dev** device handle
- **oflag** open mode
- **return** Error code, 0 means opening is successful, other means error

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

``device_close`` is used to close a channel of a dma device,this funtion calls ``dma_close`` actually.

.. code-block:: C

    int device_close(struct device *dev);

- **dev** device handle
- **return** error code, 0 means closing is successful, others mean error

**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` is used to control and modify the parameters of the dma device according to commands.This funtion calls ``dma_control`` actually.

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);

- **dev** device handle
- **cmd** device control command
- **args** control parameters
- **return** different control commands return different meanings

In addition to standard control commands, DMA devices also have their own special control commands.

.. code-block:: C

    #define DMA_CHANNEL_GET_STATUS  0x10
    #define DMA_CHANNEL_START       0x11
    #define DMA_CHANNEL_STOP        0x12
    #define DMA_CHANNEL_UPDATE      0x13

``args`` input is different depending on ``cmd``, the list is as follows:

.. list-table:: table1
    :widths: 15 10 30
    :header-rows: 1

    * - cmd
      - args
      - description
    * - DEVICE_CTRL_SET_INT
      - NULL
      - Enable DMA transfer completion interrupt
    * - DEVICE_CTRL_CLR_INT
      - NULL
      - Disable DMA transfer completion interrupt
    * - DMA_CHANNEL_GET_STATUS
      - NULL
      - Get DMA channel completion status
    * - DMA_CHANNEL_START
      - NULL
      - Open dma channel
    * - DMA_CHANNEL_STOP
      - NULL
      - Close dma channel
    * - DMA_CHANNEL_UPDATE
      - NULL
      - Update dma transmission configuration

**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` is used to register a DMA channel interrupt callback function.

.. code-block:: C

    int device_set_callback(struct device *dev, void (*callback)(struct device *dev, void *args, uint32_t size, uint32_t event));

- **dev** Device handle
- **callback** The interrupt callback function to be registered

    - **dev** device handle
    - **args** unused
    - **size** unused
    - **event** interrupt event type

``event`` type definition is as follows:

.. code-block:: C

    enum dma_event_type
    {
        DMA_EVENT_COMPLETE,
    };


**dma_channel_start**
^^^^^^^^^^^^^^^^^^^^^^

``dma_channel_start`` is used to open the DMA channel. It actually calls ``device_control``, where ``cmd`` is ``DMA_CHANNEL_START``.

.. code-block:: C

    dma_channel_start(dev)

- **dev** dma channel handle that needs to be opened


**dma_channel_stop**
^^^^^^^^^^^^^^^^^^^^^^

``dma_channel_stop`` is used to close the DMA channel. It actually calls ``device_control``, where ``cmd`` is ``DMA_CHANNEL_STOP``.

.. code-block:: C

    dma_channel_stop(dev)

- **dev** dma channel handle that needs to be closed


**dma_channel_update**
^^^^^^^^^^^^^^^^^^^^^^^

``dma_channel_update`` is used to update the DMA configuration. The actual call is ``device_control``, where ``cmd`` is ``DMA_CHANNEL_UPDATE``.

.. code-block:: C

    dma_channel_update(dev,list)

- **dev** dma channel handle that needs to be updated
- **list** dma_lli_ctrl_t handle


**dma_channel_check_busy**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``dma_channel_check_busy`` is used to query whether the currently used DMA channel has completed the transfer. It actually calls ``device_control``, where ``cmd`` is ``DMA_CHANNEL_GET_STATUS``.

.. code-block:: C

    dma_channel_check_busy(dev)

- **dev** DMA channel handle to be queried
- **return** return the current DMA status, 0 means the transfer is complete, 1 means the transfer is not complete

**dma_reload**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``dma_reload`` is used to update the configuration of a certain channel of DMA. Compared with ``dma_channel_update``, this function does not require the user to pass many parameters, but only needs to fill in the source address, destination address, and length. After this function is called, the DMA channel is not turned on. You need to manually call the ``dma_channel_start`` function.

.. code-block:: C

    int dma_reload(struct device *dev, uint32_t src_addr, uint32_t dst_addr, uint32_t transfer_size);

- **dev** DMA channel handle to be queried
- **src_addr** transmission source address
- **dst_addr** transmission destination address
- **transfer_size** the total length of transferred bytes. If the number of bits transferred is 16 bits or 32 bits, it needs to be converted into byte length here.

