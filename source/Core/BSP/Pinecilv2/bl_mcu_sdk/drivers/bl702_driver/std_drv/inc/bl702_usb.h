/**
  ******************************************************************************
  * @file    bl702_usb.h
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef __BL702_USB_H__
#define __BL702_USB_H__

#include "usb_reg.h"
#include "bl702_common.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  USB
 *  @{
 */

/** @defgroup  USB_Public_Types
 *  @{
 */

/**
 *  @brief USB end point ID
 */
typedef enum {
    EP_ID0 = 0, /*!< USB end point 0 */
    EP_ID1,     /*!< USB end point 1 */
    EP_ID2,     /*!< USB end point 2 */
    EP_ID3,     /*!< USB end point 3 */
    EP_ID4,     /*!< USB end point 4 */
    EP_ID5,     /*!< USB end point 5 */
    EP_ID6,     /*!< USB end point 6 */
    EP_ID7,     /*!< USB end point 7 */
} USB_EP_ID;

/**
 *  @brief USB end point type
 */
typedef enum {
    EP_INT = 0,  /*!< interrupt transfer ep */
    EP_ISO = 2,  /*!< isochronous transfer ep */
    EP_BULK = 4, /*!< bulk transfer ep */
    EP_CTRL = 5, /*!< control transfer ep */
} EP_XFER_Type;

/**
 *  @brief USB end point transfer directions
 */
typedef enum {
    EP_DISABLED = 0, /*!< end point disabled */
    EP_IN = 1,       /*!< IN end point,device to host */
    EP_OUT = 2,      /*!< OUT end point ,host to device */
} EP_XFER_DIR;

typedef enum {
    USB_INT_SOF = 0,
    USB_INT_RESET,
    USB_INT_VBUS_TGL,
    USB_INT_GET_DCT_CMD,
    USB_INT_EP0_SETUP_CMD,
    USB_INT_EP0_SETUP_DONE,
    USB_INT_EP0_IN_CMD,
    USB_INT_EP0_IN_DONE,
    USB_INT_EP0_OUT_CMD,
    USB_INT_EP0_OUT_DONE,
    USB_INT_EP1_CMD,
    USB_INT_EP1_DONE,
    USB_INT_EP2_CMD,
    USB_INT_EP2_DONE,
    USB_INT_EP3_CMD,
    USB_INT_EP3_DONE,
    USB_INT_EP4_CMD,
    USB_INT_EP4_DONE,
    USB_INT_EP5_CMD,
    USB_INT_EP5_DONE,
    USB_INT_EP6_CMD,
    USB_INT_EP6_DONE,
    USB_INT_EP7_CMD,
    USB_INT_EP7_DONE,
    USB_INT_RESET_END = 27,
    USB_INT_LPM_WAKEUP = 28,
    USB_INT_LPM_PACKET = 29,
    USB_INT_LOST_SOF_3_TIMES = 30,
    USB_INT_ERROR = 31,
    USB_INT_ALL = 32, /* special */
} USB_INT_Type;

typedef enum {
    USB_FIFO_EMPTY = 0,
    USB_FIFO_FULL,
} USB_FIFO_STATUS_Type;

typedef enum {
    USB_FIFO_ERROR_OVERFLOW = 0,
    USB_FIFO_ERROR_UNDERFLOW,
} USB_FIFO_ERROR_FLAG_Type;

typedef enum {
    USB_EP_STATUS_ACK = 0,
    USB_EP_STATUS_NACK,
    USB_EP_STATUS_STALL,
    USB_EP_STATUS_NSTALL,
} USB_EP_STATUS_Type;

typedef enum {
    USB_LPM_DEFAULT_RESP_ACK = 0,
    USB_LPM_DEFAULT_RESP_NACK,
    USB_LPM_DEFAULT_RESP_STALL,
    USB_LPM_DEFAULT_RESP_NYET,
} USB_LPM_DEFAULT_RESP_Type;

typedef enum {
    USB_ERROR_UTMI_RX = 0,
    USB_ERROR_XFER_TO,
    USB_ERROR_IVLD_EP,
    USB_ERROR_PID_SEQ,
    USB_ERROR_PID_CKS,
    USB_ERROR_CRC5,
    USB_ERROR_CRC16,
} USB_ERROR_Type;

/**
 *  @brief USB configuration structure type definition
 */
typedef struct
{
    BL_Fun_Type EnumOutEn;             /*!< EP0 IN direction enable or disable */
    BL_Fun_Type EnumInEn;              /*!< EP0 OUT direction enable or disable */
    uint8_t EnumMaxPacketSize;         /*!< EP0 max packet size, only valid when SoftwareCtrl is set */
    uint8_t DeviceAddress;             /*!< Device(EP0) address, only valid when SoftwareCtrl is set */
    BL_Fun_Type SoftwareCtrl;          /*!< EP0 software control enable */
    BL_Fun_Type RomBaseDescriptorUsed; /*!< Enable signal of ROM-based descriptors (don't care if SoftwareCtrl is set) */
} USB_Config_Type;

/**
 *  @brief end point configuration structure type definition
 */
typedef struct
{
    uint16_t EPMaxPacketSize; /*!< Endpoint max packet size */
    EP_XFER_DIR dir;          /*!< Endpoint direction */
    EP_XFER_Type type;        /*!< Endpoint type */
} EP_Config_Type;

typedef struct
{
    uint32_t cr_ep_size     : 9;
    uint32_t cr_ep_dir      : 2;
    uint32_t cr_ep_type     : 3;
    uint32_t cr_ep_stall    : 1;
    uint32_t cr_ep_nack     : 1;
    uint32_t cr_ep_rdy      : 1;
    uint32_t reserved_17_31 : 15;
} usb_reg_epx_config_t;

typedef struct
{
    uint32_t ep_dma_tx_en         : 1;
    uint32_t ep_dma_rx_en         : 1;
    uint32_t ep_tx_fifo_clr       : 1;
    uint32_t ep_rx_fifo_clr       : 1;
    uint32_t ep_tx_fifo_overflow  : 1;
    uint32_t ep_tx_fifo_underflow : 1;
    uint32_t ep_rx_fifo_overflow  : 1;
    uint32_t ep_rx_fifo_underflow : 1;
    uint32_t reserved_8_31        : 24;
} usb_reg_epx_fifo_config_t;

typedef struct
{
    uint32_t ep_tx_fifo_cnt   : 7;
    uint32_t reserved_7_13    : 7;
    uint32_t ep_tx_fifo_empty : 1;
    uint32_t ep_tx_fifo_full  : 1;
    uint32_t ep_rx_fifo_cnt   : 7;
    uint32_t reserved_23_29   : 7;
    uint32_t ep_rx_fifo_empty : 1;
    uint32_t ep_rx_fifo_full  : 1;
} usb_reg_epx_fifo_status_t;

typedef struct
{
    uint32_t fifo          : 8;
    uint32_t reserved_8_31 : 24;
} usb_reg_epx_fifo_t;

/*@} end of group USB_Public_Types */

/** @defgroup  USB_Public_Constants
 *  @{
 */

/** @defgroup  USB_EP_ID
 *  @{
 */
#define IS_USB_EP_ID(type) (((type) == EP_ID0) || \
                            ((type) == EP_ID1) || \
                            ((type) == EP_ID2) || \
                            ((type) == EP_ID3) || \
                            ((type) == EP_ID4) || \
                            ((type) == EP_ID5) || \
                            ((type) == EP_ID6) || \
                            ((type) == EP_ID7))

/** @defgroup  EP_XFER_TYPE
 *  @{
 */
#define IS_EP_XFER_TYPE(type) (((type) == EP_INT) ||  \
                               ((type) == EP_ISO) ||  \
                               ((type) == EP_CTRL) || \
                               ((type) == EP_BULK))

/** @defgroup  EP_XFER_DIR
 *  @{
 */
#define IS_EP_XFER_DIR(type) (((type) == EP_DISABLED) || \
                              ((type) == EP_IN) ||       \
                              ((type) == EP_OUT))

/*@} end of group USB_Public_Constants */

/** @defgroup  USB_Public_Macros
 *  @{
 */
#define USB_INT_TYPE_SOF              0x00000001
#define USB_INT_TYPE_RESET            0x00000002
#define USB_INT_TYPE_GET_DCT_CMD      0x00000008
#define USB_INT_TYPE_EP0_SETUP_CMD    0x00000010
#define USB_INT_TYPE_EP0_SETUP_DONE   0x00000020
#define USB_INT_TYPE_EP0_IN_CMD       0x00000040
#define USB_INT_TYPE_EP0_IN_DONE      0x00000080
#define USB_INT_TYPE_EP0_OUT_CMD      0x00000100
#define USB_INT_TYPE_EP0_OUT_DONE     0x00000200
#define USB_INT_TYPE_EP1_CMD          0x00000400
#define USB_INT_TYPE_EP1_DONE         0x00000800
#define USB_INT_TYPE_EP2_CMD          0x00001000
#define USB_INT_TYPE_EP2_DONE         0x00002000
#define USB_INT_TYPE_EP3_CMD          0x00004000
#define USB_INT_TYPE_EP3_DONE         0x00008000
#define USB_INT_TYPE_EP4_CMD          0x00010000
#define USB_INT_TYPE_EP4_DONE         0x00020000
#define USB_INT_TYPE_EP5_CMD          0x00040000
#define USB_INT_TYPE_EP5_DONE         0x00080000
#define USB_INT_TYPE_EP6_CMD          0x00100000
#define USB_INT_TYPE_EP6_DONE         0x00200000
#define USB_INT_TYPE_EP7_CMD          0x00400000
#define USB_INT_TYPE_EP7_DONE         0x00800000
#define USB_INT_TYPE_RESET_END        0x08000000
#define USB_INT_TYPE_LPM_WAKEUP       0x10000000
#define USB_INT_TYPE_LPM_PACKET       0x20000000
#define USB_INT_TYPE_LOST_SOF_3_TIMES 0x40000000
#define USB_INT_TYPE_ERROR            0x80000000
#define USB_INT_TYPE_ALL              (USB_INT_TYPE_SOF | \
                          USB_INT_TYPE_RESET |            \
                          USB_INT_TYPE_GET_DCT_CMD |      \
                          USB_INT_TYPE_EP0_SETUP_CMD |    \
                          USB_INT_TYPE_EP0_SETUP_DONE |   \
                          USB_INT_TYPE_EP0_IN_CMD |       \
                          USB_INT_TYPE_EP0_IN_DONE |      \
                          USB_INT_TYPE_EP0_OUT_CMD |      \
                          USB_INT_TYPE_EP0_OUT_DONE |     \
                          USB_INT_TYPE_EP1_CMD |          \
                          USB_INT_TYPE_EP1_DONE |         \
                          USB_INT_TYPE_EP2_CMD |          \
                          USB_INT_TYPE_EP2_DONE |         \
                          USB_INT_TYPE_EP3_CMD |          \
                          USB_INT_TYPE_EP3_DONE |         \
                          USB_INT_TYPE_EP4_CMD |          \
                          USB_INT_TYPE_EP4_DONE |         \
                          USB_INT_TYPE_EP5_CMD |          \
                          USB_INT_TYPE_EP5_DONE |         \
                          USB_INT_TYPE_EP6_CMD |          \
                          USB_INT_TYPE_EP6_DONE |         \
                          USB_INT_TYPE_EP7_CMD |          \
                          USB_INT_TYPE_EP7_DONE |         \
                          USB_INT_TYPE_RESET_END |        \
                          USB_INT_TYPE_LPM_WAKEUP |       \
                          USB_INT_TYPE_LPM_PACKET |       \
                          USB_INT_TYPE_LOST_SOF_3_TIMES | \
                          USB_INT_TYPE_ERROR)

#define USB_EP_TX_FIFO_EMPTY    0x00004000
#define USB_EP_TX_FIFO_FULL     0x00008000
#define USB_EP_RX_FIFO_EMPTY    0x40000000
#define USB_EP_RX_FIFO_FULL     0x80000000
#define USB_EP_FIFO_STATUS_MASK (USB_EP_TX_FIFO_EMPTY | \
                                 USB_EP_TX_FIFO_FULL |  \
                                 USB_EP_RX_FIFO_EMPTY | \
                                 USB_EP_RX_FIFO_FULL)
#define USB_EP_TX_FIFO_OVERFLOW  0x00000010
#define USB_EP_TX_FIFO_UNDERFLOW 0x00000020
#define USB_EP_RX_FIFO_OVERFLOW  0x00000040
#define USB_EP_RX_FIFO_UNDERLOW  0x00000080
#define USB_EP_FIFO_ERROR_MASK   (USB_EP_TX_FIFO_OVERFLOW | \
                                USB_EP_TX_FIFO_UNDERFLOW |  \
                                USB_EP_RX_FIFO_OVERFLOW |   \
                                USB_EP_RX_FIFO_UNDERLOW)

#define EP_ID_MAX 8

/*@} end of group USB_Public_Macros */

/** @defgroup  USB_Public_Functions
 *  @{
 */

/**
 *  @brief USB Functions
 */

/*----------*/
#ifndef BFLB_USE_HAL_DRIVER
void USB_IRQHandler(void);
#endif
/*----------*/
BL_Err_Type USB_Enable(void);
BL_Err_Type USB_Disable(void);
BL_Err_Type USB_Set_Config(BL_Fun_Type enable, USB_Config_Type *usbCfg);
BL_Err_Type USB_Set_Device_Addr(uint8_t addr);
uint8_t USB_Get_Device_Addr(void);
/*----------*/
BL_Err_Type USB_Set_EPx_Xfer_Size(USB_EP_ID epId, uint8_t size);
BL_Err_Type USB_Set_EPx_IN_Busy(USB_EP_ID epId);
BL_Err_Type USB_Set_EPx_IN_Stall(USB_EP_ID epId);
BL_Err_Type USB_Set_EPx_OUT_Busy(USB_EP_ID epId);
BL_Err_Type USB_Set_EPx_OUT_Stall(USB_EP_ID epId);
BL_Err_Type USB_Set_EPx_Rdy(USB_EP_ID epId);
BL_Sts_Type USB_Is_EPx_RDY_Free(USB_EP_ID epId);
BL_Err_Type USB_Set_EPx_STALL(USB_EP_ID epId);
BL_Err_Type USB_Clr_EPx_STALL(USB_EP_ID epId);
BL_Err_Type USB_Set_EPx_Busy(USB_EP_ID epId);
BL_Err_Type USB_Set_EPx_Status(USB_EP_ID epId, USB_EP_STATUS_Type sts);
USB_EP_STATUS_Type USB_Get_EPx_Status(USB_EP_ID epId);
/*----------*/
BL_Err_Type USB_IntEn(USB_INT_Type intType, uint8_t enable);
BL_Err_Type USB_IntMask(USB_INT_Type intType, BL_Mask_Type intMask);
BL_Sts_Type USB_Get_IntStatus(USB_INT_Type intType);
BL_Err_Type USB_Clr_IntStatus(USB_INT_Type intType);
BL_Err_Type USB_Clr_EPx_IntStatus(USB_EP_ID epId);
/*----------*/
uint16_t USB_Get_Frame_Num(void);
/*----------*/
BL_Err_Type USB_Set_EPx_Config(USB_EP_ID epId, EP_Config_Type *epCfg);
BL_Err_Type USB_Set_EPx_Type(USB_EP_ID epId, EP_XFER_Type type);
EP_XFER_Type USB_Get_EPx_Type(USB_EP_ID epId);
BL_Err_Type USB_Set_EPx_Dir(USB_EP_ID epId, EP_XFER_DIR dir);
EP_XFER_DIR USB_Get_EPx_Dir(USB_EP_ID epId);
BL_Err_Type USB_Set_EPx_Size(USB_EP_ID epId, uint32_t size);
/*----------*/
BL_Sts_Type USB_Get_EPx_TX_FIFO_Errors(USB_EP_ID epId, USB_FIFO_ERROR_FLAG_Type errFlag);
BL_Sts_Type USB_Get_EPx_RX_FIFO_Errors(USB_EP_ID epId, USB_FIFO_ERROR_FLAG_Type errFlag);
BL_Err_Type USB_Clr_EPx_TX_FIFO_Errors(USB_EP_ID epId);
BL_Err_Type USB_Clr_EPx_RX_FIFO_Errors(USB_EP_ID epId);
/*----------*/
BL_Err_Type USB_EPx_Write_Data_To_FIFO(USB_EP_ID epId, uint8_t *pData, uint16_t len);
BL_Err_Type USB_EPx_Read_Data_From_FIFO(USB_EP_ID epId, uint8_t *pBuff, uint16_t len);
/*----------*/
BL_Err_Type USB_Set_EPx_TX_DMA_Interface_Config(USB_EP_ID epId, BL_Fun_Type newState);
BL_Err_Type USB_Set_EPx_RX_DMA_Interface_Config(USB_EP_ID epId, BL_Fun_Type newState);
BL_Err_Type USB_EPx_Write_Data_To_FIFO_DMA(USB_EP_ID epId, uint8_t *pData, uint16_t len);
BL_Err_Type USB_EPx_Read_Data_From_FIFO_DMA(USB_EP_ID epId, uint8_t *pBuff, uint16_t len);
/*----------*/
uint16_t USB_Get_EPx_TX_FIFO_CNT(USB_EP_ID epId);
uint16_t USB_Get_EPx_RX_FIFO_CNT(USB_EP_ID epId);
BL_Sts_Type USB_Get_EPx_TX_FIFO_Status(USB_EP_ID epId, USB_FIFO_STATUS_Type sts);
BL_Sts_Type USB_Get_EPx_RX_FIFO_Status(USB_EP_ID epId, USB_FIFO_STATUS_Type sts);
/*----------*/
BL_Err_Type USB_Set_Internal_PullUp_Config(BL_Fun_Type newState);
/*----------*/
BL_Sts_Type USB_Get_LPM_Status(void);
uint16_t USB_Get_LPM_Packet_Attr(void);
BL_Err_Type USB_Set_LPM_Default_Response(USB_LPM_DEFAULT_RESP_Type defaultResp);
BL_Err_Type USB_LPM_Enable(void);
BL_Err_Type USB_LPM_Disable(void);
/*----------*/
BL_Err_Type USB_Device_Output_K_State(uint16_t stateWidth);
/*----------*/
uint8_t USB_Get_Current_Packet_PID(void);
uint8_t USB_Get_Current_Packet_EP(void);
/*----------*/
BL_Sts_Type USB_Get_Error_Status(USB_ERROR_Type err);
BL_Err_Type USB_Clr_Error_Status(USB_ERROR_Type err);
/*----------*/

/*@} end of group USB_Public_Functions */

/*@} end of group USB */

/*@} end of group BL702_Peripheral_Driver */

#endif /* __BL702_USB_H__ */
