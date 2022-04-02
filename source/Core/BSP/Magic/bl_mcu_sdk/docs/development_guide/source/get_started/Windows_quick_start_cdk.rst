.. _windows_cdk_quick_start:

Windows 下使用 CDK (类 MDK Keil)开发指南
=============================================

本文档介绍了如何在 Windows 下使用 `平头哥半导体 <https://www.t-heah.cn/about>`_ 开发的 CDK 集成开发环境，
来完成博流 BL702 系列 MCU 的相关软件开发工作。

关于剑池 CDK ，这是一款由平头哥半导体开发打造的集成开发环境。它拥有和传统 MCU 开发环境十分近似的操作和设置，旨在不改变用户开发习惯的基础上，全面接入云端开发资源，结合图形化的 OSTracer、Profiling 等调试分析工具，加速用户产品开发


需要的软硬件环境
-----------------------------

-  剑池 CDK 软件
-  一个 支持 risc-v 的 jtag 调试器
-  一个 USB-TTL 串口模块（如果调试器自带，忽略）


下载剑池 CDK 软件安装包
-----------------------------

- `剑池CDK <https://occ.t-head.cn/development/series/download?id=3864775351511420928&type=kind&softPlatformType=4#sticky>`_ 软件安装可以从平头哥半导体 OCC 官网获取到

- 下载完成后，解压缩，双击 ``setup.exe``，按照其提示，完成软件安装即可


下载 bl_mcu_sdk 软件开发包
----------------------------

-  从开源社区下载 bl_mcu_sdk 软件开发包。

   - 可以使用 ``git clone`` 或者直接 ``download`` 的方式下 SDK
   - 使用 ``git clone`` 前请确保已正确安装 ``git``，打开支持 ``git`` 的终端输入以下命令即可获取最新的 SDK

   .. code-block:: bash
      :linenos:
      :emphasize-lines: 1

      $ git clone https://gitee.com/bouffalolab/bl_mcu_sdk.git  --recursive


硬件连接
----------------------

-  具体开发板的连接, 请参考 :ref:`connect_hardware` 部分
-  请确保开发板被正确设置后再进行下面的步骤


测试 Hello World 工程
------------------------------

**使用 Sipeed RV-Debugger Plus 调试工程时请按照下面的步骤进行：**

.. toctree::
   :maxdepth: 1

   使用 Sipeed RV-Debugger <cdk_rv_debugger_plus>

**使用 CK-Link 调试工程时请按照下面的步骤进行：**

.. toctree::
   :maxdepth: 1

   使用 CK-Link <cdk_ck_link>


