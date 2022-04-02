/**
 * @file bsp_es8388.h
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

#ifndef __ES8388_H__
#define __ES8388_H__

#include "bflb_platform.h"

/** @addtogroup  BL702_STD_PERIPH_DRIVER
 *  @{
 */

/** @addtogroup  ES8388
 *  @{
 */

/** @defgroup  ES8388_Public_Types
 *  @{
 */

/**
 *  @brief ES8388 Role Matser Or Slave
 */
typedef enum {
    ES8388_MASTER, /*!< Master Mode */
    ES8388_SLAVE,  /*!< Slave Mode */
} ES8388_Role_Type;

/**
 *  @brief ES8388 Work Mode
 */
typedef enum {
    ES8388_CODEC_MDOE,     /*!< ES8388 work at codec mode */
    ES8388_RECORDING_MODE, /*!< ES8388 work at recording mode */
    ES8388_PLAY_BACK_MODE, /*!< ES8388 work at paly back mode */
    ES8388_BY_PASS_MODE,   /*!< ES8388 work at by pass mode */
} ES8388_Work_Mode;

/**
 *  @brief ES8388 Microphone input type
 */
typedef enum {
    ES8388_SINGLE_ENDED_MIC, /*!< Mic Single Input Mode */
    ES8388_DIFF_ENDED_MIC,   /*!< Mic Different Input Mode */
} ES8388_MIC_Input_Type;

/**
 *  @brief ES8388 Microphone pga sel type
 */
typedef enum {
    ES8388_MIC_PGA_0DB,  /*!< Mic PGA as 0db */
    ES8388_MIC_PGA_3DB,  /*!< Mic PGA as 3db */
    ES8388_MIC_PGA_6DB,  /*!< Mic PGA as 6db */
    ES8388_MIC_PGA_9DB,  /*!< Mic PGA as 9db */
    ES8388_MIC_PGA_12DB, /*!< Mic PGA as 12db */
    ES8388_MIC_PGA_15DB, /*!< Mic PGA as 15db */
    ES8388_MIC_PGA_18DB, /*!< Mic PGA as 18db */
    ES8388_MIC_PGA_21DB, /*!< Mic PGA as 21db */
    ES8388_MIC_PGA_24DB, /*!< Mic PGA as 24db */
} ES8388_MIC_Input_PGA_Type;

/**
 *  @brief ES8388 I2S Frame Type
 */
typedef enum {
    ES8388_STD_I2S_FRAME,       /*!< Standard I2S Frame */
    ES8388_LEFT_JUSTIFY_FRAME,  /*!< Left  Justify Frame */
    ES8388_RIGHT_JUSTIFY_FRAME, /*!< Right Justify Frame */
    ES8388_DSP_FRAME,           /*!< DSP Frame */
} ES8388_I2S_Frame_Type;

/**
 *  @brief ES8388 I2S Data Len Type
 */
typedef enum {
    ES8388_DATA_LEN_24, /*!< I2S Auido Data Len 24 */
    ES8388_DATA_LEN_20, /*!< I2S Auido Data Len 20 */
    ES8388_DATA_LEN_18, /*!< I2S Auido Data Len 18 */
    ES8388_DATA_LEN_16, /*!< I2S Auido Data Len 16 */
    ES8388_DATA_LEN_32, /*!< I2S Auido Data Len 32 */
} ES8388_I2S_Data_Width;

/**
 *  @brief ES8388_Cfg_Type
 */
typedef struct
{
    ES8388_Work_Mode work_mode;           /*!< ES8388 work mode */
    ES8388_Role_Type role;                /*!< ES8388 role */
    ES8388_MIC_Input_Type mic_input_mode; /*!< ES8388 mic input mode */
    ES8388_MIC_Input_PGA_Type mic_pga;    /*!< ES8388 mic PGA */
    ES8388_I2S_Frame_Type i2s_frame;      /*!< ES8388 I2S frame */
    ES8388_I2S_Data_Width data_width;     /*!< ES8388 I2S dataWitdh */
} ES8388_Cfg_Type;

/*@} end of group ES8388_Public_Types */

/** @defgroup  ES8388_Public_Constants
 *  @{
 */

/** @defgroup  ES8388_ROLE_TYPE
 *  @{
 */
#define IS_ES8388_ROLE_TYPE(type) (((type) == ES8388_MASTER) || \
                                   ((type) == ES8388_SLAVE))

/** @defgroup  ES8388_WORK_MODE
 *  @{
 */
#define IS_ES8388_WORK_MODE(type) (((type) == ES8388_CODEC_MDOE) ||     \
                                   ((type) == ES8388_RECORDING_MODE) || \
                                   ((type) == ES8388_PLAY_BACK_MODE) || \
                                   ((type) == ES8388_BY_PASS_MODE))

/** @defgroup  ES8388_MIC_INPUT_TYPE
 *  @{
 */
#define IS_ES8388_MIC_INPUT_TYPE(type) (((type) == ES8388_SINGLE_ENDED_MIC) || \
                                        ((type) == ES8388_DIFF_ENDED_MIC))

/** @defgroup  ES8388_MIC_INPUT_PGA_TYPE
 *  @{
 */
#define IS_ES8388_MIC_INPUT_PGA_TYPE(type) (((type) == ES8388_MIC_PGA_0DB) ||  \
                                            ((type) == ES8388_MIC_PGA_3DB) ||  \
                                            ((type) == ES8388_MIC_PGA_6DB) ||  \
                                            ((type) == ES8388_MIC_PGA_9DB) ||  \
                                            ((type) == ES8388_MIC_PGA_12DB) || \
                                            ((type) == ES8388_MIC_PGA_15DB) || \
                                            ((type) == ES8388_MIC_PGA_18DB) || \
                                            ((type) == ES8388_MIC_PGA_21DB) || \
                                            ((type) == ES8388_MIC_PGA_24DB))

/** @defgroup  ES8388_I2S_FRAME_TYPE
 *  @{
 */
#define IS_ES8388_I2S_FRAME_TYPE(type) (((type) == ES8388_STD_I2S_FRAME) ||       \
                                        ((type) == ES8388_LEFT_JUSTIFY_FRAME) ||  \
                                        ((type) == ES8388_RIGHT_JUSTIFY_FRAME) || \
                                        ((type) == ES8388_DSP_FRAME))

/** @defgroup  ES8388_I2S_DATA_WIDTH
 *  @{
 */
#define IS_ES8388_I2S_DATA_WIDTH(type) (((type) == ES8388_DATA_LEN_24) || \
                                        ((type) == ES8388_DATA_LEN_20) || \
                                        ((type) == ES8388_DATA_LEN_18) || \
                                        ((type) == ES8388_DATA_LEN_16) || \
                                        ((type) == ES8388_DATA_LEN_32))

/*@} end of group ES8388_Public_Constants */

/** @defgroup  ES8388_Public_Macros
 *  @{
 */

/*@} end of group ES8388_Public_Macros */

/** @defgroup  ES8388_Public_Functions
 *  @{
 */
void ES8388_Init(ES8388_Cfg_Type *cfg);
void ES8388_Reg_Dump(void);
int ES8388_Set_Voice_Volume(int volume);

/*@} end of group ES8388_Public_Functions */

/*@} end of group ES8388 */

/*@} end of group BL702_STD_PERIPH_DRIVER */

#endif /* __ES8388_H__ */
