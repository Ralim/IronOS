GPIO output - Lamp LED
=========================

This demo is based on the output mode of GPIO.

Hardware Connection
-----------------------------

This demo is based on BL706_IOT and the connection method is as follows

::

       GPIO function         GPIO pin
    ----------------------------------
        D0      <-->     GPIO22
        D1      <-->     GPIO29
        D2      <-->     GPIO30
        D3      <-->     GPIO31


.. figure:: img/blink_breath_sch.png
    :alt:

Software Implementation
-----------------------------

- See ``examples/gpio/gpio_blink`` for the software code

.. code-block:: C
    :linenos:

    gpio_set_mode(GPIO_PIN_22, GPIO_OUTPUT_PP_MODE);
    gpio_set_mode(GPIO_PIN_29, GPIO_OUTPUT_PP_MODE);
    gpio_set_mode(GPIO_PIN_30, GPIO_OUTPUT_PP_MODE);
    gpio_set_mode(GPIO_PIN_31, GPIO_OUTPUT_PP_MODE);

- Use the above code to configure ``GPIO22`` ``GPIO29`` ``GPIO30`` ``GPIO31`` to output pull-up mode.

.. code-block:: C
    :linenos:

    gpio_write(GPIO_PIN_22, 0);

- Use the above code to modify the output level value.

Compile and Program
-----------------------------

-  **CDK compilation**

   Open project:gpio_blink.cdkproj

   Refer to the steps of :ref:`windows_cdk_quick_start` to compile and download

-  **Command compilation**

.. code-block:: bash
   :linenos:

    $ cd <sdk_path>/bl_mcu_sdk
    $ make BOARD=bl706_iot APP=gpio_blink

-  **Program**

   See :ref:`bl_dev_cube`


Experimental Phenomena
-----------------------------

Video display:

.. raw:: html

    <iframe src="https://player.bilibili.com/player.html?aid=887712205&bvid=BV1xK4y1P7ur&cid=326226616&page=3" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true"> </iframe>
