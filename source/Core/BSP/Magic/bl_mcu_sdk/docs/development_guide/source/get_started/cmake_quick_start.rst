基于 cmake 框架新建工程指南
================================

本文档将介绍一下如何基于本 SDK 新建一个全新的项目

examples 的目录结构
------------------------

在 ``bl_mcu_sdk/examples`` 目录下通常存在两层子目录，第一层通常为同一类外设相关的 case 集，一般使用外设名称，第二层通常为该外设具体的某一种测试例程，该目录下通常还包含一个 ``CMakeList.txt`` 以及该 case 相关的源码。

可供用户使用的 cmake 变量如下：

- mains:main 函数入口
- TARGET_REQUIRED_SRCS:没有编译成 lib 的外部 c 文件
- TARGET_REQUIRED_PRIVATE_INCLUDE:外部 c 文件需要的头文件路径
- TARGET_REQUIRED_LIBS:目标需要的 lib 库名称，可以是 cmake 通过 generate_bin 生成的 target 或者外部导入的 lib 库
- TARGET_REQUIRED_PRIVATE_OPTIONS:私有 c flag
- LINKER_SCRIPT:重新定义链接脚本
- GLOBAL_C_FLAGS:全局 c flag
- GLOBAL_LD_FLAGS:全局 ld flag

添加单个源文件工程
------------------------

.. note:: 该源文件必须包含 c 程序入口，通常为 ``main`` 函数，源文件可以不叫 ``main.c``

-  在 ``examples`` 目录下新建一个 ``my_case`` 目录，用于存放你的 case 集
-  在 ``my_case`` 目录中新建你要测试 case 的名称，比如新建一个 ``gpio_case`` 目录
-  然后在 ``gpio_case`` 目录中添加 ``main.c`` 和 ``CMakeLists.txt`` 文件即可

目录结构如下：

.. code-block:: bash
   :linenos:

   bl_mcu_sdk
   ├── examples
       ├── my_case
           ├── gpio_case
           │   ├──CMakeLists.txt
           │   └──main.c
           └── xxxx_case

`CMakeLists.txt`:

.. code-block:: cmake
   :linenos:
   :emphasize-lines: 1-2

   set(mains main.c)
   generate_bin()

-  在 main.c 中编写好相应的功能代码后，在 bl_mcu_sdk 目录下进行编译即可，编译命令如下：

.. code-block:: bash
   :linenos:

   make BOARD=bl706_iot APP=gpio_case


添加多个源文件工程
-----------------------

.. note:: 其中一个源文件必须包含 c 程序入口，通常为 ``main`` 函数，该源文件可以不叫 ``main.c``。该源文件依赖其他源文件。

-  添加多源文件工程的步骤与添加单个源文件工程基本步骤一致
-  在 ``gpio_case`` 目录中添加 ``test1.c``、``test1.h``、``test2.c``、``test2.h`` 等需要依赖的源文件和头文件即可

目录结构如下：

.. code-block:: bash
   :linenos:

   bl_mcu_sdk
   ├── examples
       ├── my_case
           ├── gpio_case
           │   ├──CMakeLists.txt
           │   ├──test1.c
           │   ├──test1.h
           │   ├──test2.c
           │   ├──test2.h
           │   └──main.c
           └── xxxx_case

-  那么此时 ``CMakeLists.txt`` 文件需要添加相应的依赖源文件，内容如下所示：

`CMakeLists.txt`:

.. code-block:: cmake
   :linenos:
   :emphasize-lines: 2

   set(mains main.c)
   set(TARGET_REQUIRED_SRCS test1.c test2.c)
   generate_bin()

-  编写好相应的功能代码后，在 bl_mcu_sdk 目录下进行编译即可，编译命令如下：

.. code-block:: bash
   :linenos:

   make BOARD=bl706_iot APP=gpio_case


添加有依赖库的新工程
------------------------

-  添加有依赖库的新工程的步骤与添加单个源文件工程基本步骤一致
-  如使用的依赖库在本 SDK 中已存在，那么就只需要修改 CMakeLists.txt 即可

   -  如使用的依赖库不存在则需要自行添加，详见后续说明

已存在的情况下，目录结构如下：

.. code-block:: bash
   :linenos:

   bl_mcu_sdk
   ├── examples
       ├── my_case
           ├── gpio_case
           │   ├──CMakeLists.txt
           │   └──main.c
           └── xxxx_case

-  那么此时 ``CMakeLists.txt`` 文件需要添加相应的依赖库文件即可，比如我们添加一个 FreeRTOS 组件库，内容如下所示：

`CMakeLists.txt`:

.. code-block:: cmake
   :linenos:
   :emphasize-lines: 1

   set(TARGET_REQUIRED_LIBS freertos)
   set(mains main.c)
   generate_bin()

-  编写好相应的功能代码后，在 bl_mcu_sdk 目录下进行编译即可，编译命令如下：

.. code-block:: bash
   :linenos:

   make BOARD=bl706_iot APP=gpio_case

添加新工程并设置私有编译选项（gcc option）
-------------------------------------------

-  添加新工程步骤与添加单源文件工程基本一致
-  主要是在修改 CMakeLists.txt 文件，添加私有编译选项

`CMakeLists.txt`:

.. code-block:: cmake
   :linenos:
   :emphasize-lines: 2

   set(mains main.c)
   set(TARGET_REQUIRED_PRIVATE_OPTIONS -Ofast)
   generate_bin()

-  编写好相应的功能代码后，在 bl_mcu_sdk 目录下进行编译即可，编译命令如下：

.. code-block:: bash
   :linenos:

   make BOARD=bl706_iot APP=gpio_case


添加新工程并设置私有链接脚本（ld）
------------------------------------

-  添加新工程步骤与添加单源文件工程基本一致
-  添加私有链接脚本文件，如 ``gpio_test_case.ld``

目录结构如下：

.. code-block:: bash
   :linenos:

   bl_mcu_sdk
   ├── examples
       ├── my_case
           ├── gpio_case
           │   ├──CMakeLists.txt
           │   ├──gpio_test_case.ld
           │   └──main.c
           └── xxxx_case

-  修改 CMakeLists.txt 文件，添加私有链接脚本设置

`CMakeLists.txt`:

.. code-block:: cmake
   :linenos:
   :emphasize-lines: 1

   set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/gpio_test_case.ld)
   set(mains main.c)
   generate_bin()

-  编写好相应的功能代码后，在 bl_mcu_sdk 目录下进行编译即可，编译命令如下：

.. code-block:: bash
   :linenos:

   make BOARD=bl706_iot APP=gpio_case


添加新工程并其依赖的源文件和库文件
-------------------------------------


