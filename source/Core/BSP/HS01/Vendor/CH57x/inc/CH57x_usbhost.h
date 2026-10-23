/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_usbhost.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef __CH57x_USBHOST_H__
#define __CH57x_USBHOST_H__

#ifdef __cplusplus
extern "C" {
#endif

#if DISK_LIB_ENABLE
#if DISK_WITHOUT_USB_HUB
/***************************************** 不使用U盘文件系统库或者U盘挂载USBhub下面，需要关闭下面定义 */
#define FOR_ROOT_UDISK_ONLY
#endif
/***************************************** 使用U盘文件系统库，需要开启下面定义, 不使用请关闭 */
#define DISK_BASE_BUF_LEN 512 /* 默认的磁盘数据缓冲区大小为512字节,建议选择为2048甚至4096以支持某些大扇区的U盘,为0则禁止在.H文件中定义缓冲区并由应用程序在pDISK_BASE_BUF中指定 */
#endif

// 各子程序返回状态码
#define ERR_SUCCESS       0x00 // 操作成功
#define ERR_USB_CONNECT   0x15 /* 检测到USB设备连接事件,已经连接 */
#define ERR_USB_DISCON    0x16 /* 检测到USB设备断开事件,已经断开 */
#define ERR_USB_BUF_OVER  0x17 /* USB传输的数据有误或者数据太多缓冲区溢出 */
#define ERR_USB_DISK_ERR  0x1F /* USB存储器操作失败,在初始化时可能是USB存储器不支持,在读写操作中可能是磁盘损坏或者已经断开 */
#define ERR_USB_TRANSFER  0x20 /* NAK/STALL等更多错误码在0x20~0x2F */
#define ERR_USB_UNSUPPORT 0xFB /*不支持的USB设备*/
#define ERR_USB_UNKNOWN   0xFE /*设备操作出错*/
#define ERR_AOA_PROTOCOL  0x41 /*协议版本出错 */

/*USB设备相关信息表,最多支持1个设备*/
#define ROOT_DEV_DISCONNECT 0
#define ROOT_DEV_CONNECTED  1
#define ROOT_DEV_FAILED     2
#define ROOT_DEV_SUCCESS    3
#define DEV_TYPE_KEYBOARD   (USB_DEV_CLASS_HID | 0x20)
#define DEV_TYPE_MOUSE      (USB_DEV_CLASS_HID | 0x30)
#define DEF_AOA_DEVICE      0xF0
#define DEV_TYPE_UNKNOW     0xFF

/*
约定: USB设备地址分配规则(参考USB_DEVICE_ADDR)
地址值  设备位置
0x02    内置Root-HUB下的USB设备或外部HUB
0x1x    内置Root-HUB下的外部HUB的端口x下的USB设备,x为1~n
*/
#define HUB_MAX_PORTS       4
#define WAIT_USB_TOUT_200US 800 // 等待USB中断超时时间

typedef struct {
  uint8_t  DeviceStatus;  // 设备状态,0-无设备,1-有设备但尚未初始化,2-有设备但初始化枚举失败,3-有设备且初始化枚举成功
  uint8_t  DeviceAddress; // 设备被分配的USB地址
  uint8_t  DeviceSpeed;   // 0为低速,非0为全速
  uint8_t  DeviceType;    // 设备类型
  uint16_t DeviceVID;
  uint16_t DevicePID;
  uint8_t  GpVar[4];     // 通用变量，存放端点
  uint8_t  GpHUBPortNum; // 通用变量,如果是HUB，表示HUB端口数
} _RootHubDev;

typedef struct {
  UINT8  DeviceStatus;  // 设备状态,0-无设备,1-有设备但尚未初始化,2-有设备但初始化枚举失败,3-有设备且初始化枚举成功
  UINT8  DeviceAddress; // 设备被分配的USB地址
  UINT8  DeviceSpeed;   // 0为低速,非0为全速
  UINT8  DeviceType;    // 设备类型
  UINT16 DeviceVID;
  UINT16 DevicePID;
  UINT8  GpVar[4]; // 通用变量
} _DevOnHubPort;   // 假定:不超过1个外部HUB,每个外部HUB不超过HUB_MAX_PORTS个端口(多了不管)

extern _RootHubDev   ThisUsbDev;
extern _DevOnHubPort DevOnHubPort[HUB_MAX_PORTS]; // 假定:不超过1个外部HUB,每个外部HUB不超过HUB_MAX_PORTS个端口(多了不管)
extern uint8_t       UsbDevEndp0Size;             // USB设备的端点0的最大包尺寸 */
extern uint8_t       FoundNewDev;

extern uint8_t *pHOST_RX_RAM_Addr;
extern uint8_t *pHOST_TX_RAM_Addr;
#define pSetupReq ((PUSB_SETUP_REQ)pHOST_TX_RAM_Addr)
extern uint8_t Com_Buffer[];

/* 以下为USB主机请求包 */
extern const uint8_t SetupGetDevDescr[];     // 获取设备描述符*/
extern const uint8_t SetupGetCfgDescr[];     // 获取配置描述符*/
extern const uint8_t SetupSetUsbAddr[];      // 设置USB地址*/
extern const uint8_t SetupSetUsbConfig[];    // 设置USB配置*/
extern const uint8_t SetupSetUsbInterface[]; // 设置USB接口配置*/
extern const uint8_t SetupClrEndpStall[];    // 清除端点STALL*/

/**
 * @brief   关闭ROOT-HUB端口,实际上硬件已经自动关闭,此处只是清除一些结构状态
 */
void DisableRootHubPort(void);

/**
 * @brief   分析ROOT-HUB状态,处理ROOT-HUB端口的设备插拔事件
 *          如果设备拔出,函数中调用DisableRootHubPort()函数,将端口关闭,插入事件,置相应端口的状态位
 *
 * @return  返回ERR_SUCCESS为没有情况,返回ERR_USB_CONNECT为检测到新连接,返回ERR_USB_DISCON为检测到断开
 */
uint8_t AnalyzeRootHub(void);

/**
 * @brief   设置USB主机当前操作的USB设备地址
 *
 * @param   addr    - USB设备地址
 */
void SetHostUsbAddr(uint8_t addr);

/**
 * @brief   设置当前USB速度
 *
 * @param   FullSpeed   - USB速度
 */
void SetUsbSpeed(uint8_t FullSpeed);

/**
 * @brief   检测到设备后,复位总线,为枚举设备准备,设置为默认为全速
 */
void ResetRootHubPort(void);

/**
 * @brief   使能ROOT-HUB端口,相应的bUH_PORT_EN置1开启端口,设备断开可能导致返回失败
 *
 * @return  返回ERR_SUCCESS为检测到新连接,返回ERR_USB_DISCON为无连接
 */
uint8_t EnableRootHubPort(void);

/**
 * @brief   等待USB中断
 *
 * @return  返回ERR_SUCCESS 数据接收或者发送成功,返回ERR_USB_UNKNOWN 数据接收或者发送失败
 */
uint8_t WaitUSB_Interrupt(void);

/**
 * @brief   传输事务,输入目的端点地址/PID令牌,同步标志,以20uS为单位的NAK重试总时间(0则不重试,0xFFFF无限重试),返回0成功,超时/出错重试
 *          本子程序着重于易理解,而在实际应用中,为了提供运行速度,应该对本子程序代码进行优化
 *
 * @param   endp_pid    - 令牌和地址, 高4位是token_pid令牌, 低4位是端点地址
 * @param   tog         - 同步标志
 * @param   timeout     - 超时时间
 *
 * @return  ERR_USB_UNKNOWN 超时，可能硬件异常
 *          ERR_USB_DISCON  设备断开
 *          ERR_USB_CONNECT 设备连接
 *          ERR_SUCCESS     传输完成
 */
uint8_t USBHostTransact(uint8_t endp_pid, uint8_t tog, uint32_t timeout);

/**
 * @brief   执行控制传输,8字节请求码在pSetupReq中,DataBuf为可选的收发缓冲区
 *
 * @param   DataBuf     - 如果需要接收和发送数据,那么DataBuf需指向有效缓冲区用于存放后续数据
 * @param   RetLen      - 实际成功收发的总长度保存在RetLen指向的字节变量中
 *
 * @return  ERR_USB_BUF_OVER    IN状态阶段出错
 *          ERR_SUCCESS         数据交换成功
 */
uint8_t HostCtrlTransfer(uint8_t *DataBuf, uint8_t *RetLen);

/**
 * @brief   复制控制传输的请求包
 *
 * @param   pReqPkt     - 控制请求包地址
 */
void CopySetupReqPkg(const uint8_t *pReqPkt);

/**
 * @brief   获取设备描述符,返回在 pHOST_TX_RAM_Addr 中
 *
 * @return  ERR_USB_BUF_OVER    描述符长度错误
 *          ERR_SUCCESS         成功
 */
uint8_t CtrlGetDeviceDescr(void);

/**
 * @brief   获取配置描述符,返回在 pHOST_TX_RAM_Addr 中
 *
 * @return  ERR_USB_BUF_OVER    描述符长度错误
 *          ERR_SUCCESS         成功
 */
uint8_t CtrlGetConfigDescr(void);

/**
 * @brief   设置USB设备地址
 *
 * @param   addr    - 设备地址
 *
 * @return  ERR_SUCCESS     成功
 */
uint8_t CtrlSetUsbAddress(uint8_t addr);

/**
 * @brief   设置USB设备配置
 *
 * @param   cfg     - 配置值
 *
 * @return  ERR_SUCCESS     成功
 */
uint8_t CtrlSetUsbConfig(uint8_t cfg);

/**
 * @brief   清除端点STALL
 *
 * @param   endp    - 端点地址
 *
 * @return  ERR_SUCCESS     成功
 */
uint8_t CtrlClearEndpStall(uint8_t endp);

/**
 * @brief   设置USB设备接口
 *
 * @param   cfg     - 配置值
 *
 * @return  ERR_SUCCESS     成功
 */
uint8_t CtrlSetUsbIntercace(uint8_t cfg);

/**
 * @brief   USB主机功能初始化
 */
void     USB_HostInit(void);
uint8_t  EnumAllHubPort(void);                // 枚举所有ROOT-HUB端口下外部HUB后的二级USB设备
void     SelectHubPort(uint8_t HubPortIndex); // HubPortIndex=0选择操作指定的ROOT-HUB端口,否则选择操作指定的ROOT-HUB端口的外部HUB的指定端口
uint16_t SearchTypeDevice(uint8_t type);      // 在ROOT-HUB以及外部HUB各端口上搜索指定类型的设备所在的端口号,输出端口号为0xFFFF则未搜索到.
uint8_t  SETorOFFNumLock(uint8_t *buf);       // NumLock的点灯判断

/**
 * @brief   初始化指定ROOT-HUB端口的USB设备
 *
 * @return  错误码
 */
uint8_t InitRootDevice(void);

/**
 * @brief   获取HID设备报表描述符,返回在TxBuffer中
 *
 * @return  错误码
 */
uint8_t CtrlGetHIDDeviceReport(uint8_t infc);

/**
 * @brief   获取HUB描述符,返回在Com_Buffer中
 *
 * @return  错误码
 */
uint8_t CtrlGetHubDescr(void);

/**
 * @brief   查询HUB端口状态,返回在Com_Buffer中
 *
 * @param   HubPortIndex    - 端口号
 *
 * @return  错误码
 */
uint8_t HubGetPortStatus(uint8_t HubPortIndex);

/**
 * @brief   设置HUB端口特性
 *
 * @param   HubPortIndex    - 端口号
 * @param   FeatureSelt     - 端口特性
 *
 * @return  错误码
 */
uint8_t HubSetPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt);

/**
 * @brief   清除HUB端口特性
 *
 * @param   HubPortIndex    - 端口号
 * @param   FeatureSelt     - 端口特性
 *
 * @return  错误码
 */
uint8_t HubClearPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt);

#ifdef __cplusplus
}
#endif

#endif // __CH57x_USBHOST_H__
