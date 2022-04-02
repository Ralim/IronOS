/**
  ******************************************************************************
  * @file    bl702_dma.h
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
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
#ifndef __BL702_DMA_H__
#define __BL702_DMA_H__

#include "dma_reg.h"
#include "bl702_common.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  DMA
 *  @{
 */

/** @defgroup  DMA_Public_Types
 *  @{
 */

/**
 *  @brief DMA endian type definition
 */
typedef enum {
    DMA_LITTLE_ENDIAN = 0, /*!< DMA use little endian */
    DMA_BIG_ENDIAN,        /*!< DMA use big endian */
} DMA_Endian_Type;

/**
 *  @brief DMA synchronization logic  type definition
 */
typedef enum {
    DMA_SYNC_LOGIC_ENABLE = 0, /*!< DMA synchronization logic enable */
    DMA_SYNC_LOGIC_DISABLE,    /*!< DMA synchronization logic disable */
} DMA_Sync_Logic_Type;

/**
 *  @brief DMA transfer width type definition
 */
typedef enum {
    DMA_TRNS_WIDTH_8BITS = 0, /*!< DMA transfer width:8 bits */
    DMA_TRNS_WIDTH_16BITS,    /*!< DMA transfer width:16 bits */
    DMA_TRNS_WIDTH_32BITS,    /*!< DMA transfer width:32 bits */
} DMA_Trans_Width_Type;

/**
 *  @brief DMA transfer direction type definition
 */
typedef enum {
    DMA_TRNS_M2M = 0, /*!< DMA transfer tyep:memory to memory */
    DMA_TRNS_M2P,     /*!< DMA transfer tyep:memory to peripheral */
    DMA_TRNS_P2M,     /*!< DMA transfer tyep:peripheral to memory */
    DMA_TRNS_P2P,     /*!< DMA transfer tyep:peripheral to peripheral */
} DMA_Trans_Dir_Type;

/**
 *  @brief DMA burst size type definition
 */
typedef enum {
    DMA_BURST_SIZE_1 = 0, /*!< DMA transfer width:8 bits */
    DMA_BURST_SIZE_4,     /*!< DMA transfer width:16 bits */
    DMA_BURST_SIZE_8,     /*!< DMA transfer width:32 bits */
    DMA_BURST_SIZE_16,    /*!< DMA transfer width:64 bits */
} DMA_Burst_Size_Type;

/**
 *  @brief DMA destination peripheral type definition
 */
typedef enum {
    DMA_REQ_UART0_RX = 0, /*!< DMA request peripheral:UART0 RX */
    DMA_REQ_UART0_TX,     /*!< DMA request peripheral:UART0 TX */
    DMA_REQ_UART1_RX,     /*!< DMA request peripheral:UART1 RX */
    DMA_REQ_UART1_TX,     /*!< DMA request peripheral:UART1 TX */
    DMA_REQ_I2C_RX = 6,   /*!< DMA request peripheral:I2C RX */
    DMA_REQ_I2C_TX,       /*!< DMA request peripheral:I2C TX */
    DMA_REQ_SPI_RX = 10,  /*!< DMA request peripheral:SPI RX */
    DMA_REQ_SPI_TX,       /*!< DMA request peripheral:SPI TX */
    DMA_REQ_I2S_RX = 20,  /*!< DMA request peripheral:SPI RX */
    DMA_REQ_I2S_TX,       /*!< DMA request peripheral:SPI TX */
    DMA_REQ_GPADC0 = 22,  /*!< DMA request peripheral:GPADC0 */
    DMA_REQ_GPADC1,       /*!< DMA request peripheral:GPADC1 */
    DMA_REQ_NONE = 0,     /*!< DMA request peripheral:None */
} DMA_Periph_Req_Type;

/**
 *  @brief DMA channel type definition
 */
typedef enum {
    DMA_CH0 = 0, /*!< DMA channel 0 */
    DMA_CH1,     /*!< DMA channel 1 */
    DMA_CH2,     /*!< DMA channel 2 */
    DMA_CH3,     /*!< DMA channel 3 */
    DMA_CH4,     /*!< DMA channel 4 */
    DMA_CH5,     /*!< DMA channel 5 */
    DMA_CH6,     /*!< DMA channel 6 */
    DMA_CH7,     /*!< DMA channel 7 */
    DMA_CH_MAX,  /*!<  */
} DMA_Chan_Type;

/**
 *  @brief DMA LLI Structure PING-PONG
 */
typedef enum {
    PING_INDEX = 0, /*!< PING INDEX */
    PONG_INDEX,     /*!< PONG INDEX */
} DMA_LLI_PP_Index_Type;

/**
 *  @brief DMA interrupt type definition
 */
typedef enum {
    DMA_INT_TCOMPLETED = 0, /*!< DMA completed interrupt */
    DMA_INT_ERR,            /*!< DMA error interrupt */
    DMA_INT_ALL,            /*!< All the interrupt */
} DMA_INT_Type;

/**
 *  @brief DMA Configuration Structure type definition
 */
typedef struct
{
    DMA_Endian_Type endian;        /*!< DMA endian type */
    DMA_Sync_Logic_Type syncLogic; /*!< DMA synchronization logic */
} DMA_Cfg_Type;

/**
 *  @brief DMA channel Configuration Structure type definition
 */
typedef struct
{
    uint32_t srcDmaAddr;                 /*!< Source address of DMA transfer */
    uint32_t destDmaAddr;                /*!< Destination address of DMA transfer */
    uint32_t transfLength;               /*!< Transfer length, 0~4095, this is burst count */
    DMA_Trans_Dir_Type dir;              /*!< Transfer dir control. 0: Memory to Memory, 1: Memory to peripheral, 2: Peripheral to memory */
    DMA_Chan_Type ch;                    /*!< Channel select 0-7 */
    DMA_Trans_Width_Type srcTransfWidth; /*!< Transfer width. 0: 8  bits, 1: 16  bits, 2: 32  bits */
    DMA_Trans_Width_Type dstTransfWidth; /*!< Transfer width. 0: 8  bits, 1: 16  bits, 2: 32  bits */
    DMA_Burst_Size_Type srcBurstSzie;    /*!< Number of data items for burst transaction length. Each item width is as same as tansfer width.
                                                 0: 1 item, 1: 4 items, 2: 8 items, 3: 16 items */
    DMA_Burst_Size_Type dstBurstSzie;    /*!< Number of data items for burst transaction length. Each item width is as same as tansfer width.
                                                 0: 1 item, 1: 4 items, 2: 8 items, 3: 16 items */
    BL_Fun_Type dstAddMode;              /*!<  */
    BL_Fun_Type dstMinMode;              /*!<  */
    uint8_t fixCnt;                      /*!<  */
    uint8_t srcAddrInc;                  /*!< Source address increment. 0: No change, 1: Increment */
    uint8_t destAddrInc;                 /*!< Destination address increment. 0: No change, 1: Increment */
    DMA_Periph_Req_Type srcPeriph;       /*!< Source peripheral select */
    DMA_Periph_Req_Type dstPeriph;       /*!< Destination peripheral select */
} DMA_Channel_Cfg_Type;

/**
 *  @brief DMA LLI control structure type definition
 */
typedef struct
{
    uint32_t srcDmaAddr;            /*!< Source address of DMA transfer */
    uint32_t destDmaAddr;           /*!< Destination address of DMA transfer */
    uint32_t nextLLI;               /*!< Next LLI address */
    struct DMA_Control_Reg dmaCtrl; /*!< DMA transaction control */
} DMA_LLI_Ctrl_Type;

/**
 *  @brief DMA LLI configuration structure type definition
 */
typedef struct
{
    DMA_Trans_Dir_Type dir;        /*!< Transfer dir control. 0: Memory to Memory, 1: Memory to peripheral, 2: Peripheral to memory */
    DMA_Periph_Req_Type srcPeriph; /*!< Source peripheral select */
    DMA_Periph_Req_Type dstPeriph; /*!< Destination peripheral select */
} DMA_LLI_Cfg_Type;

/**
 *  @brief DMA LLI Ping-Pong Buf definition
 */
typedef struct
{
    uint8_t idleIndex;                             /*!< Index Idle lliListHeader */
    uint8_t dmaChan;                               /*!< DMA LLI Channel used */
    DMA_LLI_Ctrl_Type *lliListHeader[2];           /*!< Ping-Pong BUf List Header */
    void (*onTransCompleted)(DMA_LLI_Ctrl_Type *); /*!< Completed Transmit One List Callback Function */
} DMA_LLI_PP_Buf;

/**
 *  @brief DMA LLI Ping-Pong Structure definition
 */
typedef struct
{
    uint8_t trans_index;                  /*!< Ping or Pong Trigger TC */
    uint8_t dmaChan;                      /*!< DMA LLI Channel used */
    struct DMA_Control_Reg dmaCtrlRegVal; /*!< DMA Basic Pararmeter */
    DMA_LLI_Cfg_Type *DMA_LLI_Cfg;        /*!< LLI Config parameter */
    uint32_t operatePeriphAddr;           /*!< Operate Peripheral register address */
    uint32_t chache_buf_addr[2];          /*!< Ping-Pong structure chache */
    BL_Fun_Type is_single_mode;           /*!< is Ping-pong running forever or single mode ,if is single mode ping-pong will run only once
                                                 after one start */
} DMA_LLI_PP_Struct;

/*@} end of group DMA_Public_Types */

/** @defgroup  DMA_Public_Constants
 *  @{
 */

/** @defgroup  DMA_ENDIAN_TYPE
 *  @{
 */
#define IS_DMA_ENDIAN_TYPE(type) (((type) == DMA_LITTLE_ENDIAN) || \
                                  ((type) == DMA_BIG_ENDIAN))

/** @defgroup  DMA_SYNC_LOGIC_TYPE
 *  @{
 */
#define IS_DMA_SYNC_LOGIC_TYPE(type) (((type) == DMA_SYNC_LOGIC_ENABLE) || \
                                      ((type) == DMA_SYNC_LOGIC_DISABLE))

/** @defgroup  DMA_TRANS_WIDTH_TYPE
 *  @{
 */
#define IS_DMA_TRANS_WIDTH_TYPE(type) (((type) == DMA_TRNS_WIDTH_8BITS) ||  \
                                       ((type) == DMA_TRNS_WIDTH_16BITS) || \
                                       ((type) == DMA_TRNS_WIDTH_32BITS))

/** @defgroup  DMA_TRANS_DIR_TYPE
 *  @{
 */
#define IS_DMA_TRANS_DIR_TYPE(type) (((type) == DMA_TRNS_M2M) || \
                                     ((type) == DMA_TRNS_M2P) || \
                                     ((type) == DMA_TRNS_P2M) || \
                                     ((type) == DMA_TRNS_P2P))

/** @defgroup  DMA_BURST_SIZE_TYPE
 *  @{
 */
#define IS_DMA_BURST_SIZE_TYPE(type) (((type) == DMA_BURST_SIZE_1) || \
                                      ((type) == DMA_BURST_SIZE_4) || \
                                      ((type) == DMA_BURST_SIZE_8) || \
                                      ((type) == DMA_BURST_SIZE_16))

/** @defgroup  DMA_PERIPH_REQ_TYPE
 *  @{
 */
#define IS_DMA_PERIPH_REQ_TYPE(type) (((type) == DMA_REQ_UART0_RX) || \
                                      ((type) == DMA_REQ_UART0_TX) || \
                                      ((type) == DMA_REQ_UART1_RX) || \
                                      ((type) == DMA_REQ_UART1_TX) || \
                                      ((type) == DMA_REQ_I2C_RX) ||   \
                                      ((type) == DMA_REQ_I2C_TX) ||   \
                                      ((type) == DMA_REQ_SPI_RX) ||   \
                                      ((type) == DMA_REQ_SPI_TX) ||   \
                                      ((type) == DMA_REQ_I2S_RX) ||   \
                                      ((type) == DMA_REQ_I2S_TX) ||   \
                                      ((type) == DMA_REQ_GPADC0) ||   \
                                      ((type) == DMA_REQ_GPADC1) ||   \
                                      ((type) == DMA_REQ_NONE))

/** @defgroup  DMA_CHAN_TYPE
 *  @{
 */
#define IS_DMA_CHAN_TYPE(type) (((type) == DMA_CH0) || \
                                ((type) == DMA_CH1) || \
                                ((type) == DMA_CH2) || \
                                ((type) == DMA_CH3) || \
                                ((type) == DMA_CH4) || \
                                ((type) == DMA_CH5) || \
                                ((type) == DMA_CH6) || \
                                ((type) == DMA_CH7) || \
                                ((type) == DMA_CH_MAX))

/** @defgroup  DMA_LLI_PP_INDEX_TYPE
 *  @{
 */
#define IS_DMA_LLI_PP_INDEX_TYPE(type) (((type) == PING_INDEX) || \
                                        ((type) == PONG_INDEX))

/** @defgroup  DMA_INT_TYPE
 *  @{
 */
#define IS_DMA_INT_TYPE(type) (((type) == DMA_INT_TCOMPLETED) || \
                               ((type) == DMA_INT_ERR) ||        \
                               ((type) == DMA_INT_ALL))

/*@} end of group DMA_Public_Constants */

/** @defgroup  DMA_Public_Macros
 *  @{
 */
#define DMA_PINC_ENABLE  1
#define DMA_PINC_DISABLE 0
#define DMA_MINC_ENABLE  1
#define DMA_MINC_DISABLE 0

/*@} end of group DMA_Public_Macros */

/** @defgroup  DMA_Public_Functions
 *  @{
 */

/**
 *  @brief DMA Functions
 */
#ifndef BFLB_USE_HAL_DRIVER
void DMA_ALL_IRQHandler(void);
#endif
void DMA_Enable(void);
void DMA_Disable(void);
void DMA_Channel_Init(DMA_Channel_Cfg_Type *chCfg);
void DMA_Channel_Update_SrcMemcfg(uint8_t ch, uint32_t memAddr, uint32_t len);
void DMA_Channel_Update_DstMemcfg(uint8_t ch, uint32_t memAddr, uint32_t len);
uint32_t DMA_Channel_TranferSize(uint8_t ch);
BL_Sts_Type DMA_Channel_Is_Busy(uint8_t ch);
void DMA_Channel_Enable(uint8_t ch);
void DMA_Channel_Disable(uint8_t ch);
void DMA_LLI_Init(uint8_t ch, DMA_LLI_Cfg_Type *lliCfg);
void DMA_LLI_Update(uint8_t ch, uint32_t LLI);
void DMA_IntMask(uint8_t ch, DMA_INT_Type intType, BL_Mask_Type intMask);
void DMA_LLI_PpBuf_Start_New_Transmit(DMA_LLI_PP_Buf *dmaPpBuf);
DMA_LLI_Ctrl_Type *DMA_LLI_PpBuf_Remove_Completed_List(DMA_LLI_PP_Buf *dmaPpBuf);
void DMA_LLI_PpBuf_Append(DMA_LLI_PP_Buf *dmaPpBuf, DMA_LLI_Ctrl_Type *dmaLliList);
void DMA_LLI_PpBuf_Destroy(DMA_LLI_PP_Buf *dmaPpBuf);
void DMA_Int_Callback_Install(DMA_Chan_Type dmaChan, DMA_INT_Type intType, intCallback_Type *cbFun);
void DMA_LLI_PpStruct_Start(DMA_LLI_PP_Struct *dmaPpStruct);
void DMA_LLI_PpStruct_Stop(DMA_LLI_PP_Struct *dmaPpStruct);
BL_Err_Type DMA_LLI_PpStruct_Init(DMA_LLI_PP_Struct *dmaPpStruct);
BL_Err_Type DMA_LLI_PpStruct_Set_Transfer_Len(DMA_LLI_PP_Struct *dmaPpStruct,
                                              uint16_t Ping_Transfer_len, uint16_t Pong_Transfer_len);

/*@} end of group DMA_Public_Functions */

/*@} end of group DMA */

/*@} end of group BL702_Peripheral_Driver */

#endif /* __BL702_DMA_H__ */
