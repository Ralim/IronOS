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

static int DSP_COS(void)
{
    // f32_cos
    float32_t f32_cosoutput, f32_cosoutput_ref;
    uint16_t i;
    BENCH_START(riscv_cos_f32);
    for (int i = 0; i < 1000; i++)
        f32_cosoutput = riscv_cos_f32(PI);
    BENCH_END(riscv_cos_f32);
    f32_cosoutput_ref = ref_cos_f32(PI);
    if (fabs(f32_cosoutput - f32_cosoutput_ref) > DELTAF32) {
        BENCH_ERROR(riscv_cos_f32);
        printf("expect: %f, actual: %f\n", f32_cosoutput_ref, f32_cosoutput);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_cos_f32);
    // q31_cos
    q31_t q31_cosoutput, q31_cosoutput_ref;
    BENCH_START(riscv_cos_q31);
    for (int i = 0; i < 1000; i++)
        q31_cosoutput = riscv_cos_q31(0x7ffffff0);
    BENCH_END(riscv_cos_q31);
    q31_cosoutput_ref = ref_cos_q31(0x7ffffff0);
    if (labs(q31_cosoutput - q31_cosoutput_ref) > DELTAQ31) {
        BENCH_ERROR(riscv_cos_q31);
        printf("expect: %x, actual: %x\n", q31_cosoutput_ref, q31_cosoutput);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_cos_q31);
    // q15_cos
    q15_t q15_cosoutput, q15_cosoutput_ref;
    BENCH_START(riscv_cos_q15);
    for (int i = 0; i < 1000; i++)
        q15_cosoutput = riscv_cos_q15(0x7000);
    BENCH_END(riscv_cos_q15);
    q15_cosoutput_ref = ref_cos_q15(0x7000);
    if (abs(q15_cosoutput - q15_cosoutput_ref) > DELTAQ15) {
        BENCH_ERROR(riscv_cos_q15);
        printf("expect: %x, actual: %x\n", q15_cosoutput_ref, q15_cosoutput);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_cos_q15);
    printf("all cos tests are passed,well done!\n");
}
/*
*********************************************************************************************************
* 函 数 名: DSP_Sine
* 功能说明: 求sine函数
* 形 参：无
* 返 回 值: 无
*********************************************************************************************************
*/
static int DSP_SIN(void)
{
    // f32_sin
    float32_t f32_sinoutput, f32_sinoutput_ref;
    uint16_t i;
    BENCH_START(riscv_sin_f32);
    for (int i = 0; i < 1000; i++)
        f32_sinoutput = riscv_sin_f32(PI);
    BENCH_END(riscv_sin_f32);
    f32_sinoutput_ref = ref_sin_f32(PI);
    if (fabs(f32_sinoutput - f32_sinoutput_ref) > DELTAF32) {
        BENCH_ERROR(riscv_sin_f32);
        printf("expect: %f, actual: %f\n", f32_sinoutput_ref, f32_sinoutput);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_sin_f32);
    // q31_sin
    q31_t q31_sinoutput, q31_sinoutput_ref;
    BENCH_START(riscv_sin_q31);
    for (int i = 0; i < 1000; i++)
        q31_sinoutput = riscv_sin_q31(PI);
    BENCH_END(riscv_sin_q31);
    q31_sinoutput_ref = ref_sin_q31(PI);
    if (labs(q31_sinoutput - q31_sinoutput_ref) > DELTAQ31) {
        BENCH_ERROR(riscv_sin_q31);
        printf("expect: %x, actual: %x\n", q31_sinoutput_ref, q31_sinoutput);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_sin_q31);
    // q15_sin
    q15_t q15_sinoutput, q15_sinoutput_ref;
    BENCH_START(riscv_sin_q15);
    for (int i = 0; i < 1000; i++)
        q15_sinoutput = riscv_sin_q15(PI);
    BENCH_END(riscv_sin_q15);
    q15_sinoutput_ref = ref_sin_q15(PI);
    if (abs(q15_sinoutput - q15_sinoutput_ref) > DELTAQ15) {
        BENCH_ERROR(riscv_sin_q15);
        printf("expect: %x, actual: %x\n", q15_sinoutput_ref, q15_sinoutput);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_sin_q15);
    printf("all sin tests are passed,well done!\n");
}

int main()
{
    BENCH_INIT;
    DSP_COS();
    DSP_SIN();
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
