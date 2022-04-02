/*
 * Copyright (C) 2010-2018 Arm Limited or its affiliates. All rights reserved.
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

#ifndef _REF_FUNCTIONS_H_
#define _REF_FUNCTIONS_H_

#include "riscv_math.h"
#include "riscv_nnfunctions.h"
#include "riscv_nnsupportfunctions.h"
#include "fully_connected_testing_weights.h"

#ifdef __cplusplus
extern    "C"
{
#endif

/*
 *
 * Activation Functions
 *
 */
void riscv_nn_activations_direct_q7_ref(q7_t * data, uint16_t size, uint16_t int_width, riscv_nn_activation_type type);
void riscv_nn_activations_direct_q15_ref(q15_t * data, uint16_t size, uint16_t int_width, riscv_nn_activation_type type);
void riscv_relu_q7_ref(q7_t *data, uint16_t size);
void riscv_relu_q15_ref(q15_t *data, uint16_t size);
void riscv_relu6_s8_ref(q7_t *data, uint16_t size);
/*
 *
 * Basic math functions
 *
 */
riscv_status riscv_elementwise_add_s8_ref(const int8_t *input_1_vect,
                           const int8_t *input_2_vect,
                           const int32_t input_1_offset,
                           const int32_t input_1_mult,
                           const int32_t input_1_shift,
                           const int32_t input_2_offset,
                           const int32_t input_2_mult,
                           const int32_t input_2_shift,
                           const int32_t left_shift,
                           int8_t *output,
                           const int32_t out_offset,
                           const int32_t out_mult,
                           const int32_t out_shift,
                           const int32_t out_activation_min,
                           const int32_t out_activation_max,
                           const uint32_t block_size);
riscv_status riscv_elementwise_mul_s8_ref(const int8_t *input_1_vect,
                           const int8_t *input_2_vect,
                           const int32_t input_1_offset,
                           const int32_t input_2_offset,
                           int8_t *output,
                           const int32_t out_offset,
                           const int32_t out_mult,
                           const int32_t out_shift,
                           const int32_t out_activation_min,
                           const int32_t out_activation_max,
                           const uint32_t block_size);
/*
 *
 * Concatenation Functions
 *
 */
void riscv_concatenation_s8_w_ref(const int8_t *input,
                                const uint16_t input_x,
                                const uint16_t input_y,
                                const uint16_t input_z,
                                const uint16_t input_w,
                                int8_t *output,
                                const uint32_t offset_w);
void riscv_concatenation_s8_x_ref(const int8_t *input,
                                const uint16_t input_x,
                                const uint16_t input_y,
                                const uint16_t input_z,
                                const uint16_t input_w,
                                int8_t *output,
                                const uint16_t output_x,
                                const uint32_t offset_x);
void riscv_concatenation_s8_y_ref(const int8_t *input,
                                const uint16_t input_x,
                                const uint16_t input_y,
                                const uint16_t input_z,
                                const uint16_t input_w,
                                int8_t *output,
                                const uint16_t output_y,
                                const uint32_t offset_y);
void riscv_concatenation_s8_z_ref(const int8_t *input,
                                const uint16_t input_x,
                                const uint16_t input_y,
                                const uint16_t input_z,
                                const uint16_t input_w,
                                int8_t *output,
                                const uint16_t output_z,
                                const uint32_t offset_z);
/*
 *
 * Convolution Functions
 *
 */
riscv_status riscv_convolve_1_x_n_s8_ref(const nmsis_nn_context* ctx,
                                     const nmsis_nn_conv_params* conv_params,
                                     const nmsis_nn_per_channel_quant_params* quant_params,
                                     const nmsis_nn_dims* input_dims,
                                     const q7_t *input_data,
                                     const nmsis_nn_dims* filter_dims,
                                     const q7_t *filter_data,
                                     const nmsis_nn_dims* bias_dims,
                                     const int32_t *bias_data,
                                     const nmsis_nn_dims* output_dims,
                                     q7_t *output_data);

int32_t riscv_convolve_1_x_n_s8_get_buffer_size_ref(const nmsis_nn_dims* input_dims,
                                                  const nmsis_nn_dims* filter_dims);

riscv_status riscv_convolve_1x1_HWC_q7_fast_nonsquare_ref(const q7_t * Im_in,
                                                      const uint16_t dim_im_in_x,
                                                      const uint16_t dim_im_in_y,
                                                      const uint16_t ch_im_in,
                                                      const q7_t * wt,
                                                      const uint16_t ch_im_out,
                                                      const uint16_t dim_kernel_x,
                                                      const uint16_t dim_kernel_y,
                                                      const uint16_t padding_x,
                                                      const uint16_t padding_y,
                                                      const uint16_t stride_x,
                                                      const uint16_t stride_y,
                                                      const q7_t * bias,
                                                      const uint16_t bias_shift,
                                                      const uint16_t out_shift,
                                                      q7_t * Im_out,
                                                      const uint16_t dim_im_out_x,
                                                      const uint16_t dim_im_out_y,
                                                      q15_t * bufferA,
                                                      q7_t * bufferB);

riscv_status riscv_convolve_1x1_s8_fast_ref(const nmsis_nn_context *ctx,
                                        const nmsis_nn_conv_params *conv_params,
                                        const nmsis_nn_per_channel_quant_params *quant_params,
                                        const nmsis_nn_dims *input_dims,
                                        const q7_t *input_data,
                                        const nmsis_nn_dims *filter_dims,
                                        const q7_t *filter_data,
                                        const nmsis_nn_dims *bias_dims,
                                        const int32_t *bias_data,
                                        const nmsis_nn_dims *output_dims,
                                        q7_t *output_data);

int32_t riscv_convolve_1x1_s8_fast_get_buffer_size_ref(const nmsis_nn_dims *input_dims);

riscv_status riscv_convolve_HWC_q7_basic_nonsquare_ref(const q7_t * Im_in,
                                                   const uint16_t dim_im_in_x,
                                                   const uint16_t dim_im_in_y,
                                                   const uint16_t ch_im_in,
                                                   const q7_t * wt,
                                                   const uint16_t ch_im_out,
                                                   const uint16_t dim_kernel_x,
                                                   const uint16_t dim_kernel_y,
                                                   const uint16_t padding_x,
                                                   const uint16_t padding_y,
                                                   const uint16_t stride_x,
                                                   const uint16_t stride_y,
                                                   const q7_t * bias,
                                                   const uint16_t bias_shift,
                                                   const uint16_t out_shift,
                                                   q7_t * Im_out,
                                                   const uint16_t dim_im_out_x,
                                                   const uint16_t dim_im_out_y,
                                                   q15_t * bufferA,
                                                   q7_t * bufferB);

riscv_status
riscv_convolve_HWC_q7_basic_ref(const q7_t * Im_in,
                              const uint16_t dim_im_in,
                              const uint16_t ch_im_in,
                              const q7_t * wt,
                              const uint16_t ch_im_out,
                              const uint16_t dim_kernel,
                              const uint16_t padding,
                              const uint16_t stride,
                              const q7_t * bias,
                              const uint16_t bias_shift,
                              const uint16_t out_shift,
                              q7_t * Im_out,
                              const uint16_t dim_im_out,
                              q15_t * bufferA,
                              q7_t * bufferB);

riscv_status riscv_convolve_HWC_q7_fast_nonsquare_ref(const q7_t * Im_in,
                                                  const uint16_t dim_im_in_x,
                                                  const uint16_t dim_im_in_y,
                                                  const uint16_t ch_im_in,
                                                  const q7_t * wt,
                                                  const uint16_t ch_im_out,
                                                  const uint16_t dim_kernel_x,
                                                  const uint16_t dim_kernel_y,
                                                  const uint16_t padding_x,
                                                  const uint16_t padding_y,
                                                  const uint16_t stride_x,
                                                  const uint16_t stride_y,
                                                  const q7_t * bias,
                                                  const uint16_t bias_shift,
                                                  const uint16_t out_shift,
                                                  q7_t * Im_out,
                                                  const uint16_t dim_im_out_x,
                                                  const uint16_t dim_im_out_y,
                                                  q15_t * bufferA,
                                                  q7_t * bufferB);

riscv_status
riscv_convolve_HWC_q7_fast_ref(const q7_t * Im_in,
                             const uint16_t dim_im_in,
                             const uint16_t ch_im_in,
                             const q7_t * wt,
                             const uint16_t ch_im_out,
                             const uint16_t dim_kernel,
                             const uint16_t padding,
                             const uint16_t stride,
                             const q7_t * bias,
                             const uint16_t bias_shift,
                             const uint16_t out_shift,
                             q7_t * Im_out,
                             const uint16_t dim_im_out,
                             q15_t * bufferA,
                             q7_t * bufferB);

riscv_status
riscv_convolve_HWC_q7_RGB_ref(const q7_t * Im_in,
                            const uint16_t dim_im_in,
                            const uint16_t ch_im_in,
                            const q7_t * wt,
                            const uint16_t ch_im_out,
                            const uint16_t dim_kernel,
                            const uint16_t padding,
                            const uint16_t stride,
                            const q7_t * bias,
                            const uint16_t bias_shift,
                            const uint16_t out_shift,
                            q7_t * Im_out, const uint16_t dim_im_out, q15_t * bufferA, q7_t * bufferB);

riscv_status
riscv_convolve_HWC_q15_basic_ref(const q15_t * Im_in,
                               const uint16_t dim_im_in,
                               const uint16_t ch_im_in,
                               const q15_t * wt,
                               const uint16_t ch_im_out,
                               const uint16_t dim_kernel,
                               const uint16_t padding,
                               const uint16_t stride,
                               const q15_t * bias,
                               const uint16_t bias_shift,
                               const uint16_t out_shift,
                               q15_t * Im_out,
                               const uint16_t dim_im_out,
                               q15_t * bufferA,
                               q7_t * bufferB);

riscv_status
riscv_convolve_HWC_q15_fast_nonsquare_ref(const q15_t * Im_in,
                                        const uint16_t dim_im_in_x,
                                        const uint16_t dim_im_in_y,
                                        const uint16_t ch_im_in,
                                        const q15_t * wt,
                                        const uint16_t ch_im_out,
                                        const uint16_t dim_kernel_x,
                                        const uint16_t dim_kernel_y,
                                        const uint16_t padding_x,
                                        const uint16_t padding_y,
                                        const uint16_t stride_x,
                                        const uint16_t stride_y,
                                        const q15_t * bias,
                                        const uint16_t bias_shift,
                                        const uint16_t out_shift,
                                        q15_t * Im_out,
                                        const uint16_t dim_im_out_x,
                                        const uint16_t dim_im_out_y,
                                        q15_t * bufferA,
                                        q7_t * bufferB);

riscv_status
riscv_convolve_HWC_q15_fast_ref(const q15_t * Im_in,
                              const uint16_t dim_im_in,
                              const uint16_t ch_im_in,
                              const q15_t * wt,
                              const uint16_t ch_im_out,
                              const uint16_t dim_kernel,
                              const uint16_t padding,
                              const uint16_t stride,
                              const q15_t * bias,
                              const uint16_t bias_shift,
                              const uint16_t out_shift,
                              q15_t * Im_out,
                              const uint16_t dim_im_out,
                              q15_t * bufferA,
                              q7_t * bufferB);

riscv_status riscv_convolve_s8_ref(const nmsis_nn_context* ctx,
                               const nmsis_nn_conv_params* conv_params,
                               const nmsis_nn_per_channel_quant_params* quant_params,
                               const nmsis_nn_dims* input_dims,
                               const q7_t *input_data,
                               const nmsis_nn_dims* filter_dims,
                               const q7_t *filter_data,
                               const nmsis_nn_dims* bias_dims,
                               const int32_t *bias_data,
                               const nmsis_nn_dims* output_dims,
                               q7_t *output_data);

int32_t riscv_convolve_s8_get_buffer_size_ref(const nmsis_nn_dims* input_dims,
                                        const nmsis_nn_dims* filter_dims);

riscv_status riscv_convolve_wrapper_s8_ref(const nmsis_nn_context* ctx,
                                       const nmsis_nn_conv_params* conv_params,
                                       const nmsis_nn_per_channel_quant_params* quant_params,
                                       const nmsis_nn_dims* input_dims,
                                       const q7_t *input_data,
                                       const nmsis_nn_dims* filter_dims,
                                       const q7_t *filter_data,
                                       const nmsis_nn_dims* bias_dims,
                                       const int32_t *bias_data,
                                       const nmsis_nn_dims* output_dims,
                                       q7_t *output_data);

int32_t riscv_convolve_wrapper_s8_get_buffer_size_ref(const nmsis_nn_conv_params* conv_params,
                                                    const nmsis_nn_dims* input_dims,
                                                    const nmsis_nn_dims* filter_dims,
                                                    const nmsis_nn_dims* output_dims);

riscv_status riscv_depthwise_conv_3x3_s8_ref(const nmsis_nn_context *ctx,
                                         const nmsis_nn_dw_conv_params *dw_conv_params,
                                         const nmsis_nn_per_channel_quant_params *quant_params,
                                         const nmsis_nn_dims *input_dims,
                                         const q7_t *input,
                                         const nmsis_nn_dims *filter_dims,
                                         const q7_t *kernel,
                                         const nmsis_nn_dims *bias_dims,
                                         const int32_t *bias,
                                         const nmsis_nn_dims *output_dims,
                                         q7_t *output);

riscv_status riscv_depthwise_conv_s8_opt_ref(const nmsis_nn_context *ctx,
                                         const nmsis_nn_dw_conv_params *dw_conv_params,
                                         const nmsis_nn_per_channel_quant_params *quant_params,
                                         const nmsis_nn_dims *input_dims,
                                         const q7_t *input,
                                         const nmsis_nn_dims *filter_dims,
                                         const q7_t *kernel,
                                         const nmsis_nn_dims *bias_dims,
                                         const int32_t *bias,
                                         const nmsis_nn_dims *output_dims,
                                         q7_t *output);

int32_t riscv_depthwise_conv_s8_opt_get_buffer_size_ref(const nmsis_nn_dims *input_dims,
                                                      const nmsis_nn_dims *filter_dims);

static void depthwise_conv_s8_mult_4_ref(const int8_t *input,
                                         const int32_t input_x,
                                         const int32_t input_y,
                                         const int32_t input_ch,
                                         const int8_t *kernel,
                                         const int32_t output_ch,
                                         const int32_t ch_mult,
                                         const int32_t kernel_x,
                                         const int32_t kernel_y,
                                         const int32_t pad_x,
                                         const int32_t pad_y,
                                         const int32_t stride_x,
                                         const int32_t stride_y,
                                         const int32_t *bias,
                                         int8_t *output,
                                         const int32_t *output_shift,
                                         const int32_t *output_mult,
                                         const int32_t output_x,
                                         const int32_t output_y,
                                         const int32_t output_offset,
                                         const int32_t input_offset,
                                         const int32_t output_activation_min,
                                         const int32_t output_activation_max);

static void depthwise_conv_s8_generic_ref(const q7_t *input,
                                      const uint16_t input_x,
                                      const uint16_t input_y,
                                      const uint16_t input_ch,
                                      const q7_t *kernel,
                                      const uint16_t output_ch,
                                      const uint16_t ch_mult,
                                      const uint16_t kernel_x,
                                      const uint16_t kernel_y,
                                      const uint16_t pad_x,
                                      const uint16_t pad_y,
                                      const uint16_t stride_x,
                                      const uint16_t stride_y,
                                      const int32_t *bias,
                                      q7_t *output,
                                      const int32_t *output_shift,
                                      const int32_t *output_mult,
                                      const uint16_t output_x,
                                      const uint16_t output_y,
                                      const int32_t output_offset,
                                      const int32_t input_offset,
                                      const int32_t output_activation_min,
                                      const int32_t output_activation_max);

riscv_status riscv_depthwise_conv_s8_ref(const nmsis_nn_context *ctx,
                                 const nmsis_nn_dw_conv_params *dw_conv_params,
                                 const nmsis_nn_per_channel_quant_params *quant_params,
                                 const nmsis_nn_dims *input_dims,
                                 const q7_t *input,
                                 const nmsis_nn_dims *filter_dims,
                                 const q7_t *kernel,
                                 const nmsis_nn_dims *bias_dims,
                                 const int32_t *bias,
                                 const nmsis_nn_dims *output_dims,
                                 q7_t *output);

static void depthwise_conv_u8_mult_4_ref(const uint8_t *input,
                                         const int32_t input_x,
                                         const int32_t input_y,
                                         const int32_t input_ch,
                                         const uint8_t *kernel,
                                         const int32_t output_ch,
                                         const int32_t ch_mult,
                                         const int32_t kernel_x,
                                         const int32_t kernel_y,
                                         const int32_t pad_x,
                                         const int32_t pad_y,
                                         const int32_t stride_x,
                                         const int32_t stride_y,
                                         const int32_t *bias,
                                         uint8_t *output,
                                         const int32_t output_shift,
                                         const int32_t output_mult,
                                         const int32_t output_x,
                                         const int32_t output_y,
                                         const int32_t output_offset,
                                         const int32_t input_offset,
                                         const int32_t filter_offset,
                                         const int32_t output_activation_min,
                                         const int32_t output_activation_max);

static void depthwise_conv_u8_generic_ref(const uint8_t *input,
                                          const int32_t input_x,
                                          const int32_t input_y,
                                          const int32_t input_ch,
                                          const uint8_t *kernel,
                                          const int32_t output_ch,
                                          const int32_t ch_mult,
                                          const int32_t kernel_x,
                                          const int32_t kernel_y,
                                          const int32_t pad_x,
                                          const int32_t pad_y,
                                          const int32_t stride_x,
                                          const int32_t stride_y,
                                          const int32_t *bias,
                                          uint8_t *output,
                                          const int32_t output_shift,
                                          const int32_t output_mult,
                                          const int32_t output_x,
                                          const int32_t output_y,
                                          const int32_t output_offset,
                                          const int32_t input_offset,
                                          const int32_t filter_offset,
                                          const int32_t output_activation_min,
                                          const int32_t output_activation_max);

riscv_status riscv_depthwise_conv_u8_basic_ver1_ref(const uint8_t *input,
                                                const uint16_t input_x,
                                                const uint16_t input_y,
                                                const uint16_t input_ch,
                                                const uint8_t *kernel,
                                                const uint16_t kernel_x,
                                                const uint16_t kernel_y,
                                                const int16_t ch_mult,
                                                const int16_t pad_x,
                                                const int16_t pad_y,
                                                const int16_t stride_x,
                                                const int16_t stride_y,
                                                const int16_t dilation_x,
                                                const int16_t dilation_y,
                                                const int32_t *bias,
                                                const int32_t input_offset,
                                                const int32_t filter_offset,
                                                const int32_t output_offset,
                                                uint8_t *output,
                                                const uint16_t output_x,
                                                const uint16_t output_y,
                                                const int32_t output_activation_min,
                                                const int32_t output_activation_max,
                                                const int32_t output_shift,
                                                const int32_t output_mult);

riscv_status riscv_depthwise_conv_wrapper_s8_ref(const nmsis_nn_context *ctx,
                                             const nmsis_nn_dw_conv_params *dw_conv_params,
                                             const nmsis_nn_per_channel_quant_params *quant_params,
                                             const nmsis_nn_dims *input_dims,
                                             const q7_t *input,
                                             const nmsis_nn_dims *filter_dims,
                                             const q7_t *filter,
                                             const nmsis_nn_dims *bias_dims,
                                             const int32_t *bias,
                                             const nmsis_nn_dims *output_dims,
                                             q7_t *output);

int32_t riscv_depthwise_conv_wrapper_s8_get_buffer_size_ref(const nmsis_nn_dw_conv_params *dw_conv_params,
                                                          const nmsis_nn_dims *input_dims,
                                                          const nmsis_nn_dims *filter_dims,
                                                          const nmsis_nn_dims *output_dims);

riscv_status riscv_depthwise_separable_conv_HWC_q7_nonsquare_ref(const q7_t * Im_in,
                                                             const uint16_t dim_im_in_x,
                                                             const uint16_t dim_im_in_y,
                                                             const uint16_t ch_im_in,
                                                             const q7_t * wt,
                                                             const uint16_t ch_im_out,
                                                             const uint16_t dim_kernel_x,
                                                             const uint16_t dim_kernel_y,
                                                             const uint16_t padding_x,
                                                             const uint16_t padding_y,
                                                             const uint16_t stride_x,
                                                             const uint16_t stride_y,
                                                             const q7_t * bias,
                                                             const uint16_t bias_shift,
                                                             const uint16_t out_shift,
                                                             q7_t * Im_out,
                                                             const uint16_t dim_im_out_x,
                                                             const uint16_t dim_im_out_y,
                                                             q15_t * bufferA,
                                                             q7_t * bufferB);



q7_t *riscv_nn_depthwise_conv_s8_core_ref(const q7_t *row,
                                        const q15_t *col,
                                        const uint16_t num_ch,
                                        const int32_t *out_shift,
                                        const int32_t *out_mult,
                                        const int32_t out_offset,
                                        const int32_t activation_min,
                                        const int32_t activation_max,
                                        const uint16_t kernel_size,
                                        const int32_t *const output_bias,
                                        q7_t *out);

q7_t     *riscv_nn_mat_mult_kernel_q7_q15_ref(const q7_t * pA,
                                        const q15_t * pInBuffer,
                                        const uint16_t ch_im_out,
                                        const uint16_t numCol_A,
                                        const uint16_t bias_shift,
                                        const uint16_t out_shift,
                                        const q7_t * bias,
                                        q7_t * pOut);

q7_t     *riscv_nn_mat_mult_kernel_q7_q15_reordered_ref(const q7_t * pA,
                                                      const q15_t * pInBuffer,
                                                      const uint16_t ch_im_out,
                                                      const uint16_t numCol_A,
                                                      const uint16_t bias_shift,
                                                      const uint16_t out_shift,
                                                      const q7_t * bias,
                                                      q7_t * pOut);

q7_t     *riscv_nn_mat_mult_kernel_q7_reordered_ref(const q7_t * pA,
                                                  const q7_t * pInBuffer,
                                                  const uint16_t ch_im_out,
                                                  const uint16_t numCol_A,
                                                  const uint16_t bias_shift,
                                                  const uint16_t out_shift, 
                                                  const q7_t * bias, 
                                                  q7_t * pOut);

q7_t *riscv_nn_mat_mult_kernel_s8_s16_ref(const q7_t *input_a,
                                        const q15_t *input_b,
                                        const uint16_t output_ch,
                                        const int32_t *out_shift,
                                        const int32_t *out_mult,
                                        const int32_t out_offset,
                                        const int16_t activation_min,
                                        const int16_t activation_max,
                                        const uint16_t num_col_a,
                                        const int32_t *const output_bias,
                                        q7_t *out_0);

q7_t *riscv_nn_mat_mult_kernel_s8_s16_reordered_ref(const q7_t *input_a,
                                                  const q15_t *input_b,
                                                  const uint16_t output_ch,
                                                  const int32_t *out_shift,
                                                  const int32_t *out_mult,
                                                  const int32_t out_offset,
                                                  const int16_t activation_min,
                                                  const int16_t activation_max,
                                                  const uint16_t num_col_a,
                                                  const int32_t *const output_bias,
                                                  q7_t *out_0);

q7_t *riscv_nn_mat_mult_s8_ref(const q7_t *input_row,
                             const q7_t *input_col,
                             const uint16_t output_ch,
                             const uint16_t col_batches,
                             const int32_t *output_shift,
                             const int32_t *output_mult,
                             const int32_t out_offset,
                             const int32_t col_offset,
                             const int32_t row_offset,
                             const int16_t activation_min,
                             const int16_t activation_max,
                             const uint16_t row_len,
                             const int32_t *const bias,
                             q7_t *out);

/*
 *
 * Fully-connected Layer Functions
 *
 */
riscv_status
riscv_fully_connected_s8_ref(const nmsis_nn_context *ctx,
                           const nmsis_nn_fc_params *fc_params,
                           const nmsis_nn_per_tensor_quant_params *quant_params,
                           const nmsis_nn_dims *input_dims,
                           const q7_t *input,
                           const nmsis_nn_dims *filter_dims,
                           const q7_t *kernel,
                           const nmsis_nn_dims *bias_dims,
                           const int32_t *bias,
                           const nmsis_nn_dims *output_dims,
                           q7_t *output);

int32_t riscv_fully_connected_s8_get_buffer_size_ref(const nmsis_nn_dims *filter_dims);

/*
 *
 * Pooling Functions
 *
 */
riscv_status riscv_avgpool_s8_ref(const nmsis_nn_context *ctx,
                          const nmsis_nn_pool_params *pool_params,
                          const nmsis_nn_dims *input_dims,
                          const q7_t *src,
                          const nmsis_nn_dims *filter_dims,
                          const nmsis_nn_dims *output_dims,
                          q7_t *dst);
int32_t riscv_avgpool_s8_get_buffer_size_ref(const int dim_dst_width,
                                       const int ch_src);
static void compare_and_replace_if_larger_q7_ref(q7_t *base,
                                             const q7_t *target,
                                             int32_t length);
static void
clamp_output_ref(q7_t *source, int32_t length, const int32_t act_min, const int32_t act_max);
riscv_status
riscv_max_pool_s8_ref(const nmsis_nn_context *ctx,
                const nmsis_nn_pool_params *pool_params,
                const nmsis_nn_dims *input_dims,
                const q7_t *src,
                const nmsis_nn_dims *filter_dims,
                const nmsis_nn_dims *output_dims,
                q7_t *dst);
/*
 *
 * Reshape Functions
 *
 */
void riscv_reshape_s8_ref(const int8_t *input,
                    int8_t *output,
                    const uint32_t total_size);
/*
 *
 * Softmax Functions
 *
 */
void riscv_softmax_q7_ref(const q7_t * vec_in, const uint16_t dim_vec, q7_t * p_out );
void riscv_softmax_q15_ref(const q15_t * vec_in, const uint16_t dim_vec, q15_t * p_out);
void riscv_softmax_s8_ref(const int8_t *input,
                    const int32_t num_rows,
                    const int32_t row_size,
                    const int32_t mult,
                    const int32_t shift,
                    const int32_t diff_min,
                    int8_t *output);
void riscv_softmax_u8_ref(const uint8_t *input,
                    const int32_t num_rows,
                    const int32_t row_size,
                    const int32_t mult,
                    const int32_t shift,
                    const int32_t diff_min,
                    uint8_t *output);
void riscv_softmax_with_batch_q7_ref(const q7_t * vec_in, const uint16_t nb_batches,const uint16_t dim_vec, q7_t * p_out );
/*
 *
 * Neural Network Data Conversion Functions
 *
 */
void riscv_nn_accumulate_q7_to_q15_ref(q15_t *pDst, const q7_t *pSrc, uint32_t length);
void riscv_nn_add_q7_ref(const q7_t *input, q31_t *output, uint32_t block_size);
q7_t *riscv_nn_depthwise_conv_nt_t_padded_s8_ref(const q7_t *lhs,
                                               const q7_t *rhs,
                                               const int32_t input_offset,
                                               const uint16_t num_ch,
                                               const int32_t *out_shift,
                                               const int32_t *out_mult,
                                               const int32_t out_offset,
                                               const int32_t activation_min,
                                               const int32_t activation_max,
                                               const uint16_t row_x_col,
                                               const int32_t *const output_bias,
                                               q7_t *out);
q7_t *riscv_nn_depthwise_conv_nt_t_s8_ref(const q7_t *lhs,
                                        const q7_t *rhs,
                                        const int32_t input_offset,
                                        const uint16_t num_ch,
                                        const int32_t *out_shift,
                                        const int32_t *out_mult,
                                        const int32_t out_offset,
                                        const int32_t activation_min,
                                        const int32_t activation_max,
                                        const uint16_t row_x_col,
                                        const int32_t *const output_bias,
                                        q7_t *out);
riscv_status riscv_nn_mat_mul_core_1x_s8_ref(int32_t row_elements,
                                         const int8_t *row_base,
                                         const int8_t *col_base,
                                         int32_t *const sum_col,
                                         int32_t *const output);
riscv_status riscv_nn_mat_mul_core_4x_s8_ref(const int32_t row_elements,
                                         const int32_t offset,
                                         const int8_t *row_base,
                                         const int8_t *col_base,
                                         int32_t *const sum_col,
                                         int32_t *const output);
riscv_status riscv_nn_mat_mult_nt_t_s8_ref(const q7_t *lhs,
                                       const q7_t *rhs,
                                       const q31_t *bias,
                                       q7_t *dst,
                                       const int32_t *dst_multipliers,
                                       const int32_t *dst_shifts,
                                       const int32_t lhs_rows,
                                       const int32_t rhs_rows,
                                       const int32_t rhs_cols,
                                       const int32_t lhs_offset,
                                       const int32_t dst_offset,
                                       const int32_t activation_min,
                                       const int32_t activation_max);
void riscv_nn_mult_q7_ref(
  q7_t * pSrcA,
  q7_t * pSrcB,
  q7_t * pDst,
  const uint16_t out_shift,
  uint32_t blockSize);
void riscv_nn_mult_q15_ref(
  q15_t * pSrcA,
  q15_t * pSrcB,
  q15_t * pDst,
  const uint16_t out_shift,
  uint32_t blockSize);
riscv_status riscv_nn_vec_mat_mult_t_s8_ref(const q7_t *lhs,
                                        const q7_t *rhs,
                                        const q31_t *bias,
                                        q7_t *dst,
                                        const int32_t lhs_offset,
                                        const int32_t rhs_offset,
                                        const int32_t dst_offset,
                                        const int32_t dst_multiplier,
                                        const int32_t dst_shift,
                                        const int32_t rhs_cols,
                                        const int32_t rhs_rows,
                                        const int32_t activation_min,
                                        const int32_t activation_max);
void riscv_q7_to_q7_no_shift_ref(const q7_t * pSrc, q7_t * pDst, uint32_t blockSize);
void riscv_q7_to_q7_reordered_no_shift_ref(const q7_t * pSrc, q7_t * pDst, uint32_t blockSize);
void riscv_q7_to_q15_no_shift_ref(const q7_t * pSrc, q15_t * pDst, uint32_t blockSize);
void riscv_q7_to_q15_reordered_no_shift_ref(const q7_t * pSrc, q15_t * pDst, uint32_t blockSize);
void riscv_q7_to_q15_reordered_with_offset_ref(const q7_t *src, q15_t *dst, uint32_t block_size, q15_t offset);
void riscv_q7_to_q15_with_offset_ref(const q7_t *src,
                                   q15_t *dst,
                                   uint32_t block_size,
                                   q15_t offset);
    void      riscv_convolve_HWC_q7_ref(const q7_t * Im_in,   // input image
                                      const uint16_t dim_im_in, // input image dimention
                                      const uint16_t ch_im_in,  // number of input image channels
                                      const q7_t * wt,  // kernel weights 
                                      const uint16_t ch_im_out, // number of filters, i.e., output image channels
                                      const uint16_t dim_kernel,    // filter kernel size
                                      const uint16_t padding,   // padding sizes
                                      const uint16_t stride,    // stride
                                      const q7_t * bias,    // bias
                                      const uint16_t bias_shift, const uint16_t out_shift, q7_t * Im_out,   // output image
                                      const uint16_t dim_im_out,    // output image dimension
                                      q15_t * bufferA,  //buffer space for input
                                      q7_t * bufferB    //buffer space for output
        );

    void      riscv_convolve_HWC_q7_ref_nonsquare(const q7_t * Im_in, // input image
                                                const uint16_t dim_im_in_x, // input image dimention x
                                                const uint16_t dim_im_in_y, // input image dimention y
                                                const uint16_t ch_im_in,    // number of input image channels
                                                const q7_t * wt,    // kernel weights 
                                                const uint16_t ch_im_out,   // number of filters, i.e., output image channels
                                                const uint16_t dim_kernel_x,    // filter kernel size x
                                                const uint16_t dim_kernel_y,    // filter kernel size y
                                                const uint16_t padding_x,   // padding sizes x
                                                const uint16_t padding_y,   // padding sizes y
                                                const uint16_t stride_x,    // stride x
                                                const uint16_t stride_y,    // stride y
                                                const q7_t * bias,  // bias
                                                const uint16_t bias_shift, const uint16_t out_shift, q7_t * Im_out, // output image
                                                const uint16_t dim_im_out_x,    // output image dimension x
                                                const uint16_t dim_im_out_y,    // output image dimension y
                                                q15_t * bufferA,    //buffer space for input
                                                q7_t * bufferB  //buffer space for output
        );

    void      riscv_convolve_HWC_q15_ref(const q15_t * Im_in, // input image
                                       const uint16_t dim_im_in,    // input image dimention
                                       const uint16_t ch_im_in, // number of input image channels
                                       const q15_t * wt,    // kernel weights 
                                       const uint16_t ch_im_out,    // number of filters, i.e., output image channels
                                       const uint16_t dim_kernel,   // filter kernel size
                                       const uint16_t padding,  // padding sizes
                                       const uint16_t stride,   // stride
                                       const q15_t * bias,  // bias
                                       const uint16_t bias_shift, const uint16_t out_shift, q15_t * Im_out, // output image
                                       const uint16_t dim_im_out,   // output image dimension
                                       q15_t * bufferA, //buffer space for input
                                       q7_t * bufferB   //buffer space for output
        );
    void      riscv_convolve_HWC_q15_nonsquare_ref(const q15_t * Im_in,
                                                      const uint16_t dim_im_in_x,
                                                      const uint16_t dim_im_in_y,
                                                      const uint16_t ch_im_in,
                                                      const q15_t * wt,
                                                      const uint16_t ch_im_out,
                                                      const uint16_t dim_kernel_x,
                                                      const uint16_t dim_kernel_y,
                                                      const uint16_t padding_x,
                                                      const uint16_t padding_y,
                                                      const uint16_t stride_x,
                                                      const uint16_t stride_y,
                                                      const q15_t * bias,
                                                      const uint16_t bias_shift,
                                                      const uint16_t out_shift,
                                                      q15_t * Im_out,
                                                      const uint16_t dim_im_out_x,
                                                      const uint16_t dim_im_out_y, 
                                                      q15_t * bufferA, 
                                                      q7_t * bufferB);
													  
    void      riscv_depthwise_separable_conv_HWC_q7_ref(const q7_t * Im_in,   // input image
                                                      const uint16_t dim_im_in, // input image dimention
                                                      const uint16_t ch_im_in,  // number of input image channels
                                                      const q7_t * wt,  // kernel weights 
                                                      const uint16_t ch_im_out, // number of filters, i.e., output image channels
                                                      const uint16_t dim_kernel,    // filter kernel size
                                                      const uint16_t padding,   // padding sizes
                                                      const uint16_t stride,    // stride
                                                      const q7_t * bias,    // bias
                                                      const uint16_t bias_shift,    // amount of left-shift for bias
                                                      const uint16_t out_shift, // amount of right-shift for output
                                                      q7_t * Im_out,    // output image
                                                      const uint16_t dim_im_out,    // output image dimension
                                                      q15_t * bufferA,  //buffer space for input
                                                      q7_t * bufferB    //buffer space for output
        );
    void      riscv_depthwise_separable_conv_HWC_q7_ref_nonsquare(const q7_t * Im_in, // input image
                                                                const uint16_t dim_im_in_x, // input image dimention x
                                                                const uint16_t dim_im_in_y, // input image dimention y
                                                                const uint16_t ch_im_in,    // number of input image channels
                                                                const q7_t * wt,    // kernel weights 
                                                                const uint16_t ch_im_out,   // number of filters, i.e., output image channels
                                                                const uint16_t dim_kernel_x,    // filter kernel size x
                                                                const uint16_t dim_kernel_y,    // filter kernel size y
                                                                const uint16_t padding_x,   // padding sizes x
                                                                const uint16_t padding_y,   // padding sizes y
                                                                const uint16_t stride_x,    // stride x
                                                                const uint16_t stride_y,    // stride y
                                                                const q7_t * bias,  // bias
                                                                const uint16_t bias_shift,  // amount of left-shift for bias
                                                                const uint16_t out_shift,   // amount of right-shift for output
                                                                q7_t * Im_out,  // output image
                                                                const uint16_t dim_im_out_x,    // output image dimension x
                                                                const uint16_t dim_im_out_y,    // output image dimension y
                                                                q15_t * bufferA,    //buffer space for input
                                                                q7_t * bufferB  //buffer space for output
        );

/*
 *
 * Fully-connected reference implemenation
 *
 */

    void      riscv_fully_connected_q7_ref(const q7_t * pV,   // pointer to vector
                                         const q7_t * pM,   // pointer to matrix
                                         const uint16_t dim_vec,    // length of the vector
                                         const uint16_t num_of_rows,    // numCol of A
                                         const uint16_t bias_shift, // amount of left-shift for bias
                                         const uint16_t out_shift,  // amount of right-shift for output
                                         const q7_t * bias, q7_t * pOut,    // output operand
                                         q15_t * vec_buffer);

    void      riscv_fully_connected_q15_ref(const q15_t * pV, // pointer to vector
                                          const q15_t * pM, // pointer to matrix
                                          const uint16_t dim_vec,   // length of the vector
                                          const uint16_t num_of_rows,   // numCol of A
                                          const uint16_t bias_shift,    // amount of left-shift for bias
                                          const uint16_t out_shift, // amount of right-shift for output
                                          const q15_t * bias, q15_t * pOut, // output operand
                                          q15_t * vec_buffer);

    void      riscv_fully_connected_mat_q7_vec_q15_ref(const q15_t * pV,  // pointer to vector
                                                     const q7_t * pM,   // pointer to matrix
                                                     const uint16_t dim_vec,    // length of the vector
                                                     const uint16_t num_of_rows,    // numCol of A
                                                     const uint16_t bias_shift, // amount of left-shift for bias
                                                     const uint16_t out_shift,  // amount of right-shift for output
                                                     const q7_t * bias, q15_t * pOut,   // output operand
                                                     q15_t * vec_buffer);

    void      riscv_fully_connected_q7_opt_ref(const q7_t * pV,   // pointer to vector
                                             const q7_t * pM,   // pointer to matrix
                                             const uint16_t dim_vec,    // length of the vector
                                             const uint16_t num_of_rows,    // numCol of A
                                             const uint16_t bias_shift, // amount of left-shift for bias
                                             const uint16_t out_shift,  // amount of right-shift for output
                                             const q7_t * bias, q7_t * pOut,    // output operand
                                             q15_t * vec_buffer);

    void      riscv_fully_connected_q15_opt_ref(const q15_t * pV, // pointer to vector
                                              const q15_t * pM, // pointer to matrix
                                              const uint16_t dim_vec,   // length of the vector
                                              const uint16_t num_of_rows,   // numCol of A
                                              const uint16_t bias_shift,    // amount of left-shift for bias
                                              const uint16_t out_shift, // amount of right-shift for output
                                              const q15_t * bias, q15_t * pOut, // output operand
                                              q15_t * vec_buffer);

    void      riscv_fully_connected_mat_q7_vec_q15_opt_ref(const q15_t * pV,  // pointer to vector
                                                         const q7_t * pM,   // pointer to matrix
                                                         const uint16_t dim_vec,    // length of the vector
                                                         const uint16_t num_of_rows,    // numCol of A
                                                         const uint16_t bias_shift, // amount of left-shift for bias
                                                         const uint16_t out_shift,  // amount of right-shift for output
                                                         const q7_t * bias, q15_t * pOut,   // output operand
                                                         q15_t * vec_buffer);

/*
 *
 * Pooling reference implemenation
 *
 */

    void      riscv_avepool_q7_HWC_ref(const q7_t * Im_in,    // input image
                                     const uint16_t dim_im_in,  // input image dimension
                                     const uint16_t ch_im_in,   // number of input image channels
                                     const uint16_t dim_kernel, // window kernel size
                                     const uint16_t padding,    // padding sizes
                                     const uint16_t stride, // stride
                                     const uint16_t dim_im_out, // output image dimension
                                     q7_t * bufferA,    // a buffer for local storage
                                     q7_t * Im_out);

    void      riscv_maxpool_q7_HWC_ref(const q7_t * Im_in,    // input image
                                     const uint16_t dim_im_in,  // input image dimension
                                     const uint16_t ch_im_in,   // number of input image channels
                                     const uint16_t dim_kernel, // window kernel size
                                     const uint16_t padding,    // padding sizes
                                     const uint16_t stride, // stride
                                     const uint16_t dim_im_out, // output image dimension
                                     q7_t * bufferA,    // a buffer for local storage
                                     q7_t * Im_out);

/*
 *
 * SVD reference implemenation
 *
 */
riscv_status ref_nn_vec_mat_mult_t_svdf_s8(const q7_t *lhs,
                                         const q7_t *rhs,
                                         q15_t *dst,
                                         const int32_t lhs_offset,
                                         const int32_t rhs_offset,
                                         const int32_t dst_offset,
                                         const int32_t dst_multiplier,
                                         const int32_t dst_shift,
                                         const int32_t rhs_cols,
                                         const int32_t rhs_rows,
                                         const int32_t activation_min,
                                         const int32_t activation_max);

riscv_status ref_svdf_s8(const nmsis_nn_context *input_ctx,
                       const nmsis_nn_context *output_ctx,
                       const nmsis_nn_svdf_params *svdf_params,
                       const nmsis_nn_per_tensor_quant_params *input_quant_params,
                       const nmsis_nn_per_tensor_quant_params *output_quant_params,
                       const nmsis_nn_dims *input_dims,
                       const q7_t *input_data,
                       const nmsis_nn_dims *state_dims,
                       q15_t *state_data,
                       const nmsis_nn_dims *weights_feature_dims,
                       const q7_t *weights_feature_data,
                       const nmsis_nn_dims *weights_time_dims,
                       const q15_t *weights_time_data,
                       const nmsis_nn_dims *bias_dims,
                       const q31_t *bias_data,
                       const nmsis_nn_dims *output_dims,
                       q7_t *output_data);

/*
 *
 * Other reference implemenation
 *
 */

    void      riscv_relu_q7_ref(q7_t * data, uint16_t size);

    void      riscv_relu_q15_ref(q15_t * data, uint16_t size);

    void      riscv_nn_mult_q7_ref(q7_t * pSrcA, q7_t * pSrcB, q7_t * pDst, const uint16_t out_shift, uint32_t blockSize);

    void      riscv_nn_mult_q15_ref(q15_t * pSrcA, q15_t * pSrcB, q15_t * pDst, const uint16_t out_shift, uint32_t blockSize);

    void        riscv_relu6_s8_ref(q7_t *data, uint16_t size);
    

#ifdef __cplusplus
}
#endif

#endif
