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

/* clang-format on */
//***************************************************************************************
//				iir		df2t
//***************************************************************************************
static void riscv_iir_df2t_f32_lp(void)
{
    uint32_t i;
    /* clang-format off */
	riscv_biquad_cascade_df2T_instance_f32 S;
    /* clang-format on */
    float32_t ScaleValue;
    riscv_biquad_cascade_df2T_init_f32(&S, numStages, IIRCoeffs32LP, &IIRStateF32[0]);
    BENCH_START(riscv_biquad_cascade_df2T_f32);
    riscv_biquad_cascade_df2T_f32(&S, testInput_f32_50Hz_200Hz, testOutput_f32, TEST_LENGTH_SAMPLES);
    BENCH_END(riscv_biquad_cascade_df2T_f32);
    riscv_biquad_cascade_df2T_init_f32(&S, numStages, IIRCoeffs32LP, &IIRStateF32[0]);
    ref_biquad_cascade_df2T_f32(&S, testInput_f32_50Hz_200Hz, testOutput_f32_ref, TEST_LENGTH_SAMPLES);
    // ScaleValue = 0.052219514664161221f * 0.04279801741658381f;
#ifndef WITH_FRONT
    float snr = riscv_snr_f32(&testOutput_f32_ref[50], &testOutput_f32[50], 269);

    if (snr < SNR_THRESHOLD_F32) {
        BENCH_ERROR(riscv_biquad_cascade_df2T_f32);
        printf("f32 biquad_cascade_df2T failed with snr:%f\n", snr);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_biquad_cascade_df2T_f32);
#else
    float snr =
        riscv_snr_f32(&testOutput_f32_ref[0], &testOutput_f32[0], TEST_LENGTH_SAMPLES);

    if (snr < SNR_THRESHOLD_F32) {
        BENCH_ERROR(riscv_biquad_cascade_df2T_f32);
        printf("f32 biquad_cascade_df2T failed with snr:%f\n", snr);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_biquad_cascade_df2T_f32);
#endif
}
static void riscv_iir_stereo_df2t_f32_lp(void)
{
    uint32_t i;
    /* clang-format off */
	riscv_biquad_cascade_stereo_df2T_instance_f32 S;
    /* clang-format on */
    // float32_t ScaleValue;
    riscv_biquad_cascade_stereo_df2T_init_f32(&S, numStages, IIRCoeffs32LP, IIRStateF32);
    BENCH_START(riscv_biquad_cascade_stereo_df2T_f32);
    riscv_biquad_cascade_stereo_df2T_f32(&S, testInput_f32_50Hz_200Hz, testOutput_f32, TEST_LENGTH_SAMPLES);
    BENCH_END(riscv_biquad_cascade_stereo_df2T_f32);
    riscv_biquad_cascade_stereo_df2T_init_f32(&S, numStages, IIRCoeffs32LP, IIRStateF32);
    ref_biquad_cascade_stereo_df2T_f32(&S, testInput_f32_50Hz_200Hz, testOutput_f32_ref, TEST_LENGTH_SAMPLES);
    // ScaleValue = 0.052219514664161221f * 0.04279801741658381f;
#ifndef WITH_FRONT
    float snr = riscv_snr_f32(&testOutput_f32_ref[50], &testOutput_f32[50], 269);

    if (snr < SNR_THRESHOLD_F32) {
        BENCH_ERROR(riscv_biquad_cascade_stereo_df2T_f32);
        printf("f32 biquad_cascade_stereo_df2T failed with snr:%f\n", snr);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_biquad_cascade_stereo_df2T_f32);
#else
    float snr =
        riscv_snr_f32(&testOutput_f32_ref[0], &testOutput_f32[0], TEST_LENGTH_SAMPLES);

    if (snr < SNR_THRESHOLD_F32) {
        BENCH_ERROR(riscv_biquad_cascade_stereo_df2T_f32);
        printf("f32 biquad_cascade_stereo_df2T failed with snr:%f\n", snr);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_biquad_cascade_stereo_df2T_f32);
#endif
}
static void riscv_iir_df2t_f64_lp(void)
{
    uint32_t i;
    /* clang-format off */
	riscv_biquad_cascade_df2T_instance_f64 S;
    /* clang-format on */
    // float32_t ScaleValue;
    riscv_biquad_cascade_df2T_init_f64(&S, numStages, IIRCoeffs64LP, &IIRStateF64[0]);
    BENCH_START(riscv_biquad_cascade_df2T_f64);
    riscv_biquad_cascade_df2T_f64(&S, testInput_f64_50Hz_200Hz, testOutput_f64, TEST_LENGTH_SAMPLES);
    BENCH_END(riscv_biquad_cascade_df2T_f64);
    riscv_biquad_cascade_df2T_init_f64(&S, numStages, IIRCoeffs32LP, &IIRStateF64[0]);
    ref_biquad_cascade_df2T_f64(&S, testInput_f64_50Hz_200Hz, testOutput_f64_ref, TEST_LENGTH_SAMPLES);
    // ScaleValue = 0.052219514664161221f * 0.04279801741658381f;
#ifndef WITH_FRONT
    float snr = riscv_snr_f32((float32_t *)&testOutput_f64_ref[50], (float32_t *)&testOutput_f64[50], 269);

    if (snr < SNR_THRESHOLD_F32) {
        BENCH_ERROR(riscv_biquad_cascade_df2T_f64);
        printf("f32 biquad_cascade_df2T failed with snr:%f\n", snr);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_biquad_cascade_df2T_f64);
#else
    float snr =
        riscv_snr_f32(&testOutput_ref[0], &testOutput[0], TEST_LENGTH_SAMPLES);

    if (snr < SNR_THRESHOLD_F32) {
        BENCH_ERROR(riscv_biquad_cascade_df2T_f64);
        printf("f32 biquad_cascade_df2T failed with snr:%f\n", snr);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_biquad_cascade_df2T_f64);
#endif
}

int main()
{
    BENCH_INIT;
    riscv_iir_df2t_f32_lp();
    riscv_iir_stereo_df2t_f32_lp();
    riscv_iir_df2t_f64_lp();
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
