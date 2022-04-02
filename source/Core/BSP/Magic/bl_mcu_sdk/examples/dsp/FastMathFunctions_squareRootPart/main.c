//
// Created by lujun on 19-6-28.
//
// This contains sin, cos and sqrt calculates
// each one has it's own function.
// All function can be found in main function.
// If you don't want to use it, then comment it.
#include "riscv_math.h"
#include <stdint.h>
#include <stdlib.h>
#include "../common.h"

#include "../HelperFunctions/math_helper.c"
#include "../HelperFunctions/ref_helper.c"

#include <stdio.h>
#define DELTAF32 (0.05f)
#define DELTAQ31 (63)
#define DELTAQ15 (1)
#define DELTAQ7 (1)

int test_flag_error = 0;

static int DSP_SQRT(void)
{
    float32_t f32_pOUT, f32_pOUT_ref;
    q31_t q31_pOUT, q31_pOUT_ref;
    q15_t q15_pOUT, q15_pOUT_ref;
    // f32_sqrt
    BENCH_START(riscv_sqrt_f32);
    for (int i = 0; i < 1000; i++)
        riscv_sqrt_f32(100, &f32_pOUT);
    BENCH_END(riscv_sqrt_f32);
    // ref_sqrt_f32(100, &f32_pOUT_ref);
    f32_pOUT_ref = 10.0f;
    if (fabs(f32_pOUT - f32_pOUT_ref) > DELTAF32) {
        BENCH_ERROR(riscv_sqrt_f32);
        printf("expect: %f, actual: %f\n", f32_pOUT_ref, f32_pOUT);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_sqrt_f32);
    // q31_sqrt
    BENCH_START(riscv_sqrt_q31);
    for (int i = 0; i < 1000; i++)
        riscv_sqrt_q31(100, &q31_pOUT);
    BENCH_END(riscv_sqrt_q31);
    ref_sqrt_q31(100, &q31_pOUT_ref);
    if (labs(q31_pOUT - q31_pOUT_ref) > DELTAQ31) {
        BENCH_ERROR(riscv_sqrt_q31);
        printf("expect: %x, actual: %x\n", q31_pOUT_ref, q31_pOUT);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_sqrt_q31);
    // q15_sqrt
    BENCH_START(riscv_sqrt_q15);
    for (int i = 0; i < 1000; i++)
        riscv_sqrt_q15(100, &q15_pOUT);
    BENCH_END(riscv_sqrt_q15);
    ref_sqrt_q15(100, &q15_pOUT_ref);
    if (abs(q15_pOUT - q15_pOUT_ref) > DELTAQ15) {
        BENCH_ERROR(riscv_sqrt_q15);
        printf("expect: %x, actual: %x\n", q15_pOUT_ref, q15_pOUT);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_sqrt_q15);
}

static int DSP_DIVIDE(void)
{
    q15_t q15_pOUT, q15_pOUT_ref;
    int16_t shift;
    // q15_sqrt
    BENCH_START(riscv_divide_q15);
    for (int i = 0; i < 1000; i++)
        riscv_divide_q15(4203, 2490, &q15_pOUT, &shift);
    BENCH_END(riscv_divide_q15);
    ref_divide_q15(4203, 2490, &q15_pOUT_ref, &shift);
    if (abs(q15_pOUT - q15_pOUT_ref) > DELTAQ15) {
        BENCH_ERROR(riscv_divide_q15);
        printf("expect: %x, actual: %x\n", q15_pOUT_ref, q15_pOUT);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_divide_q15);
}

static int DSP_EXP(void)
{
    float32_t f32_pOUT[256], f32_pOUT_ref[256];
    float32_t f32_pIN[256];
    for (int i = 0; i < 256; i++)
        f32_pIN[i] = (float32_t) rand();
    // f32_sqrt
    BENCH_START(riscv_vexp_f32);
    for (int i = 0; i < 256; i++)
        riscv_vexp_f32(f32_pIN, &f32_pOUT, 256);
    BENCH_END(riscv_vexp_f32);
    ref_vexp_f32(f32_pIN, &f32_pOUT_ref, 256);
    for (int i = 0; i < 256; i++)
        if (fabs(f32_pOUT[i] - f32_pOUT_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_vexp_f32);
            printf("index: %d, expect: %f, actual: %f\n", i, f32_pOUT_ref[i], f32_pOUT[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_vexp_f32);
}

static int DSP_LOG(void)
{
    float32_t f32_pOUT[256], f32_pOUT_ref[256];
    float32_t f32_pIN[256];
    for (int i = 0; i < 256; i++)
        f32_pIN[i] = (float32_t) rand();
    // f32_sqrt
    BENCH_START(riscv_vlog_f32);
    for (int i = 0; i < 256; i++)
        riscv_vlog_f32(f32_pIN, &f32_pOUT, 256);
    BENCH_END(riscv_vlog_f32);
    ref_vlog_f32(f32_pIN, &f32_pOUT_ref, 256);
    for (int i = 0; i < 256; i++)
        if (fabs(f32_pOUT[i] - f32_pOUT_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_vlog_f32);
            printf("index: %d, expect: %f, actual: %f\n", i, f32_pOUT_ref[i], f32_pOUT[i]);
            test_flag_error = 1;
        }
    BENCH_STATUS(riscv_vlog_f32);
}


int main()
{
    BENCH_INIT;
    DSP_SQRT();
    DSP_DIVIDE();
    DSP_EXP();
    DSP_LOG();
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
