//
// Created by lujun on 19-6-28.
//
// This contains f32, q31, q15, q7 to eachother, fill and copy
// each one has it's own function.
// All function can be found in main function.
// If you don't want to use it, then comment it.
#include "riscv_math.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "array.h"

#include "../HelperFunctions/math_helper.c"
#include "../HelperFunctions/ref_helper.c"

#include "../common.h"

#define DELTAF32 (0.05f)
#define DELTAQ31 (63)
#define DELTAQ15 (1)
#define DELTAQ7 (1)

int test_flag_error = 0;

/*
*********************************************************************************************************
*	? ? ?: DSP_Copy
*	????: ????
*	?    ?:?
*	? ? ?: ?
*********************************************************************************************************
*/
static int DSP_Copy(void)
{
    int i;
    BENCH_START(riscv_copy_f32);
    riscv_copy_f32(pSrc_f32, pDst_f32, ARRAY_SIZE);
    BENCH_END(riscv_copy_f32);
    ref_copy_f32(pSrc_f32, pDst_f32_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (fabs(pDst_f32[i] - pDst_f32_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_copy_f32);
            printf("index: %d,expect: %f, actual: %f\n", i, pDst_f32_ref[i],
                   pDst_f32[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_copy_f32);

    /*****************************************************************/
    BENCH_START(riscv_copy_q31);
    riscv_copy_q31(pSrc_q31, pDst_q31, ARRAY_SIZE);
    BENCH_END(riscv_copy_q31);
    ref_copy_q31(pSrc_q31, pDst_q31_ref, ARRAY_SIZE);

    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(pDst_q31_ref[i] - pDst_q31[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_copy_q31);
            printf("index: %d,expect: %d, actual: %d\n", i, pDst_q31_ref[i],
                   pDst_q31[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_copy_q31);

    /*****************************************************************/
    BENCH_START(riscv_copy_q15);
    riscv_copy_q15(pSrc_q15, pDst_q15, ARRAY_SIZE);
    BENCH_END(riscv_copy_q15);
    ref_copy_q15(pSrc_q15, pDst_q15_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(pDst_q15_ref[i] - pDst_q15[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_copy_q15);
            printf("index: %d,expect: %d, actual: %d\n", pDst_q15_ref[i], pDst_q15[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_copy_q15);

    /*****************************************************************/
    BENCH_START(riscv_copy_q7);
    riscv_copy_q7(pSrc_q7, pDst_q7, ARRAY_SIZE);
    BENCH_END(riscv_copy_q7);
    ref_copy_q7(pSrc_q7, pDst_q7_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(pDst_q7_ref[i] - pDst_q7[i]) > DELTAQ7) {
            BENCH_ERROR(riscv_copy_q7);
            printf("index: %d,expect: %f, actual: %f\n", i, pDst_q7_ref[i],
                   pDst_q7[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_copy_q7);

    /*****************************************************************/
    printf("all copy tests are passed,well done!\n");
}

/*
*********************************************************************************************************
*	? ? ?: DSP_Fill
*	????: ????
*	?    ?:?
*	? ? ?: ?
*********************************************************************************************************
*/
static int DSP_Fill(void)
{
    int i;
    BENCH_START(riscv_fill_f32);
    riscv_fill_f32(3.33f, pDst_f32, ARRAY_SIZE);
    BENCH_END(riscv_fill_f32);
    ref_fill_f32(3.33f, pDst_f32_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (fabs(pDst_f32[i] - pDst_f32_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_fill_f32);
            printf("index: %d,expect: %f, actual: %f\n", i, pDst_f32_ref[i],
                   pDst_f32[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_fill_f32);

    /*****************************************************************/
    BENCH_START(riscv_fill_q31);
    riscv_fill_q31(0x11111111, pDst_q31, ARRAY_SIZE);
    BENCH_END(riscv_fill_q31);
    ref_fill_q31(0x11111111, pDst_q31_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(pDst_q31_ref[i] - pDst_q31[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_fill_q31);
            printf("index: %d,expect: %d, actual: %d\n", i, pDst_q31_ref[i],
                   pDst_q31[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_fill_q31);

    /*****************************************************************/
    BENCH_START(riscv_fill_q15);
    riscv_fill_q15(0x1111, pDst_q15, ARRAY_SIZE);
    BENCH_END(riscv_fill_q15);
    ref_fill_q15(0x1111, pDst_q15_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(pDst_q15_ref[i] - pDst_q15[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_fill_q15);
            printf("index: %d,expect: %d, actual: %d\n", i, pDst_q15_ref[i],
                   pDst_q15[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_fill_q15);

    /*****************************************************************/
    BENCH_START(riscv_fill_q7);
    riscv_fill_q7(0x11, pDst_q7, ARRAY_SIZE);
    BENCH_END(riscv_fill_q7);
    ref_fill_q7(0x11, pDst_q7_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(pDst_q7_ref[i] - pDst_q7[i]) > DELTAQ7) {
            BENCH_ERROR(riscv_fill_q7);
            printf("index: %d,expect: %d, actual: %d\n", i, pDst_q7_ref[i],
                   pDst_q7[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_fill_q7);

    /*****************************************************************/
    printf("all fill tests are passed,well done!\n");
}

/*
*********************************************************************************************************
*	? ? ?: DSP_FloatToFix
*	????: ???????
*	?    ?:?
*	? ? ?: ?
*********************************************************************************************************
*/
static int DSP_FloatToFix(void)
{
    int i;
    /*****************************************************************/
    BENCH_START(riscv_float_to_q31);
    riscv_float_to_q31(pSrc_f32, pDst_q31, ARRAY_SIZE);
    BENCH_END(riscv_float_to_q31);
    ref_float_to_q31(pSrc_f32, pDst_q31_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(pDst_q31_ref[i] - pDst_q31[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_float_to_q31);
            printf("index: %d,expect: %d, actual: %d\n", i, pDst_q31_ref[i],
                   pDst_q31[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_float_to_q31);

    /*****************************************************************/
    BENCH_START(riscv_float_to_q15);
    riscv_float_to_q15(pSrc_f32, pDst_q15, ARRAY_SIZE);
    BENCH_END(riscv_float_to_q15);
    ref_float_to_q15(pSrc_f32, pDst_q15_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(pDst_q15_ref[i] - pDst_q15[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_float_to_q15);
            printf("index: %d, expect: %d, actual: %d\n", i,
                   pDst_q15_ref[i], pDst_q15[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_float_to_q15);

    /*****************************************************************/
    BENCH_START(riscv_float_to_q7);
    riscv_float_to_q7(pSrc_f32, pDst_q7, ARRAY_SIZE);
    BENCH_END(riscv_float_to_q7);
    ref_float_to_q7(pSrc_f32, pDst_q7_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(pDst_q7_ref[i] - pDst_q7[i]) > DELTAQ7) {
            BENCH_ERROR(riscv_float_to_q7);
            printf("index: %d,expect: %d, actual: %d\n", i, pDst_q7_ref[i], pDst_q7[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_float_to_q7);

    /*****************************************************************/
    printf("all float to fix tests are passed,well done!\n");
}

/*
*********************************************************************************************************
*	? ? ?: DSP_Q7
*	????: Q7???????????
*	?    ?:?
*	? ? ?: ?
*********************************************************************************************************
*/
static int DSP_Q7(void)
{
    int i;
    BENCH_START(riscv_q7_to_float);
    riscv_q7_to_float(pSrc_q7, pDst_f32, ARRAY_SIZE);
    BENCH_END(riscv_q7_to_float);
    ref_q7_to_float(pSrc_q7, pDst_f32_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (fabs(pDst_f32[i] - pDst_f32_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_q7_to_float);
            printf("expect: %f, actual: %f\n", pDst_f32_ref[i], pDst_f32[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_q7_to_float);

    /*****************************************************************/
    BENCH_START(riscv_q7_to_q31);
    riscv_q7_to_q31(pSrc_q7, pDst_q31, ARRAY_SIZE);
    BENCH_END(riscv_q7_to_q31);
    ref_q7_to_q31(pSrc_q7, pDst_q31_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(pDst_q31[i] - pDst_q31_ref[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_q7_to_q31);
            printf("expect: %d, actual: %d\n", pDst_q31_ref[i], pDst_q31[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_q7_to_q31);

    /*****************************************************************/
    BENCH_START(riscv_q7_to_q15);
    riscv_q7_to_q15(pSrc_q7, pDst_q15, ARRAY_SIZE);
    BENCH_END(riscv_q7_to_q15);
    ref_q7_to_q15(pSrc_q7, pDst_q15_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(pDst_q15[i] - pDst_q15_ref[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_q7_to_q15);
            printf("expect: %d, actual: %d\n", pDst_q15_ref[i], pDst_q15[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_q7_to_q15);

    /*****************************************************************/
    printf("all q7 to others tests are passed,well done!\n");
}

/*
*********************************************************************************************************
*	? ? ?: DSP_Q15
*	????: Q15???????????
*	?    ?:?
*	? ? ?: ?
*********************************************************************************************************
*/
static int DSP_Q15(void)
{
    int i;
    BENCH_START(riscv_q15_to_float);
    riscv_q15_to_float(pSrc_q15, pDst_f32, ARRAY_SIZE);
    BENCH_END(riscv_q15_to_float);
    ref_q15_to_float(pSrc_q15, pDst_f32_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (fabs(pDst_f32[i] - pDst_f32_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_q15_to_float);
            printf("expect: %f, actual: %f\n", pDst_f32_ref[i], pDst_f32[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_q15_to_float);

    /*****************************************************************/
    BENCH_START(riscv_q15_to_q31);
    riscv_q15_to_q31(pSrc_q15, pDst_q31, ARRAY_SIZE);
    BENCH_END(riscv_q15_to_q31);
    ref_q15_to_q31(pSrc_q15, pDst_q31_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(pDst_q31[i] - pDst_q31_ref[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_q15_to_q31);
            printf("expect: %d, actual: %d\n", pDst_q31_ref[i], pDst_q31[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_q15_to_q31);

    /*****************************************************************/
    BENCH_START(riscv_q15_to_q7);
    riscv_q15_to_q7(pSrc_q15, pDst_q7, ARRAY_SIZE);
    BENCH_END(riscv_q15_to_q7);
    ref_q15_to_q7(pSrc_q15, pDst_q7_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(pDst_q7[i] - pDst_q7_ref[i]) > DELTAQ7) {
            BENCH_ERROR(riscv_q15_to_q7);
            printf("expect: %d, actual: %d\n", pDst_q7_ref[i], pDst_q7[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_q15_to_q7);

    /*****************************************************************/
    printf("all q15 to others tests are passed,well done!\n");
}

/*
*********************************************************************************************************
*	? ? ?: DSP_Q31
*	????: Q31???????????
*	?    ?:?
*	? ? ?: ?
*********************************************************************************************************
*/
static int DSP_Q31(void)
{
    int i;
    BENCH_START(riscv_q31_to_float);
    riscv_q31_to_float(pSrc_q31, pDst_f32, ARRAY_SIZE);
    BENCH_END(riscv_q31_to_float);
    ref_q31_to_float(pSrc_q31, pDst_f32_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (fabs(pDst_f32[i] - pDst_f32_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_q31_to_float);
            printf("expect: %f, actual: %f\n", pDst_f32_ref[i], pDst_f32[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_q31_to_float);

    /*****************************************************************/
    BENCH_START(riscv_q31_to_q15);
    riscv_q31_to_q15(pSrc_q31, pDst_q15, ARRAY_SIZE);
    BENCH_END(riscv_q31_to_q15);
    ref_q31_to_q15(pSrc_q31, pDst_q15_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(pDst_q15[i] - pDst_q15_ref[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_q31_to_q15);
            printf("expect: %d, actual: %d\n", pDst_q15_ref[i], pDst_q15[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_q31_to_q15);

    /*****************************************************************/
    BENCH_START(riscv_q31_to_q7);
    riscv_q31_to_q7(pSrc_q31, pDst_q7, ARRAY_SIZE);
    BENCH_END(riscv_q31_to_q7);
    ref_q31_to_q7(pSrc_q31, pDst_q7_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(pDst_q7[i] - pDst_q7_ref[i]) > DELTAQ7) {
            BENCH_ERROR(riscv_q31_to_q7);
            printf("expect: %d, actual: %d\n", pDst_q7_ref[i], pDst_q7[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_q31_to_q7);

    /*****************************************************************/
    printf("all q31 to others tests are passed,well done!\n");
}

int main()
{
    BENCH_INIT;
    DSP_Q7();
    DSP_Q15();
    DSP_Q31();
    DSP_Fill();
    DSP_Copy();
    DSP_FloatToFix();
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
