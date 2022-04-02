//
// Created by lujun on 19-6-28.
//
// This contains fir, iir and lms filters
// each one has it's own function.
// All function can be found in main function.
// If you don't want to use it, then comment it.
//
// This part only tests result after 50 dots.If you want to test results' all
// dots, please define  WITH_FRONT
//
// If there are too much numbers which makes it hard to read,in VScode you can
// press alt and z can shrimp them to one line
//
// You MUST be careful about overflow.
#include "riscv_math.h"
#include "array.h"
#include <stdint.h>
#include "../common.h"

#include "../HelperFunctions/math_helper.c"
#include "../HelperFunctions/ref_helper.c"

#include <stdio.h>
#define DELTAF32 (0.05f)
#define DELTAQ31 (63)
#define DELTAQ15 (1)
#define DELTAQ7 (1)
#define SNR_THRESHOLD_F32 (80.0f)

int test_flag_error = 0;

// #define WITH_FRONT

//***************************************************************************************
//				fir Sparse Filters
//***************************************************************************************
static void riscv_fir_Sparse_f32_lp(void)
{
    uint32_t i;
    /* clang-format off */
	riscv_fir_sparse_instance_f32 S;
    /* clang-format on */
    for (i = 0; i < NUM_TAPS; i++)
        pTapDelay[i] = i + 1;
    riscv_fir_sparse_init_f32(&S, NUM_TAPS, &firCoeffs32LP[0], &firStatef32[0], &pTapDelay[0], MAXDelay, TEST_LENGTH_SAMPLES);
    BENCH_START(riscv_fir_sparse_f32);
        riscv_fir_sparse_f32(&S, testInput_f32_50Hz_200Hz, testOutput_f32, &pTapDelayScratch_f32[0], TEST_LENGTH_SAMPLES);
    BENCH_END(riscv_fir_sparse_f32);
    riscv_fir_sparse_init_f32(&S, NUM_TAPS, &firCoeffs32LP[0], &firStatef32[0], &pTapDelay[0], MAXDelay, TEST_LENGTH_SAMPLES);
        ref_fir_sparse_f32(&S, testInput_f32_50Hz_200Hz, testOutput_f32_ref, &pTapDelayScratch_f32[0], TEST_LENGTH_SAMPLES);
#ifndef WITH_FRONT
    float snr = riscv_snr_f32(&testOutput_f32_ref[50], &testOutput_f32[50], 269);

    if (snr < SNR_THRESHOLD_F32) {
        BENCH_ERROR(riscv_fir_sparse_f32);
        printf("f32 fir_Sparse failed with snr:%f\n", snr);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_fir_sparse_f32);
#else
    float snr =
        riscv_snr_f32(&testOutput_ref[0], &testOutput[0], TEST_LENGTH_SAMPLES);

    if (snr < SNR_THRESHOLD_F32) {
        BENCH_ERROR(riscv_fir_sparse_f32);
        printf("f32 fir_Sparse failed with snr:%f\n", snr);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_fir_sparse_f32);
#endif
}
static void riscv_fir_Sparse_q31_lp(void)
{
    uint32_t i;
    /* clang-format off */
	riscv_fir_sparse_instance_q31 S;
    /* clang-format on */
    riscv_float_to_q31(testInput_f32_50Hz_200Hz, testInput_q31_50Hz_200Hz, TEST_LENGTH_SAMPLES);
    riscv_float_to_q31(firCoeffs32LP, firCoeffs32LP_q31, NUM_TAPS);
    for (i = 0; i < NUM_TAPS; i++)
        pTapDelay[i] = i + 1;
    riscv_fir_sparse_init_q31(&S, NUM_TAPS, &firCoeffs32LP_q31[0], &firStateq31[0], &pTapDelay[0], MAXDelay, TEST_LENGTH_SAMPLES);
    BENCH_START(riscv_fir_sparse_q31);
        riscv_fir_sparse_q31(&S, testInput_q31_50Hz_200Hz, testOutput_q31, &pTapDelayScratch_q31[0], TEST_LENGTH_SAMPLES);
    BENCH_END(riscv_fir_sparse_q31);
    riscv_fir_sparse_init_q31(&S, NUM_TAPS, &firCoeffs32LP_q31[0], &firStateq31[0], &pTapDelay[0], MAXDelay, TEST_LENGTH_SAMPLES);
        ref_fir_sparse_q31(&S, testInput_q31_50Hz_200Hz, testOutput_q31_ref, &pTapDelayScratch_q31[0], TEST_LENGTH_SAMPLES);
    riscv_q31_to_float(testOutput_q31, testOutput_f32, TEST_LENGTH_SAMPLES);
    riscv_q31_to_float(testOutput_q31_ref, testOutput_f32_ref, TEST_LENGTH_SAMPLES);
#ifndef WITH_FRONT
    float snr = riscv_snr_f32(&testOutput_f32_ref[50], &testOutput_f32[50], 269);

    if (snr < SNR_THRESHOLD_F32) {
        BENCH_ERROR(riscv_fir_sparse_q31);
        printf("q31 fir_Sparse failed with snr:%f\n", snr);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_fir_sparse_q31);
#else
    float snr = riscv_snr_f32(&testOutput_f32_ref[0], &testOutput_f32[0],
                            TEST_LENGTH_SAMPLES);

    if (snr < SNR_THRESHOLD_F32) {
        BENCH_ERROR(riscv_fir_sparse_q31);
        printf("q31 fir_Sparse failed with snr:%f\n", snr);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_fir_sparse_q31);
#endif
}
static void riscv_fir_Sparse_q15_lp(void)
{
    uint32_t i;
    /* clang-format off */
	riscv_fir_sparse_instance_q15 S;
    /* clang-format on */
    riscv_float_to_q15(testInput_f32_50Hz_200Hz, testInput_q15_50Hz_200Hz, TEST_LENGTH_SAMPLES);
    riscv_float_to_q15(firCoeffs32LP, firCoeffs32LP_q15, 29);
    for (i = 0; i < NUM_TAPS; i++)
        pTapDelay[i] = i + 1;
    riscv_fir_sparse_init_q15(&S, NUM_TAPS, &firCoeffs32LP_q15[0], &firStateq15[0], &pTapDelay[0], MAXDelay, TEST_LENGTH_SAMPLES);
    BENCH_START(riscv_fir_sparse_q15);
        riscv_fir_sparse_q15(&S, testInput_q15_50Hz_200Hz, testOutput_q15, pTapDelayScratch_q15, pTapDelayScratchOUT, TEST_LENGTH_SAMPLES);
    BENCH_END(riscv_fir_sparse_q15);
    riscv_fir_sparse_init_q15(&S, NUM_TAPS, &firCoeffs32LP_q15[0], &firStateq15[0], &pTapDelay[0], MAXDelay, TEST_LENGTH_SAMPLES);
        ref_fir_sparse_q15(&S, testInput_q15_50Hz_200Hz, testOutput_q15_ref, pTapDelayScratch_q15, pTapDelayScratchOUT, TEST_LENGTH_SAMPLES);
    riscv_q15_to_float(testOutput_q15, testOutput_f32, TEST_LENGTH_SAMPLES);
    riscv_q15_to_float(testOutput_q15_ref, testOutput_f32_ref, TEST_LENGTH_SAMPLES);
#ifndef WITH_FRONT
    float snr = riscv_snr_f32(&testOutput_f32_ref[50], &testOutput_f32[50], 269);

    if (snr < SNR_THRESHOLD_F32) {
        BENCH_ERROR(riscv_fir_sparse_q15);
        printf("q15 fir_Sparse failed with snr:%f\n", snr);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_fir_sparse_q15);
#else
    float snr = riscv_snr_f32(&testOutput_f32_ref[0], &testOutput_f32[0],
                            TEST_LENGTH_SAMPLES);

    if (snr < SNR_THRESHOLD_F32) {
        BENCH_ERROR(riscv_fir_sparse_q15);
        printf("q15 fir_Sparse failed with snr:%f\n", snr);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_fir_sparse_q15);
#endif
}
static void riscv_fir_Sparse_q7_lp(void)
{
    uint32_t i;
    /* clang-format off */
	riscv_fir_sparse_instance_q7 S;
    /* clang-format on */
    riscv_float_to_q7(testInput_f32_50Hz_200Hz, testInput_q7_50Hz_200Hz, TEST_LENGTH_SAMPLES);
    riscv_float_to_q7(firCoeffs32LP, firCoeffs32LP_q7, NUM_TAPS);
    for (i = 0; i < NUM_TAPS; i++)
        pTapDelay[i] = i + 1;
    riscv_fir_sparse_init_q7(&S, NUM_TAPS, &firCoeffs32LP_q7[0], &firStateq7[0], &pTapDelay[0], MAXDelay, TEST_LENGTH_SAMPLES);
    BENCH_START(riscv_fir_sparse_q7);
        riscv_fir_sparse_q7(&S, testInput_q7_50Hz_200Hz, testOutput_q7, &pTapDelayScratch_q7[0], pTapDelayScratchOUT, TEST_LENGTH_SAMPLES);
    BENCH_END(riscv_fir_sparse_q7);
    riscv_fir_sparse_init_q7(&S, NUM_TAPS, &firCoeffs32LP_q7[0], &firStateq7[0], &pTapDelay[0], MAXDelay, TEST_LENGTH_SAMPLES);
        ref_fir_sparse_q7(&S, testInput_q7_50Hz_200Hz, testOutput_q7_ref, &pTapDelayScratch_q7[0], pTapDelayScratchOUT, TEST_LENGTH_SAMPLES);
    riscv_q7_to_float(testOutput_q7, testOutput_f32, TEST_LENGTH_SAMPLES);
    riscv_q7_to_float(testOutput_q7_ref, testOutput_f32_ref, TEST_LENGTH_SAMPLES);
#ifndef WITH_FRONT
    float snr = riscv_snr_f32(&testOutput_f32_ref[50], &testOutput_f32[50], 269);

    if (snr < SNR_THRESHOLD_F32) {
        BENCH_ERROR(riscv_fir_sparse_q7);
        printf("q7 fir_Sparse failed with snr:%f\n", snr);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_fir_sparse_q7);
#else
    float snr = riscv_snr_f32(&testOutput_f32_ref[0], &testOutput_f32[0],
                            TEST_LENGTH_SAMPLES);

    if (snr < SNR_THRESHOLD_F32) {
        BENCH_ERROR(riscv_fir_sparse_q7);
        printf("q7 fir_Sparse failed with snr:%f\n", snr);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_fir_sparse_q7);
#endif
}

int main()
{
    BENCH_INIT;
    riscv_fir_Sparse_f32_lp();
    riscv_fir_Sparse_q31_lp();
    riscv_fir_Sparse_q15_lp();
    riscv_fir_Sparse_q7_lp();
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
