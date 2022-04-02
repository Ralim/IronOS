Shell
=======================

Shell 是一个命令行解释器，Shell 为用户提供了与设备进行命令行交互的方式，用户通过串口、以太网、无线等方式将命令传输给具有 Shell 功能的设备，设备会解析命令并查找对应的函数进行执行。

当前 Shell 组件具有以下功能：

- 支持标准键盘字符控制
- 支持 TAB 键自动补全
- 支持上下键查看历史命令
- 支持左右键修改命令
- 变量导出
- 支持文件系统操作


Shell 接口
---------------

**shell_init**
^^^^^^^^^^^^^^^^^^^^^^^^

``shell_init`` 用来初始化 shell。

.. code-block:: C

    void shell_init(void);


**shell_handler**
^^^^^^^^^^^^^^^^^^^^^^^^

``shell_handler`` 用来对输入的数据进行处理。

.. code-block:: C

    void shell_handler(uint8_t data);

- **data** 接收的数据

**SHELL_CMD_EXPORT**
^^^^^^^^^^^^^^^^^^^^^^^^

``SHELL_CMD_EXPORT`` 用来注册一个命令。

.. c:macro:: SHELL_CMD_EXPORT(command, desc)

- **command** 注册的函数名，后面输入 command 来运行该函数
- **desc** 对该函数的描述

**SHELL_CMD_EXPORT_ALIAS**
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``SHELL_CMD_EXPORT_ALIAS`` 用来注册一个命令，并对命令取别名。

.. c:macro:: SHELL_CMD_EXPORT_ALIAS(command, alias, desc)

- **command** 注册的函数名
- **alias** 函数名的别名，后面输入 alias 来运行该函数
- **desc** 对该函数的描述

Shell 内置命令
-----------------

.. tip:: ``help`` 用来显示所有注册过的命令列表



