/*
 * Copyright (C) 2020-2021 Arm Limited or its affiliates. All rights reserved.
 * Copyright (c) 2019 Nuclei Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* ----------------------------------------------------------------------
 * Project:      NMSIS NN Library
 * Title:        riscv_nn_types.h
 * Description:  Public header file to contain the NMSIS-NN structs for the
 *               TensorFlowLite micro compliant functions
 *
 * $Date:        19. March 2021
 * $Revision:    V.2.0.0
 *
 * Target Processor: RISC-V Cores
 * -------------------------------------------------------------------- */

#ifndef _RISCV_NN_TYPES_H
#define _RISCV_NN_TYPES_H

#include <stdint.h>

/** NMSIS-NN object to contain the width and height of a tile */
typedef struct
{
    int32_t w; /**< Width */
    int32_t h; /**< Height */
} nmsis_nn_tile;

/** NMSIS-NN object used for the function context. */
typedef struct
{
    void *buf;    /**< Pointer to a buffer needed for the optimization */
    int32_t size; /**< Buffer size */
} nmsis_nn_context;

/** NMSIS-NN object to contain the dimensions of the tensors */
typedef struct
{
    int32_t n; /**< Generic dimension to contain either the batch size or output channels.
                     Please refer to the function documentation for more information */
    int32_t h; /**< Height */
    int32_t w; /**< Width */
    int32_t c; /**< Input channels */
} nmsis_nn_dims;

/** NMSIS-NN object for the per-channel quantization parameters */
typedef struct
{
    int32_t *multiplier; /**< Multiplier values */
    int32_t *shift;      /**< Shift values */
} nmsis_nn_per_channel_quant_params;

/** NMSIS-NN object for the per-tensor quantization parameters */
typedef struct
{
    int32_t multiplier; /**< Multiplier value */
    int32_t shift;      /**< Shift value */
} nmsis_nn_per_tensor_quant_params;

/** NMSIS-NN object for the quantized Relu activation */
typedef struct
{
    int32_t min; /**< Min value used to clamp the result */
    int32_t max; /**< Max value used to clamp the result */
} nmsis_nn_activation;

/** NMSIS-NN object for the convolution layer parameters */
typedef struct
{
    int32_t input_offset;  /**< Zero value for the input tensor */
    int32_t output_offset; /**< Zero value for the output tensor */
    nmsis_nn_tile stride;
    nmsis_nn_tile padding;
    nmsis_nn_tile dilation;
    nmsis_nn_activation activation;
} nmsis_nn_conv_params;

/** NMSIS-NN object for Depthwise convolution layer parameters */
typedef struct
{
    int32_t input_offset;  /**< Zero value for the input tensor */
    int32_t output_offset; /**< Zero value for the output tensor */
    int32_t ch_mult;       /**< Channel Multiplier. ch_mult * in_ch = out_ch */
    nmsis_nn_tile stride;
    nmsis_nn_tile padding;
    nmsis_nn_tile dilation;
    nmsis_nn_activation activation;
} nmsis_nn_dw_conv_params;
/** NMSIS-NN object for pooling layer parameters */
typedef struct
{
    nmsis_nn_tile stride;
    nmsis_nn_tile padding;
    nmsis_nn_activation activation;
} nmsis_nn_pool_params;

/** NMSIS-NN object for Fully Connected layer parameters */
typedef struct
{
    int32_t input_offset;  /**< Zero value for the input tensor */
    int32_t filter_offset; /**< Zero value for the filter tensor. Not used */
    int32_t output_offset; /**< Zero value for the output tensor */
    nmsis_nn_activation activation;
} nmsis_nn_fc_params;

/** NMSIS-NN object for SVDF layer parameters */
typedef struct
{
    int32_t rank;
    int32_t input_offset;  /**< Zero value for the input tensor */
    int32_t output_offset; /**< Zero value for the output tensor */
    nmsis_nn_activation input_activation;
    nmsis_nn_activation output_activation;
} nmsis_nn_svdf_params;

#endif // _RISCV_NN_TYPES_H
