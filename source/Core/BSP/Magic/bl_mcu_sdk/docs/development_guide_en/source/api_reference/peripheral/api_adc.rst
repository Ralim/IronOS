ADC
=========================

Introduction
------------------------
ADC (Analog-to-digital Converter) can convert continuous analog signals into discrete digital signals.

The ADC module in BL MCU series has the following characteristics:

- Support selecting 12/14/16 bits conversion result output
- ADC maximum working clock is 2MHZ
- Support 2.0V, 3.2V optional internal reference voltage
- DMA support
- Support four modes: single channel single conversion, continuous single channel conversion, single multi-channel conversion and continuous multi-channel conversion mode
- Support both single-ended and differential input modes
- 12 external analog channels
- 2 DAC internal channels
- 1 VBAT /2 channel

ADC Device Structure Definition
---------------------------------
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

- **parent**                inherits the properties of the parent class
- **clk_div**               Partial frequency clock in ADC module
- **vref**                  2.0/3.2V reference voltage optional
- **continuous_conv_mode**  Whether to select continuous mode. If it is in continuous mode, once adc_start operation, ADC will continue to work until adc_stop. If it is not in continuous mode, adc will only convert the result once every adc_start.
- **differential_mode**     Whether it is in differential mode, if it is in differential mode, negative voltage can be measured.
- **data_width**            Measurement width selection, the actual accuracy of ADC is 12 bits, but the accuracy of 14bits / 16bits can be achieved by averaging multiple times through OSR. Note that when a higher data width is selected, the frequency will decrease due to averaging. For details, please refer to the enumeration information.
- **fifo_threshold**        This parameter affects the DMA handling threshold and ADC FIFO interrupt threshold
- **gain**                  ADC gain selection for input signal
- Others to be added

ADC Device Parameter Configuration Table
------------------------------------------

Each ADC has a parameter configuration macro, the macro definition is located in the ``peripheral_config.h`` file under the ``bsp/board/xxx`` directory, and the variable definition is located in ``hal_adc.c``, so there is no need for the user to define it by himself . When the user opens the macro of the corresponding device, the configuration of the device will take effect. For example, open the macro ``BSP_USING_ADC0`` to take effect, and at the same time, the ``ADC`` device can be registered and used.

.. code-block:: C

    /*Parameter configuration macro*/
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

    /*Variable definitions*/
    static adc_device_t adcx_device[ADC_MAX_INDEX] = {
    #ifdef BSP_USING_ADC0
        ADC0_CONFIG,
    #endif
    };

.. note:: The above configuration can be modified through ``ADC_DEV(dev)->xxx`` and can only be used before ``device_open``.

ADC Device Interface
------------------------

ADC device interfaces all follow the interfaces provided by the standard device driver management layer.

**adc_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``adc_register`` is used to register an ADC device standard driver interface.Before registering, you need to open the macro definition of the corresponding ADC device. For example, define the macro ``BSP_USING_ADC0`` to use the ``ADC0`` device. After the registration is completed, other interfaces can be used. If no macro is defined, the ``ADC0`` device cannot be used.

.. code-block:: C

    int adc_register(enum adc_index_type index, const char *name);

- **index** device index to be registered
- **name** device name to be registered

``index`` is used to select ADC device configuration, one index corresponds to one ADC device configuration, for example, ``ADC0_INDEX`` corresponds to ``ADC0_CONFIG`` configuration. ``index`` has the following optional types:

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

``device_open`` is used to open an ADC device,this funtion calls ``adc_open`` actually.

.. code-block:: C

    int device_open(struct device *dev, uint16_t oflag);

- **dev** device handle
- **oflag** open mode
- **return** Error code, 0: open successfully, others: error

``oflag`` provides the following types

.. code-block:: C

    #define DEVICE_OFLAG_STREAM_TX  0x001 /* The device is turned on in rotation sending mode */
    #define DEVICE_OFLAG_STREAM_RX  0x002 /* The device is turned on in rotation receiving mode */
    #define DEVICE_OFLAG_INT_TX     0x004 /* The device is turned on in interrupt sending mode */
    #define DEVICE_OFLAG_INT_RX     0x008 /* The device is turned on in interrupt receiving mode */
    #define DEVICE_OFLAG_DMA_TX     0x010 /* The device is turned on in DMA transmission mode */
    #define DEVICE_OFLAG_DMA_RX     0x020 /* The device is turned on in DMA receiving mode */

**device_close**
^^^^^^^^^^^^^^^^

``device_close`` is used to close an ADC device,this funtion calls ``adc_close`` actually.

.. code-block:: C

    int device_close(struct device *dev);

- **dev** device handle
- **return** Error code, 0: open successfully, others: error

**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` is used to control and modify the parameters of the adc device according to commands.This funtion calls ``adc_control`` actually.

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);

- **dev** Device handle
- **cmd** Device control commands
- **args** Control parameter
- **return** Different control commands return different meanings.

In addition to standard control commands, serial devices also have their own special control commands.

.. code-block:: C

    #define DEVICE_CTRL_ADC_CHANNEL_START  0x10
    #define DEVICE_CTRL_ADC_CHANNEL_STOP   0x11
    #define DEVICE_CTRL_ADC_CHANNEL_CONFIG 0x12
    #define DEVICE_CTRL_ADC_VBAT_ON        0x13
    #define DEVICE_CTRL_ADC_VBAT_OFF       0x14
    #define DEVICE_CTRL_ADC_TSEN_ON        0x15
    #define DEVICE_CTRL_ADC_TSEN_OFF       0x16

``args`` input is different depending on ``cmd``, the list is as follows:

.. list-table:: table1
    :widths: 15 10 30
    :header-rows: 1

    * - cmd
      - args
      - description
    * - DEVICE_CTRL_SET_INT
      - adc_it_type
      - Enable ADC device interrupt
    * - DEVICE_CTRL_CLR_INT
      - adc_it_type
      - Disable ADC device interrupt
    * - DEVICE_CTRL_CONFIG
      - ADC_param_cfg_t
      - Modify ADC configuration
    * - DEVICE_CTRL_ADC_CHANNEL_CONFIG
      - adc_channel_cfg_t
      - Modify ADC channel configuration
    * - DEVICE_CTRL_ATTACH_RX_DMA
      - struct device*
      - Link receiving DMA device
    * - DEVICE_CTRL_ADC_CHANNEL_START
      - NULL
      - Start/continue ADC conversion
    * - DEVICE_CTRL_ADC_CHANNEL_STOP
      - NULL
      - Stop ADC conversion
    * - DEVICE_CTRL_ADC_VBAT_ON
      - NULL
      - Turn on the internal VDD measurement circuit
    * - DEVICE_CTRL_ADC_VBAT_OFF
      - NULL
      - Turn off the internal VDD measurement circuit
    * - DEVICE_CTRL_ADC_TSEN_ON
      - NULL
      - Turn on the internal temperature measurement circuit (requires hardware support)
    * - DEVICE_CTRL_ADC_TSEN_OFF
      - NULL
      - Turn off the internal temperature measurement circuit (requires hardware support)

**device_read**
^^^^^^^^^^^^^^^^

``device_read`` is used to receive the data of ADC device, the receiving mode can be polling, interrupt, dma.

.. code-block:: C

    int device_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size);

- **dev** Dvice handle
- **pos** No effect
- **buffer** Buffer to read
- **size** Length to read
- **return** Error code, 0: open successfully, others: error

**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` is used to register an ADC threshold interrupt callback function.

.. code-block:: C

    int device_set_callback(struct device *dev, void (*callback)(struct device *dev, void *args, uint32_t size, uint32_t event));

- **dev** Device handle
- **callback** The interrupt callback function to be registered

    - **dev** Device handle
    - **args** Receive and send buffer, the data type is uint8_t*
    - **size** Transmission length
    - **event** Type of interrupt event

``event`` type is as follows:

.. code-block:: C

    enum ADC_event_type
    {
        ADC_EVENT_FIFO_READY,
        ADC_EVENT_OVERRUN,
        ADC_EVENT_UNDERRUN,
    };