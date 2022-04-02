#include "riscv_nnexamples_nn_test.h"
#include "bench.h"
#include <stdio.h>

// New ones and Old ones can not be enabled at same time

//------------Old ones------------
//#define TEST_SIGMOID
//#define TEST_TANH
// #define TEST_POOL
// #define TEST_RELU
// #define TEST_IP
// #define TEST_CONV
// #define TEST_NONSQUARE
// #define TEST_NNMULT

//------------New ones------------
#define TEST_Activation
//#define TEST_BasicMath
//#define TEST_Concatenation
//#define TEST_Convolution_part1
//#define TEST_Convolution_part2
//#define TEST_Convolution_part3
//#define TEST_Pooling

//#define TEST_Softmax
//#define TEST_NNDataConversion
//#define TEST_BasicMathforNN

//TODO
//#define TEST_SVD //TODO memory limit
//#define TEST_Fully_connectedLayer // TODO memory limit

// #define TEST_Reshape
int test_index = 0;
q7_t test_flags[100];
bool test_pass;

int main()
{
    // common pointers for testing data
    q7_t *test1;
    q15_t *test2;
    q7_t *test3;
    q15_t *test4;

    BENCH_INIT;

    for (test_index = 0; test_index < 100; test_index++) {
        test_flags[test_index] = -1;
    }
    test_index = 0;

#ifdef TEST_Activation
#define Activation_SIZE 128
    test1 = new q7_t[Activation_SIZE];
    test2 = new q15_t[Activation_SIZE];
    test3 = new q7_t[Activation_SIZE];
    test4 = new q15_t[Activation_SIZE];

    for (int i = 0; i < Activation_SIZE; i++) {
        test1[i] = (rand() % 256 - 128);
        test3[i] = test1[i];
        test2[i] = (rand() % 65536 - 32768);
        test4[i] = test2[i];
    }

    printf("Start opt activations_direct q7 implementation\n");

    BENCH_START(riscv_nn_activations_direct_q7);
    riscv_nn_activations_direct_q7(test1, Activation_SIZE, 3, RISCV_SIGMOID);
    BENCH_END(riscv_nn_activations_direct_q7);

    printf("Start ref activations_direct q7 implementation\n");

    BENCH_START(riscv_nn_activations_direct_q7_ref);
    riscv_nn_activations_direct_q7_ref(test3, Activation_SIZE, 3, RISCV_SIGMOID);
    BENCH_END(riscv_nn_activations_direct_q7_ref);

    verify_results_q7(test1, test3, Activation_SIZE);

    printf("Start opt activations_direct q15 implementation\n");

    BENCH_START(riscv_nn_activations_direct_q15);
    riscv_nn_activations_direct_q15(test2, Activation_SIZE, 3, RISCV_SIGMOID);
    BENCH_END(riscv_nn_activations_direct_q15);

    printf("Start ref activations_direct q15 implementation\n");

    BENCH_START(riscv_nn_activations_direct_q15_ref);
    riscv_nn_activations_direct_q15_ref(test4, Activation_SIZE, 3, RISCV_SIGMOID);
    BENCH_END(riscv_nn_activations_direct_q15_ref);

    verify_results_q15(test2, test4, Activation_SIZE);

    q7_t *relu_ref_data_q7 = test1;
    q7_t *relu_opt_data_q7 = test3;
    q15_t *relu_ref_data_q15 = test2;
    q15_t *relu_opt_data_q15 = test4;

    printf("Start ref relu q7 implementation\n");

    BENCH_START(riscv_relu_q7_ref);
    riscv_relu_q7_ref(relu_ref_data_q7, Activation_SIZE);
    BENCH_END(riscv_relu_q7_ref);

    printf("Start opt relu q7 implementation\n");

    BENCH_START(riscv_relu_q7);
    riscv_relu_q7(relu_opt_data_q7, Activation_SIZE);
    BENCH_END(riscv_relu_q7);

    verify_results_q7(relu_ref_data_q7, relu_opt_data_q7, Activation_SIZE);

    printf("Start ref relu q15 implementation\n");

    BENCH_START(riscv_relu_q15_ref);
    riscv_relu_q15_ref(relu_ref_data_q15, Activation_SIZE);
    BENCH_END(riscv_relu_q15_ref);

    printf("Start opt relu q15 implementation\n");

    BENCH_START(riscv_relu_q15);
    riscv_relu_q15(relu_opt_data_q15, Activation_SIZE);
    BENCH_END(riscv_relu_q15);

    verify_results_q15(relu_ref_data_q15, relu_opt_data_q15, Activation_SIZE);

    printf("Start ref relu s8 implementation\n");

    BENCH_START(riscv_relu_s8_ref);
    riscv_relu6_s8_ref(relu_ref_data_q7, Activation_SIZE / 4);
    BENCH_END(riscv_relu_s8_ref);

    printf("Start opt relu s8 implementation\n");

    BENCH_START(riscv_relu_s8);
    riscv_relu6_s8(relu_opt_data_q7, Activation_SIZE / 4);
    BENCH_END(riscv_relu_s8);

    verify_results_q7(relu_ref_data_q7, relu_opt_data_q7, Activation_SIZE - (Activation_SIZE % 4));

    // delete[]test1;
    // delete[]test2;
    // delete[]test3;
    // delete[]test4;

#endif

#ifdef TEST_BasicMath
#define BasicMath_SIZE 200
    test1 = new q7_t[BasicMath_SIZE * 2];
    test3 = new q7_t[BasicMath_SIZE * 2];

    for (int i = 0; i < BasicMath_SIZE * 2; i++) {
        test1[i] = (rand() % 256 - 128);
    }

    for (int i = 0; i < BasicMath_SIZE; i++) {
        test3[i] = test3[i + BasicMath_SIZE] = 0;
    }

    printf("Start ref elementwise_add s8 implementation\n");

    BENCH_START(riscv_elementwise_add_s8_ref);
    riscv_elementwise_add_s8_ref(test1, test1 + BasicMath_SIZE, 9, 1073741824, 0, -5, 1073741824, -1, 20, test3, 14, 1073741824, -18, -128, 128, 32);
    BENCH_END(riscv_elementwise_add_s8_ref);

    printf("Start opt elementwise_add s8 implementation\n");

    BENCH_START(riscv_elementwise_add_s8);
    riscv_elementwise_add_s8(test1, test1 + BasicMath_SIZE, 9, 1073741824, 0, -5, 1073741824, -1, 20, test3 + BasicMath_SIZE, 14, 1073741824, -18, -128, 128, 32);
    BENCH_END(riscv_elementwise_add_s8);

    verify_results_q7(test3, test3 + BasicMath_SIZE, 32);

    printf("Start ref elementwise_mul s8 implementation\n");

    BENCH_START(riscv_elementwise_mul_s8_ref);
    riscv_elementwise_mul_s8_ref(test1, test1 + BasicMath_SIZE, 0, 0, test3, 0, 1, 0, -100, 100, BasicMath_SIZE);
    BENCH_END(riscv_elementwise_mul_s8_ref);

    printf("Start opt elementwise_mul s8 implementation\n");

    BENCH_START(riscv_elementwise_mul_s8);
    riscv_elementwise_mul_s8(test1, test1 + BasicMath_SIZE, 0, 0, test3 + BasicMath_SIZE, 0, 1, 0, -100, 100, BasicMath_SIZE);
    BENCH_END(riscv_elementwise_mul_s8);

    verify_results_q7(test3, test3 + BasicMath_SIZE, BasicMath_SIZE);

    // delete[]test1;
    // delete[]test3;

#endif

#ifdef TEST_Concatenation
#define Concatenation_SIZE 100
    test1 = new q7_t[Concatenation_SIZE];
    test3 = new q7_t[Concatenation_SIZE * 2];

    for (int i = 0; i < Concatenation_SIZE; i++) {
        test1[i] = (rand() % 256 - 128);
    }

    printf("Start ref concatenation s8 w implementation\n");

    BENCH_START(riscv_concatenation_s8_w_ref);
    riscv_concatenation_s8_w_ref(test1, 10, 2, 2, 2, test3, 0);
    BENCH_END(riscv_concatenation_s8_w_ref);

    printf("Start opt concatenation s8 w implementation\n");

    BENCH_START(riscv_concatenation_s8_w);
    riscv_concatenation_s8_w(test1, 10, 2, 2, 2, test3 + Concatenation_SIZE, 0);
    BENCH_END(riscv_concatenation_s8_w);

    verify_results_q7(test3, test3 + Concatenation_SIZE, 80);

    printf("Start ref concatenation s8 x implementation\n");

    BENCH_START(riscv_concatenation_s8_x_ref);
    riscv_concatenation_s8_x_ref(test1, 10, 2, 2, 2, test3, 10, 0);
    BENCH_END(riscv_concatenation_s8_x_ref);

    printf("Start opt concatenation s8 x implementation\n");

    BENCH_START(riscv_concatenation_s8_x);
    riscv_concatenation_s8_x(test1, 10, 2, 2, 2, test3 + Concatenation_SIZE, 10, 0);
    BENCH_END(riscv_concatenation_s8_x);

    verify_results_q7(test3, test3 + Concatenation_SIZE, 80);

    printf("Start ref concatenation s8 y implementation\n");

    BENCH_START(riscv_concatenation_s8_y_ref);
    riscv_concatenation_s8_y_ref(test1, 10, 2, 2, 2, test3, 2, 0);
    BENCH_END(riscv_concatenation_s8_y_ref);

    printf("Start opt concatenation s8 y implementation\n");

    BENCH_START(riscv_concatenation_s8_y);
    riscv_concatenation_s8_y(test1, 10, 2, 2, 2, test3 + Concatenation_SIZE, 2, 0);
    BENCH_END(riscv_concatenation_s8_y);

    verify_results_q7(test3, test3 + Concatenation_SIZE, 80);

    printf("Start ref concatenation s8 z implementation\n");

    BENCH_START(riscv_concatenation_s8_z_ref);
    riscv_concatenation_s8_z_ref(test1, 10, 2, 2, 2, test3, 2, 0);
    BENCH_END(riscv_concatenation_s8_z_ref);

    printf("Start opt concatenation s8 z implementation\n");

    BENCH_START(riscv_concatenation_s8_z);
    riscv_concatenation_s8_z(test1, 10, 2, 2, 2, test3 + Concatenation_SIZE, 2, 0);
    BENCH_END(riscv_concatenation_s8_z);

    verify_results_q7(test3, test3 + Concatenation_SIZE, 80);

    delete[] test1;
    delete[] test3;

#endif

#ifdef TEST_Convolution_part1
#define Convolution_SIZE 2000
    test1 = new q7_t[Convolution_SIZE * 4];
    test3 = new q7_t[Convolution_SIZE * 2];
    test4 = new q15_t[Convolution_SIZE * 2];

    q7_t *temp_buffer = new q7_t[Convolution_SIZE];
    // int32_t multiplier = 1082196480;
    // int32_t shift = -4;
    int32_t multiplier = 1374389535;
    int32_t shift = -5;
    int32_t *test7 = new int32_t[Convolution_SIZE * 2];
    for (int i = 0; i < Convolution_SIZE; i++) {
        test7[i] = (rand() % 256 - 128);
        test7[i + Convolution_SIZE] = test7[i];
        test1[i] = (rand() % 256 - 128);
        test1[i + Convolution_SIZE] = test1[i];
        test1[i + Convolution_SIZE * 2] = test1[i];
        test1[i + Convolution_SIZE * 3] = test1[i];
    }
    q7_t input[32] = { 60, 57, 62, 68, 56, 34, 34, 42, 63, 57, 28, 26, 36, 24, 7, 25, 31, 0, 29, 35, 1, 11, 1, 30, 31, 22, 17, 9, 19, 35, -2, 1 };
    q7_t conv_input[16] = { -85, -85, -85, -85, -43, -43, -43, -43, -85, -43, 0, 42, -85, -43, 0, 42 };
    q7_t conv_filter[12] = { 32, 64, 95, 127, -127, 127, -127, 127, -127, -127, 127, 127 };
    int32_t conv_bias[3] = { 1349, 10795, -16193 };

    for (int i = 0; i < Convolution_SIZE * 2; i++) {
        test3[i] = (rand() % 256 - 128);
        test3[i + Convolution_SIZE] = test3[i];
    }
    nmsis_nn_context ctx = { temp_buffer, 0 };
    // nmsis_nn_tile stride = {2,2}, padding = {0,0}, dilation = {1,1};
    nmsis_nn_tile stride = { 1, 1 }, padding = { 0, 0 }, dilation = { 1, 1 };
    //    stride = padding = dilation = {2,2};
    nmsis_nn_activation activation = { -128, 127 };
    nmsis_nn_conv_params conv_params = { 128, -128, stride, padding, dilation, activation };
    nmsis_nn_per_channel_quant_params quant_params = { &multiplier, &shift };
    nmsis_nn_dims input_dims = { 1, 1, 1, 32 };
    nmsis_nn_dims filter_dims = { 32, 1, 1, 32 };
    nmsis_nn_dims bias_dims = { 1, 1, 1, 32 };
    nmsis_nn_dims output_dims = { 1, 1, 1, 32 };
    nmsis_nn_dw_conv_params dw_conv_params = { 0, 0, 1, stride, padding, dilation, activation };
    printf("Start ref convolve_1_x_n s8 implementation\n");

    BENCH_START(riscv_convolve_1_x_n_s8_ref);
    riscv_convolve_1_x_n_s8_ref(&ctx, &conv_params, &quant_params, &input_dims, test1, &filter_dims, test1 + Convolution_SIZE, &bias_dims, test7, &output_dims, test3);
    BENCH_END(riscv_convolve_1_x_n_s8_ref);

    printf("Start opt convolve_1_x_n s8 implementation\n");

    BENCH_START(riscv_convolve_1_x_n_s8);
    riscv_convolve_1_x_n_s8(&ctx, &conv_params, &quant_params, &input_dims, test1, &filter_dims, test1 + Convolution_SIZE, &bias_dims, test7 + Convolution_SIZE, &output_dims, test3);
    BENCH_END(riscv_convolve_1_x_n_s8);

    verify_results_int32(test7, test7 + Convolution_SIZE, 320);

    for (int i = 0; i < Convolution_SIZE; i++) {
        test3[i] = 0;
        test3[i + Convolution_SIZE] = test3[i];
    }
    //This function may have problem
    printf("Start ref convolve_1x1_HWC_fast_nonsquare q7 implementation\n");

    BENCH_START(riscv_convolve_1x1_HWC_q7_fast_nonsquare_ref);
    riscv_convolve_1x1_HWC_q7_fast_nonsquare_ref(test1, 1, 1, 40, test1 + Convolution_SIZE, 40, 1, 1, 0, 0, 1, 1, test1 + Convolution_SIZE * 2, 0, 0, test3, 2, 2, test4, test1 + Convolution_SIZE * 3);
    BENCH_END(riscv_convolve_1x1_HWC_q7_fast_nonsquare_ref);

    printf("Start opt convolve_1x1_HWC_fast_nonsquare q7 implementation\n");
    BENCH_START(riscv_convolve_1x1_HWC_q7_fast_nonsquare);
    riscv_convolve_1x1_HWC_q7_fast_nonsquare(test1, 1, 1, 40, test1 + Convolution_SIZE, 40, 1, 1, 0, 0, 1, 1, test1 + Convolution_SIZE * 2, 0, 0, test3 + Convolution_SIZE, 2, 2, test4, test1 + Convolution_SIZE * 3);
    BENCH_END(riscv_convolve_1x1_HWC_q7_fast_nonsquare);

    verify_results_q7(test3, test3 + Convolution_SIZE, 40);

    printf("Start ref convolve s8 implementation\n");

    BENCH_START(riscv_convolve_s8_ref);
    riscv_convolve_s8_ref(&ctx, &conv_params, &quant_params, &input_dims, conv_input, &filter_dims, conv_filter, &bias_dims, conv_bias, &output_dims, test3);
    BENCH_END(riscv_convolve_s8_ref);

    printf("Start opt convolve s8 implementation\n");

    BENCH_START(riscv_convolve_s8);
    riscv_convolve_s8(&ctx, &conv_params, &quant_params, &input_dims, conv_input, &filter_dims, conv_filter, &bias_dims, conv_bias, &output_dims, test3 + Convolution_SIZE);
    BENCH_END(riscv_convolve_s8);
    verify_results_q7(test3, test3 + Convolution_SIZE, 12);
    filter_dims.w = 3;
    filter_dims.h = 3;
    dw_conv_params.padding.w = 0;
    input_dims.c = 4;
    printf("Start ref depthwise_conv_3x3 s8 implementation\n");

    BENCH_START(riscv_depthwise_conv_3x3_s8_ref);
    riscv_depthwise_conv_3x3_s8_ref(&ctx, &dw_conv_params, &quant_params, &input_dims, test1, &filter_dims, test1 + Convolution_SIZE, &bias_dims, test7, &output_dims, test3);
    BENCH_END(riscv_depthwise_conv_3x3_s8_ref);

    printf("Start opt depthwise_conv_3x3 s8 implementation\n");

    BENCH_START(riscv_depthwise_conv_3x3_s8);
    riscv_depthwise_conv_3x3_s8(&ctx, &dw_conv_params, &quant_params, &input_dims, test1, &filter_dims, test1 + Convolution_SIZE, &bias_dims, test7 + Convolution_SIZE, &output_dims, test3);
    BENCH_END(riscv_depthwise_conv_3x3_s8);

    verify_results_int32(test7, test7 + Convolution_SIZE, 360);
    dw_conv_params.ch_mult = 3;

    printf("Start ref depthwise_conv_s8 s8 implementation\n");

    BENCH_START(riscv_depthwise_conv_s8_ref);
    riscv_depthwise_conv_s8_ref(&ctx, &dw_conv_params, &quant_params, &input_dims, test1, &filter_dims, test1 + Convolution_SIZE, &bias_dims, test7, &output_dims, test3);
    BENCH_END(riscv_depthwise_conv_s8_ref);

    printf("Start opt depthwise_conv_s8 s8 implementation\n");

    BENCH_START(riscv_depthwise_conv_s8);
    riscv_depthwise_conv_s8(&ctx, &dw_conv_params, &quant_params, &input_dims, test1, &filter_dims, test1 + Convolution_SIZE, &bias_dims, test7 + Convolution_SIZE, &output_dims, test3);
    BENCH_END(riscv_depthwise_conv_s8);

    verify_results_int32(test7, test7 + Convolution_SIZE, 140);
    dw_conv_params.ch_mult = 4;

    printf("Start ref depthwise_conv_s8_opt s8 implementation\n");

    for (int i = 0; i < Convolution_SIZE; i++) {
        test3[i] = 0;
        test3[i + Convolution_SIZE] = test3[i];
    }

    BENCH_START(riscv_depthwise_conv_s8_opt_ref);
    riscv_depthwise_conv_s8_opt_ref(&ctx, &dw_conv_params, &quant_params, &input_dims, input, &filter_dims, test1 + Convolution_SIZE, &bias_dims, test7, &output_dims, test3);
    BENCH_END(riscv_depthwise_conv_s8_opt_ref);

    printf("Start opt depthwise_conv_s8_opt s8 implementation\n");

    BENCH_START(riscv_depthwise_conv_s8_opt);
    riscv_depthwise_conv_s8_opt(&ctx, &dw_conv_params, &quant_params, &input_dims, input, &filter_dims, test1 + Convolution_SIZE, &bias_dims, test7, &output_dims, test3 + Convolution_SIZE);
    BENCH_END(riscv_depthwise_conv_s8_opt);

    verify_results_q7(test3, test3 + Convolution_SIZE, 32);
    uint8_t *test10 = new uint8_t[Convolution_SIZE];
    uint8_t *test11 = new uint8_t[Convolution_SIZE];
    int32_t *test12 = new int32_t[Convolution_SIZE];
    uint8_t *test13 = new uint8_t[Convolution_SIZE * 2];
    for (int i = 0; i < Convolution_SIZE; i++) {
        test10[i] = (rand() % 256 - 128);
        test11[i] = (rand() % 256 - 128);
        test12[i] = (rand() % 65536 - 32768);
    }

    printf("Start ref depthwise_conv_u8_basic_ver1 s8 implementation\n");

    BENCH_START(riscv_depthwise_conv_u8_basic_ver1_ref);
    riscv_depthwise_conv_u8_basic_ver1_ref(test10, 4, 4, 4, test11, 4, 4, 4, 4, 4, 4, 4, 4, 4, test12, 0, 0, 0, test13, 4, 4, -100, 100, 0, 1);
    BENCH_END(riscv_depthwise_conv_u8_basic_ver1_ref);

    printf("Start opt depthwise_conv_u8_basic_ver1 s8 implementation\n");

    BENCH_START(riscv_depthwise_conv_u8_basic_ver1);
    riscv_depthwise_conv_u8_basic_ver1(test10, 4, 4, 4, test11, 4, 4, 4, 4, 4, 4, 4, 4, 4, test12, 0, 0, 0, test13 + Convolution_SIZE, 4, 4, -100, 100, 0, 1);
    BENCH_END(riscv_depthwise_conv_u8_basic_ver1);

    verify_results_q7((q7_t *)test13, (q7_t *)(test13 + Convolution_SIZE), 64);

    //  set default value
    input_dims.c = 4;
    conv_params.padding.w = 0;
    conv_params.padding.h = 0;
    conv_params.stride.w = 1;
    conv_params.stride.h = 1;
    input_dims.w = 5;
    input_dims.h = 5;
    input_dims.n = 1;
    ctx = { temp_buffer + 40, 10 };
    //This function does not have connected read & write especially write size not connected
    printf("Start ref convolve_1x1_HWC_fast_nonsquare q7 implementation\n");

    BENCH_START(riscv_convolve_1x1_s8_fast_ref);
    riscv_convolve_1x1_s8_fast_ref(&ctx, &conv_params, &quant_params, &input_dims, test1, &filter_dims, test1 + Convolution_SIZE * 2, &bias_dims, test7, &output_dims, test3);
    BENCH_END(riscv_convolve_1x1_s8_fast_ref);

    printf("Start opt convolve_1x1_HWC_fast_nonsquare q7 implementation\n");
    BENCH_START(riscv_convolve_1x1_s8_fast);
    riscv_convolve_1x1_s8_fast(&ctx, &conv_params, &quant_params, &input_dims, test1, &filter_dims, test1 + Convolution_SIZE * 3, &bias_dims, test7, &output_dims, test3 + Convolution_SIZE);
    BENCH_END(riscv_convolve_1x1_s8_fast);

    verify_results_q7(test3, test3 + Convolution_SIZE, 100);

    // delete[]test1;
    // delete[]test3;
    // delete[]test4;
    // delete[]test10;
    // delete[]test11;
    // delete[]test12;
    // delete[]test13;

#endif

#ifdef TEST_Convolution_part2
#define CONV_IM_DIM  8
#define CONV_IM_CH   8
#define CONV_KER_DIM 5
#define CONV_OUT_CH  8
#define CONV_OUT_DIM 8

    test1 = new q7_t[CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH + CONV_OUT_CH];
    test2 =
        new q15_t[CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH +
                  2 * CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH + CONV_OUT_CH];
    test3 = new q7_t[CONV_IM_DIM * CONV_IM_DIM * CONV_IM_CH + 2 * CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH];
    test4 = new q15_t[CONV_IM_DIM * CONV_IM_DIM * CONV_IM_CH + 2 * CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH];

    for (int i = 0; i < CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH + CONV_OUT_CH; i++) {
        test1[i] = rand() % 200 - 100;
    }

    for (int i = 0;
         i <
         CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH +
             2 * CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH + CONV_OUT_CH;
         i++) {
        test2[i] = (rand() % 65536 - 32768);
    }

    for (int i = 0; i < CONV_IM_DIM * CONV_IM_DIM * CONV_IM_CH + 2 * CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH; i++) {
        test3[i] = rand() % 200 - 100;
    }

    for (int i = 0; i < CONV_IM_DIM * CONV_IM_DIM * CONV_IM_CH + 2 * CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH; i++) {
        test4[i] = (rand() % 65536 - 32768);
    }

    q7_t *conv_weight_q7 = test1;
    q7_t *conv_bias_q7 = test1 + CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH;

    q15_t *conv_weight_q15 = test2;
    q15_t *conv_buf = test2 + CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH;
    q15_t *conv_bias_q15 =
        test2 + CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH +
        2 * CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH;

    q7_t *conv_im_in_q7 = test3;
    q7_t *conv_im_out_ref_q7 = test3 + CONV_IM_DIM * CONV_IM_DIM * CONV_IM_CH;
    q7_t *conv_im_out_opt_q7 =
        test3 + CONV_IM_DIM * CONV_IM_DIM * CONV_IM_CH + CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH;

    q15_t *conv_im_in_q15 = test4;
    q15_t *conv_im_out_ref_q15 = test4 + CONV_IM_DIM * CONV_IM_DIM * CONV_IM_CH;
    q15_t *conv_im_out_opt_q15 =
        test4 + CONV_IM_DIM * CONV_IM_DIM * CONV_IM_CH + CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH;

    initialize_results_q7(conv_im_out_ref_q7, conv_im_out_opt_q7, CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH);

    printf("start q7 ref implementation\n");

    BENCH_START(riscv_convolve_HWC_q7_ref);
    riscv_convolve_HWC_q7_ref(conv_im_in_q7, CONV_IM_DIM, CONV_IM_CH, conv_weight_q7,
                              CONV_OUT_CH, CONV_KER_DIM, 2, 1, conv_bias_q7, 1, 7, conv_im_out_ref_q7,
                              CONV_OUT_DIM, conv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_ref);

    printf("start q7 basic implementation\n");

    BENCH_START(riscv_convolve_HWC_q7_basic);
    riscv_convolve_HWC_q7_basic(conv_im_in_q7, CONV_IM_DIM, CONV_IM_CH, conv_weight_q7,
                                CONV_OUT_CH, CONV_KER_DIM, 2, 1, conv_bias_q7, 1, 7, conv_im_out_opt_q7,
                                CONV_OUT_DIM, conv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_basic);

    verify_results_q7(conv_im_out_ref_q7, conv_im_out_opt_q7, CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH);

    printf("start q7 fast implementation\n");

    BENCH_START(riscv_convolve_HWC_q7_fast);
    riscv_convolve_HWC_q7_fast(conv_im_in_q7, CONV_IM_DIM, CONV_IM_CH, conv_weight_q7,
                               CONV_OUT_CH, CONV_KER_DIM, 2, 1, conv_bias_q7, 1, 7, conv_im_out_opt_q7,
                               CONV_OUT_DIM, conv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_fast);

    verify_results_q7(conv_im_out_ref_q7, conv_im_out_opt_q7, CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH);

    printf("start q7 ref implementation for RGB\n");

    BENCH_START(riscv_convolve_HWC_q7_ref);
    riscv_convolve_HWC_q7_ref(conv_im_in_q7, CONV_IM_DIM, 3, conv_weight_q7,
                              CONV_OUT_CH, CONV_KER_DIM, 2, 1, conv_bias_q7, 1, 7, conv_im_out_ref_q7,
                              CONV_OUT_DIM, conv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_ref);

    printf("start q7 RGB implementation for RGB\n");

    BENCH_START(riscv_convolve_HWC_q7_RGB);
    riscv_convolve_HWC_q7_RGB(conv_im_in_q7, CONV_IM_DIM, 3, conv_weight_q7,
                              CONV_OUT_CH, CONV_KER_DIM, 2, 1, conv_bias_q7, 1, 7, conv_im_out_opt_q7,
                              CONV_OUT_DIM, conv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_RGB);

    verify_results_q7(conv_im_out_ref_q7, conv_im_out_opt_q7, CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH);

    // delete[]test1;
    // delete[]test2;
    // delete[]test3;
    // delete[]test4;
#endif

#ifdef TEST_Convolution_part3
#define RCONV_IM_DIM_X  10
#define RCONV_IM_DIM_Y  8
#define RCONV_IM_CH     4
#define RCONV_KER_DIM_X 5
#define RCONV_KER_DIM_Y 3
#define RCONV_STRIDE_X  1
#define RCONV_STRIDE_Y  1
#define RCONV_PADDING_X 2
#define RCONV_PADDING_Y 1
#define RCONV_OUT_CH    4
#define RCONV_OUT_DIM_X 10
#define RCONV_OUT_DIM_Y 8

    test1 = new q7_t[RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH * RCONV_OUT_CH + RCONV_OUT_CH];
    test2 = new q15_t[2 * RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH];
    test3 =
        new q7_t[RCONV_IM_DIM_Y * RCONV_IM_DIM_X * RCONV_IM_CH + 2 * RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH];

    for (int i = 0; i < RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH * RCONV_OUT_CH + RCONV_OUT_CH; i++) {
        test1[i] = rand() % 200 - 100;
    }

    for (int i = 0;
         i < RCONV_IM_DIM_Y * RCONV_IM_DIM_X * RCONV_IM_CH + 2 * RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH; i++) {
        test3[i] = rand() % 256 - 100;
    }

    q7_t *rconv_weight_q7 = test1;
    q7_t *rconv_bias_q7 = test1 + RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH * RCONV_OUT_CH;

    q15_t *rconv_buf = test2;

    q7_t *rconv_im_in_q7 = test3;
    q7_t *rconv_im_out_ref_q7 = test3 + RCONV_IM_DIM_Y * RCONV_IM_DIM_X * RCONV_IM_CH;
    q7_t *rconv_im_out_opt_q7 =
        test3 + RCONV_IM_DIM_Y * RCONV_IM_DIM_X * RCONV_IM_CH + RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH;

    initialize_results_q7(rconv_im_out_ref_q7, rconv_im_out_opt_q7, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

    printf("start conv q7 nonsquare ref implementation\n");
    BENCH_START(riscv_convolve_HWC_q7_ref_nonsquare);
    riscv_convolve_HWC_q7_ref_nonsquare(rconv_im_in_q7, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH, rconv_weight_q7,
                                        RCONV_OUT_CH, RCONV_KER_DIM_X, RCONV_KER_DIM_Y, RCONV_PADDING_X, RCONV_PADDING_Y,
                                        RCONV_STRIDE_X, RCONV_STRIDE_Y, rconv_bias_q7, 1, 7, rconv_im_out_ref_q7,
                                        RCONV_OUT_DIM_X, RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_ref_nonsquare);

    printf("start conv q7 nonsquare opt implementation\n");
    BENCH_START(riscv_convolve_HWC_q7_fast_nonsquare);
    riscv_convolve_HWC_q7_fast_nonsquare(rconv_im_in_q7, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH, rconv_weight_q7,
                                         RCONV_OUT_CH, RCONV_KER_DIM_X, RCONV_KER_DIM_Y, RCONV_PADDING_X, RCONV_PADDING_Y,
                                         RCONV_STRIDE_X, RCONV_STRIDE_Y, rconv_bias_q7, 1, 7, rconv_im_out_opt_q7,
                                         RCONV_OUT_DIM_X, RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_fast_nonsquare);

    verify_results_q7(rconv_im_out_ref_q7, rconv_im_out_opt_q7, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

    initialize_results_q7(rconv_im_out_ref_q7, rconv_im_out_opt_q7, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

    printf("start conv q7 nonsquare ref implementation\n");
    BENCH_START(riscv_convolve_HWC_q7_ref_nonsquare);
    riscv_convolve_HWC_q7_ref_nonsquare(rconv_im_in_q7, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH, rconv_weight_q7,
                                        RCONV_OUT_CH, RCONV_KER_DIM_X, RCONV_KER_DIM_Y, RCONV_PADDING_X, RCONV_PADDING_Y,
                                        RCONV_STRIDE_X, RCONV_STRIDE_Y, rconv_bias_q7, 1, 7, rconv_im_out_ref_q7,
                                        RCONV_OUT_DIM_X, RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_ref_nonsquare);

    printf("start conv q7 nonsquare basic implementation\n");
    BENCH_START(riscv_convolve_HWC_q7_basic_nonsquare);
    riscv_convolve_HWC_q7_basic_nonsquare(rconv_im_in_q7, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH, rconv_weight_q7,
                                          RCONV_OUT_CH, RCONV_KER_DIM_X, RCONV_KER_DIM_Y, RCONV_PADDING_X, RCONV_PADDING_Y,
                                          RCONV_STRIDE_X, RCONV_STRIDE_Y, rconv_bias_q7, 1, 7, rconv_im_out_opt_q7,
                                          RCONV_OUT_DIM_X, RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_basic_nonsquare);

    verify_results_q7(rconv_im_out_ref_q7, rconv_im_out_opt_q7, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

    initialize_results_q7(rconv_im_out_ref_q7, rconv_im_out_opt_q7, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

    printf("start 1x1 conv q7 nonsquare fast implementation\n");
    BENCH_START(riscv_convolve_HWC_q7_fast_nonsquare);
    riscv_convolve_HWC_q7_fast_nonsquare(rconv_im_in_q7, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH, rconv_weight_q7,
                                         RCONV_OUT_CH, 1, 1, 0, 0, RCONV_STRIDE_X,
                                         RCONV_STRIDE_Y, rconv_bias_q7, 1, 7, rconv_im_out_ref_q7, RCONV_OUT_DIM_X,
                                         RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_fast_nonsquare);

    printf("start 1x1 conv q7 nonsquare dedicated function implementation\n");
    BENCH_START(riscv_convolve_1x1_HWC_q7_fast_nonsquare);
    riscv_convolve_1x1_HWC_q7_fast_nonsquare(rconv_im_in_q7, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH, rconv_weight_q7,
                                             RCONV_OUT_CH, 1, 1, 0, 0, RCONV_STRIDE_X,
                                             RCONV_STRIDE_Y, rconv_bias_q7, 1, 7, rconv_im_out_opt_q7, RCONV_OUT_DIM_X,
                                             RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_convolve_1x1_HWC_q7_fast_nonsquare);

    verify_results_q7(rconv_im_out_ref_q7, rconv_im_out_opt_q7, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

    printf("start depthwise separable conv q7 nonsquare ref implementation\n");
    BENCH_START(riscv_depthwise_separable_conv_HWC_q7_ref_nonsquare);
    riscv_depthwise_separable_conv_HWC_q7_ref_nonsquare(rconv_im_in_q7, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH,
                                                        rconv_weight_q7, RCONV_OUT_CH, RCONV_KER_DIM_X, RCONV_KER_DIM_Y,
                                                        RCONV_PADDING_X, RCONV_PADDING_Y, RCONV_STRIDE_X, RCONV_STRIDE_Y,
                                                        rconv_bias_q7, 1, 7, rconv_im_out_ref_q7, RCONV_OUT_DIM_X,
                                                        RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_depthwise_separable_conv_HWC_q7_ref_nonsquare);

    printf("start depthwise separable conv q7 nonsquare opt implementation\n");
    BENCH_START(riscv_depthwise_separable_conv_HWC_q7_nonsquare);
    riscv_depthwise_separable_conv_HWC_q7_nonsquare(rconv_im_in_q7, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH,
                                                    rconv_weight_q7, RCONV_OUT_CH, RCONV_KER_DIM_X, RCONV_KER_DIM_Y,
                                                    RCONV_PADDING_X, RCONV_PADDING_Y, RCONV_STRIDE_X, RCONV_STRIDE_Y,
                                                    rconv_bias_q7, 1, 7, rconv_im_out_opt_q7, RCONV_OUT_DIM_X,
                                                    RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_depthwise_separable_conv_HWC_q7_nonsquare);

    verify_results_q7(rconv_im_out_ref_q7, rconv_im_out_opt_q7, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

    delete[] test1;
    delete[] test3;

    test2 = new q15_t[RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH * RCONV_OUT_CH + RCONV_OUT_CH];                          // weights + bias
    test4 = new q15_t[2 * RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH                                                      //buffer
                      + RCONV_IM_DIM_Y * RCONV_IM_DIM_X * RCONV_IM_CH + 2 * RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH]; // i/o

    for (int i = 0; i < RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH * RCONV_OUT_CH + RCONV_OUT_CH; i++) {
        test2[i] = rand() % 256 - 100;
    }

    for (int i = 0;
         i < 2 * RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH + RCONV_IM_DIM_Y * RCONV_IM_DIM_X * RCONV_IM_CH + 2 * RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH;
         i++) {
        test4[i] = rand() % 256 - 100;
    }

    q15_t *rconv_weight_q15 = test2;
    q15_t *rconv_bias_q15 = test2 + RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH * RCONV_OUT_CH;

    rconv_buf = test4;

    q15_t *rconv_im_in_q15 = test4 + 2 * RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH;
    q15_t *rconv_im_out_ref_q15 = rconv_im_in_q15 + RCONV_IM_DIM_Y * RCONV_IM_DIM_X * RCONV_IM_CH;
    q15_t *rconv_im_out_opt_q15 = rconv_im_out_ref_q15 + RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH;

    initialize_results_q15(rconv_im_out_ref_q15, rconv_im_out_opt_q15, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

    printf("start conv q15 nonsquare ref implementation\n");
    BENCH_START(riscv_convolve_HWC_q15_nonsquare_ref);
    riscv_convolve_HWC_q15_nonsquare_ref(rconv_im_in_q15, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH, rconv_weight_q15,
                                         RCONV_OUT_CH, RCONV_KER_DIM_X, RCONV_KER_DIM_Y, RCONV_PADDING_X, RCONV_PADDING_Y,
                                         RCONV_STRIDE_X, RCONV_STRIDE_Y, rconv_bias_q15, 1, 7, rconv_im_out_ref_q15,
                                         RCONV_OUT_DIM_X, RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q15_nonsquare_ref);

    printf("start conv q5 nonsquare opt implementation\n");
    BENCH_START(riscv_convolve_HWC_q15_fast_nonsquare);
    riscv_convolve_HWC_q15_fast_nonsquare(rconv_im_in_q15, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH, rconv_weight_q15,
                                          RCONV_OUT_CH, RCONV_KER_DIM_X, RCONV_KER_DIM_Y, RCONV_PADDING_X, RCONV_PADDING_Y,
                                          RCONV_STRIDE_X, RCONV_STRIDE_Y, rconv_bias_q15, 1, 7, rconv_im_out_opt_q15,
                                          RCONV_OUT_DIM_X, RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q15_fast_nonsquare);

    verify_results_q15(rconv_im_out_ref_q15, rconv_im_out_opt_q15, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

#endif

#ifdef TEST_Fully_connectedLayer
#define IP_ROW_DIM 127
#define IP_COL_DIM 127

    q7_t ip_weights[IP_ROW_DIM * IP_COL_DIM] = IP2_WEIGHT;
    q7_t ip_q7_opt_weights[IP_ROW_DIM * IP_COL_DIM] = IP4_WEIGHT;
    q7_t ip_q7_q15_opt_weights[IP_ROW_DIM * IP_COL_DIM] = IP4_q7_q15_WEIGHT;
    q15_t ip_q15_weights[IP_ROW_DIM * IP_COL_DIM] = IP2_WEIGHT;
    q15_t ip_q15_opt_weights[IP_ROW_DIM * IP_COL_DIM] = IP4_WEIGHT_Q15;

    test1 = new q7_t[IP_COL_DIM * 2 + IP_ROW_DIM * 2];
    test2 = new q15_t[IP_COL_DIM];
    test3 = new q7_t[IP_COL_DIM * 2 + IP_ROW_DIM * 3];
    test4 = new q15_t[IP_COL_DIM * 2 + IP_ROW_DIM * 2];

    for (int i = 0; i < IP_ROW_DIM + IP_COL_DIM; i++) {
        test1[i] = rand() % 256 - 100;
    }
    for (int i = 0; i < IP_ROW_DIM * 3; i++) {
        test3[i] = 0;
    }

    q7_t *ip_bias_q7 = test1 + IP_COL_DIM;

    q7_t *ip_out_q7_ref = test3;
    q7_t *ip_out_q7_opt = test3 + IP_ROW_DIM;
    q7_t *ip_out_q7_opt_fast = test3 + 2 * IP_ROW_DIM;
    q15_t *ip_out_q15_ref = test4 + IP_COL_DIM;
    q15_t *ip_out_q15_opt = test4 + IP_COL_DIM + IP_ROW_DIM;

    initialize_results_q7(ip_out_q7_ref, ip_out_q7_opt, IP_ROW_DIM);
    initialize_results_q7(ip_out_q7_ref, ip_out_q7_opt_fast, IP_ROW_DIM);
    initialize_results_q7(ip_out_q7_ref, ip_out_q7_opt_fast, IP_ROW_DIM);

    printf("Start ref q7 implementation\n");

    BENCH_START(riscv_fully_connected_q7_ref);
    riscv_fully_connected_q7_ref(test1, ip_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, ip_bias_q7, ip_out_q7_ref, test2);
    BENCH_END(riscv_fully_connected_q7_ref);

    printf("Start q7 implementation\n");

    BENCH_START(riscv_fully_connected_q7);
    riscv_fully_connected_q7(test1, ip_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, ip_bias_q7, ip_out_q7_opt, test2);
    BENCH_END(riscv_fully_connected_q7);

    verify_results_q7(ip_out_q7_ref, ip_out_q7_opt, IP_ROW_DIM);

    printf("Start q7 ref opt implementation\n");

    BENCH_START(riscv_fully_connected_q7_opt_ref);
    riscv_fully_connected_q7_opt_ref(test1, ip_q7_opt_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, ip_bias_q7,
                                     ip_out_q7_opt_fast, test2);
    BENCH_END(riscv_fully_connected_q7_opt_ref);

    verify_results_q7(ip_out_q7_ref, ip_out_q7_opt_fast, IP_ROW_DIM);

    printf("Start q7 opt implementation\n");

    BENCH_START(riscv_fully_connected_q7_opt);
    riscv_fully_connected_q7_opt(test1, ip_q7_opt_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, ip_bias_q7, ip_out_q7_opt_fast,
                                 test2);
    BENCH_END(riscv_fully_connected_q7_opt);

    verify_results_q7(ip_out_q7_ref, ip_out_q7_opt_fast, IP_ROW_DIM);

    for (int i = 0; i < IP_ROW_DIM + IP_COL_DIM; i++) {
        test4[i] = (rand() % 20500 - 10700);
    }

    initialize_results_q15(ip_out_q15_ref, ip_out_q15_opt, IP_ROW_DIM);

    printf("Start ref q15 implementation\n");

    BENCH_START(riscv_fully_connected_q15_ref);
    riscv_fully_connected_q15_ref(test4, ip_q15_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, test2, ip_out_q15_ref, NULL);
    BENCH_END(riscv_fully_connected_q15_ref);

    printf("Start q15 implementation\n");

    BENCH_START(riscv_fully_connected_q15);
    riscv_fully_connected_q15(test4, ip_q15_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, test2, ip_out_q15_opt, NULL);
    BENCH_END(riscv_fully_connected_q15);

    verify_results_q15(ip_out_q15_ref, ip_out_q15_opt, IP_ROW_DIM);

    printf("Start ref opt q15 implementation\n");

    BENCH_START(riscv_fully_connected_q15_opt_ref);
    riscv_fully_connected_q15_opt_ref(test4, ip_q15_opt_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, test2, ip_out_q15_opt,
                                      NULL);
    BENCH_END(riscv_fully_connected_q15_opt_ref);

    verify_results_q15(ip_out_q15_ref, ip_out_q15_opt, IP_ROW_DIM);

    printf("Start opt q15 implementation\n");

    BENCH_START(riscv_fully_connected_q15_opt);
    riscv_fully_connected_q15_opt(test4, ip_q15_opt_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, test2, ip_out_q15_opt, NULL);
    BENCH_END(riscv_fully_connected_q15_opt);

    verify_results_q15(ip_out_q15_ref, ip_out_q15_opt, IP_ROW_DIM);

    initialize_results_q15(ip_out_q15_ref, ip_out_q15_opt, IP_ROW_DIM);

    printf("Start ref q7_q15 implementation\n");

    BENCH_START(riscv_fully_connected_mat_q7_vec_q15_ref);
    riscv_fully_connected_mat_q7_vec_q15_ref(test4, ip_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, ip_bias_q7, ip_out_q15_ref,
                                             test2);
    BENCH_END(riscv_fully_connected_mat_q7_vec_q15_ref);

    printf("Start q7_q15 implementation\n");

    BENCH_START(riscv_fully_connected_mat_q7_vec_q15);
    riscv_fully_connected_mat_q7_vec_q15(test4, ip_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, ip_bias_q7, ip_out_q15_opt,
                                         test2);
    BENCH_END(riscv_fully_connected_mat_q7_vec_q15);

    verify_results_q15(ip_out_q15_ref, ip_out_q15_opt, IP_ROW_DIM);

    printf("Start ref opt q7_q15 implementation\n");

    BENCH_START(riscv_fully_connected_mat_q7_vec_q15_opt_ref);
    riscv_fully_connected_mat_q7_vec_q15_opt_ref(test4, ip_q7_q15_opt_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, ip_bias_q7,
                                                 ip_out_q15_opt, test2);
    BENCH_END(riscv_fully_connected_mat_q7_vec_q15_opt_ref);

    verify_results_q15(ip_out_q15_ref, ip_out_q15_opt, IP_ROW_DIM);

    printf("Start opt q7_q15 implementation\n");

    BENCH_START(riscv_fully_connected_mat_q7_vec_q15_opt);
    riscv_fully_connected_mat_q7_vec_q15_opt(test4, ip_q7_q15_opt_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, ip_bias_q7,
                                             ip_out_q15_opt, test2);
    BENCH_END(riscv_fully_connected_mat_q7_vec_q15_opt);

    verify_results_q15(ip_out_q15_ref, ip_out_q15_opt, IP_ROW_DIM);

    int32_t fc_multiplier = 1;
    int32_t fc_shift = 0;
    test1 = new q7_t[320 * 2];
    test3 = new q7_t[320 * 2];
    q7_t *fc_temp_buffer = new q7_t[320];
    int32_t *fc_test12 = new int32_t[320];
    nmsis_nn_context fc_ctx = { fc_temp_buffer, 10 };
    nmsis_nn_tile fc_stride = { 2, 2 };
    nmsis_nn_tile fc_padding = { 2, 2 };
    nmsis_nn_tile fc_dilation = { 2, 2 };
    nmsis_nn_activation fc_activation = { -100, 100 };
    nmsis_nn_conv_params fc_conv_params = { 1, 1, fc_stride, fc_padding, fc_dilation, fc_activation };
    nmsis_nn_per_tensor_quant_params fc_quant_params = { fc_multiplier, fc_shift };
    nmsis_nn_dims fc_input_dims = { 20, 2, 4, 2 };
    nmsis_nn_dims fc_filter_dims = { 20, 2, 4, 2 };
    nmsis_nn_dims fc_bias_dims = { 20, 2, 4, 2 };
    nmsis_nn_dims fc_output_dims = { 20, 2, 4, 2 };
    nmsis_nn_fc_params fc_fc_params = { 0, 0, 0, fc_activation };

    printf("Start ref opt s8 implementation\n");

    BENCH_START(riscv_fully_connected_s8_ref);
    riscv_fully_connected_s8_ref(&fc_ctx, &fc_fc_params, &fc_quant_params, &fc_input_dims, test1, &fc_filter_dims, test1 + 320, &fc_bias_dims, fc_test12, &fc_output_dims, test3);
    BENCH_END(riscv_fully_connected_s8_ref);

    printf("Start opt s8 implementation\n");

    BENCH_START(riscv_fully_connected_s8);
    riscv_fully_connected_s8(&fc_ctx, &fc_fc_params, &fc_quant_params, &fc_input_dims, test1, &fc_filter_dims, test1 + 320, &fc_bias_dims, fc_test12, &fc_output_dims, test3 + 320);
    BENCH_END(riscv_fully_connected_s8);

    verify_results_q7(test3, test3 + 320, 40);

    delete[] test1;
    delete[] test2;
    delete[] test3;
    delete[] test4;
    // delete[]fc_temp_buffer;
    // delete[]fc_test12;

#endif

#ifdef TEST_Pooling
#define Pooling_SIZE 600
    test1 = new q7_t[Pooling_SIZE * 4];
    test2 = new q15_t[Pooling_SIZE * 2];
    test3 = new q7_t[Pooling_SIZE * 2];

    q7_t *pool_temp_buffer = new q7_t[Pooling_SIZE * 2];
    int32_t pool_multiplier = 1;
    int32_t pool_shift = 0;
    for (int i = 0; i < Pooling_SIZE; i++) {
        test2[i] = (rand() % 65536 - 32768);
        pool_temp_buffer[i] = 0;
        pool_temp_buffer[i + Pooling_SIZE] = pool_temp_buffer[i];
    }
    for (int i = 0; i < Pooling_SIZE * 2; i++) {
        test1[i] = (rand() % 210 - 105);
        test1[i + Pooling_SIZE * 2] = test1[i];
    }
    nmsis_nn_context pool_ctx = { pool_temp_buffer, 10 };
    nmsis_nn_tile pool_stride = { 0, 0 };
    nmsis_nn_tile pool_padding = { 0, 0 };
    nmsis_nn_tile pool_dilation = { 2, 2 };
    nmsis_nn_activation pool_activation = { -100, 100 };
    nmsis_nn_conv_params pool_conv_params = { 1, 1, pool_stride, pool_padding, pool_dilation, pool_activation };
    nmsis_nn_per_channel_quant_params pool_quant_params = { &pool_multiplier, &pool_shift };
    nmsis_nn_dims pool_input_dims = { 2, 3, 3, 2 };
    nmsis_nn_dims pool_filter_dims = { 2, 3, 3, 2 };
    nmsis_nn_dims pool_bias_dims = { 2, 3, 3, 2 };
    nmsis_nn_dims pool_output_dims = { 2, 9, 9, 2 };
    nmsis_nn_dw_conv_params pool_dw_conv_params = { 0, 0, 1, pool_stride, pool_padding, pool_dilation, pool_activation };
    nmsis_nn_pool_params pool_pool_params = { pool_stride, pool_padding, pool_activation };

    printf("Start ref max_pool s8 implementation\n");

    BENCH_START(riscv_max_pool_s8_ref);
    riscv_max_pool_s8_ref(&pool_ctx, &pool_pool_params, &pool_input_dims, test1, &pool_filter_dims, &pool_output_dims, test3);
    BENCH_END(riscv_max_pool_s8_ref);

    printf("Start opt max_pool s8 implementation\n");

    BENCH_START(riscv_max_pool_s8);
    riscv_max_pool_s8(&pool_ctx, &pool_pool_params, &pool_input_dims, test1, &pool_filter_dims, &pool_output_dims, test3 + Pooling_SIZE);
    BENCH_END(riscv_max_pool_s8);

    verify_results_q7(test3, test3 + Pooling_SIZE, 18);

    printf("Start ref avgpool s8 implementation\n");

    BENCH_START(riscv_avgpool_s8_ref);
    riscv_avgpool_s8_ref(&pool_ctx, &pool_pool_params, &pool_input_dims, test1, &pool_filter_dims, &pool_output_dims, test3);
    BENCH_END(riscv_avgpool_s8_ref);

    printf("Start opt avgpool s8 implementation\n");

    BENCH_START(riscv_avgpool_s8);
    riscv_avgpool_s8(&pool_ctx, &pool_pool_params, &pool_input_dims, test1, &pool_filter_dims, &pool_output_dims, test3 + Pooling_SIZE);
    BENCH_END(riscv_avgpool_s8);

    verify_results_q7(test3, test3 + Pooling_SIZE, 18);

#define POOL_IM_DIM 32
#define POOL_IM_CH  8
    test1 = new q7_t[POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH * 2];
    test2 = new q15_t[POOL_IM_DIM * POOL_IM_CH];
    test3 = new q7_t[POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH];

    q7_t *img_in = test1 + POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH;
    q7_t *pool_out_ref = test3;
    q7_t *pool_out_opt = test3 + POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH / 2;

    // copy over the img input
    for (int i = 0; i < POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH; i++) {
        img_in[i] = test1[i];
    }

    printf("Start maxpool reference implementation\n");

    BENCH_START(riscv_maxpool_q7_HWC_ref);
    riscv_maxpool_q7_HWC_ref(img_in, POOL_IM_DIM, POOL_IM_CH, 3, 0, 2, POOL_IM_DIM / 2, (q7_t *)test2, pool_out_ref);
    BENCH_END(riscv_maxpool_q7_HWC_ref);

    printf("Start maxpool opt implementation\n");
    for (int i = 0; i < POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH; i++) {
        img_in[i] = test1[i];
    }

    BENCH_START(riscv_maxpool_q7_HWC);
    riscv_maxpool_q7_HWC(img_in, POOL_IM_DIM, POOL_IM_CH, 3, 0, 2, POOL_IM_DIM / 2, (q7_t *)test2, pool_out_opt);
    BENCH_END(riscv_maxpool_q7_HWC);

    verify_results_q7(pool_out_ref, pool_out_opt, POOL_IM_DIM / 2 * POOL_IM_DIM / 2 * POOL_IM_CH);
    // copy over the img input
    for (int i = 0; i < POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH; i++) {
        img_in[i] = test1[i];
    }

    printf("Start avepool ref implementation\n");

    BENCH_START(riscv_avepool_q7_HWC_ref);
    riscv_avepool_q7_HWC_ref(img_in, POOL_IM_DIM, POOL_IM_CH, 3, 0, 2, POOL_IM_DIM / 2, (q7_t *)test2, pool_out_ref);
    BENCH_END(riscv_avepool_q7_HWC_ref);

    // copy over the img input
    for (int i = 0; i < POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH; i++) {
        img_in[i] = test1[i];
    }

    printf("Start avepool opt implementation\n");

    BENCH_START(riscv_avepool_q7_HWC);
    riscv_avepool_q7_HWC(img_in, POOL_IM_DIM, POOL_IM_CH, 3, 0, 2, POOL_IM_DIM / 2, (q7_t *)test2, pool_out_opt);
    BENCH_END(riscv_avepool_q7_HWC);

    verify_results_q7(pool_out_ref, pool_out_opt, POOL_IM_DIM / 2 * POOL_IM_DIM / 2 * POOL_IM_CH);

    delete[] test1;
    delete[] test2;
    delete[] test3;
    delete[] pool_temp_buffer;

#endif

#ifdef TEST_Softmax
#define Softmax_SIZE 100
    test1 = new q7_t[Softmax_SIZE * 2];
    test2 = new q15_t[Softmax_SIZE * 2];
    test3 = new q7_t[Softmax_SIZE * 2];
    test4 = new q15_t[Softmax_SIZE * 2];
    uint8_t *test5 = new uint8_t[Softmax_SIZE];
    uint8_t *test6 = new uint8_t[Softmax_SIZE * 2];

    int8_t *Softmax_test7 = new int8_t[Softmax_SIZE * 2];
    for (int i = 0; i < Softmax_SIZE; i++) {
        test5[i] = (rand() % 256);
    }
    for (int i = 0; i < Softmax_SIZE * 2; i++) {
        test2[i] = (rand() % 65536 - 32768);
        test1[i] = (rand() % 256 - 128);
    }

    printf("Start ref softmax q15 implementation\n");

    BENCH_START(riscv_softmax_q15_ref);
    riscv_softmax_q15_ref(test2, 6, test4);
    BENCH_END(riscv_softmax_q15_ref);

    printf("Start opt softmax q15 implementation\n");

    BENCH_START(riscv_softmax_q15);
    riscv_softmax_q15(test2, 6, test4 + Softmax_SIZE);
    BENCH_END(riscv_softmax_q15);

    verify_results_q15(test4, test4 + Softmax_SIZE, 6);

    printf("Start ref softmax q7 implementation\n");

    BENCH_START(riscv_softmax_q7_ref);
    riscv_softmax_q7_ref(test1, Softmax_SIZE, test3);
    BENCH_END(riscv_softmax_q7_ref);

    printf("Start opt softmax q7 implementation\n");

    BENCH_START(riscv_softmax_q7);
    riscv_softmax_q7(test1, Softmax_SIZE, test3 + Softmax_SIZE);
    BENCH_END(riscv_softmax_q7);

    verify_results_q7(test3, test3 + Softmax_SIZE, Softmax_SIZE);

    printf("Start ref softmax s8 implementation\n");

    BENCH_START(riscv_softmax_s8_ref);
    riscv_softmax_s8_ref(test1, 3, 30, 1, 0, -100, Softmax_test7);
    BENCH_END(riscv_softmax_s8_ref);

    printf("Start opt softmax s8 implementation\n");

    BENCH_START(riscv_softmax_s8);
    riscv_softmax_s8(test1, 3, 30, 1, 0, -100, Softmax_test7 + Softmax_SIZE);
    BENCH_END(riscv_softmax_s8);

    verify_results_q7(Softmax_test7, Softmax_test7 + Softmax_SIZE, 6);

    printf("Start ref softmax u8 implementation\n");

    BENCH_START(riscv_softmax_u8_ref);
    riscv_softmax_u8_ref(test5, 3, 30, 1, 0, -100, test6);
    BENCH_END(riscv_softmax_u8_ref);

    printf("Start opt softmax u8 implementation\n");

    BENCH_START(riscv_softmax_u8);
    riscv_softmax_u8(test5, 3, 30, 1, 0, -100, test6 + Softmax_SIZE);
    BENCH_END(riscv_softmax_u8);

    verify_results_q7((q7_t *)test6, (q7_t *)(test6 + Softmax_SIZE), 6);

    printf("Start ref softmax_with_batch q7 implementation\n");

    BENCH_START(riscv_softmax_with_batch_q7_ref);
    riscv_softmax_with_batch_q7_ref(test1, 2, 20, test3);
    BENCH_END(riscv_softmax_with_batch_q7_ref);

    printf("Start opt softmax_with_batch q7 implementation\n");

    BENCH_START(riscv_softmax_with_batch_q7);
    riscv_softmax_with_batch_q7(test1, 2, 20, test3 + Softmax_SIZE);
    BENCH_END(riscv_softmax_with_batch_q7);

    verify_results_q7(test3, test3 + Softmax_SIZE, 40);

    delete[] test1;
    delete[] test2;
    delete[] test3;
    // delete[]test4;
    // delete[]test5;
    // delete[]test6;
    // delete[]Softmax_test7;
#endif

#ifdef TEST_SVD
#define SVD_SIZE 32
    test1 = new q7_t[SVD_SIZE * 2];
    test2 = new q15_t[SVD_SIZE * 2];
    test3 = new q7_t[SVD_SIZE * 2];

    q7_t *test20;
    test20 = new q7_t[SVD_SIZE * 2];

    q7_t *test21;
    test21 = new q7_t[SVD_SIZE * 2];

    q31_t *test22;
    test22 = new q31_t[SVD_SIZE * 2];

    for (int i = 0; i < SVD_SIZE * 2; i++) {
        test1[i] = (rand() % 256 - 128);
        test20[i] = (rand() % 256 - 128);
        test21[i] = (rand() % 256 - 128);
        test22[i] = (rand() % 65535 - 32767);
    }

    nmsis_nn_context SVD_input_ctx = { test1, SVD_SIZE };
    nmsis_nn_context SVD_output_ctx_opt = { test20, SVD_SIZE };
    nmsis_nn_context SVD_output_ctx_ref = { test20 + SVD_SIZE, SVD_SIZE };
    nmsis_nn_svdf_params SVD_svdf_params = { 32, 0, 0, -100, 100, -100, 100 };
    nmsis_nn_per_tensor_quant_params SVD_input_quant_params = { 1, 0 };
    nmsis_nn_per_tensor_quant_params SVD_output_quant_params = { 1, 0 };
    nmsis_nn_dims SVD_input_dims = { 20, 32, 32, 1 };
    nmsis_nn_dims SVD_weights_feature_dims = { 20, 32, 32, 1 };
    nmsis_nn_dims SVD_weights_time_dims = { 20, 32, 32, 1 };
    // Does not matter what these arguments are
    nmsis_nn_dims SVD_state_dims = { 20, 32, 32, 1 };
    nmsis_nn_dims SVD_output_dims = { 20, 32, 32, 1 };
    nmsis_nn_dims SVD_bias_dims = { 20, 32, 32, 1 };

    printf("Start ref SVD implementation\n");

    BENCH_START(ref_svdf_s8);
    ref_svdf_s8(&SVD_input_ctx, &SVD_output_ctx_ref, &SVD_svdf_params, &SVD_input_quant_params,
                &SVD_output_quant_params, &SVD_input_dims, test1 + SVD_SIZE, &SVD_state_dims,
                test2, &SVD_weights_feature_dims, test21, &SVD_weights_time_dims,
                test2 + SVD_SIZE, &SVD_bias_dims, test22, &SVD_output_dims, test3);
    BENCH_END(ref_svdf_s8);

    printf("Start opt SVD implementation\n");

    BENCH_START(riscv_svdf_s8);
    riscv_svdf_s8(&SVD_input_ctx, &SVD_output_ctx_ref, &SVD_svdf_params, &SVD_input_quant_params,
                  &SVD_output_quant_params, &SVD_input_dims, test1 + SVD_SIZE, &SVD_state_dims,
                  test2, &SVD_weights_feature_dims, test21, &SVD_weights_time_dims,
                  test2 + SVD_SIZE, &SVD_bias_dims, test22, &SVD_output_dims, test3 + SVD_SIZE);
    BENCH_END(riscv_svdf_s8);

    verify_results_q7(test3, test3 + SVD_SIZE, SVD_SIZE);

#endif

#ifdef TEST_NNDataConversion
#define NNDataConversion_SIZE 100
    test1 = new q7_t[NNDataConversion_SIZE * 2];
    test3 = new q7_t[NNDataConversion_SIZE * 2];
    test4 = new q15_t[NNDataConversion_SIZE * 2];

    for (int i = 0; i < NNDataConversion_SIZE * 2; i++) {
        test1[i] = (rand() % 256 - 128);
    }

    printf("Start ref q7 to q15 no shift implementation\n");

    BENCH_START(riscv_q7_to_q15_no_shift_ref);
    riscv_q7_to_q15_no_shift_ref(test1, test4, NNDataConversion_SIZE);
    BENCH_END(riscv_q7_to_q15_no_shift_ref);

    printf("Start opt q7 to q15 no shift implementation\n");

    BENCH_START(riscv_q7_to_q15_no_shift);
    riscv_q7_to_q15_no_shift(test1, test4 + NNDataConversion_SIZE, NNDataConversion_SIZE);
    BENCH_END(riscv_q7_to_q15_no_shift);

    verify_results_q15(test4, test4 + NNDataConversion_SIZE, NNDataConversion_SIZE);

    printf("Start ref q7 to q15 reordered no shift implementation\n");

    BENCH_START(riscv_q7_to_q15_reordered_no_shift_ref);
    riscv_q7_to_q15_reordered_no_shift_ref(test1, test4, NNDataConversion_SIZE);
    BENCH_END(riscv_q7_to_q15_reordered_no_shift_ref);

    printf("Start opt q7 to q15 reordered no shift implementation\n");

    BENCH_START(riscv_q7_to_q15_reordered_no_shift);
    riscv_q7_to_q15_reordered_no_shift(test1, test4 + NNDataConversion_SIZE, NNDataConversion_SIZE);
    BENCH_END(riscv_q7_to_q15_reordered_no_shift);

    verify_results_q15(test4, test4 + NNDataConversion_SIZE, NNDataConversion_SIZE);

    //Only available for DSP_ON
    // printf("Start ref q7 to q15 reordered with offset implementation\n");

    // BENCH_START(riscv_q7_to_q15_reordered_with_offset_ref);
    // riscv_q7_to_q15_reordered_with_offset_ref(test1, test4, NNDataConversion_SIZE,0);
    // BENCH_END(riscv_q7_to_q15_reordered_with_offset_ref);

    // printf("Start opt q7 to q15 reordered with offset implementation\n");

    // BENCH_START(riscv_q7_to_q15_reordered_with_offset);
    // riscv_q7_to_q15_reordered_with_offset(test1, test4 + NNDataConversion_SIZE, NNDataConversion_SIZE,0);
    // BENCH_END(riscv_q7_to_q15_reordered_with_offset);

    // verify_results_q15(test4, test4 + NNDataConversion_SIZE, NNDataConversion_SIZE);

    // printf("Start ref q7 to q15 offset implementation\n");

    BENCH_START(riscv_q7_to_q15_with_offset_ref);
    riscv_q7_to_q15_with_offset_ref(test1, test4, NNDataConversion_SIZE, 0);
    BENCH_END(riscv_q7_to_q15_with_offset_ref);

    printf("Start opt q7 to q15 offset implementation\n");

    BENCH_START(riscv_q7_to_q15_with_offset);
    riscv_q7_to_q15_with_offset(test1, test4 + NNDataConversion_SIZE, NNDataConversion_SIZE, 0);
    BENCH_END(riscv_q7_to_q15_with_offset);

    verify_results_q15(test4, test4 + NNDataConversion_SIZE, NNDataConversion_SIZE);

    delete[] test1;
    delete[] test3;
    delete[] test4;
#endif

#ifdef TEST_BasicMathforNN
#define BasicMathforNN_SIZE 100
    test1 = new q7_t[BasicMathforNN_SIZE * 2];
    test2 = new q15_t[BasicMathforNN_SIZE * 2];
    test3 = new q7_t[BasicMathforNN_SIZE * 2];
    test4 = new q15_t[BasicMathforNN_SIZE * 2];

    int8_t *test8 = new int8_t[BasicMathforNN_SIZE * 2];
    int32_t *test9 = new int32_t[BasicMathforNN_SIZE * 2];
    int32_t *test14 = new int32_t[BasicMathforNN_SIZE * 2];

    q7_t *mult_out_q7 = test3;
    q7_t *mult_ref_q7 = test3 + BasicMathforNN_SIZE;
    q15_t *mult_out_q15 = test4;
    q15_t *mult_ref_q15 = test4 + BasicMathforNN_SIZE;

    for (int i = 0; i < BasicMathforNN_SIZE * 2; i++) {
        test1[i] = (rand() % 256 - 128);
        test2[i] = (rand() % 65536 - 32768);
        test9[i] = (rand() % 65536 - 32768);
        test14[i] = (rand() % 1);
    }

    printf("start testing q7_t riscv_nn_mult_q7 case 1\n\n");
    // Test q7
    BENCH_START(riscv_nn_mult_q7);
    riscv_nn_mult_q7(test1, test1 + BasicMathforNN_SIZE, mult_out_q7, 5, BasicMathforNN_SIZE);
    BENCH_END(riscv_nn_mult_q7);

    BENCH_START(riscv_nn_mult_q7_ref);
    riscv_nn_mult_q7_ref(test1, test1 + BasicMathforNN_SIZE, mult_ref_q7, 5, BasicMathforNN_SIZE);
    BENCH_END(riscv_nn_mult_q7_ref);

    verify_results_q7(mult_out_q7, mult_ref_q7, BasicMathforNN_SIZE);

    printf("start testing q7_t riscv_nn_mult_q7 case 2\n\n");

    BENCH_START(riscv_nn_mult_q7);
    riscv_nn_mult_q7(test1, test1 + BasicMathforNN_SIZE, mult_out_q7, 9, BasicMathforNN_SIZE);
    BENCH_END(riscv_nn_mult_q7);

    BENCH_START(riscv_nn_mult_q7_ref);
    riscv_nn_mult_q7_ref(test1, test1 + BasicMathforNN_SIZE, mult_ref_q7, 9, BasicMathforNN_SIZE);
    BENCH_END(riscv_nn_mult_q7_ref);

    verify_results_q7(mult_out_q7, mult_ref_q7, BasicMathforNN_SIZE);

    // Test q15
    printf("start testing q7_t riscv_nn_mult_q15 case 1\n\n");
    BENCH_START(riscv_nn_mult_q15);
    riscv_nn_mult_q15(test2, test2 + BasicMathforNN_SIZE, mult_out_q15, 13, BasicMathforNN_SIZE);
    BENCH_END(riscv_nn_mult_q15);

    BENCH_START(riscv_nn_mult_q15_ref);
    riscv_nn_mult_q15_ref(test2, test2 + BasicMathforNN_SIZE, mult_ref_q15, 13, BasicMathforNN_SIZE);
    BENCH_END(riscv_nn_mult_q15_ref);

    verify_results_q15(mult_out_q15, mult_ref_q15, BasicMathforNN_SIZE);

    printf("start testing q7_t riscv_nn_mult_q15 case 2\n\n");
    BENCH_START(riscv_nn_mult_q15);
    riscv_nn_mult_q15(test2, test2 + BasicMathforNN_SIZE, mult_out_q15, 18, BasicMathforNN_SIZE);
    BENCH_END(riscv_nn_mult_q15);

    BENCH_START(riscv_nn_mult_q15_ref);
    riscv_nn_mult_q15_ref(test2, test2 + BasicMathforNN_SIZE, mult_ref_q15, 18, BasicMathforNN_SIZE);
    BENCH_END(riscv_nn_mult_q15_ref);

    verify_results_q15(mult_out_q15, mult_ref_q15, BasicMathforNN_SIZE);

    printf("Start ref q7 to q15 accumulate implementation\n");

    BENCH_START(riscv_nn_accumulate_q7_to_q15_ref);
    riscv_nn_accumulate_q7_to_q15_ref(test4, test1, BasicMathforNN_SIZE);
    BENCH_END(riscv_nn_accumulate_q7_to_q15_ref);

    printf("Start opt q7 to q15 accumulate implementation\n");

    BENCH_START(riscv_nn_accumulate_q7_to_q15);
    riscv_nn_accumulate_q7_to_q15(test4 + BasicMathforNN_SIZE, test1, BasicMathforNN_SIZE);
    BENCH_END(riscv_nn_accumulate_q7_to_q15);

    verify_results_q15(test4, test4 + BasicMathforNN_SIZE, BasicMathforNN_SIZE);

    q31_t output_opt = 0, output_ref = 0;

    printf("Start ref q7 add implementation\n");

    BENCH_START(riscv_nn_add_q7_ref);
    riscv_nn_add_q7_ref(test1, &output_ref, BasicMathforNN_SIZE);
    BENCH_END(riscv_nn_add_q7_ref);

    printf("Start opt q7 add implementation\n");

    BENCH_START(riscv_nn_add_q7);
    riscv_nn_add_q7(test1, &output_opt, BasicMathforNN_SIZE);
    BENCH_END(riscv_nn_add_q7);

    verify_results_int32(&output_ref, &output_opt, 1);

    //Only available for RISCV_MATH_MVEI
    // printf("Start ref q7 add implementation\n");

    // BENCH_START(riscv_nn_depthwise_conv_nt_t_padded_s8_ref);
    // riscv_nn_depthwise_conv_nt_t_padded_s8_ref(test1, &output_ref, BasicMathforNN_SIZE);
    // BENCH_END(riscv_nn_depthwise_conv_nt_t_padded_s8_ref);

    // printf("Start opt q7 add implementation\n");

    // BENCH_START(riscv_nn_depthwise_conv_nt_t_padded_s8);
    // riscv_nn_depthwise_conv_nt_t_padded_s8(test1, &output_opt, BasicMathforNN_SIZE);
    // BENCH_END(riscv_nn_depthwise_conv_nt_t_padded_s8);

    // verify_results_int32(&output_ref, &output_opt, 1);

    // printf("Start ref q7 add implementation\n");

    // BENCH_START(riscv_nn_depthwise_conv_nt_t_s8_ref);
    // riscv_nn_depthwise_conv_nt_t_s8_ref(test1, &output_ref, BasicMathforNN_SIZE);
    // BENCH_END(riscv_nn_depthwise_conv_nt_t_s8_ref);

    // printf("Start opt q7 add implementation\n");

    // BENCH_START(riscv_nn_depthwise_conv_nt_t_s8);
    // riscv_nn_depthwise_conv_nt_t_s8(test1, &output_opt, BasicMathforNN_SIZE);
    // BENCH_END(riscv_nn_depthwise_conv_nt_t_s8);

    // verify_results_int32(&output_ref, &output_opt, 1);
    int32_t sum_col_opt = 0, output_opt_s8[4] = { 0 };
    int32_t sum_col_ref = 0, output_ref_s8[4] = { 0 };
    printf("Start ref q7 add implementation\n");

    BENCH_START(riscv_nn_mat_mul_core_1x_s8_ref);
    riscv_nn_mat_mul_core_1x_s8_ref(BasicMathforNN_SIZE, test8, test8 + BasicMathforNN_SIZE, &sum_col_ref, &output_ref_s8[0]);
    BENCH_END(riscv_nn_mat_mul_core_1x_s8_ref);

    printf("Start opt q7 add implementation\n");

    BENCH_START(riscv_nn_mat_mul_core_1x_s8);
    riscv_nn_mat_mul_core_1x_s8(BasicMathforNN_SIZE, test8, test8 + BasicMathforNN_SIZE, &sum_col_opt, &output_opt_s8[0]);
    BENCH_END(riscv_nn_mat_mul_core_1x_s8);

    verify_results_int32(&sum_col_opt, &sum_col_ref, 1);
    verify_results_int32(output_opt_s8, output_ref_s8, 1);

    printf("Start ref q7 add implementation\n");

    BENCH_START(riscv_nn_mat_mul_core_4x_s8_ref);
    riscv_nn_mat_mul_core_4x_s8_ref(BasicMathforNN_SIZE, 0, test8, test8 + BasicMathforNN_SIZE, &sum_col_ref, &output_ref_s8[0]);
    BENCH_END(riscv_nn_mat_mul_core_4x_s8_ref);

    printf("Start opt q7 add implementation\n");

    BENCH_START(riscv_nn_mat_mul_core_4x_s8);
    riscv_nn_mat_mul_core_4x_s8(BasicMathforNN_SIZE, 0, test8, test8 + BasicMathforNN_SIZE, &sum_col_opt, &output_opt_s8[0]);
    BENCH_END(riscv_nn_mat_mul_core_4x_s8);

    verify_results_int32(&sum_col_opt, &sum_col_ref, 1);
    verify_results_int32(output_opt_s8, output_ref_s8, 1);

    printf("Start ref mat_mult implementation\n");
    test1 = new q7_t[500];
    test3 = new q7_t[500];
    BENCH_START(riscv_nn_mat_mult_nt_t_s8_ref);
    riscv_nn_mat_mult_nt_t_s8_ref(test1, test1 + 250, test9, test3, test9, test14, 50, 50, 50, 0, 0, -100, 100);
    BENCH_END(riscv_nn_mat_mult_nt_t_s8_ref);

    printf("Start opt mat_mult implementation\n");

    BENCH_START(riscv_nn_mat_mult_nt_t_s8);
    riscv_nn_mat_mult_nt_t_s8(test1, test1 + 250, test9, test3 + 250, test9, test14, 50, 50, 50, 0, 0, -100, 100);
    BENCH_END(riscv_nn_mat_mult_nt_t_s8);

    verify_results_q7(test3, test3 + BasicMathforNN_SIZE, 250);

    printf("Start ref s8 mat_mult_nt_t implementation\n");

    BENCH_START(riscv_nn_vec_mat_mult_t_s8_ref);
    riscv_nn_vec_mat_mult_t_s8_ref(test1, test1 + 250, test9, test3, 0, 0, 0, 1, 0, 50, 50, -100, 100);
    BENCH_END(riscv_nn_vec_mat_mult_t_s8_ref);

    printf("Start opt s8 mat_mult_nt_t implementation\n");

    BENCH_START(riscv_nn_vec_mat_mult_t_s8);
    riscv_nn_vec_mat_mult_t_s8(test1, test1 + 250, test9, test3 + 250, 0, 0, 0, 1, 0, 50, 50, -100, 100);
    BENCH_END(riscv_nn_vec_mat_mult_t_s8);

    verify_results_q7(test3, test3 + BasicMathforNN_SIZE, 250);

    delete[] test1;
    delete[] test2;
    delete[] test3;
    delete[] test4;
    delete[] test8;
    delete[] test9;
    delete[] test14;

#endif

    // Old ones

#ifdef TEST_NNMULT
#define NNMULT_DIM 100
    test1 = new q7_t[NNMULT_DIM * 2];
    test2 = new q15_t[NNMULT_DIM * 2];
    test3 = new q7_t[NNMULT_DIM * 2];
    test4 = new q15_t[NNMULT_DIM * 2];

    q7_t *mult_out_q7 = test3;
    q7_t *mult_ref_q7 = test3 + NNMULT_DIM;
    q15_t *mult_out_q15 = test4;
    q15_t *mult_ref_q15 = test4 + NNMULT_DIM;

    for (int i = 0; i < NNMULT_DIM * 2; i++) {
        test1[i] = (rand() % 256 - 128);
        test2[i] = (rand() % 65536 - 32768);
    }

    printf("start testing q7_t riscv_nn_mult_q7 case 1\n\n");
    // Test q7
    BENCH_START(riscv_nn_mult_q7);
    riscv_nn_mult_q7(test1, test1 + NNMULT_DIM, mult_out_q7, 5, NNMULT_DIM);
    BENCH_END(riscv_nn_mult_q7);

    BENCH_START(riscv_nn_mult_q7_ref);
    riscv_nn_mult_q7_ref(test1, test1 + NNMULT_DIM, mult_ref_q7, 5, NNMULT_DIM);
    BENCH_END(riscv_nn_mult_q7_ref);

    verify_results_q7(mult_out_q7, mult_ref_q7, NNMULT_DIM);

    printf("start testing q7_t riscv_nn_mult_q7 case 2\n\n");

    BENCH_START(riscv_nn_mult_q7);
    riscv_nn_mult_q7(test1, test1 + NNMULT_DIM, mult_out_q7, 9, NNMULT_DIM);
    BENCH_END(riscv_nn_mult_q7);

    BENCH_START(riscv_nn_mult_q7_ref);
    riscv_nn_mult_q7_ref(test1, test1 + NNMULT_DIM, mult_ref_q7, 9, NNMULT_DIM);
    BENCH_END(riscv_nn_mult_q7_ref);

    verify_results_q7(mult_out_q7, mult_ref_q7, NNMULT_DIM);

    // Test q15
    printf("start testing q7_t riscv_nn_mult_q15 case 1\n\n");
    BENCH_START(riscv_nn_mult_q15);
    riscv_nn_mult_q15(test2, test2 + NNMULT_DIM, mult_out_q15, 13, NNMULT_DIM);
    BENCH_END(riscv_nn_mult_q15);

    BENCH_START(riscv_nn_mult_q15_ref);
    riscv_nn_mult_q15_ref(test2, test2 + NNMULT_DIM, mult_ref_q15, 13, NNMULT_DIM);
    BENCH_END(riscv_nn_mult_q15_ref);

    verify_results_q15(mult_out_q15, mult_ref_q15, NNMULT_DIM);

    printf("start testing q7_t riscv_nn_mult_q15 case 2\n\n");
    BENCH_START(riscv_nn_mult_q15);
    riscv_nn_mult_q15(test2, test2 + NNMULT_DIM, mult_out_q15, 18, NNMULT_DIM);
    BENCH_END(riscv_nn_mult_q15);

    BENCH_START(riscv_nn_mult_q15_ref);
    riscv_nn_mult_q15_ref(test2, test2 + NNMULT_DIM, mult_ref_q15, 18, NNMULT_DIM);
    BENCH_END(riscv_nn_mult_q15_ref);

    verify_results_q15(mult_out_q15, mult_ref_q15, NNMULT_DIM);

#endif

#ifdef TEST_SIGMOID

#define SIGMOID_DIM 128

    /* This part tests the running of sigmoid functions */

    test1 = new q7_t[SIGMOID_DIM];
    test2 = new q15_t[SIGMOID_DIM];
    test3 = new q7_t[SIGMOID_DIM];
    test4 = new q15_t[SIGMOID_DIM];

    for (int i = 0; i < SIGMOID_DIM; i++) {
        test1[i] = (rand() % 256 - 128);
        test2[i] = (rand() % 65536 - 32768);
        test3[i] = test1[i];
        test4[i] = test2[i];
    }

    riscv_nn_activations_direct_q7(test3, SIGMOID_DIM, 3, RISCV_SIGMOID);

    for (int i = 0; i < SIGMOID_DIM; i++) {
        printf("in: %d  out: %d\n", test1[i], test3[i]);
    }

    printf("start testing q15_t sigmoid\n\n");

    riscv_nn_activations_direct_q15(test4, SIGMOID_DIM, 3, RISCV_SIGMOID);

    for (int i = 0; i < SIGMOID_DIM; i++) {
        printf("in: %d  out: %d\n", test2[i], test4[i]);
    }

    delete[] test1;
    delete[] test2;
    delete[] test3;
    delete[] test4;

#endif

#ifdef TEST_TANH

#define TANH_DIM 128

    /* This part tests the running of sigmoid functions */

    test1 = new q7_t[TANH_DIM];
    test2 = new q15_t[TANH_DIM];
    test3 = new q7_t[TANH_DIM];
    test4 = new q15_t[TANH_DIM];

    for (int i = 0; i < TANH_DIM; i++) {
        test1[i] = (rand() % 256 - 128);
        test2[i] = (rand() % 65536 - 32768);
        test3[i] = test1[i];
        test4[i] = test2[i];
    }

    riscv_nn_activations_direct_q7(test3, TANH_DIM, 3, RISCV_TANH);

    printf("start testing q7_t tanh\n\n");

    for (int i = 0; i < TANH_DIM; i++) {
        printf("in: %d  out: %d\n", test1[i], test3[i]);
    }

    printf("start testing q15_t tanh\n\n");

    riscv_nn_activations_direct_q15(test4, TANH_DIM, 3, RISCV_TANH);

    for (int i = 0; i < TANH_DIM; i++) {
        printf("in: %d  out: %d\n", test2[i], test4[i]);
    }

    delete[] test1;
    delete[] test2;
    delete[] test3;
    delete[] test4;

#endif

#ifdef TEST_POOL

#define POOL_IM_DIM 32
#define POOL_IM_CH  8

    test1 = new q7_t[POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH * 2];
    test2 = new q15_t[POOL_IM_DIM * POOL_IM_CH];
    test3 = new q7_t[POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH];

    for (int i = 0; i < POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH; i++) {
        test1[i] = (rand() % 256 - 128);
    }

    q7_t *img_in = test1 + POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH;
    q7_t *pool_out_ref = test3;
    q7_t *pool_out_opt = test3 + POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH / 2;

    for (int i = 0; i < POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH; i++) {
        test3[i] = 0;
    }

    // copy over the img input
    for (int i = 0; i < POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH; i++) {
        img_in[i] = test1[i];
    }

    initialize_results_q7(pool_out_ref, pool_out_opt, POOL_IM_DIM / 2 * POOL_IM_DIM / 2 * POOL_IM_CH);

    printf("Start maxpool reference implementation\n");

    BENCH_START(riscv_maxpool_q7_HWC_ref);
    riscv_maxpool_q7_HWC_ref(img_in, POOL_IM_DIM, POOL_IM_CH, 3, 0, 2, POOL_IM_DIM / 2, (q7_t *)test2, pool_out_ref);
    BENCH_END(riscv_maxpool_q7_HWC_ref);

    // copy over the img input
    for (int i = 0; i < POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH; i++) {
        img_in[i] = test1[i];
    }

    printf("Start maxpool opt implementation\n");

    BENCH_START(riscv_maxpool_q7_HWC);
    riscv_maxpool_q7_HWC(img_in, POOL_IM_DIM, POOL_IM_CH, 3, 0, 2, POOL_IM_DIM / 2, (q7_t *)test2, pool_out_opt);
    BENCH_END(riscv_maxpool_q7_HWC);

    verify_results_q7(pool_out_ref, pool_out_opt, POOL_IM_DIM / 2 * POOL_IM_DIM / 2 * POOL_IM_CH);

    // copy over the img input
    for (int i = 0; i < POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH; i++) {
        img_in[i] = test1[i];
    }

    // copy over the img input
    for (int i = 0; i < POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH; i++) {
        img_in[i] = test1[i];
    }

    printf("Start avepool ref implementation\n");

    BENCH_START(riscv_avepool_q7_HWC_ref);
    riscv_avepool_q7_HWC_ref(img_in, POOL_IM_DIM, POOL_IM_CH, 3, 0, 2, POOL_IM_DIM / 2, (q7_t *)test2, pool_out_ref);
    BENCH_END(riscv_avepool_q7_HWC_ref);

    // copy over the img input
    for (int i = 0; i < POOL_IM_DIM * POOL_IM_DIM * POOL_IM_CH; i++) {
        img_in[i] = test1[i];
    }

    printf("Start avepool opt implementation\n");

    BENCH_START(riscv_avepool_q7_HWC);
    riscv_avepool_q7_HWC(img_in, POOL_IM_DIM, POOL_IM_CH, 3, 0, 2, POOL_IM_DIM / 2, (q7_t *)test2, pool_out_opt);
    BENCH_END(riscv_avepool_q7_HWC);

    // special check here
    bool if_ave_pool_match = true;
    for (int i = 0; i < POOL_IM_DIM / 2 * POOL_IM_DIM / 2 * POOL_IM_CH; i++) {
        // we tolerate at most difference of 1 here because of rounding errors
        if (pool_out_ref[i] - pool_out_opt[i] >= 2 || pool_out_opt[i] - pool_out_ref[i] >= 2) {
            printf("Output mismatch at %d, expected %d, actual %d\n", i, pool_out_ref[i], pool_out_opt[i]);
            if_ave_pool_match = false;
        }
    }
    if (if_ave_pool_match == true) {
        printf("Outputs match.\n");
    }

    delete[] test1;
    delete[] test2;
    delete[] test3;

#endif

#ifdef TEST_RELU

#define RELU_DIM 127

    test1 = new q7_t[RELU_DIM];
    test2 = new q15_t[RELU_DIM];
    test3 = new q7_t[RELU_DIM];
    test4 = new q15_t[RELU_DIM];

    for (int i = 0; i < RELU_DIM; i++) {
        test1[i] = (rand() % 256 - 128);
        test2[i] = (rand() % 65536 - 32768);
        test3[i] = test1[i];
        test4[i] = test2[i];
    }

    q7_t *relu_ref_data_q7 = test1;
    q7_t *relu_opt_data_q7 = test3;
    q15_t *relu_ref_data_q15 = test2;
    q15_t *relu_opt_data_q15 = test4;

    printf("Start ref relu q7 implementation\n");

    BENCH_START(riscv_relu_q7_ref);
    riscv_relu_q7_ref(relu_ref_data_q7, RELU_DIM);
    BENCH_END(riscv_relu_q7_ref);

    printf("Start opt relu q7 implementation\n");

    BENCH_START(riscv_relu_q7);
    riscv_relu_q7(relu_opt_data_q7, RELU_DIM);
    BENCH_END(riscv_relu_q7);

    verify_results_q7(relu_ref_data_q7, relu_opt_data_q7, RELU_DIM);

    printf("Start ref relu q15 implementation\n");

    BENCH_START(riscv_relu_q15_ref);
    riscv_relu_q15_ref(relu_ref_data_q15, RELU_DIM);
    BENCH_END(riscv_relu_q15_ref);

    printf("Start opt relu q15 implementation\n");

    BENCH_START(riscv_relu_q15);
    riscv_relu_q15(relu_opt_data_q15, RELU_DIM);
    BENCH_END(riscv_relu_q15);

    verify_results_q15(relu_ref_data_q15, relu_opt_data_q15, RELU_DIM);

    delete[] test1;
    delete[] test2;
    delete[] test3;
    delete[] test4;

#endif

#ifdef TEST_IP

#define IP_ROW_DIM 127
#define IP_COL_DIM 127

    q7_t ip_weights[IP_ROW_DIM * IP_COL_DIM] = IP2_WEIGHT;
    q7_t ip_q7_opt_weights[IP_ROW_DIM * IP_COL_DIM] = IP4_WEIGHT;
    q7_t ip_q7_q15_opt_weights[IP_ROW_DIM * IP_COL_DIM] = IP4_q7_q15_WEIGHT;
    q15_t ip_q15_weights[IP_ROW_DIM * IP_COL_DIM] = IP2_WEIGHT;
    q15_t ip_q15_opt_weights[IP_ROW_DIM * IP_COL_DIM] = IP4_WEIGHT_Q15;

    test1 = new q7_t[IP_COL_DIM + IP_ROW_DIM];
    test2 = new q15_t[IP_COL_DIM];
    test3 = new q7_t[IP_ROW_DIM * 3];
    test4 = new q15_t[IP_COL_DIM + IP_ROW_DIM * 2];

    for (int i = 0; i < IP_ROW_DIM + IP_COL_DIM; i++) {
        test1[i] = rand() % 256 - 100;
    }
    for (int i = 0; i < IP_ROW_DIM * 3; i++) {
        test3[i] = 0;
    }

    q7_t *ip_bias_q7 = test1 + IP_COL_DIM;

    q7_t *ip_out_q7_ref = test3;
    q7_t *ip_out_q7_opt = test3 + IP_ROW_DIM;
    q7_t *ip_out_q7_opt_fast = test3 + 2 * IP_ROW_DIM;
    q15_t *ip_out_q15_ref = test4 + IP_COL_DIM;
    q15_t *ip_out_q15_opt = test4 + IP_COL_DIM + IP_ROW_DIM;

    initialize_results_q7(ip_out_q7_ref, ip_out_q7_opt, IP_ROW_DIM);
    initialize_results_q7(ip_out_q7_ref, ip_out_q7_opt_fast, IP_ROW_DIM);
    initialize_results_q7(ip_out_q7_ref, ip_out_q7_opt_fast, IP_ROW_DIM);

    printf("Start ref q7 implementation\n");

    BENCH_START(riscv_fully_connected_q7_ref);
    riscv_fully_connected_q7_ref(test1, ip_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, ip_bias_q7, ip_out_q7_ref, test2);
    BENCH_END(riscv_fully_connected_q7_ref);

    printf("Start q7 implementation\n");

    BENCH_START(riscv_fully_connected_q7);
    riscv_fully_connected_q7(test1, ip_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, ip_bias_q7, ip_out_q7_opt, test2);
    BENCH_END(riscv_fully_connected_q7);

    verify_results_q7(ip_out_q7_ref, ip_out_q7_opt, IP_ROW_DIM);

    printf("Start q7 ref opt implementation\n");

    BENCH_START(riscv_fully_connected_q7_opt_ref);
    riscv_fully_connected_q7_opt_ref(test1, ip_q7_opt_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, ip_bias_q7,
                                     ip_out_q7_opt_fast, test2);
    BENCH_END(riscv_fully_connected_q7_opt_ref);

    verify_results_q7(ip_out_q7_ref, ip_out_q7_opt_fast, IP_ROW_DIM);

    printf("Start q7 opt implementation\n");

    BENCH_START(riscv_fully_connected_q7_opt);
    riscv_fully_connected_q7_opt(test1, ip_q7_opt_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, ip_bias_q7, ip_out_q7_opt_fast,
                                 test2);
    BENCH_END(riscv_fully_connected_q7_opt);

    verify_results_q7(ip_out_q7_ref, ip_out_q7_opt_fast, IP_ROW_DIM);

    for (int i = 0; i < IP_ROW_DIM + IP_COL_DIM; i++) {
        test4[i] = (rand() % 65536 - 32768);
    }

    initialize_results_q15(ip_out_q15_ref, ip_out_q15_opt, IP_ROW_DIM);

    printf("Start ref q15 implementation\n");

    BENCH_START(riscv_fully_connected_q15_ref);
    riscv_fully_connected_q15_ref(test4, ip_q15_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, test2, ip_out_q15_ref, NULL);
    BENCH_END(riscv_fully_connected_q15_ref);

    printf("Start q15 implementation\n");

    BENCH_START(riscv_fully_connected_q15);
    riscv_fully_connected_q15(test4, ip_q15_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, test2, ip_out_q15_opt, NULL);
    BENCH_END(riscv_fully_connected_q15);

    verify_results_q15(ip_out_q15_ref, ip_out_q15_opt, IP_ROW_DIM);

    printf("Start ref opt q15 implementation\n");

    BENCH_START(riscv_fully_connected_q15_opt_ref);
    riscv_fully_connected_q15_opt_ref(test4, ip_q15_opt_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, test2, ip_out_q15_opt,
                                      NULL);
    BENCH_END(riscv_fully_connected_q15_opt_ref);

    verify_results_q15(ip_out_q15_ref, ip_out_q15_opt, IP_ROW_DIM);

    printf("Start opt q15 implementation\n");

    BENCH_START(riscv_fully_connected_q15_opt);
    riscv_fully_connected_q15_opt(test4, ip_q15_opt_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, test2, ip_out_q15_opt, NULL);
    BENCH_END(riscv_fully_connected_q15_opt);

    verify_results_q15(ip_out_q15_ref, ip_out_q15_opt, IP_ROW_DIM);

    initialize_results_q15(ip_out_q15_ref, ip_out_q15_opt, IP_ROW_DIM);

    printf("Start ref q7_q15 implementation\n");

    BENCH_START(riscv_fully_connected_mat_q7_vec_q15_ref);
    riscv_fully_connected_mat_q7_vec_q15_ref(test4, ip_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, ip_bias_q7, ip_out_q15_ref,
                                             test2);
    BENCH_END(riscv_fully_connected_mat_q7_vec_q15_ref);

    printf("Start q7_q15 implementation\n");

    BENCH_START(riscv_fully_connected_mat_q7_vec_q15);
    riscv_fully_connected_mat_q7_vec_q15(test4, ip_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, ip_bias_q7, ip_out_q15_opt,
                                         test2);
    BENCH_END(riscv_fully_connected_mat_q7_vec_q15);

    verify_results_q15(ip_out_q15_ref, ip_out_q15_opt, IP_ROW_DIM);

    printf("Start ref opt q7_q15 implementation\n");

    BENCH_START(riscv_fully_connected_mat_q7_vec_q15_opt_ref);
    riscv_fully_connected_mat_q7_vec_q15_opt_ref(test4, ip_q7_q15_opt_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, ip_bias_q7,
                                                 ip_out_q15_opt, test2);
    BENCH_END(riscv_fully_connected_mat_q7_vec_q15_opt_ref);

    verify_results_q15(ip_out_q15_ref, ip_out_q15_opt, IP_ROW_DIM);

    printf("Start opt q7_q15 implementation\n");

    BENCH_START(riscv_fully_connected_mat_q7_vec_q15_opt);
    riscv_fully_connected_mat_q7_vec_q15_opt(test4, ip_q7_q15_opt_weights, IP_COL_DIM, IP_ROW_DIM, 1, 7, ip_bias_q7,
                                             ip_out_q15_opt, test2);
    BENCH_END(riscv_fully_connected_mat_q7_vec_q15_opt);

    verify_results_q15(ip_out_q15_ref, ip_out_q15_opt, IP_ROW_DIM);

    delete[] test1;
    delete[] test2;
    delete[] test3;
    delete[] test4;

#endif

#ifdef TEST_NONSQUARE

    /* Use RCONV to differential with square CONV */

#define RCONV_IM_DIM_X  10
#define RCONV_IM_DIM_Y  8
#define RCONV_IM_CH     4
#define RCONV_KER_DIM_X 5
#define RCONV_KER_DIM_Y 3
#define RCONV_STRIDE_X  1
#define RCONV_STRIDE_Y  1
#define RCONV_PADDING_X 2
#define RCONV_PADDING_Y 1
#define RCONV_OUT_CH    4
#define RCONV_OUT_DIM_X 10
#define RCONV_OUT_DIM_Y 8

    test1 = new q7_t[RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH * RCONV_OUT_CH + RCONV_OUT_CH];
    test2 = new q15_t[2 * RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH];
    test3 =
        new q7_t[RCONV_IM_DIM_Y * RCONV_IM_DIM_X * RCONV_IM_CH + 2 * RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH];

    for (int i = 0; i < RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH * RCONV_OUT_CH + RCONV_OUT_CH; i++) {
        test1[i] = rand() % 256 - 100;
    }

    for (int i = 0;
         i < RCONV_IM_DIM_Y * RCONV_IM_DIM_X * RCONV_IM_CH + 2 * RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH; i++) {
        test3[i] = rand() % 256 - 100;
    }

    q7_t *rconv_weight_q7 = test1;
    q7_t *rconv_bias_q7 = test1 + RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH * RCONV_OUT_CH;

    q15_t *rconv_buf = test2;

    q7_t *rconv_im_in_q7 = test3;
    q7_t *rconv_im_out_ref_q7 = test3 + RCONV_IM_DIM_Y * RCONV_IM_DIM_X * RCONV_IM_CH;
    q7_t *rconv_im_out_opt_q7 =
        test3 + RCONV_IM_DIM_Y * RCONV_IM_DIM_X * RCONV_IM_CH + RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH;

    initialize_results_q7(rconv_im_out_ref_q7, rconv_im_out_opt_q7, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

    printf("start conv q7 nonsquare ref implementation\n");
    BENCH_START(riscv_convolve_HWC_q7_ref_nonsquare);
    riscv_convolve_HWC_q7_ref_nonsquare(rconv_im_in_q7, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH, rconv_weight_q7,
                                        RCONV_OUT_CH, RCONV_KER_DIM_X, RCONV_KER_DIM_Y, RCONV_PADDING_X, RCONV_PADDING_Y,
                                        RCONV_STRIDE_X, RCONV_STRIDE_Y, rconv_bias_q7, 1, 7, rconv_im_out_ref_q7,
                                        RCONV_OUT_DIM_X, RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_ref_nonsquare);

    printf("start conv q7 nonsquare opt implementation\n");
    BENCH_START(riscv_convolve_HWC_q7_fast_nonsquare);
    riscv_convolve_HWC_q7_fast_nonsquare(rconv_im_in_q7, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH, rconv_weight_q7,
                                         RCONV_OUT_CH, RCONV_KER_DIM_X, RCONV_KER_DIM_Y, RCONV_PADDING_X, RCONV_PADDING_Y,
                                         RCONV_STRIDE_X, RCONV_STRIDE_Y, rconv_bias_q7, 1, 7, rconv_im_out_opt_q7,
                                         RCONV_OUT_DIM_X, RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_fast_nonsquare);

    verify_results_q7(rconv_im_out_ref_q7, rconv_im_out_opt_q7, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

    initialize_results_q7(rconv_im_out_ref_q7, rconv_im_out_opt_q7, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

    printf("start conv q7 nonsquare ref implementation\n");
    BENCH_START(riscv_convolve_HWC_q7_ref_nonsquare);
    riscv_convolve_HWC_q7_ref_nonsquare(rconv_im_in_q7, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH, rconv_weight_q7,
                                        RCONV_OUT_CH, RCONV_KER_DIM_X, RCONV_KER_DIM_Y, RCONV_PADDING_X, RCONV_PADDING_Y,
                                        RCONV_STRIDE_X, RCONV_STRIDE_Y, rconv_bias_q7, 1, 7, rconv_im_out_ref_q7,
                                        RCONV_OUT_DIM_X, RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_ref_nonsquare);

    printf("start conv q7 nonsquare basic implementation\n");
    BENCH_START(riscv_convolve_HWC_q7_basic_nonsquare);
    riscv_convolve_HWC_q7_basic_nonsquare(rconv_im_in_q7, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH, rconv_weight_q7,
                                          RCONV_OUT_CH, RCONV_KER_DIM_X, RCONV_KER_DIM_Y, RCONV_PADDING_X, RCONV_PADDING_Y,
                                          RCONV_STRIDE_X, RCONV_STRIDE_Y, rconv_bias_q7, 1, 7, rconv_im_out_opt_q7,
                                          RCONV_OUT_DIM_X, RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_basic_nonsquare);

    verify_results_q7(rconv_im_out_ref_q7, rconv_im_out_opt_q7, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

    initialize_results_q7(rconv_im_out_ref_q7, rconv_im_out_opt_q7, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

    printf("start 1x1 conv q7 nonsquare fast implementation\n");
    BENCH_START(riscv_convolve_HWC_q7_fast_nonsquare);
    riscv_convolve_HWC_q7_fast_nonsquare(rconv_im_in_q7, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH, rconv_weight_q7,
                                         RCONV_OUT_CH, 1, 1, 0, 0, RCONV_STRIDE_X,
                                         RCONV_STRIDE_Y, rconv_bias_q7, 1, 7, rconv_im_out_ref_q7, RCONV_OUT_DIM_X,
                                         RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_fast_nonsquare);

    printf("start 1x1 conv q7 nonsquare dedicated function implementation\n");
    BENCH_START(riscv_convolve_1x1_HWC_q7_fast_nonsquare);
    riscv_convolve_1x1_HWC_q7_fast_nonsquare(rconv_im_in_q7, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH, rconv_weight_q7,
                                             RCONV_OUT_CH, 1, 1, 0, 0, RCONV_STRIDE_X,
                                             RCONV_STRIDE_Y, rconv_bias_q7, 1, 7, rconv_im_out_opt_q7, RCONV_OUT_DIM_X,
                                             RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_convolve_1x1_HWC_q7_fast_nonsquare);

    verify_results_q7(rconv_im_out_ref_q7, rconv_im_out_opt_q7, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

    printf("start depthwise separable conv q7 nonsquare ref implementation\n");
    BENCH_START(riscv_depthwise_separable_conv_HWC_q7_ref_nonsquare);
    riscv_depthwise_separable_conv_HWC_q7_ref_nonsquare(rconv_im_in_q7, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH,
                                                        rconv_weight_q7, RCONV_OUT_CH, RCONV_KER_DIM_X, RCONV_KER_DIM_Y,
                                                        RCONV_PADDING_X, RCONV_PADDING_Y, RCONV_STRIDE_X, RCONV_STRIDE_Y,
                                                        rconv_bias_q7, 1, 7, rconv_im_out_ref_q7, RCONV_OUT_DIM_X,
                                                        RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_depthwise_separable_conv_HWC_q7_ref_nonsquare);

    printf("start depthwise separable conv q7 nonsquare opt implementation\n");
    BENCH_START(riscv_depthwise_separable_conv_HWC_q7_nonsquare);
    riscv_depthwise_separable_conv_HWC_q7_nonsquare(rconv_im_in_q7, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH,
                                                    rconv_weight_q7, RCONV_OUT_CH, RCONV_KER_DIM_X, RCONV_KER_DIM_Y,
                                                    RCONV_PADDING_X, RCONV_PADDING_Y, RCONV_STRIDE_X, RCONV_STRIDE_Y,
                                                    rconv_bias_q7, 1, 7, rconv_im_out_opt_q7, RCONV_OUT_DIM_X,
                                                    RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_depthwise_separable_conv_HWC_q7_nonsquare);

    verify_results_q7(rconv_im_out_ref_q7, rconv_im_out_opt_q7, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

    delete[] test1;
    delete[] test2;
    delete[] test3;

    test2 = new q15_t[RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH * RCONV_OUT_CH + RCONV_OUT_CH];                          // weights + bias
    test4 = new q15_t[2 * RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH                                                      //buffer
                      + RCONV_IM_DIM_Y * RCONV_IM_DIM_X * RCONV_IM_CH + 2 * RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH]; // i/o

    for (int i = 0; i < RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH * RCONV_OUT_CH + RCONV_OUT_CH; i++) {
        test2[i] = rand() % 256 - 100;
    }

    for (int i = 0;
         i < 2 * RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH + RCONV_IM_DIM_Y * RCONV_IM_DIM_X * RCONV_IM_CH + 2 * RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH;
         i++) {
        test4[i] = rand() % 256 - 100;
    }

    q15_t *rconv_weight_q15 = test2;
    q15_t *rconv_bias_q15 = test2 + RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH * RCONV_OUT_CH;

    rconv_buf = test4;

    q15_t *rconv_im_in_q15 = test4 + 2 * RCONV_KER_DIM_Y * RCONV_KER_DIM_X * RCONV_IM_CH;
    q15_t *rconv_im_out_ref_q15 = rconv_im_in_q15 + RCONV_IM_DIM_Y * RCONV_IM_DIM_X * RCONV_IM_CH;
    q15_t *rconv_im_out_opt_q15 = rconv_im_out_ref_q15 + RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH;

    initialize_results_q15(rconv_im_out_ref_q15, rconv_im_out_opt_q15, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

    printf("start conv q15 nonsquare ref implementation\n");
    BENCH_START(riscv_convolve_HWC_q15_nonsquare_ref);
    riscv_convolve_HWC_q15_nonsquare_ref(rconv_im_in_q15, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH, rconv_weight_q15,
                                         RCONV_OUT_CH, RCONV_KER_DIM_X, RCONV_KER_DIM_Y, RCONV_PADDING_X, RCONV_PADDING_Y,
                                         RCONV_STRIDE_X, RCONV_STRIDE_Y, rconv_bias_q15, 1, 7, rconv_im_out_ref_q15,
                                         RCONV_OUT_DIM_X, RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q15_nonsquare_ref);

    printf("start conv q5 nonsquare opt implementation\n");
    BENCH_START(riscv_convolve_HWC_q15_fast_nonsquare);
    riscv_convolve_HWC_q15_fast_nonsquare(rconv_im_in_q15, RCONV_IM_DIM_X, RCONV_IM_DIM_Y, RCONV_IM_CH, rconv_weight_q15,
                                          RCONV_OUT_CH, RCONV_KER_DIM_X, RCONV_KER_DIM_Y, RCONV_PADDING_X, RCONV_PADDING_Y,
                                          RCONV_STRIDE_X, RCONV_STRIDE_Y, rconv_bias_q15, 1, 7, rconv_im_out_opt_q15,
                                          RCONV_OUT_DIM_X, RCONV_OUT_DIM_Y, rconv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q15_fast_nonsquare);

    verify_results_q15(rconv_im_out_ref_q15, rconv_im_out_opt_q15, RCONV_OUT_DIM_Y * RCONV_OUT_DIM_X * RCONV_OUT_CH);

    delete[] test2;
    delete[] test4;
#endif

#ifdef TEST_CONV

#define CONV_IM_DIM  16
#define CONV_IM_CH   16
#define CONV_KER_DIM 5
#define CONV_OUT_CH  16
#define CONV_OUT_DIM 16

    test1 = new q7_t[CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH + CONV_OUT_CH];
    test2 =
        new q15_t[CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH +
                  2 * CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH + CONV_OUT_CH];
    test3 = new q7_t[CONV_IM_DIM * CONV_IM_DIM * CONV_IM_CH + 2 * CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH];
    test4 = new q15_t[CONV_IM_DIM * CONV_IM_DIM * CONV_IM_CH + 2 * CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH];

    for (int i = 0; i < CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH + CONV_OUT_CH; i++) {
        test1[i] = rand() % 256 - 100;
    }

    for (int i = 0;
         i <
         CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH +
             2 * CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH + CONV_OUT_CH;
         i++) {
        test2[i] = (rand() % 65536 - 32768);
    }

    for (int i = 0; i < CONV_IM_DIM * CONV_IM_DIM * CONV_IM_CH + 2 * CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH; i++) {
        test3[i] = rand() % 256 - 100;
    }

    for (int i = 0; i < CONV_IM_DIM * CONV_IM_DIM * CONV_IM_CH + 2 * CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH; i++) {
        test4[i] = (rand() % 65536 - 32768);
    }

    q7_t *conv_weight_q7 = test1;
    q7_t *conv_bias_q7 = test1 + CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH;

    q15_t *conv_weight_q15 = test2;
    q15_t *conv_buf = test2 + CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH;
    q15_t *conv_bias_q15 =
        test2 + CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH +
        2 * CONV_KER_DIM * CONV_KER_DIM * CONV_IM_CH * CONV_OUT_CH;

    q7_t *conv_im_in_q7 = test3;
    q7_t *conv_im_out_ref_q7 = test3 + CONV_IM_DIM * CONV_IM_DIM * CONV_IM_CH;
    q7_t *conv_im_out_opt_q7 =
        test3 + CONV_IM_DIM * CONV_IM_DIM * CONV_IM_CH + CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH;

    q15_t *conv_im_in_q15 = test4;
    q15_t *conv_im_out_ref_q15 = test4 + CONV_IM_DIM * CONV_IM_DIM * CONV_IM_CH;
    q15_t *conv_im_out_opt_q15 =
        test4 + CONV_IM_DIM * CONV_IM_DIM * CONV_IM_CH + CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH;

    initialize_results_q7(conv_im_out_ref_q7, conv_im_out_opt_q7, CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH);

    printf("start q7 ref implementation\n");

    BENCH_START(riscv_convolve_HWC_q7_ref);
    riscv_convolve_HWC_q7_ref(conv_im_in_q7, CONV_IM_DIM, CONV_IM_CH, conv_weight_q7,
                              CONV_OUT_CH, CONV_KER_DIM, 2, 1, conv_bias_q7, 1, 7, conv_im_out_ref_q7,
                              CONV_OUT_DIM, conv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_ref);

    printf("start q7 basic implementation\n");

    BENCH_START(riscv_convolve_HWC_q7_basic);
    riscv_convolve_HWC_q7_basic(conv_im_in_q7, CONV_IM_DIM, CONV_IM_CH, conv_weight_q7,
                                CONV_OUT_CH, CONV_KER_DIM, 2, 1, conv_bias_q7, 1, 7, conv_im_out_opt_q7,
                                CONV_OUT_DIM, conv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_basic);

    verify_results_q7(conv_im_out_ref_q7, conv_im_out_opt_q7, CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH);

    printf("start q7 fast implementation\n");

    BENCH_START(riscv_convolve_HWC_q7_fast);
    riscv_convolve_HWC_q7_fast(conv_im_in_q7, CONV_IM_DIM, CONV_IM_CH, conv_weight_q7,
                               CONV_OUT_CH, CONV_KER_DIM, 2, 1, conv_bias_q7, 1, 7, conv_im_out_opt_q7,
                               CONV_OUT_DIM, conv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_fast);

    verify_results_q7(conv_im_out_ref_q7, conv_im_out_opt_q7, CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH);

    // testing with RGB
    printf("start q7 ref implementation for RGB\n");

    BENCH_START(riscv_convolve_HWC_q7_ref);
    riscv_convolve_HWC_q7_ref(conv_im_in_q7, CONV_IM_DIM, 3, conv_weight_q7,
                              CONV_OUT_CH, CONV_KER_DIM, 2, 1, conv_bias_q7, 1, 7, conv_im_out_ref_q7,
                              CONV_OUT_DIM, conv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_ref);

    printf("start q7 basic implementation for RGB\n");

    BENCH_START(riscv_convolve_HWC_q7_basic);
    riscv_convolve_HWC_q7_basic(conv_im_in_q7, CONV_IM_DIM, 3, conv_weight_q7,
                                CONV_OUT_CH, CONV_KER_DIM, 2, 1, conv_bias_q7, 1, 7, conv_im_out_opt_q7,
                                CONV_OUT_DIM, conv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_basic);

    verify_results_q7(conv_im_out_ref_q7, conv_im_out_opt_q7, CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH);

    printf("start q7 RGB implementation for RGB\n");

    BENCH_START(riscv_convolve_HWC_q7_RGB);
    riscv_convolve_HWC_q7_RGB(conv_im_in_q7, CONV_IM_DIM, 3, conv_weight_q7,
                              CONV_OUT_CH, CONV_KER_DIM, 2, 1, conv_bias_q7, 1, 7, conv_im_out_opt_q7,
                              CONV_OUT_DIM, conv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q7_RGB);

    verify_results_q7(conv_im_out_ref_q7, conv_im_out_opt_q7, CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH);

    // testing q15
    initialize_results_q15(conv_im_out_ref_q15, conv_im_out_opt_q15, CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH);

    printf("start q15 ref implementation\n");

    BENCH_START(riscv_convolve_HWC_q15_ref);
    riscv_convolve_HWC_q15_ref(conv_im_in_q15, CONV_IM_DIM, CONV_IM_CH, conv_weight_q15,
                               CONV_OUT_CH, CONV_KER_DIM, 2, 1, conv_bias_q15, 0, 15, conv_im_out_ref_q15,
                               CONV_OUT_DIM, conv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q15_ref);

    printf("start q15 basic implementation\n");

    BENCH_START(riscv_convolve_HWC_q15_basic);
    riscv_convolve_HWC_q15_basic(conv_im_in_q15, CONV_IM_DIM, CONV_IM_CH, conv_weight_q15,
                                 CONV_OUT_CH, CONV_KER_DIM, 2, 1, conv_bias_q15, 0, 15, conv_im_out_opt_q15,
                                 CONV_OUT_DIM, conv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q15_basic);

    verify_results_q15(conv_im_out_ref_q15, conv_im_out_opt_q15, CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH);

    printf("start q15 fast implementation\n");

    BENCH_START(riscv_convolve_HWC_q15_fast);
    riscv_convolve_HWC_q15_fast(conv_im_in_q15, CONV_IM_DIM, CONV_IM_CH, conv_weight_q15,
                                CONV_OUT_CH, CONV_KER_DIM, 2, 1, conv_bias_q15, 0, 15, conv_im_out_opt_q15,
                                CONV_OUT_DIM, conv_buf, NULL);
    BENCH_END(riscv_convolve_HWC_q15_fast);

    verify_results_q15(conv_im_out_ref_q15, conv_im_out_opt_q15, CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH);

    // depthwise separable conv
    initialize_results_q7(conv_im_out_ref_q7, conv_im_out_opt_q7, CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH);

    printf("start q7 depthwise_separable_conv ref implementation\n");

    BENCH_START(riscv_depthwise_separable_conv_HWC_q7_ref);
    riscv_depthwise_separable_conv_HWC_q7_ref(conv_im_in_q7, CONV_IM_DIM, CONV_IM_CH, conv_weight_q7,
                                              CONV_OUT_CH, CONV_KER_DIM, 2, 1, conv_bias_q7, 1, 7, conv_im_out_ref_q7,
                                              CONV_OUT_DIM, conv_buf, NULL);
    BENCH_END(riscv_depthwise_separable_conv_HWC_q7_ref);

    printf("start q7 depthwise_separable_conv implementation\n");

    BENCH_START(riscv_depthwise_separable_conv_HWC_q7);
    riscv_depthwise_separable_conv_HWC_q7(conv_im_in_q7, CONV_IM_DIM, CONV_IM_CH, conv_weight_q7,
                                          CONV_OUT_CH, CONV_KER_DIM, 2, 1, conv_bias_q7, 1, 7, conv_im_out_opt_q7,
                                          CONV_OUT_DIM, conv_buf, NULL);
    BENCH_END(riscv_depthwise_separable_conv_HWC_q7);

    verify_results_q7(conv_im_out_ref_q7, conv_im_out_opt_q7, CONV_OUT_DIM * CONV_OUT_DIM * CONV_OUT_CH);

    delete[] test1;
    delete[] test2;
    delete[] test3;
    delete[] test4;

#endif

    test_pass = true;
    test_index = 0;
    while (test_flags[test_index] != -1) {
        if (test_flags[test_index]) {
            test_pass = false;
        }
        test_index++;
    }
    if (test_pass) {
        printf("All tests passed\n");
    } else {
        printf("Test failed\n");
    }
    BENCH_FINISH;
    if (test_pass) {
        return 0;
    } else {
        return 1;
    }
}
