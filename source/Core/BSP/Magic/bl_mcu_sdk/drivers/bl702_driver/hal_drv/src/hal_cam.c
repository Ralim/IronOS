/**
 * @file hal_cam.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include "hal_cam.h"
#include "bl702_cam.h"
#include "bl702_glb.h"

#ifdef BSP_USING_CAM0
static void CAM0_IRQ(void);
#endif

static cam_device_t camx_device[CAM_MAX_INDEX] = {
#ifdef BSP_USING_CAM0
    CAM0_CONFIG,
#endif
};

/**
 * @brief
 *
 * @param dev
 * @param oflag
 * @return int
 */
int cam_open(struct device *dev, uint16_t oflag)
{
    cam_device_t *cam_device = (cam_device_t *)dev;
    CAM_CFG_Type camera_cfg = { 0 };

    uint32_t tmpVal;
    /* Disable camera module */
    tmpVal = BL_RD_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_DVP_ENABLE);
    BL_WR_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE, tmpVal);

    camera_cfg.swMode = cam_device->software_mode;
    camera_cfg.frameMode = cam_device->frame_mode;
    camera_cfg.yuvMode = cam_device->yuv_format;
    camera_cfg.waitCount = 0x40;
    camera_cfg.linePol = cam_device->hsp;
    camera_cfg.framePol = cam_device->vsp;
    camera_cfg.burstType = CAM_BURST_TYPE_INCR16;
    camera_cfg.camSensorMode = CAM_SENSOR_MODE_V_AND_H;
    camera_cfg.memStart0 = cam_device->cam_write_ram_addr;
    camera_cfg.memSize0 = cam_device->cam_write_ram_size;
    camera_cfg.frameSize0 = cam_device->cam_frame_size;
    /* planar mode*/
    camera_cfg.memStart1 = cam_device->cam_write_ram_addr1;
    camera_cfg.memSize1 = cam_device->cam_write_ram_size1;
    camera_cfg.frameSize1 = cam_device->cam_frame_size1;
    CAM_Init(&camera_cfg);

    if (oflag & DEVICE_OFLAG_INT_RX) {
#ifdef BSP_USING_CAM0
        Interrupt_Handler_Register(CAM_IRQn, CAM0_IRQ);
#endif
    }

    return 0;
}

/**
 * @brief
 *
 * @param dev
 * @return int
 */
int cam_close(struct device *dev)
{
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_CAM);
    return 0;
}

/**
 * @brief
 *
 * @param dev
 * @param cmd
 * @param args
 * @return int
 */
int cam_control(struct device *dev, int cmd, void *args)
{
    cam_device_t *cam_device = (cam_device_t *)dev;

    switch (cmd) {
        case DEVICE_CTRL_SET_INT:
            if ((uint32_t)args == CAM_FRAME_IT) {
                CAM_IntMask(CAM_INT_NORMAL_0, UNMASK);
                CPU_Interrupt_Enable(CAM_IRQn);
            }

            break;
        case DEVICE_CTRL_CLR_INT:
            if ((uint32_t)args == CAM_FRAME_IT) {
                CAM_IntMask(CAM_INT_NORMAL_0, MASK);
                CPU_Interrupt_Disable(CAM_IRQn);
            }
            break;
        case DEVICE_CTRL_RESUME: {
            uint32_t tmpVal;

            /* Enable camera module */
            tmpVal = BL_RD_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE);
            tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_DVP_ENABLE);
            BL_WR_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE, tmpVal);
        } break;
        case DEVICE_CTRL_SUSPEND: {
            uint32_t tmpVal;

            /* Disable camera module */
            tmpVal = BL_RD_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE);
            tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_DVP_ENABLE);
            BL_WR_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE, tmpVal);
        } break;
        case DEVICE_CTRL_CAM_FRAME_CUT: {
            cam_frame_area_t *cfg = (cam_frame_area_t *)args;
            BL_WR_REG(CAM_BASE, CAM_HSYNC_CONTROL, ((cfg->x0 * 2) << 16) + (cfg->x1 * 2));
            BL_WR_REG(CAM_BASE, CAM_VSYNC_CONTROL, ((cfg->y0) << 16) + cfg->y1);

        } break;
        case DEVICE_CTRL_CAM_FRAME_DROP:
            if (cam_device->frame_mode == CAM_FRAME_INTERLEAVE_MODE) {
                /* Pop one frame */
                BL_WR_REG(CAM_BASE, CAM_DVP_FRAME_FIFO_POP, 1);
            } else {
                /* Pop one frame */
                BL_WR_REG(CAM_BASE, CAM_DVP_FRAME_FIFO_POP, 3);
            }
            break;
        case DEVICE_CTRL_CAM_FRAME_WRAP: {
            uint32_t tmpVal;

            tmpVal = BL_RD_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, CAM_REG_HW_MODE_FWRAP, ((uint32_t)args) & 0x01);
            BL_WR_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE, tmpVal);
        } break;
        default:
            break;
    }
    return 0;
}

/**
 * @brief
 *
 * @param dev
 * @param pos
 * @param buffer
 * @param size
 * @return int
 */
int cam_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size)
{
    if (!BL_GET_REG_BITS_VAL(BL_RD_REG(CAM_BASE, CAM_DVP_STATUS_AND_ERROR), CAM_FRAME_VALID_CNT_0)) {
        return -1;
    }

    cam_frame_info_t *frame_info = (cam_frame_info_t *)buffer;

    frame_info->frame_addr = BL_RD_REG(CAM_BASE, CAM_FRAME_START_ADDR0_0);
    frame_info->frame_count = BL_RD_REG(CAM_BASE, CAM_FRAME_BYTE_CNT0_0);

    return 0;
}
/**
 * @brief
 *
 * @param index
 * @param name
 * @return int
 */
int cam_register(enum cam_index_type index, const char *name)
{
    struct device *dev;

    if (CAM_MAX_INDEX == 0) {
        return -DEVICE_EINVAL;
    }

    dev = &(camx_device[index].parent);

    dev->open = cam_open;
    dev->close = cam_close;
    dev->control = cam_control;
    dev->write = NULL;
    dev->read = cam_read;

    dev->type = DEVICE_CLASS_CAMERA;
    dev->handle = NULL;

    return device_register(dev, name);
}

void cam_isr(cam_device_t *handle)
{
    uint32_t tmpVal;

    if (!handle->parent.callback)
        return;

    tmpVal = BL_RD_REG(CAM_BASE, CAM_DVP_STATUS_AND_ERROR);
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_NORMAL_INT_0)) {
        CAM_IntClr(CAM_INT_NORMAL_0);
        handle->parent.callback(&handle->parent, NULL, 0, CAM_EVENT_FRAME);
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_NORMAL_INT_1)) {
        CAM_IntClr(CAM_INT_NORMAL_1);
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_MEM_INT_0)) {
        CAM_IntClr(CAM_INT_MEMORY_OVERWRITE_0);
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_MEM_INT_1)) {
        CAM_IntClr(CAM_INT_MEMORY_OVERWRITE_1);
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_FRAME_INT_0)) {
        CAM_IntClr(CAM_INT_FRAME_OVERWRITE_0);
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_FRAME_INT_1)) {
        CAM_IntClr(CAM_INT_FRAME_OVERWRITE_1);
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_FIFO_INT_0)) {
        CAM_IntClr(CAM_INT_FIFO_OVERWRITE_0);
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_FIFO_INT_1)) {
        CAM_IntClr(CAM_INT_FIFO_OVERWRITE_1);
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_HCNT_INT)) {
        CAM_IntClr(CAM_INT_HSYNC_CNT_ERROR);
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_VCNT_INT)) {
        CAM_IntClr(CAM_INT_VSYNC_CNT_ERROR);
    }
}

#ifdef BSP_USING_CAM0
void CAM0_IRQ(void)
{
    cam_isr(&camx_device[CAM0_INDEX]);
}
#endif

/**
 * @brief
 *
 */
void cam_start(void)
{
    uint32_t tmpVal;

    /* Enable camera module */
    tmpVal = BL_RD_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE);
    tmpVal = BL_SET_REG_BIT(tmpVal, CAM_REG_DVP_ENABLE);
    BL_WR_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE, tmpVal);
}

/**
 * @brief
 *
 */
void cam_stop(void)
{
    uint32_t tmpVal;

    /* Disable camera module */
    tmpVal = BL_RD_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, CAM_REG_DVP_ENABLE);
    BL_WR_REG(CAM_BASE, CAM_DVP2AXI_CONFIGUE, tmpVal);
}

/**
 * @brief
 *
 * @param pic
 * @param len
 */
uint8_t cam_get_one_frame_interleave(uint8_t **pic, uint32_t *len)
{
    if (!BL_GET_REG_BITS_VAL(BL_RD_REG(CAM_BASE, CAM_DVP_STATUS_AND_ERROR), CAM_FRAME_VALID_CNT_0)) {
        return -1;
    }

    *pic = (uint8_t *)BL_RD_REG(CAM_BASE, CAM_FRAME_START_ADDR0_0);
    *len = BL_RD_REG(CAM_BASE, CAM_FRAME_BYTE_CNT0_0);
    return 0;
}

uint8_t cam_get_one_frame_planar(CAM_YUV_Mode_Type yuv, uint8_t **picYY, uint32_t *lenYY, uint8_t **picUV, uint32_t *lenUV)
{
    CAM_Planar_Frame_Info info;
    arch_memset(&info, 0, sizeof(info));

    CAM_Planar_Get_Frame_Info(&info);

    if (yuv == CAM_YUV400_EVEN || yuv == CAM_YUV400_ODD) {
        if (info.validFrames0 == 0 && info.validFrames1 == 0) {
            return ERROR;
        }
    } else {
        if (info.validFrames0 == 0 || info.validFrames1 == 0) {
            return ERROR;
        }
    }

    *picYY = (uint8_t *)(info.curFrameAddr0);
    *lenYY = info.curFrameBytes0;
    *picUV = (uint8_t *)(info.curFrameAddr1);
    *lenUV = info.curFrameBytes1;

    return SUCCESS;
}

void cam_drop_one_frame_interleave(void)
{
    /* Pop one frame */
    BL_WR_REG(CAM_BASE, CAM_DVP_FRAME_FIFO_POP, 1);
}

void cam_drop_one_frame_planar(void)
{
    /* Pop one frame */
    BL_WR_REG(CAM_BASE, CAM_DVP_FRAME_FIFO_POP, 3);
}