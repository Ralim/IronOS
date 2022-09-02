/**
  ******************************************************************************
  * @file    bl702_mjpeg.h
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
#ifndef __BL702_MJPEG_H__
#define __BL702_MJPEG_H__

#include "mjpeg_reg.h"
#include "bl702_common.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  MJPEG
 *  @{
 */

/** @defgroup  MJPEG_Public_Types
 *  @{
 */

/**
 *  @brief MJPEG YUV format definition
 */
typedef enum {
    MJPEG_YUV420,            /*!< MJPEG YUV420 planar mode */
    MJPEG_YUV400,            /*!< MJPEG YUV400 grey scale mode */
    MJPEG_YUV422_PLANAR,     /*!< MJPEG YUV422 planar mode */
    MJPEG_YUV422_INTERLEAVE, /*!< MJPEG YUV422 interleave mode */
} MJPEG_YUV_Mode_Type;

/**
 *  @brief MJPEG burst type definition
 */
typedef enum {
    MJPEG_BURST_SINGLE, /*!< MJPEG burst single */
    MJPEG_BURST_INCR4,  /*!< MJPEG burst incr4 */
    MJPEG_BURST_INCR8,  /*!< MJPEG burst incr8 */
    MJPEG_BURST_INCR16, /*!< MJPEG burst incr16 */
} MJPEG_Burst_Type;

/**
 *  @brief MJPEG swap mode block definition
 */
typedef enum {
    MJPEG_BLOCK_0, /*!< Memory block 0 */
    MJPEG_BLOCK_1, /*!< Memory block 1 */
} MJPEG_Swap_Block_Type;

/**
 *  @brief MJPEG interrupt type definition
 */
typedef enum {
    MJPEG_INT_NORMAL,          /*!< MJPEG normal write interrupt */
    MJPEG_INT_CAM_OVERWRITE,   /*!< MJPEG camera overwrite interrupt */
    MJPEG_INT_MEM_OVERWRITE,   /*!< MJPEG memory overwrite interrupt */
    MJPEG_INT_FRAME_OVERWRITE, /*!< MJPEG frame overwrite interrupt */
    MJPEG_INT_BACK_IDLE,       /*!< MJPEG back idle interrupt */
    MJPEG_INT_SWAP,            /*!< MJPEG swap memory block interrupt */
    MJPEG_INT_ALL,             /*!< MJPEG all interrupt type */
} MJPEG_INT_Type;

/**
 *  @brief MJPEG configuration strcut definition
 */
typedef struct
{
    MJPEG_Burst_Type burst;      /*!< MJPEG burst type */
    uint8_t quality;             /*!< MJPEG quantization selection, 1-75:Q table selection, others:Q100(lossless) */
    MJPEG_YUV_Mode_Type yuv;     /*!< MJPEG control YUV mode */
    uint16_t waitCount;          /*!< Cycle count in wait state, default value:0x400 */
    uint32_t bufferMjpeg;        /*!< MJPEG buffer addr */
    uint32_t sizeMjpeg;          /*!< MJPEG buffer size */
    uint32_t bufferCamYY;        /*!< CAM buffer address of Y frame */
    uint32_t sizeCamYY;          /*!< CAM buffer size of Y frame */
    uint32_t bufferCamUV;        /*!< CAM buffer address of UV frame */
    uint32_t sizeCamUV;          /*!< CAM buffer size of UV frame */
    uint16_t resolutionX;        /*!< CAM RESOLUTION X */
    uint16_t resolutionY;        /*!< CAM RESOLUTION Y */
    BL_Fun_Type bitOrderEnable;  /*!< MJPEG bitstream order adjustment */
    BL_Fun_Type evenOrderEnable; /*!< Enable:U is even byte of UV frame and V is odd byte of UV frame */
    BL_Fun_Type swapModeEnable;  /*!< Enable or disable write swap mode */
    BL_Fun_Type overStopEnable;  /*!< Enable:if camera overwrite error occur,mjpeg will stop fetch data */
    BL_Fun_Type reflectDmy;      /*!< UV dummy with relect */
    BL_Fun_Type verticalDmy;     /*!< MJPEG last half vertical block with dummy data 0x80 */
    BL_Fun_Type horizationalDmy; /*!< MJPEG last half horizational block with dummy data 0x80 */
} MJPEG_CFG_Type;

/**
 *  @brief MJPEG packet configuration strcut definition
 */
typedef struct
{
    BL_Fun_Type packetEnable; /*!< Enable or disable packet mode */
    BL_Fun_Type endToTail;    /*!< Enable or disable jpeg end jump to packet tail */
    uint16_t frameHead;       /*!< Preserve head memory space for each frame */
    BL_Fun_Type frameTail;    /*!< Enable:auto fill tail 0xFF and 0xD9 */
    uint16_t packetHead;      /*!< Packet head byte count */
    uint16_t packetBody;      /*!< Packet body byte count */
    uint16_t packetTail;      /*!< Packet tail byte count */
} MJPEG_Packet_Type;

/**
 *  @brief MJPEG frame information strcut definition
 */
typedef struct
{
    uint8_t validFrames;    /*!< Valid frames */
    uint32_t curFrameAddr;  /*!< Current frame address */
    uint32_t curFrameBytes; /*!< Current frame bytes */
    uint8_t curFrameQ;      /*!< Current frame Q mode */
    uint32_t status;        /*!< MJPEG module status */
} MJPEG_Frame_Info;

/*@} end of group MJPEG_Public_Types */

/** @defgroup  MJPEG_Public_Constants
 *  @{
 */

/** @defgroup  MJPEG_YUV_MODE_TYPE
 *  @{
 */
#define IS_MJPEG_YUV_MODE_TYPE(type) (((type) == MJPEG_YUV420) ||        \
                                      ((type) == MJPEG_YUV400) ||        \
                                      ((type) == MJPEG_YUV422_PLANAR) || \
                                      ((type) == MJPEG_YUV422_INTERLEAVE))

/** @defgroup  MJPEG_BURST_TYPE
 *  @{
 */
#define IS_MJPEG_BURST_TYPE(type) (((type) == MJPEG_BURST_SINGLE) || \
                                   ((type) == MJPEG_BURST_INCR4) ||  \
                                   ((type) == MJPEG_BURST_INCR8) ||  \
                                   ((type) == MJPEG_BURST_INCR16))

/** @defgroup  MJPEG_SWAP_BLOCK_TYPE
 *  @{
 */
#define IS_MJPEG_SWAP_BLOCK_TYPE(type) (((type) == MJPEG_BLOCK_0) || \
                                        ((type) == MJPEG_BLOCK_1))

/** @defgroup  MJPEG_INT_TYPE
 *  @{
 */
#define IS_MJPEG_INT_TYPE(type) (((type) == MJPEG_INT_NORMAL) ||          \
                                 ((type) == MJPEG_INT_CAM_OVERWRITE) ||   \
                                 ((type) == MJPEG_INT_MEM_OVERWRITE) ||   \
                                 ((type) == MJPEG_INT_FRAME_OVERWRITE) || \
                                 ((type) == MJPEG_INT_BACK_IDLE) ||       \
                                 ((type) == MJPEG_INT_SWAP) ||            \
                                 ((type) == MJPEG_INT_ALL))

/*@} end of group MJPEG_Public_Constants */

/** @defgroup  MJPEG_Public_Macros
 *  @{
 */

/*@} end of group MJPEG_Public_Macros */

/** @defgroup  MJPEG_Public_Functions
 *  @{
 */
#ifndef BFLB_USE_HAL_DRIVER
void MJPEG_IRQHandler(void);
#endif
void MJPEG_Init(MJPEG_CFG_Type *cfg);
void MJPEG_Packet_Config(MJPEG_Packet_Type *cfg);
void MJPEG_Set_YUYV_Order_Interleave(uint8_t y0, uint8_t u0, uint8_t y1, uint8_t v0);
void MJPEG_Set_YUYV_Order_Planar(uint8_t yy, uint8_t uv);
void MJPEG_Deinit(void);
void MJPEG_Enable(void);
void MJPEG_Disable(void);
void MJPEG_SW_Enable(uint8_t count);
void MJPEG_SW_Run(void);
void MJPEG_Get_Frame_Info(MJPEG_Frame_Info *info);
uint8_t MJPEG_Get_Frame_Count(void);
void MJPEG_Pop_Frame(void);
void MJPEG_Current_Block_Clear(void);
MJPEG_Swap_Block_Type MJPEG_Get_Current_Block(void);
BL_Sts_Type MJPEG_Block_Is_Full(MJPEG_Swap_Block_Type block);
BL_Sts_Type MJPEG_Current_Block_Is_Start(void);
BL_Sts_Type MJPEG_Current_Block_Is_End(void);
uint32_t MJPEG_Get_Remain_Bit(void);
void MJPEG_Set_Frame_Threshold(uint8_t count);
void MJPEG_IntMask(MJPEG_INT_Type intType, BL_Mask_Type intMask);
void MJPEG_IntClr(MJPEG_INT_Type intType);
void MJPEG_Int_Callback_Install(MJPEG_INT_Type intType, intCallback_Type *cbFun);

/*@} end of group MJPEG_Public_Functions */

/*@} end of group MJPEG */

/*@} end of group BL702_Peripheral_Driver */

#endif /* __BL702_MJPEG_H__ */
