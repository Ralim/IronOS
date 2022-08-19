/**
  ******************************************************************************
  * @file    bl702_cam.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
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

#include "bl702.h"
#include "bl702_cam.h"
#include "bl702_glb.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  CAM
 *  @{
 */

/** @defgroup  CAM_Private_Macros
 *  @{
 */

/*@} end of group CAM_Private_Macros */

/** @defgroup  CAM_Private_Types
 *  @{
 */

/*@} end of group CAM_Private_Types */

/** @defgroup  CAM_Private_Variables
 *  @{
 */
static intCallback_Type *camIntCbfArra[CAM_INT_ALL] = { NULL };

/*@} end of group CAM_Private_Variables */

/** @defgroup  CAM_Global_Variables
 *  @{
 */

/*@} end of group CAM_Global_Variables */

/** @defgroup  CAM_Private_Fun_Declaration
 *  @{
 */

/*@} end of group CAM_Private_Fun_Declaration */

/** @defgroup  CAM_Private_Functions
 *  @{
 */

/*@} end of group CAM_Private_Functions */

/** @defgroup  CAM_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Camera module init
 *
 * @param  cfg: Camera configuration structure pointer
 *
 * @return None
 *
*******************************************************************************/
void CAM_Init(CAM_CFG_Type *cfg)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_CAM_SW_MODE_TYPE(cfg->swMode));
    CHECK_PARAM(IS_CAM_FRAME_MODE_TYPE(cfg->frameMode));
    CHECK_PARAM(IS_CAM_YUV_MODE_TYPE(cfg->yuvMode));
    CHECK_PARAM(IS_CAM_FRAME_ACTIVE_POL(cfg->framePol));
    CHECK_PARAM(IS_CAM_LINE_ACTIVE_POL(cfg->linePol));
    CHECK_PARAM(IS_CAM_BURST_TYPE(cfg->burstType));
    CHECK_PARAM(IS_CAM_SENSOR_MODE_TYPE(cfg->camSensorMode));

    /* Disable clock gate */
    GLB_AHB_Slave1_Clock_Gate(DISABLE, BL_AHB_SLAVE1_CAM);

    /* Set camera configuration */
    tmpVal = BL_RD_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_DVP_ENABLE);
    BL_WR_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE, tmpVal);

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CAM_REG_SW_MODE, cfg->swMode);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CAM_REG_INTERLV_MODE, cfg->frameMode);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CAM_REG_FRAM_VLD_POL, cfg->framePol);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CAM_REG_LINE_VLD_POL, cfg->linePol);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CAM_REG_HBURST, cfg->burstType);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CAM_REG_DVP_MODE, cfg->camSensorMode);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CAM_REG_DVP_WAIT_CYCLE, cfg->waitCount);

    switch (cfg->yuvMode) {
        case CAM_YUV422:
            tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_DROP_EN);
            tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_SUBSAMPLE_EN);
            break;

        case CAM_YUV420_EVEN:
            tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_DROP_EN);
            tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_SUBSAMPLE_EN);
            tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_SUBSAMPLE_EVEN);
            break;

        case CAM_YUV420_ODD:
            tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_DROP_EN);
            tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_SUBSAMPLE_EN);
            tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_SUBSAMPLE_EVEN);
            break;

        case CAM_YUV400_EVEN:
            tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_DROP_EN);
            tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_DROP_EVEN);
            tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_SUBSAMPLE_EN);
            break;

        case CAM_YUV400_ODD:
            tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_DROP_EN);
            tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_DROP_EVEN);
            tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_SUBSAMPLE_EN);
            break;

        default:
            break;
    }

    BL_WR_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE, tmpVal);

    /* Set frame count to issue interrupt at sw mode */
    tmpVal = BL_RD_REG(CAM_BASE, CAM_INT_CONTROL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CAM_REG_FRAME_CNT_TRGR_INT, cfg->swIntCnt);
    BL_WR_REG(CAM_BASE, CAM_INT_CONTROL, tmpVal);

    /* Set camera memory start address, memory size and frame size in burst */
    BL_WR_REG(CAM_BASE, CAM_DVP2AHB_ADDR_START_0, cfg->memStart0 & 0xFFFFFFF0);

    if (cfg->burstType == CAM_BURST_TYPE_SINGLE) {
        BL_WR_REG(CAM_BASE, CAM_DVP2AHB_MEM_BCNT_0, cfg->memSize0 / 4);
        BL_WR_REG(CAM_BASE, CAM_DVP2AHB_FRAME_BCNT_0, cfg->frameSize0 / 4);
    } else if (cfg->burstType == CAM_BURST_TYPE_INCR4) {
        BL_WR_REG(CAM_BASE, CAM_DVP2AHB_MEM_BCNT_0, cfg->memSize0 / 16);
        BL_WR_REG(CAM_BASE, CAM_DVP2AHB_FRAME_BCNT_0, cfg->frameSize0 / 16);
    } else if (cfg->burstType == CAM_BURST_TYPE_INCR8) {
        BL_WR_REG(CAM_BASE, CAM_DVP2AHB_MEM_BCNT_0, cfg->memSize0 / 32);
        BL_WR_REG(CAM_BASE, CAM_DVP2AHB_FRAME_BCNT_0, cfg->frameSize0 / 32);
    } else if (cfg->burstType == CAM_BURST_TYPE_INCR16) {
        BL_WR_REG(CAM_BASE, CAM_DVP2AHB_MEM_BCNT_0, cfg->memSize0 / 64);
        BL_WR_REG(CAM_BASE, CAM_DVP2AHB_FRAME_BCNT_0, cfg->frameSize0 / 64);
    }

    if (!cfg->frameMode) {
        BL_WR_REG(CAM_BASE, CAM_DVP2AHB_ADDR_START_1, cfg->memStart1 & 0xFFFFFFF0);

        if (cfg->burstType == CAM_BURST_TYPE_SINGLE) {
            BL_WR_REG(CAM_BASE, CAM_DVP2AHB_MEM_BCNT_1, cfg->memSize1 / 4);
            BL_WR_REG(CAM_BASE, CAM_DVP2AHB_FRAME_BCNT_1, cfg->frameSize1 / 4);
        } else if (cfg->burstType == CAM_BURST_TYPE_INCR4) {
            BL_WR_REG(CAM_BASE, CAM_DVP2AHB_MEM_BCNT_1, cfg->memSize1 / 16);
            BL_WR_REG(CAM_BASE, CAM_DVP2AHB_FRAME_BCNT_1, cfg->frameSize1 / 16);
        } else if (cfg->burstType == CAM_BURST_TYPE_INCR8) {
            BL_WR_REG(CAM_BASE, CAM_DVP2AHB_MEM_BCNT_1, cfg->memSize1 / 32);
            BL_WR_REG(CAM_BASE, CAM_DVP2AHB_FRAME_BCNT_1, cfg->frameSize1 / 32);
        } else if (cfg->burstType == CAM_BURST_TYPE_INCR16) {
            BL_WR_REG(CAM_BASE, CAM_DVP2AHB_MEM_BCNT_1, cfg->memSize1 / 64);
            BL_WR_REG(CAM_BASE, CAM_DVP2AHB_FRAME_BCNT_1, cfg->frameSize1 / 64);
        }
    }

    /* Clear interrupt */
    BL_WR_REG(CAM_BASE, CAM_DVP_FRAME_FIFO_POP, 0xFFFF0);

#ifndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(CAM_IRQn, CAM_IRQHandler);
#endif
}

/****************************************************************************/ /**
 * @brief  Deinit camera module
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void CAM_Deinit(void)
{
    //GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_CAM);
}

/****************************************************************************/ /**
 * @brief  Enable camera module
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void CAM_Enable(void)
{
    uint32_t tmpVal;

    /* Enable camera module */
    tmpVal = BL_RD_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE);
    tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_DVP_ENABLE);
    BL_WR_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Disable camera module
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void CAM_Disable(void)
{
    uint32_t tmpVal;

    /* Disable camera module */
    tmpVal = BL_RD_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_DVP_ENABLE);
    BL_WR_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Camera clock gate function
 *
 * @param  enable: Enable or disable
 *
 * @return None
 *
*******************************************************************************/
void CAM_Clock_Gate(BL_Fun_Type enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CAM_REG_DVP_PIX_CLK_CG, enable);
    BL_WR_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Camera hsync crop function
 *
 * @param  start: Valid hsync start count
 * @param  end: Valid hsync end count
 *
 * @return None
 *
*******************************************************************************/
void CAM_Hsync_Crop(uint16_t start, uint16_t end)
{
    BL_WR_REG(CAM_BASE, CAM_HSYNC_CONTROL, (start << 16) + end);
}

/****************************************************************************/ /**
 * @brief  Camera vsync crop function
 *
 * @param  start: Valid vsync start count
 * @param  end: Valid vsync end count
 *
 * @return None
 *
*******************************************************************************/
void CAM_Vsync_Crop(uint16_t start, uint16_t end)
{
    BL_WR_REG(CAM_BASE, CAM_VSYNC_CONTROL, (start << 16) + end);
}

/****************************************************************************/ /**
 * @brief  Camera set total valid pix count in a line function
 *
 * @param  count: Count value
 *
 * @return None
 *
*******************************************************************************/
void CAM_Set_Hsync_Total_Count(uint16_t count)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(CAM_BASE, CAM_FRAME_SIZE_CONTROL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CAM_REG_TOTAL_HCNT, count);
    BL_WR_REG(CAM_BASE, CAM_FRAME_SIZE_CONTROL, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Camera set total valid line count in a frame function
 *
 * @param  count: Count value
 *
 * @return None
 *
*******************************************************************************/
void CAM_Set_Vsync_Total_Count(uint16_t count)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(CAM_BASE, CAM_FRAME_SIZE_CONTROL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CAM_REG_TOTAL_VCNT, count);
    BL_WR_REG(CAM_BASE, CAM_FRAME_SIZE_CONTROL, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Get one camera frame in interleave mode
 *
 * @param  info: Interleave mode camera frame infomation pointer
 *
 * @return None
 *
*******************************************************************************/
void CAM_Interleave_Get_Frame_Info(CAM_Interleave_Frame_Info *info)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(CAM_BASE, CAM_DVP_STATUS_AND_ERROR);

    info->validFrames = BL_GET_REG_BITS_VAL(tmpVal, CAM_FRAME_VALID_CNT_0);
    info->curFrameAddr = BL_RD_REG(CAM_BASE, CAM_FRAME_START_ADDR0_0);
    info->curFrameBytes = BL_RD_REG(CAM_BASE, CAM_FRAME_BYTE_CNT0_0);
    info->status = tmpVal;
}

/****************************************************************************/ /**
 * @brief  Get one camera frame in planar mode
 *
 * @param  info: Planar mode camera frame infomation pointer
 *
 * @return None
 *
*******************************************************************************/
void CAM_Planar_Get_Frame_Info(CAM_Planar_Frame_Info *info)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(CAM_BASE, CAM_DVP_STATUS_AND_ERROR);

    info->validFrames0 = BL_GET_REG_BITS_VAL(tmpVal, CAM_FRAME_VALID_CNT_0);
    info->validFrames1 = BL_GET_REG_BITS_VAL(tmpVal, CAM_FRAME_VALID_CNT_1);
    info->curFrameAddr0 = BL_RD_REG(CAM_BASE, CAM_FRAME_START_ADDR0_0);
    info->curFrameAddr1 = BL_RD_REG(CAM_BASE, CAM_FRAME_START_ADDR1_0);
    info->curFrameBytes0 = BL_RD_REG(CAM_BASE, CAM_FRAME_BYTE_CNT0_0);
    info->curFrameBytes1 = BL_RD_REG(CAM_BASE, CAM_FRAME_BYTE_CNT1_0);
    info->status = tmpVal;
}

/****************************************************************************/ /**
 * @brief  Get available count 0 of frames
 *
 * @param  None
 *
 * @return Frames count
 *
*******************************************************************************/
uint8_t CAM_Get_Frame_Count_0(void)
{
    return BL_GET_REG_BITS_VAL(BL_RD_REG(CAM_BASE, CAM_DVP_STATUS_AND_ERROR), CAM_FRAME_VALID_CNT_0);
}

/****************************************************************************/ /**
 * @brief  Get available count 1 of frames
 *
 * @param  None
 *
 * @return Frames count
 *
*******************************************************************************/
uint8_t CAM_Get_Frame_Count_1(void)
{
    return BL_GET_REG_BITS_VAL(BL_RD_REG(CAM_BASE, CAM_DVP_STATUS_AND_ERROR), CAM_FRAME_VALID_CNT_1);
}

/****************************************************************************/ /**
 * @brief  Pop one camera frame in interleave mode
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void CAM_Interleave_Pop_Frame(void)
{
    /* Pop one frame */
    BL_WR_REG(CAM_BASE, CAM_DVP_FRAME_FIFO_POP, 1);
}

/****************************************************************************/ /**
 * @brief  Pop one camera frame in planar mode
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void CAM_Planar_Pop_Frame(void)
{
    /* Pop one frame */
    BL_WR_REG(CAM_BASE, CAM_DVP_FRAME_FIFO_POP, 3);
}

/****************************************************************************/ /**
 * @brief  CAMERA Enable Disable Interrupt
 *
 * @param  intType: CAMERA Interrupt Type
 * @param  intMask: Enable or Disable
 *
 * @return None
 *
*******************************************************************************/
void CAM_IntMask(CAM_INT_Type intType, BL_Mask_Type intMask)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_CAM_INT_TYPE(intType));
    CHECK_PARAM(IS_BL_MASK_TYPE(intMask));

    tmpVal = BL_RD_REG(CAM_BASE, CAM_INT_CONTROL);

    switch (intType) {
        case CAM_INT_NORMAL_0:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_INT_NORMAL_0_EN);
            } else {
                /* Disable this interrupt */
                tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_INT_NORMAL_0_EN);
            }

            break;

        case CAM_INT_NORMAL_1:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_INT_NORMAL_1_EN);
            } else {
                /* Disable this interrupt */
                tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_INT_NORMAL_1_EN);
            }

            break;

        case CAM_INT_MEMORY_OVERWRITE_0:
        case CAM_INT_MEMORY_OVERWRITE_1:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_INT_MEM_EN);
            } else {
                /* Disable this interrupt */
                tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_INT_MEM_EN);
            }

            break;

        case CAM_INT_FRAME_OVERWRITE_0:
        case CAM_INT_FRAME_OVERWRITE_1:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_INT_FRAME_EN);
            } else {
                /* Disable this interrupt */
                tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_INT_FRAME_EN);
            }

            break;

        case CAM_INT_FIFO_OVERWRITE_0:
        case CAM_INT_FIFO_OVERWRITE_1:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_INT_FIFO_EN);
            } else {
                /* Disable this interrupt */
                tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_INT_FIFO_EN);
            }

            break;

        case CAM_INT_VSYNC_CNT_ERROR:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_INT_VCNT_EN);
            } else {
                /* Disable this interrupt */
                tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_INT_VCNT_EN);
            }

            break;

        case CAM_INT_HSYNC_CNT_ERROR:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_INT_HCNT_EN);
            } else {
                /* Disable this interrupt */
                tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_INT_HCNT_EN);
            }

            break;

        case CAM_INT_ALL:
            if (intMask == UNMASK) {
                /* Enable all interrupt */
                tmpVal |= 0x7F;
            } else {
                /* Disable all interrupt */
                tmpVal &= 0xFFFFFF80;
            }

            break;

        default:
            break;
    }

    BL_WR_REG(CAM_BASE, CAM_INT_CONTROL, tmpVal);
}

/****************************************************************************/ /**
 * @brief  CAMERA Interrupt Clear
 *
 * @param  intType: CAMERA Interrupt Type
 *
 * @return None
 *
*******************************************************************************/
void CAM_IntClr(CAM_INT_Type intType)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(CAM_BASE, CAM_DVP_FRAME_FIFO_POP);

    switch (intType) {
        case CAM_INT_NORMAL_0:
            tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_INT_NORMAL_CLR_0);
            break;

        case CAM_INT_NORMAL_1:
            tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_INT_NORMAL_CLR_1);
            break;

        case CAM_INT_MEMORY_OVERWRITE_0:
            tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_INT_MEM_CLR_0);
            break;

        case CAM_INT_MEMORY_OVERWRITE_1:
            tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_INT_MEM_CLR_1);
            break;

        case CAM_INT_FRAME_OVERWRITE_0:
            tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_INT_FRAME_CLR_0);
            break;

        case CAM_INT_FRAME_OVERWRITE_1:
            tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_INT_FRAME_CLR_1);
            break;

        case CAM_INT_FIFO_OVERWRITE_0:
            tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_INT_FIFO_CLR_0);
            break;

        case CAM_INT_FIFO_OVERWRITE_1:
            tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_INT_FIFO_CLR_1);
            break;

        case CAM_INT_VSYNC_CNT_ERROR:
            tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_INT_VCNT_CLR_0);
            break;

        case CAM_INT_HSYNC_CNT_ERROR:
            tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_INT_HCNT_CLR_0);
            break;

        case CAM_INT_ALL:
            tmpVal = 0xFFFF0;

        default:
            break;
    }

    BL_WR_REG(CAM_BASE, CAM_DVP_FRAME_FIFO_POP, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Install camera interrupt callback function
 *
 * @param  intType: CAMERA interrupt type
 * @param  cbFun: Pointer to interrupt callback function. The type should be void (*fn)(void)
 *
 * @return None
 *
*******************************************************************************/
void CAM_Int_Callback_Install(CAM_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */
    CHECK_PARAM(IS_CAM_INT_TYPE(intType));

    camIntCbfArra[intType] = cbFun;
}

/****************************************************************************/ /**
 * @brief  CAM hardware mode with frame start address wrap to memory address start function enable or disable
 *
 * @param  enable: Enable or disable
 * @return None
 *
*******************************************************************************/
void CAM_HW_Mode_Wrap(BL_Fun_Type enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CAM_REG_HW_MODE_FWRAP, enable);
    BL_WR_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Camera interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void CAM_IRQHandler(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(CAM_BASE, CAM_DVP_STATUS_AND_ERROR);

    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_NORMAL_INT_0)) {
        CAM_IntClr(CAM_INT_NORMAL_0);

        if (camIntCbfArra[CAM_INT_NORMAL_0] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_NORMAL_0]();
        }
    }

    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_NORMAL_INT_1)) {
        CAM_IntClr(CAM_INT_NORMAL_1);

        if (camIntCbfArra[CAM_INT_NORMAL_1] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_NORMAL_1]();
        }
    }

    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_MEM_INT_0)) {
        CAM_IntClr(CAM_INT_MEMORY_OVERWRITE_0);

        if (camIntCbfArra[CAM_INT_MEMORY_OVERWRITE_0] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_MEMORY_OVERWRITE_0]();
        }
    }

    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_MEM_INT_1)) {
        CAM_IntClr(CAM_INT_MEMORY_OVERWRITE_1);

        if (camIntCbfArra[CAM_INT_MEMORY_OVERWRITE_1] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_MEMORY_OVERWRITE_1]();
        }
    }

    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_FRAME_INT_0)) {
        CAM_IntClr(CAM_INT_FRAME_OVERWRITE_0);

        if (camIntCbfArra[CAM_INT_FRAME_OVERWRITE_0] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_FRAME_OVERWRITE_0]();
        }
    }

    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_FRAME_INT_1)) {
        CAM_IntClr(CAM_INT_FRAME_OVERWRITE_1);

        if (camIntCbfArra[CAM_INT_FRAME_OVERWRITE_1] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_FRAME_OVERWRITE_1]();
        }
    }

    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_FIFO_INT_0)) {
        CAM_IntClr(CAM_INT_FIFO_OVERWRITE_0);

        if (camIntCbfArra[CAM_INT_FIFO_OVERWRITE_0] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_FIFO_OVERWRITE_0]();
        }
    }

    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_FIFO_INT_1)) {
        CAM_IntClr(CAM_INT_FIFO_OVERWRITE_1);

        if (camIntCbfArra[CAM_INT_FIFO_OVERWRITE_1] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_FIFO_OVERWRITE_1]();
        }
    }

    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_HCNT_INT)) {
        CAM_IntClr(CAM_INT_HSYNC_CNT_ERROR);

        if (camIntCbfArra[CAM_INT_HSYNC_CNT_ERROR] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_HSYNC_CNT_ERROR]();
        }
    }

    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_VCNT_INT)) {
        CAM_IntClr(CAM_INT_VSYNC_CNT_ERROR);

        if (camIntCbfArra[CAM_INT_VSYNC_CNT_ERROR] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_VSYNC_CNT_ERROR]();
        }
    }
}
#endif

/*@} end of group CAM_Public_Functions */

/*@} end of group CAM */

/*@} end of group BL702_Peripheral_Driver */
