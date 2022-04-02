PWM
=========================

Introduction
------------------------

PWM is a technology that implements analog voltage control in a digital way. It modulates the width of a series of pulses, equivalent to the required waveform (including shape and amplitude), and digitally encodes the analog signal level. That is to say, by adjusting the change of the duty cycle to adjust the change of the signal, energy, etc. The duty cycle refers to the percentage of the entire signal period when the signal is at a high level. For example, the duty cycle of a square wave is 50%. The DMA device in BL series MCU has the following characteristics:

- Support 5-channel PWM
- Three clock sources can be selected (bus clock <bclk>, crystal oscillator clock <xtal_ck>, slow clock <32k>), with 16-bit clock divider
- Double threshold domain setting, increase pulse flexibility

PWM Device Structure Definition
---------------------------------

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

- **parent**                inherit the properties of the parent class
- **ch**                    channel number, ch is 0 if PWM channel 0 is enabled, ch is 1 if PWM channel 1 is enabled, and so on
- **polarity_invert_mode**  polarity invert enable
- **period**                PWM period value
- **threshold_low**         PWM low threshold
- **threshold_high**        PWM high threshold
- **it_pulse_count**        The cycle count value that triggered the interrupt condition

.. note::
    PWM actual frequency = pwm_clock_source/frequency_division/period, period is not the actual PWM period,it just has the same name.

.. note:: PWM Duty cycle = (threshold_high-threshold_low)/period * 100%

PWM Device Parameter Configuration Table
------------------------------------------

Each PWM device has a parameter configuration macro, the macro definition is located in the ``peripheral_config.h`` file under the ``bsp/board/xxx`` directory, and the variable definition is located in ``hal_pwm.c``, so the user does not need to define variable. When the user opens the macro of the corresponding device, the configuration of the device will take effect. For example, open the macro ``BSP_USING_PWM_CH2``, and ``PWM_CH2_CONFIG`` will take effect, and at the same time, channel 2 of the ``PWM`` device can be registered and used.

.. code-block:: C

    /*Parameter configuration macro*/
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


    /*Variable definitions*/
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

.. note::
    The above configuration can be modified through ``PWM_DEV(dev)->xxx``, and can only be used before calling ``device_open``.

PWM device interface
------------------------

The PWM device interfaces all follow the interfaces provided by the standard device driver management layer. And in order to facilitate the user to call, some standard interfaces are redefined using macros.

**pwm_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``pwm_register`` is used to register a channel of the PWM device standard driver interface. Before registering, you need to open the macro definition of a certain channel of the corresponding PWM device. For example, define ``BSP_USING_PWM_CH0`` before you can use the ``PWM`` channel 0 device. After the registration is completed, other interfaces can be used. If no macro is defined, the PWM device cannot be used.

.. code-block:: C

    int pwm_register(enum pwm_index_type index, const char *name);

- **index** the index of the device to be registered
- **name** name the registered device

``index`` is used to select the configuration of a certain channel of the PWM device. An index corresponds to a channel configuration of a PWM device. For example, ``PWM_CH0_INDEX`` corresponds to the configuration of PWM channel 0, and ``index`` has the following optional types:

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

``device_open`` is used to open a channel of a pwm device,this funtion calls ``pwm_open`` actually.

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

``device_close`` is used to close a channel of a pwm device.this funtion calls ``pwm_close`` actually.

.. code-block:: C

    int device_close(struct device *dev);

- **dev** device handle
- **return** error code, 0 means closing is successful, others mean error

**device_control**
^^^^^^^^^^^^^^^^^^^

``device_control`` is used to control and modify the parameters of the PWM device according to commands.This funtion calls ``pwm_control`` actually.

.. code-block:: C

    int device_control(struct device *dev, int cmd, void *args);

- **dev** device handle
- **cmd** device control command
- **args** control parameters
- **return** different control commands return different meanings

In addition to standard control commands, PWM devices also have their own special control commands.

.. code-block:: C

    #define DEIVCE_CTRL_PWM_IT_PULSE_COUNT_CONFIG 0x10

``args`` input is different depending on ``cmd``, the list is as follows:

.. list-table:: table1
    :widths: 15 10 30
    :header-rows: 1

    * - cmd
      - args
      - description
    * - DEVICE_CTRL_RESUME
      - NULL
      - Enable the current PWM channel
    * - DEVICE_CTRL_SUSPEND
      - NULL
      - Disable the current PWM channel
    * - DEVICE_CTRL_PWM_FREQUENCE_CONFIG
      - uint32_t
      - Configure the current PWM channel period
    * - DEVICE_CTRL_PWM_DUTYCYCLE_CONFIG
      - pwm_dutycycle_config_t
      - Configure the current PWM channel duty cycle
    * - DEVICE_CTRL_PWM_IT_PULSE_COUNT_CONFIG
      - uint32_t
      - Configure the trigger PWM interrupt period value

**device_set_callback**
^^^^^^^^^^^^^^^^^^^^^^^^

``device_set_callback`` is used to register a PWM channel interrupt callback function.

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

    enum pwm_event_type
    {
        PWM_EVENT_COMPLETE,
    };

**pwm_channel_start**
^^^^^^^^^^^^^^^^^^^^^^

``pwm_channel_start`` is used to start the PWM channel. It actually calls ``device_control``, where ``cmd`` is ``DEVICE_CTRL_RESUME``.

.. code-block:: C

    pwm_channel_start(dev)

- **dev** pwm channel handle that needs to be opened


**pwm_channel_stop**
^^^^^^^^^^^^^^^^^^^^^^

``pwm_channel_stop`` is used to close the PWM channel. It actually calls ``device_control``, where ``cmd`` is ``DEVICE_CTRL_SUSPEND``.

.. code-block:: C

    pwm_channel_stop(dev)

- **dev** pwm channel handle that needs to be closed


**pwm_channel_update**
^^^^^^^^^^^^^^^^^^^^^^^

``pwm_channel_update`` is used to update the frequency and duty cycle of the PWM channel. The actual call is ``device_control``, where ``cmd`` is ``DEVICE_CTRL_CONFIG``.

.. code-block:: C

    pwm_channel_update(dev,cfg)

- **dev** pwm channel handle that needs to be updated
- **cfg** pwm_config_t handle


**pwm_it_pulse_count_update**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``pwm_it_pulse_count_update`` is used to update the count value of the PWM channel. The PWM interrupt needs to be enabled before it takes effect. When the PWM count reaches the set period count value, an interrupt will be generated. The actual call is ``device_control``, where ``cmd`` is ``DEIVCE_CTRL_PWM_IT_PULSE_COUNT_CONFIG``.

.. code-block:: C

    pwm_it_pulse_count_update(dev,count)

- **dev** pwm channel handle that needs to update the cycle count value
- **count** cycle count value

