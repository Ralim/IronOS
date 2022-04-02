I2C
=========================

Introduction
------------------------

I2C (Inter-Intergrated Circuit) is a serial communication bus that uses a multi-master-slave architecture to connect low-speed peripheral devices. Each device has a unique address identification, and can be used as a transmitter or receiver. Each device connected to the bus can set the address with software through a unique address and the always-existing relationship between master and slave, and the host can be used as a host transmitter or host receiver. If two or more hosts are initialized at the same time, data transmission can prevent data from being destroyed through conflict detection and arbitration. The I2C devices in the BL series MCU have the following characteristics:

- Flexible configuration of slave address ``slaveAddr``, register address ``subAddr``
- Clock frequency that can be flexibly adjusted
- Support polling, interrupt, DMA transfer


I2C Device Structure Definition
----------------------------------

.. code-block:: C

    typedef struct i2c_device
    {
        struct device parent;
        uint8_t id;
        uint8_t mode;
        uint32_t phase;
    } i2c_device_t;

- **parent** inherit the properties of the parent class
- **ch** i2c id, 0 means i2c0, 1 means i2c1
- **mode** i2c transmission mode, 0 means using hardware i2c, 1 means using software i2c, current software i2c is temporarily invalid
- **phase** i2c clock phase div, i2c_clk = i2c_source_clk/(4*(phase+1))
- TODO

I2C Device Parameter Configuration Table
--------------------------------------------

Each I2C device has a parameter configuration macro, the macro definition is located in the ``peripheral_config.h`` file under the ``bsp/board/xxx`` directory, and the variable definition is located in ``hal_i2c.c``, so the user does not need to define variable. When the user opens the macro of the corresponding device, the configuration of the device will take effect. For example, open the macro ``BSP_USING_I2C0``, ``I2C0_CONFIG`` will take effect, and the ``I2C`` device can be registered and used.

.. code-block:: C

    /*Parameter configuration macro*/
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

    /*Variable definition*/
    static i2c_device_t i2cx_device[I2C_MAX_INDEX] =
    {
    #ifdef BSP_USING_I2C0
        I2C0_CONFIG,
    #endif
    };

.. note::
    The above configuration can be modified through ``I2C_DEV(dev)->xxx`` and can only be used before calling ``device_open``.

I2C Device Interface
------------------------

The I2C device standard interface currently only uses ``device_open``, and provides a standard data transceiver interface.

**i2c_register**
^^^^^^^^^^^^^^^^^^^^^^^^

``i2c_register`` is used to register an I2C device standard driver interface. The macro definition of the corresponding I2C device needs to be opened before registration. For example, define the macro ``BSP_USING_I2C0`` to use the ``I2C0`` device. After the registration is completed, other interfaces can be used. If the macro is not defined, the ``I2C0`` device cannot be used.

.. code-block:: C

    int i2c_register(enum i2c_index_type index, const char *name);

- **index** device index to be registered
- **name** device name to be registered

``index`` is used to select I2C device, one index corresponds to one I2C device configuration, for example, ``I2C0_INDEX`` corresponds to ``I2C0_CONFIG`` configuration, ``index`` has the following optional types

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

``device_open`` is used to open an i2c device, this funtion calls ``i2c_open`` actually.

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

**i2c_transfer**
^^^^^^^^^^^^^^^^

``i2c_transfer`` is used to transfer i2c msg. The member ``flags`` in ``i2c_msg_t`` structure indicates whether the direction of the transfer is writing or reading, and the length of the specified register address is 0, 1, 2.

.. code-block:: C

    int i2c_transfer(struct device *dev, i2c_msg_t msgs[], uint32_t num);

- **dev** device handle
- **msgs** message to be transmitted
- **num** the number of messages
- **return** error code, 0 means opening is successful, others mean error

``i2c_msg_t`` structure is defined as follows:

.. code-block:: C

    typedef struct i2c_msg
    {
        uint8_t slaveaddr;
        uint32_t subaddr;
        uint16_t flags;
        uint16_t len;
        uint8_t *buf;
    } i2c_msg_t;

- **slaveaddr** i2c slave device 7-bit slave address
- **subaddr** i2c slave device register address
- **flags** read and write mode and register address length
- **len** transmission data length
- **buf** data buffer

``flags`` definition is as follows:

.. code-block:: C

    /*Read and write mode*/
    #define I2C_WR 0x0000
    #define I2C_RD 0x0001

    /*Register address length*/
    #define SUB_ADDR_0BYTE 0x0010
    #define SUB_ADDR_1BYTE 0x0020
    #define SUB_ADDR_2BYTE 0x0040