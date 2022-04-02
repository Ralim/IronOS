=================
EMAC 以太网应用
=================

BL70x 系列芯片拥有一个 EMAC 模块，该模块是一个兼容 `IEEE 802.3 <https://en.wikipedia.org/wiki/Ethernet_flow_control>`_ 的 10/100 Mbps 以太网 MAC。
EMAC 主要通过 RMII 接口连接到 PHY 芯片，以进行以太网通信。

在 BL706 上移植了 `LwIP <https://en.wikipedia.org/wiki/LwIP>`_ 网络协议栈， LwIP 是一个专为嵌入式系统设计的被广泛使用的开源的轻量级 TCP/IP 协议栈。
LwIP 协议栈可以在裸机中或在 RTOS 操作系统中运行，目前下列例程中使用 LwIP 时都是运行在 FreeRTOS 实时操作系统上的。

以下例程都是通过 RMII 接口连接到 PHY 8720 芯片进行通信的。


.. toctree::
    :maxdepth: 1

    ARP - 发送 ARP 裸包<eth_basic>
    UDP - LwIP UDP echo<eth_udp_echo>
    TCP - LwIP TCP echo<eth_tcp_echo>
    HTTP - LwIP HTTP server<eth_http_server>
    IPERF - LwIP iperf<eth_lwip_iperf>