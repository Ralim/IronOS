/**
  ******************************************************************************
  * @file    bl702_cam.h
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
#ifndef __BL702_CAM_H__
#define __BL702_CAM_H__

#include "cam_reg.h"
#include "bl702_common.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  CAM
 *  @{
 */

/** @defgroup  CAM_Public_Types
 *  @{
 */

/**
 *  @brief CAM AHB burst type definition
 */
typedef enum {
    CAM_BURST_TYPE_SINGLE, /*!< Camera AHB burst type:single */
    CAM_BURST_TYPE_INCR4,  /*!< Camera AHB burst type:incrementing 4 */
    CAM_BURST_TYPE_INCR8,  /*!< Camera AHB burst type:incrementing 8 */
    CAM_BURST_TYPE_INCR16, /*!< Camera AHB burst type:incrementing 16 */
} CAM_Burst_Type;

/**
 *  @brief CAM software mode type definition
 */
typedef enum {
    CAM_SW_MODE_AUTO,   /*!< CAM auto mode with mjpeg */
    CAM_SW_MODE_MANUAL, /*!< CAM manual mode(software mode) */
} CAM_SW_Mode_Type;

/**
 *  @brief CAM frame mode type definition
 */
typedef enum {
    CAM_PLANAR_MODE,     /*!< CAM planar mode(YYYY.../UVUV...) */
    CAM_INTERLEAVE_MODE, /*!< CAM interleave mode(YUYVYUYV...) */
} CAM_Frame_Mode_Type;

/**
 *  @brief CAM YUV mode type definition
 */
typedef enum {
    CAM_YUV422,      /*!< CAM YUV422 mode */
    CAM_YUV420_EVEN, /*!< CAM YUV420 mode(raw data:YUYV YUYV..., processed data:YUYV YY...) which dropped odd pix data in
                                                 odd rows */
    CAM_YUV420_ODD,  /*!< CAM YUV420 mode(raw data:UYVY UYVY..., processed data:UYVY YY...) which dropped even pix data
                                                 in odd rows */
    CAM_YUV400_EVEN, /*!< CAM YUV400 mode(raw data:YUYV YUYV..., processed data:YY YY...) which dropped odd pix data */
    CAM_YUV400_ODD,  /*!< CAM YUV400 mode(raw data:UYVY UYVY..., processed data:YY YY...) which dropped even pix data */
} CAM_YUV_Mode_Type;

/**
 *  @brief CAM line active polarity type definition
 */
typedef enum {
    CAM_LINE_ACTIVE_POLARITY_LOW,  /*!< CAM line active polarity low */
    CAM_LINE_ACTIVE_POLARITY_HIGH, /*!< CAM line active polarity high */
} CAM_Line_Active_Pol;

/**
 *  @brief CAM frame active polarity type definition
 */
typedef enum {
    CAM_FRAME_ACTIVE_POLARITY_LOW,  /*!< CAM frame active polarity low */
    CAM_FRAME_ACTIVE_POLARITY_HIGH, /*!< CAM frame active polarity high */
} CAM_Frame_Active_Pol;

/**
 *  @brief CAM sensor mode type definition
 */
typedef enum {
    CAM_SENSOR_MODE_V_AND_H, /*!< CAM sensor type v and h */
    CAM_SENSOR_MODE_V_OR_H,  /*!< CAM sensor type v  or h */
    CAM_SENSOR_MODE_V,       /*!< CAM sensor type v */
    CAM_SENSOR_MODE_H,       /*!< CAM sensor type h */
} CAM_Sensor_Mode_Type;

/**
 *  @brief CAM interrupt type definition
 */
typedef enum {
    CAM_INT_NORMAL_0,           /*!< Interleave mode: normal write interrupt,     planar mode:even byte normal write interrupt */
    CAM_INT_NORMAL_1,           /*!< Interleave mode: no use,                     planar mode:odd byte normal write interrupt */
    CAM_INT_MEMORY_OVERWRITE_0, /*!< Interleave mode: memory overwrite interrupt, planar mode:even byte memory overwrite interrupt */
    CAM_INT_MEMORY_OVERWRITE_1, /*!< Interleave mode: no use,                     planar mode:odd byte memory overwrite interrupt */
    CAM_INT_FRAME_OVERWRITE_0,  /*!< Interleave mode: frame overwrite interrupt,  planar mode:even byte frame overwrite interrupt */
    CAM_INT_FRAME_OVERWRITE_1,  /*!< Interleave mode: no use,                     planar mode:odd byte frame overwrite interrupt */
    CAM_INT_FIFO_OVERWRITE_0,   /*!< Interleave mode: fifo overwrite interrupt,   planar mode:even byte fifo overwrite interrupt */
    CAM_INT_FIFO_OVERWRITE_1,   /*!< Interleave mode: no use,                     planar mode:odd byte fifo overwrite interrupt */
    CAM_INT_VSYNC_CNT_ERROR,    /*!< Vsync valid line count non-match interrupt */
    CAM_INT_HSYNC_CNT_ERROR,    /*!< Hsync valid pixel count non-match interrupt */
    CAM_INT_ALL,                /*!< All of interrupt */
} CAM_INT_Type;

/**
 *  @brief CAM configuration strcut definition
 */
typedef struct
{
    CAM_SW_Mode_Type swMode;            /*!< Software mode */
    uint8_t swIntCnt;                   /*!< Set frame count to issue interrupt at software mode */
    CAM_Frame_Mode_Type frameMode;      /*!< Frame mode */
    CAM_YUV_Mode_Type yuvMode;          /*!< YUV mode */
    CAM_Frame_Active_Pol framePol;      /*!< Frame polarity */
    CAM_Line_Active_Pol linePol;        /*!< Line polarity */
    CAM_Burst_Type burstType;           /*!< AHB burst type */
    CAM_Sensor_Mode_Type camSensorMode; /*!< CAM sensor mode */
    uint8_t waitCount;                  /*!< cycles in FSM wait mode, default value:0x40 */
    uint32_t memStart0;                 /*!< Interleave mode:data start address,   planar mode:even byte start address */
    uint32_t memSize0;                  /*!< Interleave mode:memory size in burst, planar mode:even byte memory size in burst */
    uint32_t frameSize0;                /*!< Interleave mode:frame size in burst,  planar mode:even byte frame size in burst */
    uint32_t memStart1;                 /*!< Interleave mode:no use,               planar mode:odd byte start address */
    uint32_t memSize1;                  /*!< Interleave mode:no use,               planar mode:odd byte memory size in burst */
    uint32_t frameSize1;                /*!< Interleave mode:no use,               planar mode:odd byte frame size in burst */
} CAM_CFG_Type;

/**
 *  @brief CAM interleave mode frame information strcut definition
 */
typedef struct
{
    uint8_t validFrames;    /*!< Valid frames */
    uint32_t curFrameAddr;  /*!< Current frame address */
    uint32_t curFrameBytes; /*!< Current frame bytes */
    uint32_t status;        /*!< CAM module status */
} CAM_Interleave_Frame_Info;

/**
 *  @brief CAM planar mode frame information strcut definition
 */
typedef struct
{
    uint8_t validFrames0;    /*!< Even byte frame counts in memory */
    uint8_t validFrames1;    /*!< Odd byte frame counts in memory */
    uint32_t curFrameAddr0;  /*!< Current even frame address */
    uint32_t curFrameAddr1;  /*!< Current odd frame address */
    uint32_t curFrameBytes0; /*!< Current even frame bytes */
    uint32_t curFrameBytes1; /*!< Current odd frame bytes */
    uint32_t status;         /*!< CAM module status */
} CAM_Planar_Frame_Info;

/*@} end of group CAM_Public_Types */

/** @defgroup  CAM_Public_Constants
 *  @{
 */

/** @defgroup  CAM_BURST_TYPE
 *  @{
 */
#define IS_CAM_BURST_TYPE(type) (((type) == CAM_BURST_TYPE_SINGLE) || \
                                 ((type) == CAM_BURST_TYPE_INCR4) ||  \
                                 ((type) == CAM_BURST_TYPE_INCR8) ||  \
                                 ((type) == CAM_BURST_TYPE_INCR16))

/** @defgroup  CAM_SW_MODE_TYPE
 *  @{
 */
#define IS_CAM_SW_MODE_TYPE(type) (((type) == CAM_SW_MODE_AUTO) || \
                                   ((type) == CAM_SW_MODE_MANUAL))

/** @defgroup  CAM_FRAME_MODE_TYPE
 *  @{
 */
#define IS_CAM_FRAME_MODE_TYPE(type) (((type) == CAM_PLANAR_MODE) || \
                                      ((type) == CAM_INTERLEAVE_MODE))

/** @defgroup  CAM_YUV_MODE_TYPE
 *  @{
 */
#define IS_CAM_YUV_MODE_TYPE(type) (((type) == CAM_YUV422) ||      \
                                    ((type) == CAM_YUV420_EVEN) || \
                                    ((type) == CAM_YUV420_ODD) ||  \
                                    ((type) == CAM_YUV400_EVEN) || \
                                    ((type) == CAM_YUV400_ODD))

/** @defgroup  CAM_LINE_ACTIVE_POL
 *  @{
 */
#define IS_CAM_LINE_ACTIVE_POL(type) (((type) == CAM_LINE_ACTIVE_POLARITY_LOW) || \
                                      ((type) == CAM_LINE_ACTIVE_POLARITY_HIGH))

/** @defgroup  CAM_FRAME_ACTIVE_POL
 *  @{
 */
#define IS_CAM_FRAME_ACTIVE_POL(type) (((type) == CAM_FRAME_ACTIVE_POLARITY_LOW) || \
                                       ((type) == CAM_FRAME_ACTIVE_POLARITY_HIGH))

/** @defgroup  CAM_SENSOR_MODE_TYPE
 *  @{
 */
#define IS_CAM_SENSOR_MODE_TYPE(type) (((type) == CAM_SENSOR_MODE_V_AND_H) || \
                                       ((type) == CAM_SENSOR_MODE_V_OR_H) ||  \
                                       ((type) == CAM_SENSOR_MODE_V) ||       \
                                       ((type) == CAM_SENSOR_MODE_H))

/** @defgroup  CAM_INT_TYPE
 *  @{
 */
#define IS_CAM_INT_TYPE(type) (((type) == CAM_INT_NORMAL_0) ||           \
                               ((type) == CAM_INT_NORMAL_1) ||           \
                               ((type) == CAM_INT_MEMORY_OVERWRITE_0) || \
                               ((type) == CAM_INT_MEMORY_OVERWRITE_1) || \
                               ((type) == CAM_INT_FRAME_OVERWRITE_0) ||  \
                               ((type) == CAM_INT_FRAME_OVERWRITE_1) ||  \
                               ((type) == CAM_INT_FIFO_OVERWRITE_0) ||   \
                               ((type) == CAM_INT_FIFO_OVERWRITE_1) ||   \
                               ((type) == CAM_INT_VSYNC_CNT_ERROR) ||    \
                               ((type) == CAM_INT_HSYNC_CNT_ERROR) ||    \
                               ((type) == CAM_INT_ALL))

/*@} end of group CAM_Public_Constants */

/** @defgroup  CAM_Public_Macros
 *  @{
 */

/*@} end of group CAM_Public_Macros */

/** @defgroup  CAM_Public_Functions
 *  @{
 */
#ifndef BFLB_USE_HAL_DRIVER
void CAM_IRQHandler(void);
#endif
void CAM_Init(CAM_CFG_Type *cfg);
void CAM_Deinit(void);
void CAM_Enable(void);
void CAM_Disable(void);
void CAM_Clock_Gate(BL_Fun_Type enable);
void CAM_Hsync_Crop(uint16_t start, uint16_t end);
void CAM_Vsync_Crop(uint16_t start, uint16_t end);
void CAM_Set_Hsync_Total_Count(uint16_t count);
void CAM_Set_Vsync_Total_Count(uint16_t count);
void CAM_Interleave_Get_Frame_Info(CAM_Interleave_Frame_Info *info);
void CAM_Planar_Get_Frame_Info(CAM_Planar_Frame_Info *info);
uint8_t CAM_Get_Frame_Count_0(void);
uint8_t CAM_Get_Frame_Count_1(void);
void CAM_Interleave_Pop_Frame(void);
void CAM_Planar_Pop_Frame(void);
void CAM_IntMask(CAM_INT_Type intType, BL_Mask_Type intMask);
void CAM_IntClr(CAM_INT_Type intType);
void CAM_Int_Callback_Install(CAM_INT_Type intType, intCallback_Type *cbFun);
void CAM_HW_Mode_Wrap(BL_Fun_Type enable);

/*@} end of group CAM_Public_Functions */

/*@} end of group CAM */

/*@} end of group BL702_Peripheral_Driver */

#endif /* __BL702_CAM_H__ */
