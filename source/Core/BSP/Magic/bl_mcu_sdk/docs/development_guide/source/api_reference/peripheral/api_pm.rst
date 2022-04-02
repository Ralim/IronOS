PM 设备
=========================

简介
------------------------

PM 设备接口
------------------------

**pm_pds_mode_enter**
^^^^^^^^^^^^^^^^^^^^^^^^

``pm_pds_mode_enter`` 用来进入 pds 模式

.. code-block:: C

    void pm_pds_mode_enter(enum pm_pds_sleep_level pds_level, uint32_t sleep_time);

- **pds_level** PDS 等级
- **sleep_time** 睡眠时间，单位 1/32768 hz

其中 ``pds_level`` 提供以下类型：

.. code-block:: C

    enum pm_pds_sleep_level {
        PM_PDS_LEVEL_0,
        PM_PDS_LEVEL_1,
        PM_PDS_LEVEL_2,
        PM_PDS_LEVEL_3,
        PM_PDS_LEVEL_4, /*do not recommend to use*/
        PM_PDS_LEVEL_5, /*do not recommend to use*/
        PM_PDS_LEVEL_6, /*do not recommend to use*/
        PM_PDS_LEVEL_7, /*do not recommend to use*/
        PM_PDS_LEVEL_31 = 31,
    };

.. warning:: pds level 4~7 不推荐使用

**pm_hbn_mode_enter**
^^^^^^^^^^^^^^^^^^^^^^^^

``pm_hbn_mode_enter`` 用来进入 hbn 模式

.. code-block:: C

    void pm_hbn_mode_enter(enum pm_hbn_sleep_level hbn_level, uint8_t sleep_time);

- **hbn_level** HBN 等级
- **sleep_time** 睡眠时间，单位秒


其中 ``hbn_level`` 提供以下类型：

.. code-block:: C

    enum pm_hbn_sleep_level {
        PM_HBN_LEVEL_0,
        PM_HBN_LEVEL_1,
        PM_HBN_LEVEL_2,
    };

**pm_set_wakeup_callback**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``pm_set_wakeup_callback`` 用来设置从唤醒恢复时跳转的函数地址，hbn level1 以上不可用。

.. code-block:: C

    void pm_set_wakeup_callback(void (*wakeup_callback)(void));

- **wakeup_callback** 跳转的函数地址


**pm_hbn_out0_irq_register**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``pm_hbn_out0_irq_register`` 用来注册 hbn out0 中断，out0下中断有：GPIO9~GPIO12,RTC。中断回调函数为 ``pm_irq_callback``。

.. code-block:: C

    void pm_hbn_out0_irq_register(void);

**pm_hbn_out1_irq_register**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``pm_hbn_out1_irq_register`` 用来注册 hbn out1 中断，out1下中断有：ACOMP0、ACOMP1、BOR、PIR。中断回调函数为 ``pm_irq_callback``。

.. code-block:: C

    void pm_hbn_out1_irq_register(void);

``pm_irq_callback`` 中 ``event`` 提供以下类型：

.. code-block:: C

    enum pm_event_type {
        PM_HBN_GPIO9_WAKEUP_EVENT,
        PM_HBN_GPIO10_WAKEUP_EVENT,
        PM_HBN_GPIO11_WAKEUP_EVENT,
        PM_HBN_GPIO12_WAKEUP_EVENT,
        PM_HBN_RTC_WAKEUP_EVENT,
        PM_HBN_ACOMP0_WAKEUP_EVENT,
        PM_HBN_ACOMP1_WAKEUP_EVENT,
    };