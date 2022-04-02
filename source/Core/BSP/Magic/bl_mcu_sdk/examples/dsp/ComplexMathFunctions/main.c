//
// Created by lujun on 19-6-28.
//
// This contains complx calculate in Conjugate, Dot Product, Magnitude,
// Magnitude Squared, by-Complex Multiplication, by-Real Multiplication each one
// has it's own function. All function can be found in main function. If you
// don't want to use it, then comment it. contains following functions,eachone
// includes q15,q31 and f32 versions
#include "riscv_math.h"
#include <stdint.h>
#include "array.h"
#include "../common.h"

#include "../HelperFunctions/math_helper.c"
#include "../HelperFunctions/ref_helper.c"

#include <stdio.h>
#include <stdlib.h>
#define DELTAF32 (2.0f)
#define DELTAQ31 (63)
#define DELTAQ15 (1)
#define DELTAQ7 (1)

#define ENABLE_ALL
#define CmplxMag
#define CmplxDotProduct
#define CONJ
#define MagSquared
#define CmplxMult
#define CmplxMultReal

int test_flag_error = 0;

/*
*********************************************************************************************************
*	DSP_MagSquared
*********************************************************************************************************
*/
static int DSP_MagSquared(void)
{
    uint8_t i;
    /**********************************************************************************/
    BENCH_START(riscv_cmplx_mag_squared_f32);
    riscv_cmplx_mag_squared_f32(f32_pSrcA, f32_pDst, ARRAY_SIZE);
    BENCH_END(riscv_cmplx_mag_squared_f32);
    ref_cmplx_mag_squared_f32(f32_pSrcA, f32_pDst_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (fabs(f32_pDst[i] - f32_pDst_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_cmplx_mag_squared_f32);
            printf("index: %d, expect: %f, actual: %f\n", i, f32_pDst_ref[i],
                   f32_pDst[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_cmplx_mag_squared_f32);
    /***Q31*******************************************************************************/
    BENCH_START(riscv_cmplx_mag_squared_q31);
    riscv_cmplx_mag_squared_q31(q31_pSrcA, q31_pDst, ARRAY_SIZE);
    BENCH_END(riscv_cmplx_mag_squared_q31);
    ref_cmplx_mag_squared_q31(q31_pSrcA, q31_pDst_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (labs(q31_pDst[i] - q31_pDst_ref[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_cmplx_mag_squared_q31);
            printf("index: %d, expect: %x, actual: %x\n", i, q31_pDst_ref[i],
                   q31_pDst[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_cmplx_mag_squared_q31);
    /***Q15*******************************************************************************/
    BENCH_START(riscv_cmplx_mag_squared_q15);
    riscv_cmplx_mag_squared_q15(q15_pSrcA, q15_pDst, ARRAY_SIZE);
    BENCH_END(riscv_cmplx_mag_squared_q15);
    ref_cmplx_mag_squared_q15(q15_pSrcA, q15_pDst_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(q15_pDst[i] - q15_pDst_ref[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_cmplx_mag_squared_q15);
            printf("index: %d, expect: %x, actual: %x\n", i, q15_pDst_ref[i],
                   q15_pDst[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_cmplx_mag_squared_q15);
}
/*
*********************************************************************************************************
* DSP_CmplxMult
*********************************************************************************************************
*/
static int DSP_CmplxMult(void)
{
    uint8_t i;
    /**********************************************************************************/
    BENCH_START(riscv_cmplx_mult_cmplx_f32);
    riscv_cmplx_mult_cmplx_f32(f32_pSrcA, f32_pSrcB, f32_pDst, ARRAY_SIZE);
    BENCH_END(riscv_cmplx_mult_cmplx_f32);
    ref_cmplx_mult_cmplx_f32(f32_pSrcA, f32_pSrcB, f32_pDst_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (fabs(f32_pDst[i] - f32_pDst_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_cmplx_mult_cmplx_f32);
            printf("index: %d, expect: %f, actual: %f\n", i, f32_pDst_ref[i],
                   f32_pDst[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_cmplx_mult_cmplx_f32);
    /***Q31*******************************************************************************/
    BENCH_START(riscv_cmplx_mult_cmplx_q31);
    riscv_cmplx_mult_cmplx_q31(q31_pSrcA, q31_pSrcB, q31_pDst, ARRAY_SIZE);
    BENCH_END(riscv_cmplx_mult_cmplx_q31);
    ref_cmplx_mult_cmplx_q31(q31_pSrcA, q31_pSrcB, q31_pDst_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (labs(q31_pDst[i] - q31_pDst_ref[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_cmplx_mult_cmplx_q31);
            printf("index: %d, expect: %x, actual: %x\n", i, q31_pDst_ref[i],
                   q31_pDst[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_cmplx_mult_cmplx_q31);
    /***Q15*******************************************************************************/
    BENCH_START(riscv_cmplx_mult_cmplx_q15);
    riscv_cmplx_mult_cmplx_q15(q15_pSrcA, q15_pSrcB, q15_pDst, ARRAY_SIZE);
    BENCH_END(riscv_cmplx_mult_cmplx_q15);
    ref_cmplx_mult_cmplx_q15(q15_pSrcA, q15_pSrcB, q15_pDst_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(q15_pDst[i] - q15_pDst_ref[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_cmplx_mult_cmplx_q15);
            printf("index: %d, expect: %x, actual: %x\n", i, q15_pDst_ref[i],
                   q15_pDst[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_cmplx_mult_cmplx_q15);
}

/*
*********************************************************************************************************
* DSP_CmplxMultReal
*********************************************************************************************************
*/
static int DSP_CmplxMultReal(void)
{
    uint8_t i;
    /**********************************************************************************/
    BENCH_START(riscv_cmplx_mult_real_f32);
    riscv_cmplx_mult_real_f32(f32_pSrcA, f32_pSrcB, f32_pDst,
                            ARRAY_SIZE);
    BENCH_END(riscv_cmplx_mult_real_f32);
    ref_cmplx_mult_real_f32(f32_pSrcA, f32_pSrcB, f32_pDst_ref,
                            ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE * 2; i++) {
        if (fabs(f32_pDst[i] - f32_pDst_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_cmplx_mult_real_f32);
            printf("index: %d, expect: %f, actual: %f\n", i,
                   f32_pDst_ref[i], f32_pDst[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_cmplx_mult_real_f32);
    /***Q31*******************************************************************************/
    BENCH_START(riscv_cmplx_mult_real_q31);
    riscv_cmplx_mult_real_q31(q31_pSrcA, q31_pSrcB, q31_pDst,
                            ARRAY_SIZE);
    BENCH_END(riscv_cmplx_mult_real_q31);
    ref_cmplx_mult_real_q31(q31_pSrcA, q31_pSrcB, q31_pDst_ref,
                            ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE * 2; i++) {
        if (labs(q31_pDst[i] - q31_pDst_ref[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_cmplx_mult_real_q31);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q31_pDst_ref[i], q31_pDst[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_cmplx_mult_real_q31);
    /***Q15*******************************************************************************/
    BENCH_START(riscv_cmplx_mult_real_q15);
    riscv_cmplx_mult_real_q15(q15_pSrcA, q15_pSrcB, q15_pDst,
                            ARRAY_SIZE);
    BENCH_END(riscv_cmplx_mult_real_q15);
    ref_cmplx_mult_real_q15(q15_pSrcA, q15_pSrcB, q15_pDst_ref,
                            ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE * 2; i++) {
        if (abs(q15_pDst[i] - q15_pDst_ref[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_cmplx_mult_real_q15);
            printf("index: %d, expect: %x, actual: %x\n", i,
                   q15_pDst_ref[i], q15_pDst[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_cmplx_mult_real_q15);
}

/*
*********************************************************************************************************
* DSP_CONJ
*********************************************************************************************************
*/
static int DSP_CONJ(void)
{
    uint8_t i;
    /**********************************************************************************/
    BENCH_START(riscv_cmplx_conj_f32);
    riscv_cmplx_conj_f32(f32_pSrcA, f32_pDst, ARRAY_SIZE);
    BENCH_END(riscv_cmplx_conj_f32);
    ref_cmplx_conj_f32(f32_pSrcA, f32_pDst_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (fabs(f32_pDst[i] - f32_pDst_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_cmplx_conj_f32);
            printf("index: %d, expect: %f, actual: %f\n", i, f32_pDst_ref[i],
                   f32_pDst[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_cmplx_conj_f32);
    /***Q31*******************************************************************************/
    BENCH_START(riscv_cmplx_conj_q31);
    riscv_cmplx_conj_q31(q31_pSrcA, q31_pDst, ARRAY_SIZE);
    BENCH_END(riscv_cmplx_conj_q31);
    ref_cmplx_conj_q31(q31_pSrcA, q31_pDst_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (labs(q31_pDst[i] - q31_pDst_ref[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_cmplx_conj_q31);
            printf("index: %d, expect: %x, actual: %x\n", i, q31_pDst_ref[i],
                   q31_pDst[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_cmplx_conj_q31);
    /***Q15*******************************************************************************/
    BENCH_START(riscv_cmplx_conj_q15);
    riscv_cmplx_conj_q15(q15_pSrcA, q15_pDst, ARRAY_SIZE);
    BENCH_END(riscv_cmplx_conj_q15);
    ref_cmplx_conj_q15(q15_pSrcA, q15_pDst_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(q15_pDst[i] - q15_pDst_ref[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_cmplx_conj_q15);
            printf("index: %d, expect: %x, actual: %x\n", i, q15_pDst_ref[i],
                   q15_pDst[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_cmplx_conj_q15);
}

/*
*********************************************************************************************************
* DSP_CmplxDotProduct
*********************************************************************************************************
*/
static int DSP_CmplxDotProduct(void)
{
	float32_t f32_realResult;
	float32_t f32_imagResult;
	float32_t f32_realResult_ref;
	float32_t f32_imagResult_ref;
	q63_t q31_realResult;
	q63_t q31_imagResult;
	q63_t q31_realResult_ref;
	q63_t q31_imagResult_ref;
	q31_t q15_realResult;
	q31_t q15_imagResult;
	q31_t q15_realResult_ref;
	q31_t q15_imagResult_ref;
    /* clang-format on */

    /**********************************************************************************/
    BENCH_START(riscv_cmplx_dot_prod_f32);
    riscv_cmplx_dot_prod_f32(f32_pSrcA, f32_pSrcB, ARRAY_SIZE, &f32_realResult,
                           &f32_imagResult);
    BENCH_END(riscv_cmplx_dot_prod_f32);
    ref_cmplx_dot_prod_f32(f32_pSrcA, f32_pSrcB, ARRAY_SIZE,
                           &f32_realResult_ref, &f32_imagResult_ref);
    if ((fabs(f32_realResult - f32_realResult_ref) > DELTAF32) ||
        (fabs(f32_imagResult - f32_imagResult_ref) > DELTAF32)) {
        BENCH_ERROR(riscv_cmplx_dot_prod_f32);
        printf("real expect: %f, actual: %f\nimag expect: %f, actual: %f\n",
               f32_realResult_ref, f32_realResult, f32_imagResult_ref,
               f32_imagResult);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_cmplx_dot_prod_f32);
    /***Q31*******************************************************************************/
    BENCH_START(riscv_cmplx_dot_prod_q31);
    riscv_cmplx_dot_prod_q31(q31_pSrcA, q31_pSrcB, ARRAY_SIZE, &q31_realResult,
                           &q31_imagResult);
    BENCH_END(riscv_cmplx_dot_prod_q31);
    ref_cmplx_dot_prod_q31(q31_pSrcA, q31_pSrcB, ARRAY_SIZE,
                           &q31_realResult_ref, &q31_imagResult_ref);
    if ((labs(q31_realResult - q31_realResult_ref) > DELTAQ31) ||
        (labs(q31_imagResult - q31_imagResult_ref) > DELTAQ31)) {
        BENCH_ERROR(riscv_cmplx_dot_prod_q31);
        printf("real expect: %x, actual: %x\nimag expect: %x, actual: %x\n",
               q31_realResult_ref, q31_realResult, q31_imagResult_ref,
               q31_imagResult);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_cmplx_dot_prod_q31);
    /***Q15*******************************************************************************/
    BENCH_START(riscv_cmplx_dot_prod_q15);
    riscv_cmplx_dot_prod_q15(q15_pSrcA, q15_pSrcB, ARRAY_SIZE, &q15_realResult,
                           &q15_imagResult);
    BENCH_END(riscv_cmplx_dot_prod_q15);
    ref_cmplx_dot_prod_q15(q15_pSrcA, q15_pSrcB, ARRAY_SIZE,
                           &q15_realResult_ref, &q15_imagResult_ref);
    if ((abs(q15_realResult - q15_realResult_ref) > DELTAQ15) ||
        (abs(q15_imagResult - q15_imagResult_ref) > DELTAQ15)) {
        BENCH_ERROR(riscv_cmplx_dot_prod_q15);
        printf("real expect: %x, actual: %x\nimag expect: %x, actual: %x\n",
               q15_realResult_ref, q15_realResult, q15_imagResult_ref,
               q15_imagResult);
        test_flag_error = 1;
    }
    BENCH_STATUS(riscv_cmplx_dot_prod_q15);
}

/*
*********************************************************************************************************
* DSP_CmplxMag
*********************************************************************************************************
*/
static int DSP_CmplxMag(void)
{
    uint8_t i;
    /**********************************************************************************/
    BENCH_START(riscv_cmplx_mag_f32);
    riscv_cmplx_mag_f32(f32_pSrcA, f32_pDst, ARRAY_SIZE);
    BENCH_END(riscv_cmplx_mag_f32);
    ref_cmplx_mag_f32(f32_pSrcA, f32_pDst_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (fabs(f32_pDst[i] - f32_pDst_ref[i]) > DELTAF32) {
            BENCH_ERROR(riscv_cmplx_mag_f32);
            printf("index: %d, expect: %f, actual: %f\n", i, f32_pDst_ref[i],
                   f32_pDst[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_cmplx_mag_f32);
    /***Q31*******************************************************************************/
    BENCH_START(riscv_cmplx_mag_q31);
    riscv_cmplx_mag_q31(q31_pSrcA, q31_pDst, ARRAY_SIZE);
    BENCH_END(riscv_cmplx_mag_q31);
    ref_cmplx_mag_q31(q31_pSrcA, q31_pDst_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (labs(q31_pDst[i] - q31_pDst_ref[i]) > DELTAQ31) {
            BENCH_ERROR(riscv_cmplx_mag_q31);
            printf("index: %d, expect: %x, actual: %x\n", i, q31_pDst_ref[i],
                   q31_pDst[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_cmplx_mag_q31);
    /***Q15*******************************************************************************/
    BENCH_START(riscv_cmplx_mag_q15);
    riscv_cmplx_mag_q15(q15_pSrcA, q15_pDst, ARRAY_SIZE);
    BENCH_END(riscv_cmplx_mag_q15);
    ref_cmplx_mag_q15(q15_pSrcA, q15_pDst_ref, ARRAY_SIZE);
    for (i = 0; i < ARRAY_SIZE; i++) {
        if (abs(q15_pDst[i] - q15_pDst_ref[i]) > DELTAQ15) {
            BENCH_ERROR(riscv_cmplx_mag_q15);
            printf("index: %d, expect: %x, actual: %x\n", i, q15_pDst_ref[i],
                   q15_pDst[i]);
            test_flag_error = 1;
        }
    }
    BENCH_STATUS(riscv_cmplx_mag_q15);
}

int main()
{
    BENCH_INIT;
#if defined CmplxMag || defined ENABLE_ALL
    DSP_CmplxMag();
#endif
#if defined CmplxDotProduct || defined ENABLE_ALL
    DSP_CmplxDotProduct();
#endif
#if defined CONJ || defined ENABLE_ALL
    DSP_CONJ();
#endif
#if defined MagSquared || defined ENABLE_ALL
    DSP_MagSquared();
#endif
#if defined CmplxMult || defined ENABLE_ALL
    DSP_CmplxMult();
#endif
#if defined CmplxMultReal || defined ENABLE_ALL
    DSP_CmplxMultReal();
#endif
    BENCH_FINISH;
    if (test_flag_error) {
        printf("test error apprears, please recheck.\n");
        return 1;
    } else {
        printf("all test are passed. Well done!\n");
    }
    return 0;
}
